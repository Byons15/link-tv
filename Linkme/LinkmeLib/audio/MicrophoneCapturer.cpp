#include "MicrophoneCapturer.h"

MicrophoneCapturer::MicrophoneCapturer()
{
	InitCapturer();
}

MicrophoneCapturer::~MicrophoneCapturer()
{
}

winrt::com_ptr<IMMDevice> MicrophoneCapturer::DeviceSelect(winrt::com_ptr<IMMDeviceEnumerator> enumeraotr)
{
	winrt::com_ptr<IMMDevice> device;
	winrt::check_hresult(enumeraotr->GetDefaultAudioEndpoint(eCapture, eConsole, device.put()));
	return device;
}