#include "AudioEncoder.h"
#include "../bns/FFmpegException.h"
#include <sstream>
#include <Windows.h>

using namespace bns;

AudioEncoder::AudioEncoder(AVCodecContext* ctx, const AudioFormat* inputs, uint32_t inputCount, IMuxer* muxer)
	:codecCtx(ctx, AVCodecContextDeleter),
	frame(av_frame_alloc(), AVFrameDeleter),
	filterGraph(nullptr, AVFilterGraphDeleter),
	muxer(muxer)
{
	CheckAVResult(avcodec_open2(ctx, ctx->codec, NULL));

	frame->format = codecCtx->sample_fmt;
	frame->sample_rate = codecCtx->sample_rate;
	frame->nb_samples = codecCtx->frame_size;
	frame->channels = codecCtx->channels;
	frame->channel_layout = codecCtx->channel_layout;
	CheckAVResult(av_frame_get_buffer(frame.get(), 0));

	for (auto i = 0; i < inputCount; ++i)
	{
		Track track;
		track.inputInfo = inputs[i];
		tracks.push_back(std::move(track));
	}

	MakeFilter();
}

void AudioEncoder::SetVolume(int track, float vol)
{
	tracks[track].volume = vol;
	MakeFilter();
}

void AudioEncoder::Write(const uint8_t* data, size_t size, int trackIndex)
{
	int ret = 0;
	auto& track = tracks[trackIndex];

	if (size == 0 && data == nullptr)
	{
		ret = av_buffersrc_add_frame_flags(track.ctx, NULL, NULL);
		if (ret < 0)
			throw FFmpegException(ret);

		Encodec();
		return;
	}
	
	size_t bufferSize = track.frame->linesize[0];
	size_t srcCursor = 0;
	while (size > 0)
	{
		auto len = min(bufferSize - track.frameBufferCursor, size);
		CopyAudioData(track.frame->data[0] + track.frameBufferCursor, data, srcCursor, len);
		track.frameBufferCursor += len;
		
		if (track.frameBufferCursor == bufferSize)
		{
			//track.frame->pts = track.frameNum * track.frame->nb_samples;
			CheckAVResult(av_buffersrc_add_frame_flags(track.ctx, track.frame.get(), AV_BUFFERSRC_FLAG_KEEP_REF));

			track.frameBufferCursor = 0;
			++track.frameNum;

			Encodec();
		}

		srcCursor += len;
		size -= len;
	}
}

