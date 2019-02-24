#pragma once
#include "Interface_AutoUpdate.h"
#include "Mesh.h"
#include "Camera.h"
#include <DirectXMath.h>
#include "Physics.h"

using namespace DirectX;

class CharacterController : public AutoUpdate
{
public:
	CharacterController();

	
	Camera& GetCamera();


private:

	virtual void Update() override
	{
		static float t = 0.0f;
		static DWORD dwTimeStart = 0;
		DWORD dwTimeCur = GetTickCount();
		if (dwTimeStart == 0)
			dwTimeStart = dwTimeCur;
		t = (dwTimeCur - dwTimeStart) / 1000.0f;

		static DWORD timeDeltaUp = 2;
		static DWORD timeDeltaDown = 2;
		static DWORD timeDeltaLeft = 2;
		static DWORD timeDeltaRight = 2;


		XMVECTOR moveDirLook = XMVectorSet(mCam.GetLookAtVector().x, 0, mCam.GetLookAtVector().z, 0);
		XMVECTOR moveDirRight = XMVectorSet(mCam.GetRightVector().x, 0, mCam.GetRightVector().z, 0);



		float moveSpeed = 1.0f;
		if (GetAsyncKeyState(0x57)) //W key
			mCollisionBox.SetPosition(mCollisionBox.GetPosition() + moveDirLook * moveSpeed * -1);

		if (GetAsyncKeyState(0x53)) //S key
			mCollisionBox.SetPosition(mCollisionBox.GetPosition() + moveDirLook * moveSpeed);


		if (GetAsyncKeyState(0x41)) //A key
			mCollisionBox.SetPosition(mCollisionBox.GetPosition() + moveDirRight * moveSpeed * -1);


		if (GetAsyncKeyState(0x44)) //D key
			mCollisionBox.SetPosition(mCollisionBox.GetPosition() + moveDirRight * moveSpeed);



		if (GetAsyncKeyState(VK_UP) && ((timeGetTime() - timeDeltaUp) > 10))
		{
			timeDeltaUp = timeGetTime();
			mCam.Pitch(-1);
		}

		if (GetAsyncKeyState(VK_DOWN) && ((timeGetTime() - timeDeltaDown) > 10))
		{
			timeDeltaDown = timeGetTime();
			mCam.Pitch(1);
		}

		if (GetAsyncKeyState(VK_LEFT) && ((timeGetTime() - timeDeltaLeft) > 10))
		{
			timeDeltaLeft = timeGetTime();
			mCam.Yaw(-1);
		}

		if (GetAsyncKeyState(VK_RIGHT) && ((timeGetTime() - timeDeltaRight) > 10))
		{
			timeDeltaRight = timeGetTime();
			mCam.Yaw(1);
		}
	}

	CollisionBox mCollisionBox;
	Mesh mCharacterMesh;
	Camera mCam;
};