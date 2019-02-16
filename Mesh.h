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

struct MeshVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 uv;
};

struct LineVertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};



class Line : public WorldObject
{
public:
	Line();

	void CreateLine(float x1, float y1, float z1, float x2, float y2, float z2, XMFLOAT4 color, XMFLOAT4 color2);
	void SetSecondPoint(float x2, float y2, float z2);
	std::vector<LineVertex>& GetVertexList();

private:
	std::vector<LineVertex> mVertexList;
	float mx1, my1, mz1, mx2, my2, mz2;
	XMFLOAT4 mcolor1, mcolor2;
};

class Mesh : public WorldObject
{
public:
	Mesh();
	~Mesh();



	
	void CreateDummyCube(int width, int height);
	void CreateFloor(std::wstring pathToHeightmap, int length, int width);


	void SetTexture(std::wstring path);
	std::vector<MeshVertex>& GetVertexList();
	std::vector<WORD>& GetIndexList();
	std::wstring GetTexturePath();
	
	
protected:
	std::vector<MeshVertex> mVertexList;
	std::vector<WORD>   mIndexList;
	std::wstring mTexturePath;


};