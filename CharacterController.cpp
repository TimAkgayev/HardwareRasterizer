#include "CharacterController.h"

CharacterController::CharacterController()
{
	mCharacterMesh.CreateDummyCube(20, 20);
	PositionLink::CreateOffsetPositionLink(&mCollisionBox, &mCam);
	
}

Camera& CharacterController::GetCamera()
{
	return mCam;
}

