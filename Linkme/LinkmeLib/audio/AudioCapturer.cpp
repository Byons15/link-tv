#include "AudioCapturer.h"
#include <Audioclient.h>
#include "../bns/EndCaller.h"

#pragma comment(lib, "Synchronization.lib")
#pragma comment(lib, "winmm.lib")

#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

using winrt::com_ptr;
using winrt::check_hresult;

AudioCapturer::AudioCapturer()
{
}

AudioCapturer::~AudioCapturer()
{
    Stop();
}

void AudioCapturer::Start(const std::shared_ptr<IEncoder>& encoder, int track)
{
    output = encoder;
    outputTrack = track;
    startEvent.SetEvent();
}

void AudioCapturer::Stop()
{
    if (!startEvent.is_signaled() && worker.IsRuning())
    {
        worker.RequestInterrupt();
        startEvent.SetEvent();
        worker.Wait();
    }
}

AudioFormat AudioCapturer::Format()
{
    return fmt;
}

void AudioCapturer::InitCapturer()
{
    std::promise<AudioFormat> fmtPromise;
    auto future = fmtPromise.get_future();
    worker.Lanuch(&AudioCapturer::Run, this, std::ref(fmtPromise));
    fmt = future.get();
}

void AudioCapturer::Run(std::promise<AudioFormat>& fmtPromise)
{
    const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
    const IID IID_IAudioClient = __uuidof(IAudioClient);
    const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    com_ptr<IMMDeviceEnumerator> deviceEnumerator;
    com_ptr<IMMDevice> device;
    com_ptr<IAudioClient> audioClient;
    com_ptr<IAudioCaptureClient> captureClient;
    UINT32 packetLength = 0;
    BYTE* pcmData;
    DWORD flags;
    int frameSize = 0;
    wil::unique_couninitialize_call coUninit;
    wil::unique_cotaskmem_secure_ptr<WAVEFORMATEX> pwfx;
    EDataFlow track;

    try
    {
        check_hresult(CoInitialize(0));

        check_hresult(CoCreateInstance(
            CLSID_MMDeviceEnumerator, NULL,
            CLSCTX_ALL, IID_IMMDeviceEnumerator,
            deviceEnumerator.put_void()));
        
        device = DeviceSelect(deviceEnumerator);

        com_ptr<IMMEndpoint> endPoint = device.as<IMMEndpoint>();

        check_hresult(endPoint->GetDataFlow(&track));

        check_hresult(device->Activate(
            IID_IAudioClient, CLSCTX_ALL,
            NULL, audioClient.put_void()));

        WAVEFORMATEX* waveFormat = NULL;
        check_hresult(audioClient->GetMixFormat(&waveFormat));

        pwfx.reset(waveFormat);

        AudioFormat fmt;
        fmt.channelLayout = KSAUDIO_SPEAKER_MONO;
        fmt.rate = pwfx->nSamplesPerSec;
        fmt.fmt = WAVE_FORMAT_IEEE_FLOAT;
        WAVEFORMATEXTENSIBLE* pwfxEx = nullptr;
        if (pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
        {
            pwfxEx = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pwfx.get());
            fmt.channelLayout = pwfxEx->dwChannelMask;
        }

        frameSize = pwfx->wBitsPerSample / 8 * pwfx->nChannels;

        check_hresult(audioClient->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            track == eCapture ? 0 : AUDCLNT_STREAMFLAGS_LOOPBACK,
            hnsRequestedDuration,
            0,
            pwfx.get(),
            NULL));

        check_hresult(audioClient->GetBufferSize(&bufferFrameCount));

        check_hresult(audioClient->GetService(
            IID_IAudioCaptureClient,
            captureClient.put_void()));

        hnsActualDuration = (double)REFTIMES_PER_SEC *
            bufferFrameCount / pwfx->nSamplesPerSec;

        fmtPromise.set_value(fmt);
    }
    catch (...)
    {
        fmtPromise.set_exception(std::current_exception());
        return;
    }

    startEvent.wait();
    check_hresult(audioClient->Start());

    bns::EndCaller audioClientStop(&IAudioClient::Stop, audioClient.get());

    while (!worker.IsRequestInterrupt())
    {
        worker.ProcessAccumulateDelegate();

        // 每次循环填充大约一半的共享缓冲区
        Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

        check_hresult(captureClient->GetNextPacketSize(&packetLength));

        while (packetLength != 0)
        {
            // 获取可用的共享缓冲
            uint64_t devicePos;
            check_hresult(captureClient->GetBuffer(&pcmData, &numFramesAvailable, &flags, &devicePos, NULL));

            // 根据flags确定pData中是否存在有效数据，如果不存在在则写入等长的内容全为0的缓冲
            output->WriteAudioFrame((flags & AUDCLNT_BUFFERFLAGS_SILENT) ? nullptr : pcmData, numFramesAvailable * frameSize, track);
            check_hresult(captureClient->ReleaseBuffer(numFramesAvailable));
            check_hresult(captureClient->GetNextPacketSize(&packetLength));
        }
    }
}
