#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>
#include <winrt/base.h>
#include "../IDesktopCapturer.h"
#include <atomic>
#include <mutex>

class DesktopCapturer
	:public ISprite
{
public:
	DesktopCapturer(int monitorIndex);

	virtual void LoadContext(ID3D11Device* device, ID3D11DeviceContext* context) override;
	virtual SIZE Size() override;
	virtual int FPS() override { return fps; }
	virtual void Paint(ID3D11Texture2D** targets, int numTarget, const RECT& targetRect, const SIZE& scene) override;

	virtual ~DesktopCapturer();

private:
	void MakeD3DContext();
	void MakeDuplication();
	void SetViewPort(int w, int h);

	bool CheckDupl(HRESULT hr);

	HRESULT CopyFrame(winrt::com_ptr<ID3D11Texture2D>& frame,  const RECT& targetRect, const SIZE& scene);

	winrt::com_ptr<ID3D11Device> device;
	winrt::com_ptr<ID3D11DeviceContext> context;
	winrt::com_ptr<ID3D11VertexShader> vshader;
	winrt::com_ptr<ID3D11PixelShader> pshader;
	winrt::com_ptr<ID3D11InputLayout> inputLayout;
	winrt::com_ptr<ID3D11SamplerState> sampler;
	winrt::com_ptr<ID3D11Buffer> vertexBuffer;

	winrt::com_ptr<IDXGIOutputDuplication> dxgiOutputDupl;
	
	winrt::com_ptr<ID3D11Texture2D> desktopFrame;

	int outputIndex;
	std::atomic_uint outputSize;
	std::atomic_int fps;
	int rotation;
};

