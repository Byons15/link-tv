#pragma once
#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <winrt/base.h>
#include "../ICanvasView.h"
#include "../ITextureSwapChain.h"
#include "../bns/Thread.h"
#include "../desktop/DesktopCapturer.h"

class CanvasView
	:public ICanvasView
{
public:
	CanvasView() = default;
	CanvasView(ITextureSwapChain* swapChain);
	CanvasView(HWND hwnd, ITextureSwapChain* swapChain);

	virtual void Paint() override;
	virtual void Resize(uint32_t w, uint32_t h) override;
	void SetOutputWindow(HWND hwnd);
	void SetSwapChain(ITextureSwapChain* swapChain);

	virtual HWND OutputWindow() override { return outputWindow; }

	virtual ~CanvasView();

private:

	void D3DPaint();
	void ResizeSwapChain(uint32_t w, uint32_t h);
	void MakeRTV();
	void SetViewPort(uint32_t w, uint32_t h);

	void WindowWorker(ITextureSwapChain* texSwapChain);

	static LRESULT WINAPI WndProcJump(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bns::Thread windowWorker;

	HWND outputWindow = NULL;

	winrt::com_ptr<IDXGISwapChain> swapChain;
	winrt::com_ptr<ID3D11Device> device;
	winrt::com_ptr<ID3D11DeviceContext> context;
	winrt::com_ptr<ID3D11VertexShader> vshader;
	winrt::com_ptr<ID3D11PixelShader> pshader;
	winrt::com_ptr<ID3D11InputLayout> inputLayout;
	winrt::com_ptr<ID3D11SamplerState> sampler;
	winrt::com_ptr<ID3D11Buffer> vertexBuffer;
	winrt::com_ptr<ID3D11RenderTargetView> rtv;

	ITextureSwapChain* texSwapChain;
};

