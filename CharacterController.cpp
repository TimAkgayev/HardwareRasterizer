#include "CharacterController.h"
#include <time.h>

CharacterController::CharacterController(Terrain* terrain)
{
	mMoveSpeed = 100.0f;
	mTurnSpeed = 200.0f;

	pAssociatedTerrain = terrain;

	XMFLOAT2 lowerLeft, upperRight;
    pAssociatedTerrain->GetBoundingCoordinates(&lowerLeft, &upperRight);
	
	float playerPosX; 
	float playerPosZ;
	
	srand(time(NULL));

	if (lowerLeft.x > 0 || upperRight.x > 0)
	{
		playerPosX = (rand() % int( abs(lowerLeft.x) - 1 + abs(upperRight.x) - 1 + 0.5f)) - int(abs(lowerLeft.x) - 1 + 0.5f);
		playerPosZ = (rand() % int(abs(lowerLeft.y) - 1 + abs(upperRight.y) - 1 + 0.5f)) - int(abs(lowerLeft.y)  - 1 + 0.5f);
	}
	else if (lowerLeft.x < 0 && upperRight.x < 0)
	{
		playerPosX = (rand() % int(abs(lowerLeft.x) - 1 + (upperRight.x - 1) + 0.5f)) - int(abs(lowerLeft.x) - 1 + 0.5f);
		playerPosZ = (rand() % int(abs(lowerLeft.y) - 1 + (upperRight.y - 1) + 0.5f)) - int(abs(lowerLeft.y) - 1 + 0.5f);
	}

	

	XMFLOAT3 playerPosf = { playerPosX, 0.0f, playerPosZ };
	
	float* playerPosY = new float;
	pAssociatedTerrain->GetHeightAtPosition(playerPosf, &playerPosY);

	XMVECTOR finalPosition = { playerPosX, *playerPosY, playerPosZ, 0.0f };
	
	mCollisionBox.init(XMFLOAT3(playerPosX, *playerPosY, playerPosZ), XMLoadFloat3(&mCam.GetRightVector()), 10, 10);
	mCam.SetPosition(XMFLOAT3(playerPosX, *playerPosY + 400, playerPosZ));

	delete playerPosY;
	playerPosY = nullptr;

}

CharacterController::~CharacterController()
{
	
}

Camera& CharacterController::GetCamera()
{
	return mCam;
}

void CharacterController::Move(XMVECTOR direction)
{
	
    
	//check height at that point and update
	XMVECTOR boxPosV = mCollisionBox.GetCenter();
	XMVECTOR boxPosAfterV = boxPosV + direction * mMoveSpeed;
	
	//check collisions		
	if (mCollisionBox.checkForCollision(boxPosAfterV))
		return;

	XMFLOAT3 boxPosAfterF;
	XMStoreFloat3(&boxPosAfterF, boxPosAfterV);

	float* playerPosY = new float;
	pAssociatedTerrain->GetHeightAtPosition(boxPosAfterF, &playerPosY);

	XMVECTOR finalMoveVector = { boxPosAfterF.x, *playerPosY, boxPosAfterF.z, 0.0f };
	finalMoveVector = finalMoveVector - boxPosV;

	delete playerPosY;
	playerPosY = nullptr;

	XMVECTOR camPosFinalV = XMLoadFloat3(&mCam.GetPosition()) + finalMoveVector;
	XMVECTOR boxPosFinalV = mCollisionBox.GetCenter() + finalMoveVector;
	
	XMFLOAT3 camPosFinalF;
	XMFLOAT3 boxPosFinalF;

	XMStoreFloat3(&camPosFinalF, camPosFinalV);
	XMStoreFloat3(&boxPosFinalF, boxPosFinalV);

	mCam.SetPosition(camPosFinalF);
	mCollisionBox.SetPosition(boxPosFinalF);

	

}

void CharacterController::SetMoveSpeed(float speed)
{
	mMoveSpeed = speed;
}

void CharacterController::SetTurnSpeed(float anglesPerSecond)
{
	mTurnSpeed = anglesPerSecond;
}

void CharacterController::Update(float dt)
{

	static DWORD timeDeltaUp = 2;
	static DWORD timeDeltaDown = 2;
	static DWORD timeDeltaLeft = 2;
	static DWORD timeDeltaRight = 2;


	XMVECTOR moveDirLook = XMVectorSet(mCam.GetLookAtVector().x, 0, mCam.GetLookAtVector().z, 0);
	XMVECTOR moveDirRight = XMVectorSet(mCam.GetRightVector().x, 0, mCam.GetRightVector().z, 0);

	


	if (GetAsyncKeyState(0x57)) //W key
		Move(dt*moveDirLook * -1*mMoveSpeed);

	if (GetAsyncKeyState(0x53)) //S key
		Move(dt* moveDirLook*mMoveSpeed);


	if (GetAsyncKeyState(0x41)) //A key
		Move(dt*moveDirRight * -1 * mMoveSpeed);


	if (GetAsyncKeyState(0x44)) //D key
		Move(dt*moveDirRight*mMoveSpeed);



	if (GetAsyncKeyState(VK_UP))
	{
	
		mCam.Pitch(-1*dt*mTurnSpeed);
	}

	if (GetAsyncKeyState(VK_DOWN))
	{

		mCam.Pitch(1 * dt*mTurnSpeed);
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
	
		mCam.Yaw(-1 * dt*mTurnSpeed);
	}

	if (GetAsyncKeyState(VK_RIGHT))
	{
		
		mCam.Yaw(1 * dt*mTurnSpeed);
	}
	

}


