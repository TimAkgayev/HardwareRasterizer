#include "Camera.h"


Camera::Camera()
{
	mUp = MakeVector(0.0f, 1.0f, 0.0f);
	mRight = MakeVector(1.0f, 0.0f, 0.0f);


	mLookAt = XMVector3Cross(mUp, mRight);
	mLookAt = XMVector3Normalize(mLookAt);

	mPosition = MakeVector(2.0F, 4.0f, -500.0f);
}


XMVECTOR Camera::MakeVector(float x, float y, float z, float w)
{
	return XMLoadFloat4(&XMFLOAT4(x, y, z, w));
}

XMVECTOR Camera::MakeVector(float x, float y, float z)
{
	return XMLoadFloat3(&XMFLOAT3(x, y, z));
}

void Camera::Move(XMFLOAT3 dir)
{
	XMVECTOR tdir = XMLoadFloat3(&dir);
	Move(tdir);
}

void Camera::Move(XMVECTOR dir)
{
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



XMFLOAT3 Camera::GetLookAtVector()
{
	XMFLOAT3 fLookAt;
	XMStoreFloat3(&fLookAt, mLookAt);
	return fLookAt;
}

XMFLOAT3 Camera::GetRightVector()
{
	XMFLOAT3 fRight;
	XMStoreFloat3(&fRight, mRight);
	return fRight;
}

XMFLOAT3 Camera::GetUpVector()
{
	XMFLOAT3 fUp;
	XMStoreFloat3(&fUp, mUp);
	return fUp;
}

void Camera::Roll(float degrees)
{
	XMVECTOR rotQuaternion = XMQuaternionRotationAxis(mLookAt, degrees * XM_PI / 180);

	XMVector3Rotate(mUp, rotQuaternion);
	XMVector3Rotate(mRight, rotQuaternion);

	mUp = XMVector3Normalize(mUp);
	mRight = XMVector3Normalize(mRight);

}

XMMATRIX Camera::Pitch(float degrees)
{

	XMVECTOR rotQuaternion = XMQuaternionRotationAxis(mRight, degrees * XM_PI / 180);
	mUp = XMVector3Rotate(mUp, rotQuaternion);
	mUp = XMVector3Normalize(mUp);

	mLookAt = XMVector3Cross(mUp, mRight);
	mLookAt = XMVector3Normalize(mLookAt);

	return XMMatrixRotationQuaternion(rotQuaternion);


}

XMMATRIX Camera::Yaw(float degrees)
{
	XMVECTOR globalUp = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMVECTOR rotQuaternion = XMQuaternionRotationAxis(globalUp, degrees * XM_PI / 180);
	
	mRight = XMVector3Rotate(mRight, rotQuaternion);
	mRight = XMVector3Normalize(mRight);

	mUp = XMVector3Rotate(mUp, rotQuaternion);
	mUp = XMVector3Normalize(mUp);

	mLookAt = XMVector3Cross(mUp, mRight);
	mLookAt = XMVector3Normalize(mLookAt);

	return XMMatrixRotationQuaternion(rotQuaternion);

}


XMMATRIX Camera::GetViewTranslationMatrix()
{
	XMVECTOR row = MakeVector(1.0f, 0.0f, 0.0f, 0.0f);


	XMMATRIX matTranslate = XMMatrixIdentity();
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, mPosition);
	row = MakeVector(-camPos.x, -camPos.y, -camPos.z, 1.0f);
	matTranslate.r[3] = row;

	return matTranslate;
}

XMMATRIX Camera::Camera::GetViewRotationMatrix()
{
	XMVECTOR row4 = MakeVector(0.0f, 0.0f, 0.0f, 1.0f);
	XMMATRIX rotationMatrix = XMMatrixLookAtRH(mPosition, mLookAt + mPosition, mUp);
	rotationMatrix.r[3] = row4;
	return rotationMatrix;
}

XMMATRIX Camera::GetViewMatrix()
{

	return XMMatrixLookAtRH(mPosition, mLookAt + mPosition, mUp);
}

void Camera::SetPosition(XMVECTOR & position)
{
	mPosition = position;
}

XMVECTOR & Camera::GetPosition()
{
	return mPosition;
}
