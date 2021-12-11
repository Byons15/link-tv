#pragma once
extern "C"
{
#include <libavformat\avformat.h>
#include <libavformat\avio.h>
#include <libavcodec\avcodec.h>
#include <libavutil\opt.h>
#include <libswresample\swresample.h>
#include <libavutil\avutil.h>
#include <libavfilter\avfilter.h>
}
#include <memory>
#include "../bns/FFmpegUtil.h"
#include "AudioEncoder.h"
#include <string>
#include "../bns/EndCaller.h"
#include "VideoEncoder.h"
#include "../IEncoder.h"

class Encoder :
	private IMuxer,
	public IEncoder
{
public:
	Encoder(const EncoderDesc& desc);
	~Encoder();

	virtual void WriteVideoFrame(const uint8_t* data, uint64_t pts) override;
	virtual void WriteAudioFrame(const uint8_t* data, size_t dataLen, uint16_t track) override;

	virtual int VideoFrameFormat() override;
	virtual int AudioFrameForamt() override;

private:
	virtual void WriteVideoPacket(std::shared_ptr<AVPacket> pkt) override;
	virtual void WriteAudioPacket(std::shared_ptr<AVPacket> pkt) override;

	AVCodecContext* CreateVCodecCtx(int64_t bitRate, uint32_t fps, uint32_t w, uint32_t h);
	AVCodecContext* CreateACodecCtx(int64_t bitRate);

	void FmtCtxDeleter(AVFormatContext* ctx) noexcept;

	static void CodecCtxDeleter(AVCodecContext* ctx) noexcept;

	void WritePacket(std::shared_ptr<AVPacket> pkt, uint32_t streamIndex);

	std::unique_ptr<AVFormatContext, std::function<void(AVFormatContext*)>> fmtCtx;

	AVRational videoTimeBase;
	int videoStreamIndex;
	std::unique_ptr<VideoEncoder> videoEncoder;

	std::unique_ptr<AudioEncoder> audioEncoder;
	AVRational audioTimeBase;
	int audioStreamIndex;

	bns::Thread worker;
};
