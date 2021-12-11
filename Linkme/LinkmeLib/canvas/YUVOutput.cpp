#include "YUVOutput.h"
#include "../D3D11Helper.h"
#include "../VertexShader.h"
#include "../YUVPixelShader.h"
#include "../IEncoder.h"
#include "../ITextureSwapChain.h"

using winrt::check_hresult;
using namespace  DirectX;

void YUVOutput::Start(ITextureSwapChain* swapChain, int w, int h, int fps, const std::shared_ptr<IEncoder>& encoder)
{
	D3D11Helper::CreateDevice(device, context);
	sampler = D3D11Helper::CreateSampler(device);
	check_hresult(device->CreateVertexShader(g_VS, ARRAYSIZE(g_VS), 0, vshader.put()));
	check_hresult(device->CreatePixelShader(g_YUVPSMain, ARRAYSIZE(g_YUVPSMain), 0, pshader.put()));
	inputLayout = D3D11Helper::CreateInputLayout(device, g_VS, ARRAYSIZE(g_VS));

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX) * 6;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&subData, sizeof(subData));
	VERTEX vertices[] =
	{
		{XMFLOAT3(-1.0f, -1.0f, 0), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f)},
		{XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f)},
		{XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(1.0f, 1.0f, 0), XMFLOAT2(1.0f, 0.0f)},
	};
	subData.pSysMem = vertices;
	check_hresult(device->CreateBuffer(&vertexBufferDesc, &subData, vertexBuffer.put()));

	this->fps = fps;
	this->encoder = encoder;
	this->size = SIZE{ w, h };
	this->swapChain = swapChain;

	swapChain->SetDevice(device.get(), context.get());
	D3D11_TEXTURE2D_DESC targetTexDesc = swapChain->Desc();
	targetTexDesc.CPUAccessFlags = 0;
	targetTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	targetTexDesc.Usage = D3D11_USAGE_DEFAULT;
	targetTexDesc.MiscFlags = 0;
	check_hresult(device->CreateTexture2D(&targetTexDesc, nullptr, targetTex.put()));
	check_hresult(device->CreateRenderTargetView(targetTex.get(), nullptr, rtv.put()));

	D3D11_BUFFER_DESC yuvBufferDesc;
	ZeroMemory(&yuvBufferDesc, sizeof(yuvBufferDesc));
	yuvBufferDesc.CPUAccessFlags = 0;
	yuvBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	yuvBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	yuvBufferDesc.MiscFlags = 0;
	yuvBufferDesc.ByteWidth = w * h + w * h / 2;
	check_hresult(device->CreateBuffer(&yuvBufferDesc, nullptr, yuvBuffer.put()));

	D3D11_BUFFER_DESC& yuvTempBufferDesc = yuvBufferDesc;
	yuvTempBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	yuvTempBufferDesc.Usage = D3D11_USAGE_STAGING;
	yuvTempBufferDesc.MiscFlags = 0;
	yuvTempBufferDesc.BindFlags = 0;
	check_hresult(device->CreateBuffer(&yuvTempBufferDesc, nullptr, yuvTempBuffer.put()));

	D3D11_VIEWPORT VP;
	VP.Width = static_cast<FLOAT>(w);
	VP.Height = static_cast<FLOAT>(h);
	VP.MinDepth = 0.0f;
	VP.MaxDepth = 1.0f;
	VP.TopLeftX = 0.0f;
	VP.TopLeftY = 0.0f;
	context->RSSetViewports(1, &VP);

	worker.Lanuch(&YUVOutput::Run, this);
}

YUVOutput::~YUVOutput()
{
	if (worker.IsRuning())
	{
		worker.RequestInterrupt();
		worker.Wait();
	}

	swapChain->UnloadDevice();
}

void YUVOutput::Run()
{
	TIMECAPS tc;
	ZeroMemory(&tc, sizeof(tc));
	check_hresult(timeGetDevCaps(&tc, sizeof(tc)));
	check_hresult(timeBeginPeriod(tc.wPeriodMin));

	HRESULT hr = S_OK;

	auto startTimestamp = timeGetTime();
	uint64_t curPts = 0;
	uint64_t pts = 0;
	double frameInterval = 1000.0 / fps;

	try
	{
		while (!worker.IsRequestInterrupt())
		{
			worker.ProcessAccumulateDelegate();
			
			auto tex = swapChain->BeginUse();

			// 计算当前帧的位置
			curPts = (uint64_t)((timeGetTime() - startTimestamp) / frameInterval);
			if (curPts <= pts)
			{
				swapChain->EndUse();
				continue;
			}
			pts = curPts;

			// 转码
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			winrt::com_ptr< ID3D11ShaderResourceView> srv;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = swapChain->Desc().Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = swapChain->Desc().MipLevels - 1;
			srvDesc.Texture2D.MipLevels = swapChain->Desc().MipLevels;
			check_hresult(device->CreateShaderResourceView(tex, &srvDesc, srv.put()));
			auto srvs = srv.get();
			context->PSSetShaderResources(0, 1, &srvs);

			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			ZeroMemory(&uavDesc, sizeof(uavDesc));
			uavDesc.Format = DXGI_FORMAT_R8_UINT;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.NumElements = size.cx * size.cy + size.cx * size.cy / 2;
			uavDesc.Buffer.Flags = 0;
			winrt::com_ptr<ID3D11UnorderedAccessView> uav;
			check_hresult(device->CreateUnorderedAccessView(yuvBuffer.get(), &uavDesc, uav.put()));

			FLOAT blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
			auto rtvs = rtv.get();
			auto uavs = uav.get();
			context->OMSetRenderTargetsAndUnorderedAccessViews(1, &rtvs, 0, 1, 1, &uavs, NULL);
			context->OMSetBlendState(nullptr, blendFactor, 0xFFFFFFFF);
			context->VSSetShader(vshader.get(), nullptr, 0);
			auto samplers = sampler.get();
			context->VSSetSamplers(0, 1, &samplers);
			context->PSSetShader(pshader.get(), nullptr, 0);
			context->IASetInputLayout(inputLayout.get());
			UINT stride = sizeof(VERTEX);
			UINT offset = 0;
			auto vertexBuffers = vertexBuffer.get();
			context->IASetVertexBuffers(0, 1, &vertexBuffers, &stride, &offset);
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->Draw(6, 0);

			swapChain->EndUse();

			// 取出YUV数据并发送到编码器
			context->CopyResource(yuvTempBuffer.get(), yuvBuffer.get());
			D3D11_MAPPED_SUBRESOURCE dms;
			ZeroMemory(&dms, sizeof(dms));
			check_hresult(context->Map(yuvTempBuffer.get(), 0, D3D11_MAP_READ, 0, &dms));

			encoder->WriteVideoFrame((const uint8_t*)dms.pData, pts);

			context->Unmap(yuvTempBuffer.get(), 0);
		}
	}
	catch (...)
	{
		timeEndPeriod(tc.wPeriodMin);
		std::rethrow_exception(std::current_exception());
	}

	timeEndPeriod(tc.wPeriodMin);
}
