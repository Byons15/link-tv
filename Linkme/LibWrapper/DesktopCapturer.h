#pragma once
#include "NativePtr.h"
#include <LinkmeLib/IDesktopCapturer.h>

namespace LinkmeLib
{
	public ref class DesktopCapturer :
		public NativePtr<ISprite>
	{
	public:

		DesktopCapturer(int monitorIndex)
		{
			SharedPtr = CreateDesktopCapturer(monitorIndex);
		}

	};
}

