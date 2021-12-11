#pragma once

#include "NativePtr.h"
#include <LinkmeLib/ICanvas.h>
#include "Encoder.h"
#include <LinkmeLib/ICanvasView.h>

namespace LinkmeLib
{
	public ref class Canvas :public NativePtr<ICanvas>
	{
	public:
		
		Canvas(int w, int h)
			:NativePtr<ICanvas>(CreateCanvas(w, h))
		{
			view = gcnew NativePtr<ICanvasView>(CreateCanvasViewWithPopupWindow(Ptr->ViewTextureSwapChain()));
		}

		Canvas(int w, int h, System::IntPtr viewHandle)
			:NativePtr<ICanvas>(CreateCanvas(w, h))
		{
			view = gcnew NativePtr<ICanvasView>(CreateCanvasView((HWND)viewHandle.ToPointer(), Ptr->ViewTextureSwapChain()));
		}

		property System::IntPtr ViewHandle
		{
			System::IntPtr get()
			{
				return System::IntPtr(view->Ptr->OutputWindow());
			}
		}

		void ViewResize(int w, int h)
		{
			view->Ptr->Resize(w, h);
		}

		void ViewPaint()
		{
			view->Ptr->Paint();
		}

		void AddSprite(NativePtr<ISprite>^ sprite)
		{
			Ptr->AddSprite(sprite->SharedPtr);
		}

		void RemoveSprite(NativePtr<ISprite>^ sprite)
		{
			Ptr->RemoveSprite(sprite->SharedPtr);
		}

		void StartOutput(int w, int h, int fps, Encoder^ encoder)
		{
			Ptr->StartOutput(SIZE{ w, h }, fps, encoder->SharedPtr);
		}

		void StopOutput()
		{
			Ptr->StopOutput();
		}

		~Canvas()
		{
		}

	private:
		NativePtr<ICanvasView>^ view;
	};
}

