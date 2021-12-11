#include "ICanvas.h"
#include "canvas/Canvas.h"

std::shared_ptr<ICanvas> CreateCanvas(uint32_t w, uint32_t h)
{
    return std::make_shared<Canvas>(w, h);
}
