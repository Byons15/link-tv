#include "Canvas.h"
#include <dxgi.h>
#include <dxgi1_2.h>
#include "../D3D11Helper.h"
#include "../CanvasPixelShader.h"
#include "../VertexShader.h"
#include <libavutil/frame.h>
#include "YUVOutput.h"

using namespace winrt;
using namespace DirectX;

Canvas::Canvas(uint32_t w, uint32_t h)
	:size{(LONG)w, (LONG)h},
	viewTexSwapChain(w, h),
	outputSwapChain(w, h)
{
	D3D11Helper::CreateDevice(device, context);
	viewTexSwapChain.SetBackDevice(device.get(), context.get());
	outputSwapChain.SetBackDevice(device.get(), context.get());

	worker.Lanuch(&Canvas::Run, this);
}

void Canvas::AddSprite(const std::shared_ptr<ISprite>& sprite)
{
	worker.Invoke([this, sprite]() 
		{
			Sprite s;
			s.sprite = sprite;
			sprite->LoadContext(device.get(), context.get());
			auto size = sprite->Size();
			s.rect = RECT{0, 0, size.cx, size.cy};
			sprites.push_back(s);
		}
	)->get_future().get();
}

void Canvas::RemoveSprite(const std::shared_ptr<ISprite>& sprite)
{
	worker.Invoke([this, sprite]() 
		{
			auto target = sprites.begin();
			for (auto iter = sprites.begin(); iter != sprites.end(); ++iter)
			{
				if (iter->sprite == sprite)
				{
					target = iter;
					break;
				}
			}

			if (target != sprites.end())
				sprites.erase(target);
		}
	)->get_future().get();
}

Canvas::~Canvas()
{
	if (worker.IsRuning())
	{
		worker.RequestInterrupt();
		worker.Wait();
	}
}

void Canvas::StartOutput(SIZE size, uint32_t fps, const std::shared_ptr<IEncoder>& encoder)
{
	worker.Invoke([this, &size, fps, &encoder]() 
		{
			switch (encoder->VideoFrameFormat())
			{
			case AV_PIX_FMT_YUV420P:
				output.reset(new YUVOutput());
				break;

			case AV_PIX_FMT_D3D11:
			default:
				throw std::runtime_error("Unsupported format");
				break;
			}

			output->Start(&outputSwapChain, size.cx, size.cy, fps, encoder);

		})->get_future().get();
}

void Canvas::StopOutput()
{
	worker.Invoke([this]() 
		{
			output = nullptr;
		})->get_future().get();
}

ITextureSwapChain* Canvas::ViewTextureSwapChain()
{
	return &viewTexSwapChain;
}

SIZE Canvas::Size()
{
	return size;
}

void Canvas::Run()
{
	while (!worker.IsRequestInterrupt())
	{
		worker.ProcessAccumulateDelegate();

		ID3D11Texture2D* targets[2] = { viewTexSwapChain.BeginUseBack(), outputSwapChain.BeginUseBack() };
		ID3D11RenderTargetView* rtvs[2];
		check_hresult(device->CreateRenderTargetView(targets[0], nullptr, rtvs));
		check_hresult(device->CreateRenderTargetView(targets[1], nullptr, &(rtvs[1])));
		context->OMSetRenderTargets(2, rtvs, nullptr);

		for (auto i = 0; i < sprites.size(); ++i)
		{
			auto& sprite = sprites[i];
			
			sprite.sprite->Paint(targets, 2, sprite.rect, size);
		}

		outputSwapChain.EndUseBack();
		viewTexSwapChain.EndUseBack();

		Sleep(10);
	}
}

void Canvas::SetGraphicsVertex(const RECT& rect, VERTEX* vertices)
{
	float centerX = size.cx / 2.0f;
	float centerY = size.cy / 2.0f;

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