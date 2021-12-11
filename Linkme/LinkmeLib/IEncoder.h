#pragma once
#include <cinttypes>
#include <memory>

struct IEncoder;

typedef struct AudioFormat_
{
	uint32_t fmt;
	uint64_t rate;
	uint64_t channelLayout;
}AudioFormat;

typedef struct EncoderDesc_
{
	const char* outputURL;

	bool hasVideo;
	uint64_t videoBitRate;
	uint32_t w, h;
	uint32_t fps;
		
	bool hasAudio;
	AudioFormat* audioInputs;
	uint32_t audioTracks;
}EncoderDesc;

__declspec(dllexport) std::shared_ptr<IEncoder>  _stdcall CreateEncoder(const EncoderDesc* desc);

struct __declspec(dllexport) IEncoder
{
	virtual void WriteVideoFrame(const uint8_t* data, uint64_t pts) = 0;
	virtual void WriteAudioFrame(const uint8_t* data, size_t dataLen, uint16_t track) = 0;
	virtual int VideoFrameFormat() = 0;
	virtual int AudioFrameForamt() = 0;
	virtual ~IEncoder() = default;
};
