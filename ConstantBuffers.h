#pragma once
#include "d3dcommon.h"

namespace ConstantBuffers
{


	struct ProjectionVariables
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Projection;
	};

	
	void InitAll(ID3D10Device* device);

	extern ID3D10Buffer*  ViewWorldProjBuffer;
};