void AudioEncoder::MakeFilter()
{
	filterGraph.reset(avfilter_graph_alloc());

	AVFilterContext* outCtx = nullptr;
	CheckAVResult(avfilter_graph_create_filter(&outCtx, avfilter_get_by_name("abuffersink"),
		"out", NULL, NULL, filterGraph.get()));

	CheckAVResult(av_opt_set_bin(outCtx, "sample_fmts",
		(uint8_t*)&codecCtx->sample_fmt, sizeof(codecCtx->sample_fmt), AV_OPT_SEARCH_CHILDREN));

	CheckAVResult(av_opt_set_bin(outCtx, "channel_layouts",
		(uint8_t*)&codecCtx->channel_layout, sizeof(codecCtx->channel_layout), AV_OPT_SEARCH_CHILDREN));

	CheckAVResult(av_opt_set_bin(outCtx, "sample_rates",
		(uint8_t*)&codecCtx->sample_rate, sizeof(codecCtx->sample_rate), AV_OPT_SEARCH_CHILDREN));

	AVFilterInOut* filterOut = avfilter_inout_alloc();

	filterOut->name = av_strdup("out");
	filterOut->filter_ctx = outCtx;
	filterOut->pad_idx = 0;
	filterOut->next = nullptr;

	filterOutCtx = outCtx;

	std::ostringstream filterDesc;
	for (auto i = 0; i < tracks.size(); ++i)
		filterDesc << "[in" << i << ']' << "aresample=" << codecCtx->sample_rate << "[a" << i << "],";

	for(auto i = 0; i < tracks.size(); ++i)
		filterDesc << "[a" << i << "]aformat=sample_fmts=" << av_get_sample_fmt_name(codecCtx->sample_fmt)
		<< ":channel_layouts=" << codecCtx->channel_layout << "[b" << i << "],";

	for (auto i = 0; i < tracks.size(); ++i)
		filterDesc << "[b" << i << ']' << "volume=" << tracks[i].volume << "[c" << i << "],";

	for (auto i = 0; i < tracks.size(); ++i)
		filterDesc << "[c" << i << ']';

	filterDesc << "amix=inputs=" << tracks.size() << ":duration=longest:dropout_transition=0[out]";

	std::unique_ptr<char[]> padArgs(new char[128]);
	std::unique_ptr<char[]> filterName(new char[32]);
	int trackIndex = 0;
	std::unique_ptr<AVFilterInOut* []> filterInputs(new AVFilterInOut* [tracks.size()]());
	for (auto i = 0; i < tracks.size(); ++i)
	{
		auto& info = tracks[i].inputInfo;
		Track& track = tracks[i];
		track.frame.reset(av_frame_alloc());

		track.frame->channel_layout = info.channelLayout;
		track.frame->channels = av_get_channel_layout_nb_channels(track.frame->channel_layout);
		track.frame->nb_samples = (int)((double)(codecCtx->frame_size) / codecCtx->sample_rate * info.rate);
		track.frame->linesize[0] = track.frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)info.fmt) * track.frame->channels;
		track.frame->format = info.fmt;
		track.frame->sample_rate = info.rate;
		CheckAVResult(av_frame_get_buffer(track.frame.get(), 0));

		sprintf_s(padArgs.get(), 128, "time_base=%d/%lld:sample_rate=%lld:sample_fmt=%s:channel_layout=0x%I64x",
			1, info.rate, info.rate, av_get_sample_fmt_name((AVSampleFormat)info.fmt), info.channelLayout);

		sprintf_s(filterName.get(), 32, "in%d", trackIndex++);

		AVFilterContext* filterInCtx = nullptr;
		CheckAVResult(avfilter_graph_create_filter(&filterInCtx, avfilter_get_by_name("abuffer"),
			filterName.get(), padArgs.get(), 0, filterGraph.get()));

		AVFilterInOut* filterIn = avfilter_inout_alloc();

		filterIn->name = av_strdup(filterName.get());
		filterIn->filter_ctx = filterInCtx;
		filterIn->next = nullptr;
		filterIn->pad_idx = 0;

		track.ctx = filterInCtx;
		track.inputInfo = info;
		track.frameBufferCursor = 0;
		track.frameNum = 0;

		filterInputs[i] = filterIn;
	}

	for (auto i = 0; i < tracks.size() - 1; ++i)
		filterInputs[i]->next = filterInputs[i + 1];

	CheckAVResult(avfilter_graph_parse_ptr(filterGraph.get(), filterDesc.str().c_str(), &filterOut, filterInputs.get(), 0));

	avfilter_inout_free(filterInputs.get());
	avfilter_inout_free(&filterOut);

	CheckAVResult(avfilter_graph_config(filterGraph.get(), 0));
}

void AudioEncoder::Encodec()
{
	std::unique_ptr<AVFrame, AVFrameDeleter_t> tempFrame(av_frame_alloc(), AVFrameDeleter);

	int ret = 0;
	while (ret >= 0)
	{
		ret = av_buffersink_get_frame(filterOutCtx, tempFrame.get());
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			break;

		if (ret < 0)
			throw FFmpegException(ret);

		frameNum += tempFrame->nb_samples;
		tempFrame->pts = frameNum;
		ret = avcodec_send_frame(codecCtx.get(), tempFrame.get());
		if (ret < 0)
			throw FFmpegException(ret);
		
		av_frame_unref(tempFrame.get());
		
		while (ret >= 0)
		{
			std::shared_ptr<AVPacket> pkt;
			pkt.reset(av_packet_alloc(), AVPacketDeleter);

			ret = avcodec_receive_packet(codecCtx.get(), pkt.get());
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			{
				ret = 0;
				return;
			}
			else if (ret < 0)
			{
				throw FFmpegException(ret);
			}

			muxer->WriteAudioPacket(pkt);
		}
	}
}

void AudioEncoder::CopyAudioData(uint8_t* dest, const uint8_t* src, size_t srcOffset, size_t len)
{
	if (src)
		CopyMemory(dest, src + srcOffset, len);
	else
		ZeroMemory(dest, len);
}