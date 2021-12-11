#pragma once
#include "NativePtr.h"
#include "../LinkmeLib/IEncoder.h"
#include <Windows.h>

using namespace System::Runtime::InteropServices;

namespace LinkmeLib
{
	public enum class AudioChannelLayout
	{
		Left = 0x00000001,
		Right = 0x00000002,
		Stereo = Left | Right,
	};

	public enum class AudioSampleFormat
	{
		None = -1,
		U8,          ///< unsigned 8 bits
		S16,         ///< signed 16 bits
		S32,         ///< signed 32 bits
		FLT,         ///< float
		DBL,         ///< double

		U8P,         ///< unsigned 8 bits, planar
		S16P,        ///< signed 16 bits, planar
		S32P,        ///< signed 32 bits, planar
		FLTP,        ///< float, planar
		DBLP,        ///< double, planar
		S64,         ///< signed 64 bits
		S64P,        ///< signed 64 bits, planar

		NB           ///< Number of sample formats. DO NOT USE if linking dynamically
	};

	public value struct AudioInfo
	{
		int fmt;
		uint64_t rate;
		uint64_t channelLayout;
	};

	public ref class Encoder :public NativePtr<IEncoder>
	{
	public:
		Encoder(int w, int h, int fps, long long videoBitRate, array<LinkmeLib::AudioInfo>^ audioInputs, System::String^ outputUrl)
		{
			EncoderDesc desc;
			desc.audioTracks = audioInputs->Length;
			desc.audioInputs = new ::AudioFormat[desc.audioTracks];
			for (auto i = 0; i < audioInputs->Length; ++i)
			{
				desc.audioInputs[i].channelLayout = (uint64_t)(audioInputs[i].channelLayout);
				desc.audioInputs[i].rate = audioInputs[i].rate;
				desc.audioInputs[i].fmt = (uint32_t)(audioInputs[i].fmt);
			}
			desc.w = w;
			desc.h = h;
			desc.videoBitRate = videoBitRate;
			desc.hasAudio = true;
			desc.hasVideo = true;
			desc.fps = fps;

			auto unicodeBytes = (wchar_t*)(void*)Marshal::StringToHGlobalUni(outputUrl);
			auto len = WideCharToMultiByte(CP_UTF8, 0, unicodeBytes, -1, NULL, 0, NULL, NULL);
			char* utf8Bytes = new char[len + 1];
			utf8Bytes[len] = '\0';
			WideCharToMultiByte(CP_UTF8, 0, unicodeBytes, -1, utf8Bytes, len, NULL, NULL);
			Marshal::FreeHGlobal((System::IntPtr)unicodeBytes);
			desc.outputURL = utf8Bytes;

			SharedPtr = CreateEncoder(&desc);

			delete[] utf8Bytes;
			delete[] desc.audioInputs;
		}
	};
}

