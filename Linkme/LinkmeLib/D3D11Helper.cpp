#include "D3D11Helper.h"

using namespace DirectX;

void D3D11Helper::CreateDevice(winrt::com_ptr<ID3D11Device>& device, winrt::com_ptr<ID3D11DeviceContext>& context)
{
	HRESULT hr = S_OK;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT NumDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL featureLevel;

	device = nullptr;
	context = nullptr;

	for (UINT driverTypeIndex = 0; driverTypeIndex < NumDriverTypes; ++driverTypeIndex)
	{
		hr = D3D11CreateDevice(nullptr, driverTypes[driverTypeIndex], nullptr, 0, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, device.put(), &featureLevel, context.put());
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
	{
		winrt::throw_hresult(hr);
	}
}

winrt::com_ptr<ID3D11InputLayout> D3D11Helper::CreateInputLayout(winrt::com_ptr<ID3D11Device>& device, const BYTE shaderCode[], uint64_t codeLen)
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	UINT NumElements = ARRAYSIZE(layout);

	winrt::com_ptr<ID3D11InputLayout> inputLayout;
	winrt::check_hresult(device->CreateInputLayout(layout, NumElements, shaderCode, codeLen, inputLayout.put()));
	return inputLayout;
}

winrt::com_ptr<ID3D11SamplerState> D3D11Helper::CreateSampler(winrt::com_ptr<ID3D11Device>& device)
{
	D3D11_SAMPLER_DESC sampDesc;
	RtlZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	winrt::com_ptr<ID3D11SamplerState> sampler;
	winrt::check_hresult(device->CreateSamplerState(&sampDesc, sampler.put()));
	return sampler;
}

winrt::com_ptr<ID3D11Buffer> D3D11Helper::CreateVertexBuffer(winrt::com_ptr<ID3D11Device>& device, D3D11_USAGE usageFlags, D3D11_CPU_ACCESS_FLAG CPUAccessFlags)
{
	D3D11_BUFFER_DESC bufferDesc;
	RtlZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = usageFlags;
	bufferDesc.ByteWidth = sizeof(VERTEX) * 6;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = CPUAccessFlags;

	winrt::com_ptr<ID3D11Buffer> buffer;
	winrt::check_hresult(device->CreateBuffer(&bufferDesc, NULL, buffer.put()));
	return buffer;
}

void D3D11Helper::RectToVertex(const RECT& srcRect, const SIZE& destSize, VERTEX* vertices)
{
	float centerX = destSize.cx / 2.0f;
	float centerY = destSize.cy / 2.0f;

	auto& rect = srcRect;

	vertices[0] = VERTEX{ XMFLOAT3{(rect.left - centerX) / centerX, -1 * (rect.bottom - centerY) / centerY, 0.0f},
							XMFLOAT2{0.0f, 1.0f} };
	vertices[1] = VERTEX{ XMFLOAT3{(rect.left - centerX) / centerX, -1 * (rect.top - centerY) / centerY, 0.0f},
							XMFLOAT2{0.0f, 0.0f} };
	vertices[2] = VERTEX{ XMFLOAT3{(rect.right - centerX) / centerX, -1 * (rect.bottom - centerY) / centerY, 0.0f},
							XMFLOAT2{1.0f, 1.0f} };
	vertices[3] = vertices[2];
	vertices[4] = vertices[1];
	vertices[5] = VERTEX{ XMFLOAT3{(rect.right - centerX) / centerX, -1 * (rect.top - centerY) / centerY, 0.0f},
							XMFLOAT2{1.0f, 0.0f} };
}
