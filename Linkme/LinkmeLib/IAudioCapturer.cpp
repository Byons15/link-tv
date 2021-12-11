#include "IAudioCapturer.h"
#include "audio/MicrophoneCapturer.h"
#include "audio/SpeakerCapturer.h"

std::shared_ptr<IAudioCapturer> _stdcall CreateAudioCapturer(DEVICE device)
{
	switch (device)
	{
	case SPEAKER:
		return std::make_shared<SpeakerCapturer>();
	case MICROPHONE:
		return std::make_shared <MicrophoneCapturer>();
	default:
		return nullptr;
	}
	return nullptr;
}
