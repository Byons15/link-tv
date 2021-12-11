#include "ICanvasView.h"
#include "canvas/CanvasView.h"

std::shared_ptr<ICanvasView> _stdcall CreateCanvasView(HWND hwnd, ITextureSwapChain* textureSwapChain)
{
	return std::make_shared<CanvasView>(hwnd, textureSwapChain);
}

std::shared_ptr<ICanvasView> _stdcall CreateCanvasViewWithPopupWindow(ITextureSwapChain* textureSwapChain)
{
	return std::make_shared<CanvasView>(textureSwapChain);
}
