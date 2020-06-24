#pragma once
#include "Camera.h"
#include <DirectXMath.h>
#include "PhysicsEngine.h"
#include "Terrain.h"
#include "GameTimer.h"

using namespace DirectX;

__declspec(align(16)) class CharacterController
{
public:
	CharacterController(ID3D11DeviceContext* context, Terrain* terrain, PhysicsEngine* physicsEngine, XMFLOAT3* position = nullptr);
	~CharacterController();

	Camera& GetCamera();

	void Update(float dt);

	void Move(XMVECTOR direction);
	void Jump();

	void SetTerrain(Terrain* terrain);

	XMFLOAT3 GetPosition()
	{
		XMFLOAT3 centerF;
		XMStoreFloat3(&centerF, mPlayerCollisionBox.GetCenter());
		return centerF;
	}

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	

	void SetMoveSpeed(float speed);
	void SetTurnSpeed(float anglesPerSecond);

protected:

	Terrain* mTerrain;
	PhysicsEngine* mPhysicsEngine;

	Camera mCam;
	CollisionBox mPlayerCollisionBox;


	ID3D11DeviceContext* mDeviceContext;

	float mMoveSpeed;
	float mTurnSpeed;

	// Jump values
	bool mIsAirborne;
	float mJumpVelocity; 
	float mJumpTime;
	XMVECTOR mJumpStartPos;

protected:

	XMVECTOR mGetRandomPositionOnTerrain();
};