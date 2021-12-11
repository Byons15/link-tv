#pragma once
#include "../ICanvasOutput.h"
#include <winrt/base.h>
#include <d3d11.h>
#include "../bns/Thread.h"
#include "../ITextureSwapChain.h"

class YUVOutput :
	public ICanvasOutput
{
public:
	YUVOutput() = default;
	virtual void Start(ITextureSwapChain* swapChain, int w, int h, int fps, const std::shared_ptr<IEncoder>& encoder) override;
	virtual ~YUVOutput();
private:
	void Run();

	winrt::com_ptr<ID3D11Device> device;
	winrt::com_ptr<ID3D11DeviceContext> context;
	winrt::com_ptr<ID3D11VertexShader> vshader;
	winrt::com_ptr<ID3D11PixelShader> pshader;
	winrt::com_ptr<ID3D11InputLayout> inputLayout;
	winrt::com_ptr<ID3D11SamplerState> sampler;
	winrt::com_ptr<ID3D11Buffer> vertexBuffer;

	winrt::com_ptr<ID3D11Texture2D> targetTex;
	winrt::com_ptr<ID3D11RenderTargetView> rtv;

	winrt::com_ptr<ID3D11Buffer> yuvBuffer;
	winrt::com_ptr<ID3D11Buffer> yuvTempBuffer;

	bns::Thread worker;

	SIZE size;
	uint32_t fps;
	std::shared_ptr<IEncoder> encoder;

	ITextureSwapChain* swapChain;
};

