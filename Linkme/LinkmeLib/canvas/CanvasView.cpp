#include "CanvasView.h"
#include "../D3D11Helper.h"
#include "../VertexShader.h"
#include "../PixelShader.h"

using namespace DirectX;

using winrt::check_hresult;
using winrt::check_bool;

CanvasView::CanvasView(ITextureSwapChain* swapChain)
{
	windowWorker.Lanuch(&CanvasView::WindowWorker, this, swapChain);
}

CanvasView::CanvasView(HWND hwnd, ITextureSwapChain* texSwapChain)
{
	SetOutputWindow(hwnd);
	SetSwapChain(texSwapChain);
}

void CanvasView::D3DPaint()
{
	if (texSwapChain == nullptr)
		return;

	ID3D11Texture2D* tex = texSwapChain->BeginUse();
	winrt::com_ptr<ID3D11ShaderResourceView> srv;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = texSwapChain->Desc().Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = texSwapChain->Desc().MipLevels - 1;
	srvDesc.Texture2D.MipLevels = texSwapChain->Desc().MipLevels;
	auto hr = device->CreateShaderResourceView(tex, nullptr, srv.put());
	if (FAILED(hr))
	{
		texSwapChain->EndUse();
		winrt::throw_hresult(hr);
	}

	auto rtvs = rtv.get();
	auto srvs = srv.get();
	auto samplers = sampler.get();
	auto vertexBuffers = vertexBuffer.get();

	UINT Stride = sizeof(VERTEX);
	UINT Offset = 0;
	FLOAT blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	context->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
	context->OMSetRenderTargets(1, &rtvs, nullptr);
	context->VSSetShader(vshader.get(), nullptr, 0);
	context->PSSetShader(pshader.get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, &srvs);
	context->PSSetSamplers(0, 1, &samplers);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetVertexBuffers(0, 1, &vertexBuffers, &Stride, &Offset);
	context->IASetInputLayout(inputLayout.get());

	context->Draw(6, 0);

	srv = nullptr;
	texSwapChain->EndUse();

	swapChain->Present(0, 0);
}

void CanvasView::Resize(uint32_t w, uint32_t h)
{
	if (windowWorker.IsRuning())
		PostMessage(outputWindow, WM_SIZE, 0, MAKELPARAM(w, h));
	else
		ResizeSwapChain(w, h);
}

void CanvasView::Paint()
{
	if (!windowWorker.IsRuning())
		D3DPaint();
}

void CanvasView::ResizeSwapChain(uint32_t w, uint32_t h)
{
	rtv = nullptr;
	DXGI_SWAP_CHAIN_DESC desc;
	swapChain->GetDesc(&desc);
	check_hresult(swapChain->ResizeBuffers(desc.BufferCount, w, h, desc.BufferDesc.Format, desc.Flags));
	MakeRTV();
	SetViewPort(w, h);
}

