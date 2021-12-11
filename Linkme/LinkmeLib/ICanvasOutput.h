#pragma once
#include <cinttypes>
#include <Windows.h>
#include <memory>

struct IEncoder;
struct ITextureSwapChain;

struct __declspec(dllexport) ICanvasOutput
{
	virtual void Start(ITextureSwapChain* swapChain, int w, int h, int fps, const std::shared_ptr<IEncoder>& encoder) = 0;
	virtual ~ICanvasOutput() = default;
};
