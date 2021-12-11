#pragma once
#include <d3d11.h>
#include <dxgi1_2.h>
#include <winrt/base.h>
#include <array>
#include "../ITextureSwapChain.h"
#include <mutex>
#include <winnt.h>
#include <wil/resource.h>
#include <concurrent_queue.h>

class TextureSwapChain
	:public ITextureSwapChain
{
public:
	TextureSwapChain(int w, int h);
	TextureSwapChain(winrt::com_ptr<ID3D11Device>& backDevice, winrt::com_ptr<ID3D11DeviceContext>& backContext, 
		int w, int h);

	void SetBackDevice(ID3D11Device* device, ID3D11DeviceContext* context);

	virtual void SetDevice(ID3D11Device* device, ID3D11DeviceContext* context) override;

	virtual ID3D11Texture2D* BeginUse() override;

	virtual void EndUse() override;

	virtual const D3D11_TEXTURE2D_DESC& Desc() override { return texDesc; };

	virtual void UnloadDevice() override;

	ID3D11Texture2D* BeginUseBack();

	void EndUseBack();

	static constexpr int SWAP_CHAIN_SIZE = 2;

private:
	D3D11_TEXTURE2D_DESC texDesc;

	struct Buffer
	{
		winrt::com_ptr<ID3D11Texture2D> backTex, fontTex;
		winrt::com_ptr<IDXGIKeyedMutex> backKeyMutex, fontKeyMutex;
		HANDLE sharedHandle;
	};
	winrt::com_ptr<ID3D11Device> backDevice, fontDevice;
	winrt::com_ptr<ID3D11DeviceContext> backContext, fontContext;

	std::array<Buffer, SWAP_CHAIN_SIZE> buffer;
	concurrency::concurrent_queue< std::array<Buffer, SWAP_CHAIN_SIZE>::iterator > backQueue, fontQueue; 
	wil::unique_handle backSignal, fontSignal;
	std::array<Buffer, SWAP_CHAIN_SIZE>::iterator backUsing, fontUsing;
	std::atomic_bool fontUserReady;
};

