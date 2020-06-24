#include "CharacterController.h"
#include <time.h>

CharacterController::CharacterController(ID3D11DeviceContext* context, Terrain* terrain, PhysicsEngine* physicsEngine, XMFLOAT3* position)
{
	mMoveSpeed = 100.0f;
	mTurnSpeed = 200.0f;

	mDeviceContext = context;
	mPhysicsEngine = physicsEngine;
	mTerrain = terrain;

	//not jumping atm
	mIsAirborne = false;
	mJumpVelocity = 50.0f;

	mTerrain = terrain;

	if (position)
	{
		mPlayerCollisionBox.init(*position, XMLoadFloat3(&mCam.GetRightVector()), 10, 10);
		mCam.SetPosition(XMFLOAT3(position->x, position->y + 2, position->z));
	}
	else //get a random position in the world
	{

		XMFLOAT2 lowerLeft, upperRight;
		mTerrain->GetBoundingRect(lowerLeft, upperRight);

		float playerPosX;
		float playerPosZ;

		srand((UINT)time(NULL));

		if (lowerLeft.x > 0 || upperRight.x > 0)
		{
			playerPosX = (float)(rand() % int(abs(lowerLeft.x) - 1 + abs(upperRight.x) - 1 + 0.5f)) - int(abs(lowerLeft.x) - 1 + 0.5f);
			playerPosZ = (float)(rand() % int(abs(lowerLeft.y) - 1 + abs(upperRight.y) - 1 + 0.5f)) - int(abs(lowerLeft.y) - 1 + 0.5f);
		}
		else if (lowerLeft.x < 0 && upperRight.x < 0)
		{
			playerPosX = (float)(rand() % int(abs(lowerLeft.x) - 1 + (upperRight.x - 1) + 0.5f)) - int(abs(lowerLeft.x) - 1 + 0.5f);
			playerPosZ = (float)(rand() % int(abs(lowerLeft.y) - 1 + (upperRight.y - 1) + 0.5f)) - int(abs(lowerLeft.y) - 1 + 0.5f);
		}


		XMFLOAT3 playerPosf = { playerPosX, 0.0f, playerPosZ };

		float playerPosY;


		XMFLOAT3 finalPosition;
		finalPosition.x = playerPosX;
		finalPosition.y = playerPosZ;
		finalPosition.z = 0.0f;

		mTerrain->GetHeightAtPosition(finalPosition, playerPosY);


		mPlayerCollisionBox.init(XMFLOAT3(playerPosX, playerPosY, playerPosZ), XMLoadFloat3(&mCam.GetRightVector()), 10, 10);
		mCam.SetPosition(XMFLOAT3(playerPosX, playerPosY + 2, playerPosZ));
	}


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
	XMVECTOR boxPosV = mPlayerCollisionBox.GetCenter();
	XMVECTOR boxPosAfterV = boxPosV + direction * mMoveSpeed;

	//check collisions		
	if (mPhysicsEngine->CheckForCollision(mPlayerCollisionBox, boxPosAfterV) == true)
		return;

	XMFLOAT3 boxPosAfterF;
	XMStoreFloat3(&boxPosAfterF, boxPosAfterV);

	float playerPosY;
	mTerrain->GetHeightAtPosition(boxPosAfterF, playerPosY);

	if (playerPosY < 0)
		int x = 0;

	XMVECTOR finalMoveVector = { boxPosAfterF.x, playerPosY, boxPosAfterF.z, 0.0f };
	finalMoveVector = finalMoveVector - boxPosV;


	XMVECTOR camPosFinalV = XMLoadFloat3(&mCam.GetPosition()) + finalMoveVector;
	XMVECTOR boxPosFinalV = mPlayerCollisionBox.GetCenter() + finalMoveVector;
	
	XMFLOAT3 camPosFinalF;
	XMFLOAT3 boxPosFinalF;

	XMStoreFloat3(&camPosFinalF, camPosFinalV);
	XMStoreFloat3(&boxPosFinalF, boxPosFinalV);

	if (camPosFinalF.y < 400)
		int x = 0;

		
	if (mIsAirborne)
	{
		//only set the x and z coordinates and leave the y alone
		XMVECTOR boxPosition = mPlayerCollisionBox.GetCenter();
		XMFLOAT3 boxPositionF;
		XMStoreFloat3(&boxPositionF, boxPosition);

		XMFLOAT3 camPosFinalNoYF = { camPosFinalF.x, mCam.GetPosition().y, camPosFinalF.z };
		XMFLOAT3 boxPosFinalNoYF = { boxPosFinalF.x, boxPositionF.y, boxPosFinalF.z };

		mCam.SetPosition(camPosFinalNoYF);
		mPlayerCollisionBox.SetCenter(boxPosFinalNoYF);
	}
	else
	{
		mCam.SetPosition(camPosFinalF);
		mPlayerCollisionBox.SetCenter(boxPosFinalF);
	}

	

}

