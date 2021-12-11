#pragma once
#include "ISprite.h"
#include <memory>

__declspec(dllexport) std::shared_ptr<ISprite> _stdcall CreateDesktopCapturer(int monitorIndex);
