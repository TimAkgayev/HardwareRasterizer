#pragma once
#include "WorldObject.h"
#include <DirectXMath.h>

class RasterizerInterface
{
public:
	virtual void DrawWorldObject(WorldObject* object, DirectX::XMMATRIX& wordTransform) = 0;
	virtual void SetViewMatrix(DirectX::XMMATRIX& view) = 0;
};