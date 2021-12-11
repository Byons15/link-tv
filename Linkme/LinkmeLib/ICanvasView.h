#pragma once
#include<cinttypes>
#include "ITextureSwapChain.h"
#include <memory>

struct __declspec(dllexport) ICanvasView
{
	virtual void Paint() = 0;
	virtual void Resize(uint32_t w, uint32_t h) = 0;
	virtual HWND OutputWindow() = 0;

	virtual ~ICanvasView() = default;
};


__declspec(dllexport) std::shared_ptr<ICanvasView> _stdcall CreateCanvasView(HWND hwnd, ITextureSwapChain* swapChain);
__declspec(dllexport) std::shared_ptr<ICanvasView> _stdcall CreateCanvasViewWithPopupWindow(ITextureSwapChain* swapChain);
