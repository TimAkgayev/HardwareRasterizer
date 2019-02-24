#pragma once
#include <vector>
#include <DirectXMath.h>
#include "Interface_LinkSystem.h"
using namespace DirectX;


class CollisionBox : public PositionLink
{
public:
	CollisionBox();
	~CollisionBox();

	CollisionBox(DirectX::XMVECTOR position, float width, float height);

	virtual void SetPosition(XMVECTOR& position) override;
	virtual XMVECTOR& GetPosition() override;

protected:

	XMVECTOR mPosition;
	float mWidth;
	float mHeight;

private:
	static std::vector<CollisionBox*> mCollisionBoxList;
};