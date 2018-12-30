//GCamera.cpp
#include "Camera.h"



Camera::Camera(void)
{
	

	mPosition = XMFLOAT3(0.0f, -1.0f, -5.0f);
	mTarget = XMFLOAT3(0.0f, 1.0f, 0.0f);
	mUp = _ConvertVectorToFloat(_ConvertFloatToVector(mPosition) + _ConvertFloatToVector(XMFLOAT3(0, 1, 0)));
	this->initViewMatrix();

	mAngle = 0.0f;
	mClientWidth = 0.0f;
	mClientHeight = 0.0f;
	mNearest = 0.0f;
	mFarthest = 0.0f;

	//XMStoreFloat4x4(&mView, XMMatrixIdentity());
	XMStoreFloat4x4(&mProj, XMMatrixIdentity());
	XMStoreFloat4x4(&mOrtho, XMMatrixIdentity());
}

Camera::Camera(const Camera& camera)
{
	*this = camera;
}

Camera& Camera::operator=(const Camera& camera)
{
	mPosition = camera.mPosition;
	mTarget = camera.mTarget;
	mUp = camera.mUp;

	mAngle = camera.mAngle;
	mClientWidth = camera.mClientWidth;
	mClientHeight = camera.mClientHeight;
	mNearest = camera.mNearest;
	mFarthest = camera.mFarthest;

	mView = camera.mView;
	mProj = camera.mProj;
	mOrtho = camera.mOrtho;
	return *this;
}

