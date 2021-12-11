#include "IDesktopCapturer.h"
#include "desktop/DesktopCapturer.h"

std::shared_ptr<ISprite> CreateDesktopCapturer(int monitorIndex)
{
    return std::make_shared<DesktopCapturer>(monitorIndex);
}
