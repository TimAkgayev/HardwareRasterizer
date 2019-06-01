#pragma once
#include <DirectXMath.h>
#include <d3d10.h>
using namespace DirectX;

namespace Vertex {

	struct PosTex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	struct PosColor
	{

		XMFLOAT3 pos;
		XMFLOAT4 color;
	};

	struct Skybox
	{
		XMFLOAT3 pos;
	};
};
