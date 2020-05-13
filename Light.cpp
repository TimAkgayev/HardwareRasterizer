#include "Light.h"

Light::Light(ID3D11DeviceContext* context, XMFLOAT3 pos, XMFLOAT3 at, XMFLOAT4 color)
{
	mPosition = pos;
	mLookAt = at;
	mColor = color;
	mDeviceContext = context;

	mUpdateBuffer();
}

Light::~Light()
{
}

void Light::SetPosition(XMFLOAT3 pos)
{
	mPosition = pos;
	mUpdateBuffer();

}

void Light::SetLookAt(XMFLOAT3 lookat)
{
	mLookAt = lookat;
	mUpdateBuffer();

}

void Light::SetColor(XMFLOAT4 color)
{
	mColor = color;
	mUpdateBuffer();

}

XMVECTOR Light::GetPosition()
{
	return XMLoadFloat3(&mPosition);
}

void Light::mUpdateBuffer()
{

	XMVECTOR eye = XMLoadFloat3(&mPosition);
	XMVECTOR at = XMLoadFloat3(&mLookAt);
	XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };


	ConstantBuffers::DirectionalLight dirLight;
	XMStoreFloat4x4(&dirLight.LightViewMatrix, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));


	XMVECTOR ldir = XMVector3Normalize(at - eye);
	XMStoreFloat3(&dirLight.LightDirection, -ldir);

	dirLight.LightColor = mColor;

	XMMATRIX lightProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, 800.0f / (FLOAT)600, 10.0f, 1000.0f);
	XMStoreFloat4x4(&dirLight.LightProjectionMatrix, XMMatrixTranspose(lightProjection));

	XMStoreFloat4(&dirLight.LightPosition, eye);

	XMMATRIX lightWorld = XMMatrixIdentity();
	XMStoreFloat4x4(&dirLight.LightWorldMatrix, XMMatrixTranspose(lightWorld));

	mDeviceContext->UpdateSubresource(ConstantBuffers::DirectionalLightBuffer, 0, NULL, &dirLight, 0, 0);

	ConstantBuffers::LightVariables lightVars;
	lightVars.AmbientLight = { 0.1f, 0.1f, 0.1f, 1.0f };
	mDeviceContext->UpdateSubresource(ConstantBuffers::LightVariablesBuffer, 0, NULL, &lightVars, 0, 0);
}
