#pragma once
#include "../IEncoder.h"
#include "../bns/Thread.h"
#include <winrt/base.h>
#include "../ICanvas.h"
#include <memory>
#include <d3d11.h>
#include "../D3D11Helper.h"
#include "../ICanvasOutput.h"
#include <vector>
#include "CanvasView.h"
#include "TextureSwapChain.h"
#include "../ISprite.h"

class Canvas
	:public ICanvas
{
public:
	
	Canvas(uint32_t w, uint32_t h);

	virtual void AddSprite(const std::shared_ptr<ISprite>& sprite) override;
	virtual void RemoveSprite(const std::shared_ptr<ISprite>& sprite) override;

	virtual void StartOutput(SIZE size, uint32_t fps, const std::shared_ptr<IEncoder>& encoder) override;
	virtual void StopOutput() override;

	virtual ITextureSwapChain* ViewTextureSwapChain() override;

	virtual SIZE Size() override;

	virtual ~Canvas();

private:
	void Run();

	void SetGraphicsVertex(const RECT& rect, VERTEX* vertices);

	struct Sprite
	{
		std::shared_ptr<ISprite> sprite;
		RECT rect;
	};

	std::vector<Sprite> sprites;

	bns::Thread worker;

	winrt::com_ptr<ID3D11Device> device;
	winrt::com_ptr<ID3D11DeviceContext> context;

	SIZE size;

	TextureSwapChain viewTexSwapChain;

	std::unique_ptr<ICanvasOutput> output;
	TextureSwapChain outputSwapChain;
};

