#pragma once
#include "IEncoder.h"

struct IAudioCapturer;

struct __declspec(dllexport) IAudioCapturer
{
	virtual void Start(const std::shared_ptr<IEncoder>& encoder, int track) = 0;
	virtual void Stop() = 0;
	virtual AudioFormat Format() = 0;
	virtual ~IAudioCapturer() = default;
};


enum DEVICE
{
	SPEAKER,
	MICROPHONE,
};
__declspec(dllexport) std::shared_ptr<IAudioCapturer> _stdcall CreateAudioCapturer(DEVICE device);
