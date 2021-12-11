#pragma once
#include <Windows.h>
#include <cinttypes>
#include <memory>
#include "ISprite.h"

struct ICanvasOutput;
struct ICanvasView;
struct ITextureSwapChain;
struct IEncoder;

struct __declspec(dllexport) ICanvas
{
	virtual void AddSprite(const std::shared_ptr<ISprite>& sprite) = 0;
	virtual void RemoveSprite(const std::shared_ptr<ISprite>& sprite) = 0;
	
	virtual void StartOutput(SIZE size, uint32_t fps, const std::shared_ptr<IEncoder>& encoder) = 0;
	virtual void StopOutput() = 0;

	virtual ITextureSwapChain* ViewTextureSwapChain() = 0;

	virtual SIZE Size() = 0;

	virtual ~ICanvas() = default;
};

__declspec(dllexport) std::shared_ptr<ICanvas> CreateCanvas(uint32_t w, uint32_t h);
