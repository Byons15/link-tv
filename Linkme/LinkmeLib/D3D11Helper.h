#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <winrt/base.h>

typedef struct _VERTEX
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT2 TexCoord;
} VERTEX;

class D3D11Helper
{
public:
	static void CreateDevice(winrt::com_ptr<ID3D11Device>& device, winrt::com_ptr<ID3D11DeviceContext>& context);
	static winrt::com_ptr<ID3D11InputLayout> CreateInputLayout(winrt::com_ptr<ID3D11Device>& device, const BYTE shaderCode[], uint64_t codeLen);
	static winrt::com_ptr<ID3D11SamplerState> CreateSampler(winrt::com_ptr<ID3D11Device>& device);
	static winrt::com_ptr<ID3D11Buffer> CreateVertexBuffer(winrt::com_ptr<ID3D11Device>& device, D3D11_USAGE usageFlags, D3D11_CPU_ACCESS_FLAG CPUAccessFlags);
	static void RectToVertex(const RECT& srcRect, const SIZE& destSize, VERTEX* vertices);
};

