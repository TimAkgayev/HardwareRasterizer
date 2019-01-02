#pragma once
#include <Windows.h>
#include <d3d10.h>
#include "SMath.h"
#include <DirectXMath.h>

using namespace DirectX;

struct DX10VERTEX
{
	DX10VERTEX() {}
	DX10VERTEX(XMFLOAT3 p, XMFLOAT4 c, XMFLOAT2 uv) 
	{
		pos = p;
		color = c;
		texCoord = uv;
	}

	XMFLOAT3 pos;
	XMFLOAT4 color;
	XMFLOAT2 texCoord;

};


//basic POS | DIFFUSE | TEX vertex
class VERTEX2D
{
public:

	VERTEX2D() {}
	VERTEX2D(VECTOR2D pos, DWORD color, int u, int v)
	{
		this->pos = pos;
		this->color = color;
		this->u = u;
		this->v = v;
	}
	VERTEX2D(const VERTEX2D& rhs)
	{
		this->pos = rhs.pos;
		this->color = rhs.color;
		this->u = rhs.u;
		this->v = rhs.v;
	}


	VECTOR2D pos;
	DWORD  color;
	int u;
	int v;
};

typedef struct VERTEX3D_TYP
{
	VECTOR3D pos;
	DWORD color;
	int u;
	int v;
}VERTEX3D;


void TransformVertexList(VERTEX2D* vertexbuffer, int numVerts, MATRIX3D* transform);
