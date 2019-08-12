#pragma once
#include "common_includes.h"

namespace ConstantBuffers
{

	__declspec(align(16)) struct DirectionalLight
	{
		DirectX::XMFLOAT3   LightDirection;
		float _padding;
		DirectX::XMFLOAT4   LightColor;
		DirectX::XMFLOAT4X4 LightWorldMatrix;
		DirectX::XMFLOAT4X4 LightViewMatrix;
		DirectX::XMFLOAT4X4 LightProjectionMatrix;
		DirectX::XMFLOAT4   LightPosition;
	};

	__declspec(align(16)) struct ProjectionVariables
	{
		
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

	__declspec(align(16)) struct WorldMatrices
	{
		DirectX::XMMATRIX World;
	};

	__declspec(align(16)) struct LightVariables
	{
		DirectX::XMFLOAT4 AmbientLight;
	};

	__declspec(align(16)) struct CameraPosition
	{
		DirectX::XMFLOAT3 EyePosition;
		float _padding;
	};

	__declspec(align(16)) struct Material
	{
		float Ka;
		float Kd;
		float Ks;
		float A;
	};

	
	void InitAll(ID3D11Device* device);

	extern ID3D11Buffer*  ViewProjBuffer;
	extern ID3D11Buffer*  WorldMatrixBuffer;
	extern ID3D11Buffer*  DirectionalLightBuffer;
	extern ID3D11Buffer*  CameraPositionBuffer;
	extern ID3D11Buffer*  LightVariablesBuffer;
	extern ID3D11Buffer*  MaterialBuffer;



};
