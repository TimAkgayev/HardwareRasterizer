#include "Mesh.h"

Mesh::Mesh()
{
	ObjectType = MESH_OBJECT;
}

Mesh::~Mesh()
{
}

void Mesh::CreateDummyCube(int width, int height)
{
	Vertex mesh[] =
	{
		{ XMFLOAT3(-width/2.0f, height/2.0f, -width/2.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(width/2.0f, height/2.0f, -width/2.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(width/2.0f, height/2.0f, width/2.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-width/2.0f, height/2.0f, width/2.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-width/2.0f, -height/2.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(width/2.0f, -height/2.0f, -width/2.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(width/2.0f, -height/2.0f, width/2.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-width/2.0f, -height/2.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	};

	for (Vertex v : mesh)
		mVertexList.push_back(v);
	
	//create index buffer
	WORD indices[] =
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,
	};

	for (WORD i : indices)
		mIndexList.push_back(i);




}


std::vector<Vertex>& Mesh::GetVertexList() 
{
	return mVertexList;
}

std::vector<WORD>& Mesh::GetIndexList() 
{
	return mIndexList;
}

