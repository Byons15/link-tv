#include "VideoEncoder.h"

extern "C" {
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

#include "../bns/FFmpegUtil.h"
#include "../bns/FFmpegException.h"

using namespace bns;

VideoEncoder::VideoEncoder(AVCodecContext* codecCtx, IMuxer* muxer)
	:m_codecCtx(codecCtx, AVCodecContextDeleter),
	muxer(muxer),
	frame(av_frame_alloc(), AVFrameDeleter)
{
	frame->format = codecCtx->pix_fmt;
	frame->height = codecCtx->height;
	frame->width = codecCtx->width;
	CheckAVResult(av_frame_get_buffer(frame.get(), 0));

	AVDictionary* options = nullptr;
	
	CheckAVResult(avcodec_open2(codecCtx, codecCtx->codec, &options));

	waitFrameEvent.SetEvent();

	worker.Lanuch(&VideoEncoder::Run, this);
}

VideoEncoder::~VideoEncoder()
{
	WriteFrame(nullptr, 0);
	worker.RequestInterrupt();
	worker.Wait();
}

void VideoEncoder::WriteFrame(const uint8_t* yuvData, uint64_t pts)
{
	if (!yuvData)
	{
		waitFrameEvent.wait();
		avcodec_send_frame(m_codecCtx.get(), nullptr);
		receivePktEvent.SetEvent();
		return;
	}

	// 我也不知道要不要用这种方式补帧
	// 出现跳帧说明调用方已经很忙了，再在这里循环补帧搞不好会雪上加霜
	while (++frame->pts < pts)
	{
		frame->pict_type = AV_PICTURE_TYPE_P;

		waitFrameEvent.wait();
		CheckAVResult(avcodec_send_frame(m_codecCtx.get(), frame.get()));

		receivePktEvent.SetEvent();
	}
	frame->pict_type = AV_PICTURE_TYPE_NONE; 

	CopyMemory(frame->data[0], yuvData, frame->width * frame->height);
	CopyMemory(frame->data[1], yuvData + frame->width * frame->height, frame->width * frame->height / 4);
	CopyMemory(frame->data[2], yuvData + frame->width * frame->height + frame->width * frame->height / 4,
			   frame->width * frame->height / 4);

	frame->pts = pts;

	waitFrameEvent.wait();
	auto ret = avcodec_send_frame(m_codecCtx.get(), frame.get());
	if (ret < 0)
		throw FFmpegException(ret);

	receivePktEvent.SetEvent();
}

void VideoEncoder::Run()
{
	while (!worker.IsRequestInterrupt())
	{
		receivePktEvent.wait();

		int ret = 0;
		while (ret >= 0)
		{
			AVPacket* pkt = av_packet_alloc();
			ret = avcodec_receive_packet(m_codecCtx.get(), pkt);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			{
				av_packet_free(&pkt);
				break;
			}
			else if (ret < 0)
			{
				av_packet_free(&pkt);
				throw FFmpegException(ret);
			}

			std::shared_ptr<AVPacket> spkt(pkt, AVPacketDeleter);
			muxer->WriteVideoPacket(spkt);
		}
		waitFrameEvent.SetEvent();
	}
}