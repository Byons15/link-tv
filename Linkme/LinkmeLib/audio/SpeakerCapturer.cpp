#include "SpeakerCapturer.h"
#include <mmeapi.h>

#pragma comment(lib, "winmm.lib")

using winrt::check_hresult;

SpeakerCapturer::SpeakerCapturer()
{
    WAVEFORMATEX silentWaveFormat = { 0 };
    silentWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    silentWaveFormat.nChannels = 2;
    silentWaveFormat.nSamplesPerSec = 44100;
    silentWaveFormat.wBitsPerSample = 16;
    silentWaveFormat.nBlockAlign = silentWaveFormat.nChannels * silentWaveFormat.wBitsPerSample / 8;                //字节对齐
    silentWaveFormat.nAvgBytesPerSec = silentWaveFormat.nSamplesPerSec * silentWaveFormat.nBlockAlign;        //请求的平均数据传输率，单位 byte/s。该值对于创建缓冲区大小有用
    silentWaveFormat.cbSize = 0;

    waveOutCallbackEvent.create();
    check_hresult(waveOutOpen(slientWaveOut.addressof(), WAVE_MAPPER, &silentWaveFormat,
        (DWORD_PTR)(waveOutCallbackEvent.get()), 0, CALLBACK_EVENT));

    size_t silentWaveBufferSize = silentWaveFormat.nBlockAlign * 1024;

    slientWaveBuffer.reset(new uint8_t[silentWaveBufferSize]());

    WAVEHDR silentWaveHDR;

    silentWaveHDR.lpData = (LPSTR)slientWaveBuffer.get();
    silentWaveHDR.dwBufferLength = silentWaveBufferSize;
    silentWaveHDR.dwFlags = WHDR_BEGINLOOP;
    silentWaveHDR.dwLoops = MAXDWORD;

    waveOutPrepareHeader(slientWaveOut.get(), &silentWaveHDR, sizeof(silentWaveHDR));
    waveOutWrite(slientWaveOut.get(), &silentWaveHDR, sizeof(silentWaveHDR));

    InitCapturer();
}

winrt::com_ptr<IMMDevice> SpeakerCapturer::DeviceSelect(winrt::com_ptr<IMMDeviceEnumerator> enumerator)
{
    winrt::com_ptr<IMMDevice> device;
    check_hresult(enumerator->GetDefaultAudioEndpoint(eRender, eConsole, device.put()));
    return device;
}

MMRESULT __stdcall SpeakerCapturer::SlientWaveClose(HWAVEOUT hout)
{
    waveOutPause(hout);
    return waveOutClose(hout);
}