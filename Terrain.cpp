#include "Terrain.h"

Terrain::~Terrain()
{
	if (mHeightMap)
		delete mHeightMap;
	mHeightMap = nullptr;
}

void Terrain::CreateFromHeightMap(std::wstring pathToHeightmap, float floorScale, float heightScale)
{

	mVertexList.clear();
	mIndexList.clear();

	mHeightMap = new SoftwareBitmap::Bitmap(pathToHeightmap);
	int bmpHeight = mHeightMap->GetHeight();
	int bmpWidth = mHeightMap->GetWidth();

	mFloorScale = floorScale;
	mHeightScale = heightScale;

	//create the vertices
	MeshVertex* floorMesh = new MeshVertex[bmpWidth*bmpHeight];
	UCHAR* source_mem = (UCHAR*)mHeightMap->GetData();

	//position the terrain so it's in the middle of the level
	float dimx_center = bmpWidth * floorScale / 2.0f;
	float dimy_center = bmpHeight * floorScale / 2.0f;


	for (int xdim = 0; xdim < bmpWidth; xdim++)
	{
		for (int zdim = 0; zdim < bmpHeight; zdim++)
		{
			float height = source_mem[xdim + zdim*mHeightMap->GetPitch()];
			floorMesh[xdim + zdim*bmpWidth] = { XMFLOAT3((float)xdim*floorScale - dimx_center, height * heightScale , (float)zdim*floorScale - dimy_center), XMFLOAT2(float(xdim) / bmpWidth, float(zdim) / bmpHeight) };
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

void Terrain::GetHeightAtPosition(float x, float y, float** height)
{
	int bmpHeight = mHeightMap->GetHeight();
	int bmpWidth = mHeightMap->GetWidth();

	float dimx_center = bmpWidth * mFloorScale / 2.0f;
	float dimy_center = bmpHeight * mFloorScale / 2.0f;

	//transform position to zero
	float normalizedX = x + dimx_center;
	float normalizedY = y + dimy_center;

	//make sure the player is within level limits
	if (normalizedX >= bmpWidth || normalizedY >= bmpHeight || normalizedX <= -bmpWidth || normalizedY <= -bmpHeight)
	{
		*height = nullptr;
		return;
	}

	//find the fractional position
	float xPos = normalizedX / bmpWidth * mFloorScale;
	float yPos = normalizedY / bmpHeight * mFloorScale;



	UCHAR* bmpData = mHeightMap->GetData();
	float outHeight = bmpData[bmpWidth * int(xPos + 0.5f) + bmpHeight*int(yPos + 0.5f)*mHeightMap->GetPitch()];

	**height = outHeight;

}