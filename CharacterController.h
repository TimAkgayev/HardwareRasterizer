#pragma once
#include "Camera.h"
#include <DirectXMath.h>
#include "CollisionBox.h"
#include "Terrain.h"

using namespace DirectX;

__declspec(align(16)) class CharacterController
{
public:
	CharacterController(Terrain* terrain);
	~CharacterController();

	Camera& GetCamera();

	void Update(float dt);

	void Move(XMVECTOR direction);

	XMFLOAT3 GetPosition()
	{
		XMFLOAT3 centerF;
		XMStoreFloat3(&centerF, mCollisionBox.GetCenter());
		return centerF;
	}

	void SetMoveSpeed(float speed);
	void SetTurnSpeed(float anglesPerSecond);

protected:

	CollisionBox mCollisionBox;
	Camera mCam;

	Terrain* pAssociatedTerrain;
	float mMoveSpeed;
	float mTurnSpeed;
};