#pragma once
#include <d3d10.h>
#include <DirectXMath.h>
#include <Windows.h>
#include "WorldObject.h"
#include <Bitmap.h>
#include <vector>
#include <string>
#include <gdiplus.h>
#include <gdiplus.h>
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

class Mesh : public WorldObject
{
public:
	Mesh();
	~Mesh();



	void CreateDummyCube(int width, int height);
	void CreateFloor(std::wstring pathToHeightmap, int length, int width);
	std::vector<Vertex>& GetVertexList() ;
	std::vector<WORD>& GetIndexList() ;
	
protected:
	std::vector<Vertex> mVertexList;
	std::vector<WORD>   mIndexList;



};