#include "Camera.h"


Camera::Camera()
{
	XMFLOAT3 upF, rightF;
	upF = { 0.0f, 1.0f, 0.0f };
	rightF = { 1.0f, 0.0f, 0.0f };

	mUp = XMLoadFloat3(&upF);
	mRight = XMLoadFloat3(&rightF);


	mLookAt = XMVector3Cross(mUp, mRight);
	mLookAt = XMVector3Normalize(mLookAt);

	XMFLOAT3 posF = { 0.0F, 300.0f, 0.0f };
	mPosition = XMLoadFloat3(&posF);

	mProjectionMatrix = XMMatrixPerspectiveFovLH(3.14159265f / 4.0f, 4.0f/3.0f, 1.0f, 1000.0f);

	mIsLocked = false;
}


void Camera::Move(XMFLOAT3 dir)
{
	XMVECTOR tdir = XMLoadFloat3(&dir);
	Move(tdir);
}

void Camera::Move(XMVECTOR dir)
{
	if(!mIsLocked)
		mPosition += dir;
}

void Camera::MoveAlongLookAt(float amount)
{

	XMVECTOR VecToMove = mLookAt * amount;
	Move(VecToMove);
}

void Camera::MoveAlongRight(float amount)
{

	XMVECTOR VecToMove = mRight * amount;
	Move(VecToMove);

}

void Camera::MoveAlongUp(float amount)
{
	auto VecToMove = mUp * amount;
	Move(VecToMove);
}



XMFLOAT3 Camera::GetLookAtVector() const
{
	XMFLOAT3 fLookAt;
	XMStoreFloat3(&fLookAt, mLookAt);
	return fLookAt;
}

XMFLOAT3 Camera::GetRightVector() const
{
	XMFLOAT3 fRight;
	XMStoreFloat3(&fRight, mRight);
	return fRight;
}

XMFLOAT3 Camera::GetUpVector() const
{
	XMFLOAT3 fUp;
	XMStoreFloat3(&fUp, mUp);
	return fUp;
}

void Camera::Roll(float degrees)
{
	if (mIsLocked)
		return;

	XMVECTOR rotQuaternion = XMQuaternionRotationAxis(mLookAt, degrees * XM_PI / 180);

	XMVector3Rotate(mUp, rotQuaternion);
	XMVector3Rotate(mRight, rotQuaternion);

	mUp = XMVector3Normalize(mUp);
	mRight = XMVector3Normalize(mRight);

}

XMMATRIX Camera::Pitch(float degrees)
{

	XMVECTOR rotQuaternion = { 0.0f, 0.0f, 0.0f ,0.0f };

	if (mIsLocked)
		return  XMMatrixRotationQuaternion(rotQuaternion);


	rotQuaternion = XMQuaternionRotationAxis(mRight, degrees * XM_PI / 180);
	mUp = XMVector3Rotate(mUp, rotQuaternion);
	mUp = XMVector3Normalize(mUp);

	mLookAt = XMVector3Cross(mUp, mRight);
	mLookAt = XMVector3Normalize(mLookAt);

	return XMMatrixRotationQuaternion(rotQuaternion);


}

XMMATRIX Camera::Yaw(float degrees)
{

	XMVECTOR rotQuaternion = { 0.0f, 0.0f, 0.0f ,0.0f };

	if (mIsLocked)
		return XMMatrixRotationQuaternion(rotQuaternion);



	XMVECTOR globalUp = { 0.0f, 1.0f, 0.0f, 0.0f };
	rotQuaternion = XMQuaternionRotationAxis(globalUp, degrees * XM_PI / 180);
	
	mRight = XMVector3Rotate(mRight, rotQuaternion);
	mRight = XMVector3Normalize(mRight);

	mUp = XMVector3Rotate(mUp, rotQuaternion);
	mUp = XMVector3Normalize(mUp);

	mLookAt = XMVector3Cross(mUp, mRight);
	mLookAt = XMVector3Normalize(mLookAt);

	return XMMatrixRotationQuaternion(rotQuaternion);

}


XMMATRIX Camera::GetViewTranslationMatrix() const
{
	XMFLOAT4 rowF = { 1.0f, 0.0f, 0.0f, 0.0f };
	XMVECTOR row = XMLoadFloat4(&rowF);


	XMMATRIX matTranslate = XMMatrixIdentity();
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, mPosition);

	rowF = { -camPos.x, -camPos.y, -camPos.z, 1.0f };
	row = XMLoadFloat4(&rowF);
	matTranslate.r[3] = row;

	return matTranslate;
}

XMMATRIX Camera::GetViewRotationMatrix() const
{
	XMFLOAT4 row4F = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMVECTOR row4 = XMLoadFloat4(&row4F);
	XMMATRIX rotationMatrix = XMMatrixLookAtRH(mPosition, mLookAt + mPosition, mUp);
	rotationMatrix.r[3] = row4;
	return rotationMatrix;
}

XMMATRIX Camera::GetViewMatrix() const
{

	return XMMatrixLookAtRH(mPosition, mLookAt + mPosition, mUp);
}

XMMATRIX Camera::GetProjectionMatrix() const
{
	return mProjectionMatrix;
}

void Camera::SetPosition(XMFLOAT3 & position)
{
	if(!mIsLocked)
		mPosition = XMLoadFloat3(&position);
}

XMFLOAT3 Camera::GetPosition() const
{
	XMFLOAT3 outF;
	XMStoreFloat3(&outF, mPosition);
	return outF;
}

void Camera::Lock()
{
	mIsLocked = true;
}

void Camera::Unlock()
{
	mIsLocked = false;
}
