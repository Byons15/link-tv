#pragma once
#include <d3d11.h>

struct __declspec(dllexport) ITextureSwapChain
{
	virtual void SetDevice(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
	virtual ID3D11Texture2D* BeginUse() = 0;
	virtual void EndUse() = 0;
	virtual const D3D11_TEXTURE2D_DESC& Desc() = 0;
	virtual void UnloadDevice() = 0;

	virtual ~ITextureSwapChain() = default;
};

