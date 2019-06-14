#pragma once
#include "common_includes.h"
#include "Interface_LinkSystem.h"
using namespace DirectX;

class Camera
{
public:
	Camera();

	void Move(XMFLOAT3 dir);
	void Move(XMVECTOR dir);

	void MoveAlongLookAt(float amount);
	void MoveAlongRight(float amount);
	void MoveAlongUp(float amount);
	

	XMFLOAT3 GetLookAtVector() const;
	XMFLOAT3 GetRightVector() const;
	XMFLOAT3 GetUpVector() const;

	XMMATRIX GetViewTranslationMatrix() const;
	XMMATRIX GetViewRotationMatrix() const;
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;

	void Roll(float degrees);
	XMMATRIX Pitch(float degrees);
	XMMATRIX Yaw(float degrees);


	virtual void SetPosition(XMFLOAT3& position);
	virtual XMFLOAT3 GetPosition() const;
	

protected:
	XMVECTOR mUp;
	XMVECTOR mRight;
	XMVECTOR mLookAt;

	XMVECTOR mPosition;

	XMMATRIX mProjectionMatrix;
};