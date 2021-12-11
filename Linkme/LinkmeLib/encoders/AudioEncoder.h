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
#include <libavfilter\buffersrc.h>
#include <libavfilter\buffersink.h>
}
#include <array>
#include <memory>
#include "../bns/FFmpegUtil.h"
#include <vector>
#include "IMuxer.h"
#include "../IEncoder.h"

class AudioEncoder
{
public:
	AudioEncoder(AVCodecContext* ctx, const AudioFormat* inputs, uint32_t inputCount, IMuxer* muxer);

	void SetVolume(int track, float vol);
	void Write(const uint8_t* data, size_t size, int trackIndex);

private:
	void MakeFilter();
	void Encodec();
	static void CopyAudioData(uint8_t *dest, const uint8_t* src, size_t srcOffset, size_t len);

	std::unique_ptr<AVCodecContext, bns::AVCodecContextDeleter_t> codecCtx;
	std::unique_ptr<AVFrame, bns::AVFrameDeleter_t> frame;
	int64_t frameNum = 0;

	AVFilterContext* filterOutCtx;
	std::unique_ptr<AVFilterGraph, bns::AVFilterGraphDeleter_t> filterGraph;

	struct Track
	{
		AVFilterContext* ctx = nullptr;
		std::unique_ptr<AVFrame, bns::AVFrameDeleter_t> frame;
		size_t frameBufferCursor = 0;
		size_t frameNum = 0;
		AudioFormat inputInfo;
		float volume = 1.0f;

		Track()
			:frame(nullptr, bns::AVFrameDeleter)
		{}
	};
	
	std::vector<Track> tracks;
	IMuxer* muxer;
};
