#include "DesktopCapturer.h"
#include "../D3D11Helper.h"
#include "../VertexShader.h"
#include "../PixelShader.h"

using namespace winrt;
using namespace DirectX;

DesktopCapturer::DesktopCapturer(int monitorIndex)
	:outputIndex(monitorIndex)
{
}

void DesktopCapturer::LoadContext(ID3D11Device* device, ID3D11DeviceContext* context)
{
	this->device.attach(device);
	this->context.attach(context);
	MakeD3DContext();
	MakeDuplication();
}

SIZE DesktopCapturer::Size()
{
	unsigned size = outputSize;
	return SIZE{LOWORD(size), HIWORD(size)};
}

void DesktopCapturer::Paint(ID3D11Texture2D** targets, int numTarget, const RECT& targetRect, const SIZE& scene)
{
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	com_ptr<IDXGIResource> frameResource;
	auto hr = dxgiOutputDupl->AcquireNextFrame(500, &frameInfo, frameResource.put());
	if (hr == DXGI_ERROR_WAIT_TIMEOUT)
	{
		return;
	}

	if (hr == DXGI_ERROR_ACCESS_LOST)
	{
		MakeDuplication();
		return;
	}

	if (FAILED(hr))
		winrt::throw_hresult(hr);

	desktopFrame = frameResource.as<ID3D11Texture2D>();

	for (auto i = 0; i < numTarget; ++i)
	{
		context->CopySubresourceRegion(targets[i], 0, targetRect.left, targetRect.top, 0, desktopFrame.get(), 0, nullptr);
	}

	dxgiOutputDupl->ReleaseFrame();
}

DesktopCapturer::~DesktopCapturer()
{
}

void DesktopCapturer::MakeD3DContext()
{
	vshader = nullptr;
	pshader = nullptr;
	inputLayout = nullptr;
	sampler = nullptr;
	vertexBuffer = nullptr;

	check_hresult(device->CreateVertexShader(g_VS, ARRAYSIZE(g_VS), nullptr, vshader.put()));
	check_hresult(device->CreatePixelShader(g_PS, ARRAYSIZE(g_PS), nullptr, pshader.put()));
	inputLayout = D3D11Helper::CreateInputLayout(device, g_VS, ARRAYSIZE(g_VS));
	sampler = D3D11Helper::CreateSampler(device);

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX) * 6;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	check_hresult(device->CreateBuffer(&vertexBufferDesc, nullptr, vertexBuffer.put()));
}

void DesktopCapturer::MakeDuplication()
{
	com_ptr<IDXGIDevice> dxgiDevice = device.as<IDXGIDevice>();

	com_ptr<IDXGIAdapter> dxgiAdapter;
	check_hresult(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), dxgiAdapter.put_void()));

	com_ptr<IDXGIOutput> dxgiOutput;
	check_hresult(dxgiAdapter->EnumOutputs(outputIndex, dxgiOutput.put()));

	com_ptr<IDXGIOutput1> dxgiOutput1 = dxgiOutput.as<IDXGIOutput1>();

	dxgiOutputDupl = nullptr;
	check_hresult(dxgiOutput1->DuplicateOutput(device.get(), dxgiOutputDupl.put()));

	DXGI_OUTDUPL_DESC outDuplDesc;
	ZeroMemory(&outDuplDesc, sizeof(outDuplDesc));
	dxgiOutputDupl->GetDesc(&outDuplDesc);

	outputSize = MAKELONG((long)outDuplDesc.ModeDesc.Width, (long)outDuplDesc.ModeDesc.Height);
	fps = outDuplDesc.ModeDesc.RefreshRate.Numerator / outDuplDesc.ModeDesc.RefreshRate.Denominator;
	rotation = outDuplDesc.Rotation;
}

void DesktopCapturer::SetViewPort(int w, int h)
{
	D3D11_VIEWPORT VP;
	VP.Width = static_cast<FLOAT>(w);
	VP.Height = static_cast<FLOAT>(h);
	VP.MinDepth = 0.0f;
	VP.MaxDepth = 1.0f;
	VP.TopLeftX = 0.0f;
	VP.TopLeftY = 0.0f;
	context->RSSetViewports(1, &VP);
}

bool DesktopCapturer::CheckDupl(HRESULT hr)
{
	if (FAILED(hr))
	{
		MakeDuplication();
		return false;
	}

	return true;
}

HRESULT DesktopCapturer::CopyFrame(winrt::com_ptr<ID3D11Texture2D>& tex, const RECT& targetRect, const SIZE& scene)
{
	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);

	winrt::com_ptr<ID3D11ShaderResourceView> srv;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = desc.MipLevels - 1;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;

	auto hr = device->CreateShaderResourceView(tex.get(), &srvDesc, srv.put());
	if (FAILED(hr))
		return hr;

	VERTEX vertices[6];
	D3D11Helper::RectToVertex(targetRect, scene, vertices);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = context->Map(vertexBuffer.get(), 0, D3D11_MAP_WRITE, 0, &mappedResource);
	if (FAILED(hr))
		return hr;

	CopyMemory(mappedResource.pData, vertices, sizeof(vertices));

	context->Unmap(vertexBuffer.get(), 0);

	SetViewPort(scene.cx, scene.cy);

	auto srvs = srv.get();
	auto samplers = sampler.get();
	FLOAT blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	auto vertexBuffers = vertexBuffer.get();

	context->PSSetShaderResources(0, 1, &srvs);
	context->OMSetBlendState(nullptr, blendFactor, 0xFFFFFFFF);
	context->VSSetShader(vshader.get(), nullptr, 0);
	context->VSSetSamplers(0, 1, &samplers);
	context->PSSetShader(pshader.get(), nullptr, 0);
	context->IASetInputLayout(inputLayout.get());
	context->IASetVertexBuffers(0, 1, &vertexBuffers, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->Draw(6, 0);

	return S_OK;
}