void CharacterController::Jump()
{
	//start timer and go through the curve
	if (!mIsAirborne)
	{
		mJumpTime = 0.0f;
		mJumpStartPos = mPlayerCollisionBox.GetCenter();
		mIsAirborne = true;
	}
}

void CharacterController::SetTerrain(Terrain * terrain)
{

	
}

void CharacterController::SetMoveSpeed(float speed)
{
	mMoveSpeed = speed;
}

void CharacterController::SetTurnSpeed(float anglesPerSecond)
{
	mTurnSpeed = anglesPerSecond;
}

XMVECTOR CharacterController::mGetRandomPositionOnTerrain()
{
	return XMVECTOR();
}

void CharacterController::Update(float dt)
{

	static DWORD timeDeltaUp = 2;
	static DWORD timeDeltaDown = 2;
	static DWORD timeDeltaLeft = 2;
	static DWORD timeDeltaRight = 2;


	XMVECTOR moveDirLook = XMVectorSet(mCam.GetLookAtVector().x, 0, mCam.GetLookAtVector().z, 0);
	XMVECTOR moveDirRight = XMVectorSet(mCam.GetRightVector().x, 0, mCam.GetRightVector().z, 0);

	
	if (GetAsyncKeyState(VK_SPACE))
		Jump();


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
	

	//jump
	if (mIsAirborne)
	{
		//increment jump timer
		mJumpTime += dt;

		//set velocity vector
		XMFLOAT3 velF = { 0.0f, mJumpVelocity, 0.0f };
		XMVECTOR vel = XMLoadFloat3(&velF);

		//set gravity vector
		XMFLOAT3 gravF = { 0.0f, -9.8f, 0.0f };
		XMVECTOR grav = XMLoadFloat3(&gravF);

		//projectile motion equation 
		//rf = ri + vi*t = 1/2*g*t^2
		float t = mJumpTime*4;
		XMVECTOR posAfterJump = mJumpStartPos + vel * t + 0.5*grav*t*t;
		 

		XMFLOAT3 posAfterJumpF;
		XMStoreFloat3(&posAfterJumpF, posAfterJump);



		//every frame check to make sure that we're not hitting land
		float currentTerrainHeight;
		XMVECTOR currentPosition = mPlayerCollisionBox.GetCenter();
		XMFLOAT3 currentPositionF;
		XMStoreFloat3(&currentPositionF, currentPosition);
		mTerrain->GetHeightAtPosition(currentPositionF, currentTerrainHeight);

		if (currentTerrainHeight >= posAfterJumpF.y)
		{
			mIsAirborne = false;
			return;
		}

		else
		{
			

			//find the difference between current box position and position after the jump and offset the camera by that difference
			XMVECTOR boxPos = mPlayerCollisionBox.GetCenter();
			XMVECTOR posDiff = posAfterJump - boxPos;
			XMFLOAT3 boxPosF;
			XMStoreFloat3(&boxPosF, boxPos);
			XMFLOAT3 camPosF = mCam.GetPosition();
			XMVECTOR camPos = XMLoadFloat3(&camPosF);
			XMVECTOR newCamPos = camPos + posDiff;
			XMFLOAT3 newCamPosF;
			XMStoreFloat3(&newCamPosF, newCamPos);

			XMFLOAT3 finalBoxPos = { boxPosF.x, posAfterJumpF.y, boxPosF.z };

			mPlayerCollisionBox.SetCenter(finalBoxPos);
			mCam.SetPosition(XMFLOAT3(mCam.GetPosition().x, newCamPosF.y, mCam.GetPosition().z));
		}
	

	}


}