void Camera::CheckUserInput()
{
	if(GetAsyncKeyState(0x57)) //W key
		Move(XMFLOAT3(0.0f, 0.0f, 1.0f));

	if (GetAsyncKeyState(0x53)) //S key
		Move(XMFLOAT3(0.0f, 0.0f, -1.0f));

	if (GetAsyncKeyState(0x41)) //A key
		Move(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	
	if (GetAsyncKeyState(0x44)) //D key
		Move(XMFLOAT3(1.0f, 0.0f, 0.0f));
	
}

void Camera::initViewMatrix()
{
	XMStoreFloat4x4(&mView, XMMatrixLookAtLH(XMLoadFloat3(&mPosition), XMLoadFloat3(&mTarget),
		XMLoadFloat3(&this->Up())));
}



void Camera::InitProjMatrix(const float angle, const float client_width, const float client_height,
	const float near_plane, const float far_plane)
{
	mAngle = angle;
	mClientWidth = client_width;
	mClientHeight = client_height;
	mNearest = near_plane;
	mFarthest = far_plane;
	XMStoreFloat4x4(&mProj, XMMatrixPerspectiveFovLH(angle, client_width / client_height,
		near_plane, far_plane));
}

void Camera::Move(XMFLOAT3 direction)
{
	mPosition = _ConvertVectorToFloat(XMVector3Transform(_ConvertFloatToVector(mPosition),
		XMMatrixTranslation(direction.x, direction.y, direction.z)));
	mTarget = _ConvertVectorToFloat(XMVector3Transform(_ConvertFloatToVector(mTarget),
		XMMatrixTranslation(direction.x, direction.y, direction.z)));
	mUp = _ConvertVectorToFloat(XMVector3Transform(_ConvertFloatToVector(mUp),
		XMMatrixTranslation(direction.x, direction.y, direction.z)));

	this->initViewMatrix();
}

void Camera::Rotate(XMFLOAT3 axis, float degrees)
{
	if (XMVector3Equal(_ConvertFloatToVector(axis), XMVectorZero()) ||
		degrees == 0.0f)
		return;

	// rotate vectors
	XMFLOAT3 look_at_target = _ConvertVectorToFloat(_ConvertFloatToVector(mTarget) - _ConvertFloatToVector(mPosition));
	XMFLOAT3 look_at_up = _ConvertVectorToFloat(_ConvertFloatToVector(mUp) - _ConvertFloatToVector(mPosition));
	look_at_target = _ConvertVectorToFloat(XMVector3Transform(_ConvertFloatToVector(look_at_target),
		XMMatrixRotationAxis(_ConvertFloatToVector(axis), XMConvertToRadians(degrees))));
	look_at_up = _ConvertVectorToFloat(XMVector3Transform(_ConvertFloatToVector(look_at_up),
		XMMatrixRotationAxis(_ConvertFloatToVector(axis), XMConvertToRadians(degrees))));

	// restore vectors's end points mTarget and mUp from new rotated vectors
	mTarget = _ConvertVectorToFloat(_ConvertFloatToVector(mPosition) + _ConvertFloatToVector(look_at_target));
	mUp = _ConvertVectorToFloat(_ConvertFloatToVector(mPosition) + _ConvertFloatToVector(look_at_up));

	this->initViewMatrix();
}

void Camera::Target(XMFLOAT3 new_target)
{
	if (XMVector3Equal(_ConvertFloatToVector(new_target), _ConvertFloatToVector(mPosition)) ||
		XMVector3Equal(_ConvertFloatToVector(new_target), _ConvertFloatToVector(mTarget)))
		return;

	XMFLOAT3 old_look_at_target = _ConvertVectorToFloat(_ConvertFloatToVector(mTarget) - _ConvertFloatToVector(mPosition));
	XMFLOAT3 new_look_at_target = _ConvertVectorToFloat(_ConvertFloatToVector(new_target) - _ConvertFloatToVector(mPosition));
	float angle = XMConvertToDegrees(XMVectorGetX(
		XMVector3AngleBetweenNormals(XMVector3Normalize(_ConvertFloatToVector(old_look_at_target)),
			XMVector3Normalize(_ConvertFloatToVector(new_look_at_target)))));
	if (angle != 0.0f && angle != 360.0f && angle != 180.0f)
	{
		XMVECTOR axis = XMVector3Cross(_ConvertFloatToVector(old_look_at_target), _ConvertFloatToVector(new_look_at_target));
		Rotate(_ConvertVectorToFloat(axis), angle);
	}
	mTarget = new_target;
	this->initViewMatrix();
}

// Set camera position
void Camera::Position(XMFLOAT3& new_position)
{
	XMFLOAT3 move_vector = _ConvertVectorToFloat(_ConvertFloatToVector(new_position) - _ConvertFloatToVector(mPosition));
	XMFLOAT3 target = mTarget;
	this->Move(move_vector);
	this->Target(target);
}

void Camera::Angle(float angle)
{
	mAngle = angle;
	InitProjMatrix(mAngle, mClientWidth, mClientHeight, mNearest, mFarthest);
}

void Camera::NearestPlane(float nearest)
{
	mNearest = nearest;
	OnResize(mClientWidth, mClientHeight);
}

void Camera::FarthestPlane(float farthest)
{
	mFarthest = farthest;
	OnResize(mClientWidth, mClientHeight);
}

void Camera::InitOrthoMatrix(const float clientWidth, const float clientHeight,
	const float nearZ, const float fartherZ)
{
	XMStoreFloat4x4(&mOrtho, XMMatrixOrthographicLH(clientWidth, clientHeight, 0.0f, fartherZ));
}

void Camera::OnResize(uint32_t new_width, uint32_t new_height)
{
	mClientWidth = new_width;
	mClientHeight = new_height;
	InitProjMatrix(mAngle, static_cast<float>(new_width), static_cast<float>(new_height), mNearest, mFarthest);
	InitOrthoMatrix(static_cast<float>(new_width), static_cast<float>(new_height), 0.0f, mFarthest);
}


inline XMVECTOR Camera::_ConvertFloatToVector(XMFLOAT3& val)
{
	return XMLoadFloat3(&val);
}

inline XMFLOAT3 Camera::_ConvertVectorToFloat(XMVECTOR& vec)
{
	XMFLOAT3 val;
	XMStoreFloat3(&val, vec);
	return val;
}

inline XMFLOAT4X4 Camera::_ConvertMatrixToFloat(XMMATRIX& matrix)
{
	XMFLOAT4X4 val;
	XMStoreFloat4x4(&val, matrix);
	return val;
}

XMMATRIX Camera:: _ConvertFloatToMatrix(XMFLOAT4X4& val)
{
	return XMLoadFloat4x4(&val);
}