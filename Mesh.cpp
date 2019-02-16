#include "Mesh.h"


Line::Line()
{
	ObjectType = LINE_OBJECT;
}

void Line::CreateLine(float x1, float y1, float z1, float x2, float y2, float z2, XMFLOAT4 color1, XMFLOAT4 color2)
{
	LineVertex mesh[2] =
	{
		{ XMFLOAT3(x1, y1, z1), color1 },
		{ XMFLOAT3(x2, y2, z2), color2 }
	};

	mx1 = x1;
	my1 = y1;
	mz1 = z1;
	mx2 = x2;
	my2 = y2;
	mz2 = z2;

	mcolor1 = color1;
	mcolor2 = color2;

	for (LineVertex v : mesh)
		mVertexList.push_back(v);
}

void Line::SetSecondPoint(float x2, float y2, float z2)
{
	mVertexList.clear();

	LineVertex mesh[2] =
	{
		{ XMFLOAT3(mx1, my1, mz1), mcolor1 },
		{ XMFLOAT3(x2, y2, z2), mcolor2 }
	};

	mx2 = x2;
	my2 = y2;
	mz2 = z2;

	for (LineVertex v : mesh)
		mVertexList.push_back(v);
}

std::vector<LineVertex>& Line::GetVertexList()
{
	return mVertexList;
}


Mesh::Mesh()
{
	ObjectType = MESH_OBJECT;
	
}

Mesh::~Mesh()
{
}


void Mesh::CreateDummyCube(int width, int height)
{
	MeshVertex mesh[] =
	{
		{ XMFLOAT3(-width/2.0f, height/2.0f, -width/2.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(width/2.0f, height/2.0f, -width/2.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(width/2.0f, height/2.0f, width/2.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-width/2.0f, height/2.0f, width/2.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-width/2.0f, -height/2.0f, -width/2.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(width/2.0f, -height/2.0f, -width/2.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(width/2.0f, -height/2.0f, width/2.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-width/2.0f, -height/2.0f, width/2.0f), XMFLOAT2(0.5f, 0.0f) }
	};

	for (MeshVertex v : mesh)
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

void Mesh::CreateFloor(std::wstring pathToHeightmap, int length, int width)
{
	mVertexList.clear();
	mIndexList.clear();


	SoftwareBitmap::Bitmap heightMap(pathToHeightmap);

	int bmpHeight = heightMap.GetHeight();
	int bmpWidth = heightMap.GetWidth();



	//create the vertices
	MeshVertex* floorMesh = new MeshVertex[bmpWidth*bmpHeight];
	UCHAR* source_mem = (UCHAR*)heightMap.GetData();
	for (int xdim = 0; xdim < bmpWidth; xdim++)
	{
		for (int zdim = 0; zdim < bmpHeight; zdim++)
		{
			
			float height = source_mem[xdim + zdim*heightMap.GetPitch()];
		
			float color = (height)/ 255.0f;
			floorMesh[xdim + zdim*bmpWidth] = { XMFLOAT3((float)xdim, height , (float)zdim), XMFLOAT2(float(xdim)/ bmpWidth, float(zdim)/ bmpHeight) };
		} 
	}


	mVertexList = std::vector<MeshVertex>(floorMesh, floorMesh + (bmpWidth*bmpHeight));

	delete floorMesh;
	floorMesh = NULL;
	
	//create the indices
	int numXCells = bmpWidth - 1;
	int numYCells = bmpHeight - 1;
	int vertsPerRow = bmpWidth;

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


void Mesh::SetTexture(std::wstring path)
{
	mTexturePath = path;
}


std::vector<MeshVertex>& Mesh::GetVertexList() 
{
	return mVertexList;
}

std::vector<WORD>& Mesh::GetIndexList() 
{
	return mIndexList;
}

std::wstring Mesh::GetTexturePath()
{
	return mTexturePath;
}


