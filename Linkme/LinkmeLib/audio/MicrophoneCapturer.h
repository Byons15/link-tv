#pragma once
#include "AudioCapturer.h"

class MicrophoneCapturer 
    :public AudioCapturer
{
public:
    MicrophoneCapturer();
    ~MicrophoneCapturer();

    const std::vector<std::string>& DeviceList() const noexcept { return deviceList; }

protected:
    virtual winrt::com_ptr<IMMDevice> DeviceSelect(winrt::com_ptr<IMMDeviceEnumerator> enumeraotr) override;

private:
    std::vector<std::string> deviceList;
};


#define DeviceListEmptyException DeviceListEmptyException(__FILE__, __FUNCTION__, __LINE__)