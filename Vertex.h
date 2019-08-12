#pragma once
#include "common_includes.h"
using namespace DirectX;

namespace Vertex {


	struct PosColor
	{

		XMFLOAT3 pos;
		XMFLOAT4 color;
	};

	struct Skybox
	{
		XMFLOAT3 pos;
	};

	struct PosNormTex
	{
		XMFLOAT3 pos;
		XMFLOAT3 norm;
		XMFLOAT2 uv;
	};
};
