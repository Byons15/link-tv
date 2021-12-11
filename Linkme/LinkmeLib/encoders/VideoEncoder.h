#pragma once
extern "C" 
{
#include <libavcodec\avcodec.h>
}
#include <mutex>
#include "../bns/Thread.h"
#include "../bns/FFmpegUtil.h"
#include "IMuxer.h"
#include <WinInet.h>
#include <wil/resource.h>

class VideoEncoder
{
public:
	VideoEncoder(AVCodecContext* codecCtx, IMuxer* muxer);
	~VideoEncoder();

	void WriteFrame(const uint8_t* yuvData, uint64_t pts);

private:
	void Run();
	
	wil::slim_event waitFrameEvent, receivePktEvent;
	std::unique_ptr<AVCodecContext, bns::AVCodecContextDeleter_t> m_codecCtx;

	std::unique_ptr<AVFrame, bns::AVFrameDeleter_t> frame;
	IMuxer* muxer;

	bns::Thread worker;
};