void CanvasView::SetOutputWindow(HWND hwnd)
{
	swapChain = nullptr;
	device = nullptr;
	context = nullptr;
	vshader = nullptr;
	pshader = nullptr;
	sampler = nullptr;
	inputLayout = nullptr;
	vertexBuffer = nullptr;

	RECT rect;
	GetClientRect(hwnd, &rect);

	// 创建交换链、上下文、设备
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = rect.right - rect.left;
	swapChainDesc.BufferDesc.Height = rect.bottom - rect.top;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	unsigned flags = 0;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = S_OK;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE, D3D_DRIVER_TYPE_SOFTWARE
	};

	unsigned totalDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	unsigned int totalFeatureLevels = ARRAYSIZE(featureLevels);

	for (int i = 0; i < totalDriverTypes; ++i)
	{
		hr = D3D11CreateDeviceAndSwapChain(nullptr, driverTypes[i],
			nullptr, flags, featureLevels, totalFeatureLevels, D3D11_SDK_VERSION,
			&swapChainDesc, swapChain.put(), device.put(), nullptr,
			context.put());

		if (SUCCEEDED(hr))
			break;
	}

	check_hresult(hr);

	check_hresult(device->CreateVertexShader(g_VS, ARRAYSIZE(g_VS), nullptr, vshader.put()));
	check_hresult(device->CreatePixelShader(g_PS, ARRAYSIZE(g_PS), nullptr, pshader.put()));
	sampler = D3D11Helper::CreateSampler(device);
	inputLayout = D3D11Helper::CreateInputLayout(device, g_VS, ARRAYSIZE(g_VS));

	VERTEX vertices[] =
	{
		{XMFLOAT3(-1.0f, -1.0f, 0), XMFLOAT2(0.0f, 1.0f)},
		{XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f)},
		{XMFLOAT3(1.0f, -1.0f, 0), XMFLOAT2(1.0f, 1.0f)},
		{XMFLOAT3(-1.0f, 1.0f, 0), XMFLOAT2(0.0f, 0.0f)},
		{XMFLOAT3(1.0f, 1.0f, 0), XMFLOAT2(1.0f, 0.0f)},
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VERTEX) * ARRAYSIZE(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	RtlZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	check_hresult(device->CreateBuffer(&vertexBufferDesc, &initData, vertexBuffer.put()));

	MakeRTV();
	SetViewPort(rect.right - rect.left, rect.bottom - rect.top);

	outputWindow = hwnd;
}

void CanvasView::SetSwapChain(ITextureSwapChain* swapChain)
{
	texSwapChain = swapChain;
	texSwapChain->SetDevice(device.get(), context.get());
}

CanvasView::~CanvasView()
{
	if (windowWorker.IsRuning())
	{
		windowWorker.RequestInterrupt();
		windowWorker.Wait();
	}

	texSwapChain->UnloadDevice();
}

void CanvasView::MakeRTV()
{
	winrt::com_ptr<ID3D11Texture2D> backBuffer;
	check_hresult(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), backBuffer.put_void()));

	check_hresult(device->CreateRenderTargetView(backBuffer.get(), nullptr, rtv.put()));

	auto rtvs = rtv.get();
	context->OMSetRenderTargets(1, &rtvs, nullptr);
}

void CanvasView::SetViewPort(uint32_t w, uint32_t h)
{
	D3D11_VIEWPORT viewPort;
	viewPort.Width = static_cast<FLOAT>(w);
	viewPort.Height = static_cast<FLOAT>(h);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	context->RSSetViewports(1, &viewPort);
}

void CanvasView::WindowWorker(ITextureSwapChain* texSwapChain)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProcJump;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hIcon = 0;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOWTEXT);
	wc.lpszMenuName = NULL;
	wc.hIconSm = 0;
	wc.lpszClassName = L"LinkmeD3DWindow";

	HWND hwnd = NULL;
	HRESULT hr = S_OK;

	check_bool(RegisterClassEx(&wc));

	hwnd = CreateWindow(L"LinkmeD3DWindow", NULL, WS_OVERLAPPEDWINDOW,
		0, 0, 800, 600, NULL, NULL, GetModuleHandle(nullptr), this);

	if (!hwnd)
		winrt::throw_last_error();

	check_hresult(SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this));

	SetOutputWindow(hwnd);
	SetSwapChain(texSwapChain);

	ShowWindow(hwnd, SW_SHOW);

	MSG msg;
	while (!windowWorker.IsRequestInterrupt())
	{
		windowWorker.ProcessAccumulateDelegate();

		if (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				break;
		}

		D3DPaint();
	}
}

LRESULT __stdcall CanvasView::WndProcJump(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	if (wMsg == WM_CREATE)
	{
		CREATESTRUCT* param = (CREATESTRUCT*)lParam;
		auto instance = (CanvasView*)param->lpCreateParams;
		return instance->WndProc(hWnd, wMsg, wParam, lParam);
	}

	return ((CanvasView*)(GetWindowLongPtr(hWnd, GWLP_USERDATA)))->WndProc(hWnd, wMsg, wParam, lParam);
}

LRESULT CanvasView::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SIZE:
		ResizeSwapChain(LOWORD(lParam), HIWORD(lParam));
		break;

	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
