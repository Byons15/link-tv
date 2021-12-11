#pragma once
#include "AudioCapturer.h"
#include <memory>

class SpeakerCapturer : public AudioCapturer
{
public:
    SpeakerCapturer();

protected:
    virtual winrt::com_ptr<IMMDevice> DeviceSelect(winrt::com_ptr<IMMDeviceEnumerator> enumerator) override;

private:
    std::unique_ptr<uint8_t[]> slientWaveBuffer;

    static MMRESULT __stdcall SlientWaveClose(HWAVEOUT hout);

    wil::unique_event waveOutCallbackEvent;
    wil::unique_any<HWAVEOUT, MMRESULT(__stdcall*)(HWAVEOUT), &SpeakerCapturer::SlientWaveClose> slientWaveOut;
};

