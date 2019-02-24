#pragma once
#include <DirectXMath.h>
#include "Interface_LinkSystem.h"
using namespace DirectX;

class Camera : public PositionLink
{
public:
	Camera();

	XMVECTOR MakeVector(float x, float y, float z, float w);
	XMVECTOR MakeVector(float x, float y, float z);


	void Move(XMFLOAT3 dir);
	void Move(XMVECTOR dir);

	void MoveAlongLookAt(float amount);
	void MoveAlongRight(float amount);
	void MoveAlongUp(float amount);
	

	XMFLOAT3 GetLookAtVector();
	XMFLOAT3 GetRightVector();
	XMFLOAT3 GetUpVector();


	void Roll(float degrees);
	XMMATRIX Pitch(float degrees);
	XMMATRIX Yaw(float degrees);


	XMMATRIX GetViewTranslationMatrix();
	XMMATRIX GetViewRotationMatrix();
	XMMATRIX GetViewMatrix();

	virtual void SetPosition(XMVECTOR& position) override;
	virtual XMVECTOR& GetPosition() override;
	

protected:
	XMVECTOR mUp;
	XMVECTOR mRight;
	XMVECTOR mLookAt;

	XMVECTOR mPosition;
};