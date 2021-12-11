#pragma once
#include <d3d11.h>

struct __declspec(dllexport) ISprite
{
public:
	virtual void LoadContext(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
	virtual SIZE Size() = 0;
	virtual int FPS() = 0;
	virtual void Paint(ID3D11Texture2D** targets, int numTarget, const RECT& targetRect, const SIZE& sceneSize) = 0;
	~ISprite() = default;
};