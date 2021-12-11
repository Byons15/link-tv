#pragma once

#include "NativePtr.h"
#include <LinkmeLib/IAudioCapturer.h>
#include "Encoder.h"

namespace LinkmeLib 
{
	public enum class AudioDevice
	{
		Speaker = 0,
		Microphone = 1,
	};

	public ref class AudioCapturer :public NativePtr<IAudioCapturer>
	{
	public:
		AudioCapturer(AudioDevice device)
		{
			SharedPtr = CreateAudioCapturer((DEVICE)device);
		}

		void Start(Encoder^ encoder, int track)
		{
			Ptr->Start(encoder->SharedPtr, track);
		}

		property AudioInfo Format
		{
			AudioInfo get()
			{
				auto fmt = Ptr->Format();
				AudioInfo info;
				info.channelLayout = fmt.channelLayout;
				info.fmt = fmt.fmt;
				info.rate = fmt.rate;
				return info;
			}
		}

		void Stop()
		{
			Ptr->Stop();
		}

	};
}