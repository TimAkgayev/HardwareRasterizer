#pragma once
#include <d3d10.h>
#include <DirectXMath.h>
#include <Windows.h>
#include "WorldObject.h"
#include <vector>;
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

class Mesh : public WorldObject
{
public:
	Mesh();
	~Mesh();

	void CreateDummyCube(int width, int height);

	std::vector<Vertex>& GetVertexList() ;
	std::vector<WORD>& GetIndexList() ;
	XMMATRIX GetWorldMatrix() const;
protected:
	std::vector<Vertex> mVertexList;
	std::vector<WORD> mIndexList;
	XMMATRIX mWorldMatrix;

};