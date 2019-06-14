#pragma once
#include "common_includes.h"

namespace ConstantBuffers
{

	__declspec(align(16)) struct DirectionalLight
	{
		DirectX::XMFLOAT3 LightDirection;
		DirectX::XMFLOAT4 LightColor;

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
	
	void InitAll(ID3D11Device* device);

	extern ID3D11Buffer*  ViewProjBuffer;
	extern ID3D11Buffer*  WorldMatrixBuffer;
	extern ID3D11Buffer*  DirectionalLightBuffer;


};
