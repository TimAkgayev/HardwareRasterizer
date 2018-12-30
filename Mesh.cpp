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
		{ XMFLOAT3(-width/2.0f, -height/2.0f, -width/2.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(width/2.0f, -height/2.0f, -width/2.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(width/2.0f, -height/2.0f, width/2.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-width/2.0f, -height/2.0f, width/2.0f), XMFLOAT4(0.5f, 0.0f, 0.3f, 1.0f) },
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

void Mesh::CreateFloor(std::string pathToHeightmap, int length, int width)
{
	mVertexList.clear();
	mIndexList.clear();

	Bitmap heightMap(pathToHeightmap);
	POINT dim = heightMap.GetDimensions();

	//create the vertices
	Vertex* floorMesh = new Vertex[dim.x*dim.y];

	for (int xdim = 0; xdim < dim.x; xdim++)
	{
		for (int zdim = 0; zdim < dim.y; zdim++)
		{
			float height = heightMap.GetData()[xdim + zdim*dim.x];
		
			float color = (height)/ 255.0f;
			floorMesh[xdim + zdim*dim.x] = { XMFLOAT3((float)xdim, 1.0f , (float)zdim), XMFLOAT4(0.0f, color, color, 1.0f) };
		} 
	}


	mVertexList = std::vector<Vertex>(floorMesh, floorMesh + (dim.x*dim.y));
	delete floorMesh;
	floorMesh = NULL;
	
	//create the indices
	int numXCells = dim.x - 1;
	int numYCells = dim.y - 1;
	int vertsPerRow = dim.x;

	for (int cellX = 0; cellX < numXCells; cellX++)
	{
		for (int cellY = 0; cellY < numYCells; cellY++)
		{
			int	index0 = cellY * vertsPerRow + cellX;
			int	index1 = index0 + 1;
			int	index2 = (cellY + 1) * vertsPerRow + cellX;
			int	index3 = index2 + 1;

			
			mIndexList.push_back(index0);
			mIndexList.push_back(index3);
			mIndexList.push_back(index1);
			mIndexList.push_back(index0);
			mIndexList.push_back(index2);
			mIndexList.push_back(index3);

		}
	}
}


std::vector<Vertex>& Mesh::GetVertexList() 
{
	return mVertexList;
}

std::vector<WORD>& Mesh::GetIndexList() 
{
	return mIndexList;
}

