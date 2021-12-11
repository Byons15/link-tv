#pragma once
#include "../bns/Thread.h"
#include <mmdeviceapi.h>
#include <WinInet.h>
#include <wil\resource.h>
#include <future>
#include <winrt/base.h>
#include "../IEncoder.h"
#include "../IAudioCapturer.h"

using Microsoft::WRL::ComPtr;

class AudioCapturer
	:public IAudioCapturer
{
public:
	AudioCapturer();
	virtual ~AudioCapturer();

	virtual void Start(const std::shared_ptr<IEncoder>& encoder, int track) override;
	virtual void Stop() override;
	virtual AudioFormat Format() override;

protected:
	void InitCapturer();

	virtual winrt::com_ptr<IMMDevice> DeviceSelect(winrt::com_ptr<IMMDeviceEnumerator> enumeraotr) = 0;

private:
	void Run(std::promise<AudioFormat>& fmtPromise);

	wil::slim_event_manual_reset startEvent;
	std::shared_ptr<IEncoder> output;
	int outputTrack;

	AudioFormat fmt;
	bns::Thread worker;
};

