#include "TextureSwapChain.h"

using namespace winrt;

TextureSwapChain::TextureSwapChain(int w, int h)
{
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.Width = w;
	texDesc.Height = h;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
}

TextureSwapChain::TextureSwapChain(winrt::com_ptr<ID3D11Device>& backDevice, winrt::com_ptr<ID3D11DeviceContext>& backContext,
	int w, int h)
	:TextureSwapChain(w, h)
{
	SetBackDevice(backDevice.get(), backContext.get());
}

void TextureSwapChain::SetBackDevice(ID3D11Device* device, ID3D11DeviceContext* context)
{
	backDevice.attach(device);
	backContext.attach(context);

	for (auto& buffer : buffer)
	{
		buffer.backTex = nullptr;
		check_hresult(backDevice->CreateTexture2D(&texDesc, nullptr, buffer.backTex.put()));
		com_ptr<IDXGIResource> dxgiResource = buffer.backTex.as<IDXGIResource>();
		check_hresult(dxgiResource->GetSharedHandle(&buffer.sharedHandle));
		buffer.backKeyMutex = buffer.backTex.as<IDXGIKeyedMutex>();
	}

	backQueue.push(buffer.begin());
	fontQueue.push(buffer.begin() + 1);

	backSignal.reset(CreateSemaphore(NULL, 1, 2, NULL));
	fontSignal.reset(CreateSemaphore(NULL, 1, 2, NULL));

	fontUserReady = false;
}

void TextureSwapChain::SetDevice(ID3D11Device* device, ID3D11DeviceContext* context)
{
	fontDevice.attach(device);
	fontContext.attach(context);

	for (auto& buffer : buffer)
	{
		buffer.fontTex = nullptr;
		check_hresult(fontDevice->OpenSharedResource(buffer.sharedHandle, __uuidof(ID3D11Texture2D), buffer.fontTex.put_void()));
		buffer.fontKeyMutex = buffer.fontTex.as<IDXGIKeyedMutex>();
	}

	fontUserReady = true;
}

ID3D11Texture2D* TextureSwapChain::BeginUse()
{
	WaitForSingleObject(fontSignal.get(), 500);

	fontQueue.try_pop(fontUsing);
	auto hr = fontUsing->fontKeyMutex->AcquireSync(0, 50);
	if (hr == WAIT_TIMEOUT || hr == WAIT_ABANDONED)
		throw_hresult(E_FAIL);
	else if (FAILED(hr))
		throw_hresult(hr);

	return fontUsing->fontTex.get();
}

void TextureSwapChain::EndUse()
{
	check_hresult(fontUsing->fontKeyMutex->ReleaseSync(0));
	backQueue.push(fontUsing);
	ReleaseSemaphore(backSignal.get(), 1, NULL);
}

void TextureSwapChain::UnloadDevice()
{
	fontUserReady = false;
}

ID3D11Texture2D* TextureSwapChain::BeginUseBack()
{
	WaitForSingleObject(backSignal.get(), 500);
	backQueue.try_pop(backUsing);
	auto hr = backUsing->backKeyMutex->AcquireSync(0, 50);
	if (hr == WAIT_TIMEOUT || hr == WAIT_ABANDONED)
		throw_hresult(E_FAIL);
	else if (FAILED(hr))
		throw_hresult(hr);

	return backUsing->backTex.get();
}

void TextureSwapChain::EndUseBack()
{
	check_hresult(backUsing->backKeyMutex->ReleaseSync(0));

	if (fontUserReady) 
	{
		fontQueue.push(backUsing);
		ReleaseSemaphore(fontSignal.get(), 1, NULL);
	}
	else
	{
		backQueue.push(backUsing);
		ReleaseSemaphore(backSignal.get(), 1, NULL);
	}
}
