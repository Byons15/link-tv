#include "Encoder.h"

#include <stdexcept>

extern "C"
{
#include <libavutil\buffer.h>
#include <libavcodec\avcodec.h>
#include <libavutil/frame.h>
}

#include "../bns/FFmpegException.h"

using namespace bns;

Encoder::Encoder(const EncoderDesc& desc)
	:fmtCtx(nullptr, std::bind(&Encoder::FmtCtxDeleter, this, std::placeholders::_1))
{

	std::string outUrl(desc.outputURL);

	auto isNetworkURL = outUrl.substr(0, 7) == "rtmp://";

	AVDictionary* headerOption = nullptr;
	const char* fmtName = isNetworkURL ? "flv" : nullptr;

	if (outUrl.substr(outUrl.length() - 5) == ".mp4")
		CheckAVResult(av_dict_set_int(&headerOption, "video_track_timescale", desc.fps, 0));

	AVFormatContext* ctx = nullptr;
	CheckAVResult(avformat_alloc_output_context2(&ctx, NULL, fmtName, desc.outputURL));
	
	fmtCtx.reset(ctx);

	if (desc.hasVideo) 
	{
		AVCodecContext* videoCodcCtx = CreateVCodecCtx(desc.videoBitRate, desc.fps, desc.w, desc.h);

		av_opt_set(videoCodcCtx->priv_data, "preset", "fast", 0);

		if (isNetworkURL)
			av_opt_set(videoCodcCtx->priv_data, "tune", "zerolatency", 0);

		videoEncoder.reset(new VideoEncoder(videoCodcCtx, this));

		auto stream = avformat_new_stream(fmtCtx.get(), NULL);
		videoTimeBase = videoCodcCtx->time_base;
		stream->time_base.num = 1;
		stream->time_base.den = desc.fps;
		stream->avg_frame_rate = AVRational{ 1, static_cast<const int>(desc.fps) };
		videoStreamIndex = stream->index;

		avcodec_parameters_from_context(stream->codecpar, videoCodcCtx);
	}

	if (desc.hasAudio)
	{
		auto audioCodecCtx = CreateACodecCtx(96000);

		if (!desc.hasVideo)
			audioCodecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

		audioEncoder.reset(new AudioEncoder(audioCodecCtx, desc.audioInputs, desc.audioTracks, this));
		auto stream = avformat_new_stream(fmtCtx.get(), NULL);
		audioTimeBase = stream->time_base = audioCodecCtx->time_base;
		audioStreamIndex = stream->index;
		avcodec_parameters_from_context(stream->codecpar, audioCodecCtx);
	}

	CheckAVResult(avio_open2(&fmtCtx->pb, desc.outputURL, AVIO_FLAG_WRITE, 0, NULL));
	
	CheckAVResult(avformat_write_header(fmtCtx.get(), &headerOption));

	worker.Lanuch();
}

Encoder::~Encoder()
{
	videoEncoder.reset();
	audioEncoder.reset();
	
	worker.RequestInterrupt();
	worker.Wait();
	worker.ProcessAccumulateDelegate();
}

void Encoder::WriteVideoFrame(const uint8_t* data, uint64_t pts)
{
	videoEncoder->WriteFrame(data, pts);
}

void Encoder::WriteAudioFrame(const uint8_t* data, size_t dataLen, uint16_t track)
{
	audioEncoder->Write(data, dataLen, track);
}

int Encoder::VideoFrameFormat()
{
	return fmtCtx->streams[videoStreamIndex]->codecpar->format;
}

int Encoder::AudioFrameForamt()
{
	return fmtCtx->streams[audioStreamIndex]->codecpar->format;
}

void Encoder::WriteVideoPacket(std::shared_ptr<AVPacket> pkt)
{
	worker.BeginInvoke([pkt, this]() 
		{
			WritePacket(pkt, videoStreamIndex);
		});
}

void Encoder::WriteAudioPacket(std::shared_ptr<AVPacket> pkt)
{
	worker.BeginInvoke([pkt, this]()
		{
			WritePacket(pkt, audioStreamIndex);
		});
}

AVCodecContext* Encoder::CreateVCodecCtx(int64_t bitRate, uint32_t fps, uint32_t w, uint32_t h)
{
	AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	//AVCodec* codec = avcodec_find_encoder_by_name("h264_nvenc");
	AVCodecContext* ctx = avcodec_alloc_context3(codec);

	ctx->codec = codec;
	ctx->bit_rate = bitRate;
	ctx->height = h;
	ctx->width = w;
	ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	ctx->rc_min_rate = ctx->bit_rate;
	ctx->rc_max_rate = ctx->bit_rate;
	ctx->bit_rate_tolerance = ctx->bit_rate;
	ctx->rc_buffer_size = ctx->bit_rate;
	ctx->rc_initial_buffer_occupancy = ctx->rc_buffer_size * 3 / 4;
	ctx->framerate = AVRational{ (int)fps, 1 };
	ctx->time_base = AVRational{ 1, (int)fps };
	ctx->gop_size = 10;
	ctx->max_b_frames = 0;

	return ctx;
}

AVCodecContext* Encoder::CreateACodecCtx(int64_t bitRate)
{
	AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
	AVCodecContext* ctx = avcodec_alloc_context3(codec);

	ctx->codec = codec;
	ctx->sample_fmt = AV_SAMPLE_FMT_FLTP;
	ctx->bit_rate = bitRate;
	ctx->channel_layout = AV_CH_LAYOUT_STEREO;
	ctx->channels = av_get_channel_layout_nb_channels(ctx->channel_layout);
	ctx->sample_rate = 44100;
	ctx->frame_size = 1024;

	return ctx;
}

void Encoder::FmtCtxDeleter(AVFormatContext* ctx) noexcept
{
	if (ctx->pb)
	{
		av_write_trailer(ctx);
		avio_close(ctx->pb);
	}

	avformat_free_context(ctx);
}

void Encoder::CodecCtxDeleter(AVCodecContext* ctx) noexcept
{
	avcodec_close(ctx);
}

void Encoder::WritePacket(std::shared_ptr<AVPacket> pkt, uint32_t streamIndex)
{
	if (!pkt)
	{
		av_interleaved_write_frame(fmtCtx.get(), NULL);
		return;
	}

	if (pkt->size == 0)
		return;

	pkt->stream_index = streamIndex;

	AVRational timeBase;
	int ret = 0;
	
	switch (fmtCtx->streams[streamIndex]->codecpar->codec_type)
	{
	case AVMEDIA_TYPE_AUDIO:
		timeBase = audioTimeBase;
		break;
	case AVMEDIA_TYPE_VIDEO:
		timeBase = videoTimeBase;
		break;
	default:
		DebugBreak();
		break;
	}

	av_packet_rescale_ts(pkt.get(), timeBase, fmtCtx->streams[streamIndex]->time_base);
	CheckAVResult(av_interleaved_write_frame(fmtCtx.get(), pkt.get()));
}
