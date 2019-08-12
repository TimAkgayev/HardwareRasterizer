#pragma once
#include <DirectXMath.h>
#include "ConstantBuffers.h"

using namespace DirectX;

class Light
{
public:
	Light(ID3D11DeviceContext* context, XMFLOAT3 pos, XMFLOAT3 at, XMFLOAT4 color);
	~Light();

	void SetPosition(XMFLOAT3 pos);
	void SetLookAt(XMFLOAT3 lookat);
	void SetColor(XMFLOAT4 color);

	XMVECTOR GetPosition();
	
private:

	ID3D11DeviceContext* mDeviceContext;
	void mUpdateBuffer();

	XMFLOAT3 mPosition;
	XMFLOAT3 mLookAt;
	XMFLOAT4 mColor;
};