#include "Terrain.h"



Terrain::Terrain()
{

	mDebugLBound.SetIsWireframe(true);
	mDebugRBound.SetIsWireframe(true);
	mDebugUpperBound.SetIsWireframe(true);
	mDebugLowBound.SetIsWireframe(true);

}

Terrain::~Terrain()
{
	if (mHeightMap)
		delete mHeightMap;
	mHeightMap = nullptr;

	ReleaseCOM(mVB);
	ReleaseCOM(mIB);

}

void Terrain::draw()
{
	UINT stride = sizeof(Vertex::PosTex);
	UINT offset = 0;

	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dDevice->VSSetShader(Shaders::VS_SimpleProjection);
	md3dDevice->PSSetShader(Shaders::PS_SimpleTexture);
	md3dDevice->VSSetConstantBuffers(0, 1, &ConstantBuffers::ViewWorldProjBuffer);
	md3dDevice->PSSetConstantBuffers(0, 1, &ConstantBuffers::ViewWorldProjBuffer);
	md3dDevice->PSSetShaderResources(0, 1, &mTextureResourceView);
	md3dDevice->IASetInputLayout(InputLayout::PosTex);
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->DrawIndexed(mNumIndices, 0, 0);

	mDebugLBound.draw();
	mDebugRBound.draw();
	mDebugUpperBound.draw();
	mDebugLowBound.draw();

}

void GetCorrectHeightAtPoint(float x, float y, float** height)
{
}

void Terrain::CreateFromHeightMap(ID3D10Device* device, std::wstring pathToHeightmap, float floorScale, float heightScale)
{

	md3dDevice = device;

	mHeightMap = new SoftwareBitmap::Bitmap(pathToHeightmap);
	int bmpHeight = mHeightMap->GetHeight();
	int bmpWidth = mHeightMap->GetWidth();

	mFloorScale = floorScale;
	mHeightScale = heightScale;

	//create the vertices
	Vertex::PosTex* floorMesh = new Vertex::PosTex[bmpWidth*bmpHeight];
	UCHAR* source_mem = (UCHAR*)mHeightMap->GetData();

	//position the terrain so it's in the middle of the level
	float dimx_center = bmpWidth * floorScale / 2.0f;
	float dimy_center = bmpHeight * floorScale / 2.0f;

	//save bounds
	mUpperRightBound = { dimx_center, dimy_center };
	mLowerLeftBound = { -dimx_center, -dimy_center };

	for (int zdim = 0; zdim < bmpHeight; zdim++)
	{
		for (int xdim = 0; xdim < bmpWidth; xdim++)
		{
			float height = source_mem[xdim + zdim*mHeightMap->GetPitch()];
			floorMesh[xdim + zdim*bmpWidth] = { XMFLOAT3((float)xdim*floorScale - dimx_center, height * heightScale , (float)zdim*floorScale - dimy_center), XMFLOAT2(float(zdim) / bmpHeight,float(xdim) / bmpWidth) };
		}
	}

	//save the vertex list so that we don't have to lock the vertex buffer constantly
	mVertexList = std::vector<Vertex::PosTex>(floorMesh, floorMesh + bmpWidth*bmpHeight);
	
	//create the vertex buffer
	//create a vertex buffer
	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosTex) * mVertexList.size(); //total size of buffer in bytes
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = floorMesh;
	md3dDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);

	delete floorMesh;
	floorMesh = NULL;


	//create the indices
	int numXCells = bmpWidth - 1;
	int numYCells = bmpHeight - 1;
	int vertsPerRow = bmpWidth;


	std::vector<DWORD> indices;

	for (int cellX = 0; cellX < numXCells; cellX++)
	{
		for (int cellY = 0; cellY < numYCells; cellY++)
		{
			int	index0 = cellY * vertsPerRow + cellX;
			int	index1 = index0 + 1;
			int	index2 = (cellY + 1) * vertsPerRow + cellX;
			int	index3 = index2 + 1;


			indices.push_back(index0);
			indices.push_back(index3);
			indices.push_back(index1);

			indices.push_back(index0);
			indices.push_back(index2);
			indices.push_back(index3);

		}
	}


	
	//create the index buffer
	bufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(DWORD) * indices.size();
	bufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;


	InitData.pSysMem = &indices[0];
	md3dDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);


	mNumIndices = indices.size();


}


XMFLOAT3 FindMidPoint(XMFLOAT3 p1, XMFLOAT3 p2)
{
	//divide the square into two triangles and find which triangle the player is in
	XMVECTOR point0 = XMLoadFloat3(&p1);
	XMVECTOR point1 = XMLoadFloat3(&p2);

	XMVECTOR baseMidpoint = (point1 + point0) / 2;
	XMFLOAT3 outFloat;
	XMStoreFloat3(&outFloat, baseMidpoint);

	return outFloat;
}



float FindTriangleArea(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2)
{
	XMVECTOR point0 = XMLoadFloat3(&p0);
	XMVECTOR point1 = XMLoadFloat3(&p1);
	XMVECTOR point2 = XMLoadFloat3(&p2);

	XMVECTOR BA = point1 - point0;
	XMVECTOR BC = point1 - point2;

	XMVECTOR BAlen, BClen;
	XMFLOAT3 BAlenf, BClenf;

	BAlen = XMVector3Length(BA);
	BClen = XMVector3Length(BC);

	XMStoreFloat3(&BAlenf, BAlen);
	XMStoreFloat3(&BClenf, BClen);


	XMVECTOR angle = XMVector3AngleBetweenVectors(BA, BC);
	XMFLOAT3 anglef;
	XMStoreFloat3(&anglef, angle);


	return 0.5f * BAlenf.x * BClenf.x * sin(anglef.x);
}

void Terrain::GetBoundPoints(XMFLOAT3** ill, XMFLOAT3** ilu, XMFLOAT3** irl, XMFLOAT3** iru)
{
	*ill = &ll;
	*ilu = &lu;
	*irl = &rl;
	*iru = &ru;
}


void Terrain::CreateCollisionBoxes()
{

	XMFLOAT2 ll, ur, ul, lr;
	GetBoundingCoordinates(&ll, &ur);


	ul.x = ll.x;
	ul.y = ur.y;

	lr.x = ur.x;
	lr.y = ll.y;


	mBoxLeftBound.init(ll, ul, 500, 1500);
	mBoxRightBound.init(ul, ur, 500, 1500);
	mBoxLowBound.init(ur, lr, 500, 1500);
	mBoxUpperBound.init(lr, ll, 500, 1500);

	
	XMVECTOR xAxis = mBoxLeftBound.GetXAxis();
	XMVECTOR yAxis = mBoxLeftBound.GetYAxis();
	XMVECTOR zAxis = mBoxLeftBound.GetZAxis();
	XMVECTOR center = mBoxLeftBound.GetCenter();
	XMVECTOR extents = mBoxLeftBound.GetExtents();
	XMFLOAT3 extentsF;
	XMStoreFloat3(&extentsF, extents);

	XMFLOAT3 points[8];
	points[0] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMVECTOR p1 = zAxis * extentsF.z * 2;
	XMVECTOR p2 = zAxis*extentsF.z * 2 + xAxis*extentsF.x * 2;
	XMVECTOR p3 = xAxis * extentsF.x * 2;

	XMFLOAT3 p1F, p2F, p3F;
	XMStoreFloat3(&p1F, p1);
	XMStoreFloat3(&p2F, p2);
	XMStoreFloat3(&p3F, p3);

	p1F.y = p2F.y = p3F.y = 0.0f;
	
	XMVECTOR height = yAxis * extentsF.y * 2;
	XMFLOAT3 heightF;
	XMStoreFloat3(&heightF, height);
	points[4] = XMFLOAT3(0.0f, heightF.y, 0.0f);
	XMVECTOR p5 = zAxis * extentsF.z * 2;
	XMVECTOR p6 = zAxis*extentsF.z * 2 + xAxis*extentsF.x * 2;
	XMVECTOR p7 = xAxis * extentsF.x * 2;

	XMFLOAT3 p5F, p6F, p7F;
	XMStoreFloat3(&p5F, p5);
	XMStoreFloat3(&p6F, p6);
	XMStoreFloat3(&p7F, p7);
	
	p5F.y = p6F.y = p7F.y = heightF.y;

	points[1] = p1F;
	points[2] = p2F;
	points[3] = p3F;
	points[5] = p5F;
	points[6] = p6F;
	points[7] = p7F;

	for (int i = 0; i < 8; i++)
	{
		XMVECTOR v = XMLoadFloat3(&points[i]);
		v += center;

		XMStoreFloat3(&points[i], v);
	}

	mDebugLBound.init(md3dDevice, points, XMFLOAT4(255.0f, 0.0, 0.0f, 255.0f));





	 xAxis = mBoxUpperBound.GetXAxis();
	 yAxis = mBoxUpperBound.GetYAxis();
	 zAxis = mBoxUpperBound.GetZAxis();
	 center = mBoxUpperBound.GetCenter();
	 extents = mBoxUpperBound.GetExtents();
	
	XMStoreFloat3(&extentsF, extents);


	points[0] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	 p1 = zAxis * extentsF.z * 2;
	 p2 = zAxis*extentsF.z * 2 + xAxis*extentsF.x * 2;
	 p3 = xAxis * extentsF.x * 2;

	
	XMStoreFloat3(&p1F, p1);
	XMStoreFloat3(&p2F, p2);
	XMStoreFloat3(&p3F, p3);

	p1F.y = p2F.y = p3F.y = 0.0f;

	 height = yAxis * extentsF.y * 2;
	
	XMStoreFloat3(&heightF, height);
	points[4] = XMFLOAT3(0.0f, heightF.y, 0.0f);
	 p5 = zAxis * extentsF.z * 2;
	 p6 = zAxis*extentsF.z * 2 + xAxis*extentsF.x * 2;
	 p7 = xAxis * extentsF.x * 2;

	
	XMStoreFloat3(&p5F, p5);
	XMStoreFloat3(&p6F, p6);
	XMStoreFloat3(&p7F, p7);

	p5F.y = p6F.y = p7F.y = heightF.y;

	points[1] = p1F;
	points[2] = p2F;
	points[3] = p3F;
	points[5] = p5F;
	points[6] = p6F;
	points[7] = p7F;

	for (int i = 0; i < 8; i++)
	{
		XMVECTOR v = XMLoadFloat3(&points[i]);
		v += center;

		XMStoreFloat3(&points[i], v);
	}

	mDebugUpperBound.init(md3dDevice, points, XMFLOAT4(255.0f, 0.0, 0.0f, 255.0f));


	xAxis = mBoxRightBound.GetXAxis();
	yAxis = mBoxRightBound.GetYAxis();
	zAxis = mBoxRightBound.GetZAxis();
	center = mBoxRightBound.GetCenter();
	extents = mBoxRightBound.GetExtents();

	XMStoreFloat3(&extentsF, extents);


	points[0] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p1 = zAxis * extentsF.z * 2;
	p2 = zAxis*extentsF.z * 2 + xAxis*extentsF.x * 2;
	p3 = xAxis * extentsF.x * 2;


	XMStoreFloat3(&p1F, p1);
	XMStoreFloat3(&p2F, p2);
	XMStoreFloat3(&p3F, p3);

	p1F.y = p2F.y = p3F.y = 0.0f;

	height = yAxis * extentsF.y * 2;

	XMStoreFloat3(&heightF, height);
	points[4] = XMFLOAT3(0.0f, heightF.y, 0.0f);
	p5 = zAxis * extentsF.z * 2;
	p6 = zAxis*extentsF.z * 2 + xAxis*extentsF.x * 2;
	p7 = xAxis * extentsF.x * 2;


	XMStoreFloat3(&p5F, p5);
	XMStoreFloat3(&p6F, p6);
	XMStoreFloat3(&p7F, p7);

	p5F.y = p6F.y = p7F.y = heightF.y;

	points[1] = p1F;
	points[2] = p2F;
	points[3] = p3F;
	points[5] = p5F;
	points[6] = p6F;
	points[7] = p7F;

	for (int i = 0; i < 8; i++)
	{
		XMVECTOR v = XMLoadFloat3(&points[i]);
		v += center;

		XMStoreFloat3(&points[i], v);
	}

	mDebugRBound.init(md3dDevice, points, XMFLOAT4(255.0f, 0.0, 0.0f, 255.0f));


	xAxis = mBoxLowBound.GetXAxis();
	yAxis = mBoxLowBound.GetYAxis();
	zAxis = mBoxLowBound.GetZAxis();
	center = mBoxLowBound.GetCenter();
	extents = mBoxLowBound.GetExtents();

	XMStoreFloat3(&extentsF, extents);


	points[0] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p1 = zAxis * extentsF.z * 2;
	p2 = zAxis*extentsF.z * 2 + xAxis*extentsF.x * 2;
	p3 = xAxis * extentsF.x * 2;


	XMStoreFloat3(&p1F, p1);
	XMStoreFloat3(&p2F, p2);
	XMStoreFloat3(&p3F, p3);

	p1F.y = p2F.y = p3F.y = 0.0f;

	height = yAxis * extentsF.y * 2;

	XMStoreFloat3(&heightF, height);
	points[4] = XMFLOAT3(0.0f, heightF.y, 0.0f);
	p5 = zAxis * extentsF.z * 2;
	p6 = zAxis*extentsF.z * 2 + xAxis*extentsF.x * 2;
	p7 = xAxis * extentsF.x * 2;


	XMStoreFloat3(&p5F, p5);
	XMStoreFloat3(&p6F, p6);
	XMStoreFloat3(&p7F, p7);

	p5F.y = p6F.y = p7F.y = heightF.y;

	points[1] = p1F;
	points[2] = p2F;
	points[3] = p3F;
	points[5] = p5F;
	points[6] = p6F;
	points[7] = p7F;

	for (int i = 0; i < 8; i++)
	{
		XMVECTOR v = XMLoadFloat3(&points[i]);
		v += center;

		XMStoreFloat3(&points[i], v);
	}

	mDebugLowBound.init(md3dDevice, points, XMFLOAT4(255.0f, 0.0, 0.0f, 255.0f));

}

void Terrain::SetTexture(std::wstring texturePath)
{
	D3DX10CreateShaderResourceViewFromFile(md3dDevice, texturePath.c_str(), NULL, NULL, &mTextureResourceView, NULL);
}

void Terrain::GetHeightAtPosition(XMFLOAT3& playerPos, float** OutHeight)
{
	

	int vertexArrayHeight = mHeightMap->GetHeight();
	int vertexArrayWidth = mHeightMap->GetWidth();

	int xBoundRight, xBoundLeft;
	//find the x-coordinate of the containing square
	for (int x = 0; x < vertexArrayWidth; x++)
	{
		if (mVertexList[x].pos.x > playerPos.x)
		{
			xBoundRight = x;
			xBoundLeft = x - 1;
			break;
		}
	}


	int yBoundUp, yBoundDown;
	//find the y-coordinate of the containing square
	for (int y = 0; y < vertexArrayHeight; y++)
	{
		if (mVertexList[xBoundLeft + y*vertexArrayWidth].pos.z > playerPos.z)
		{
			yBoundUp = y;
			yBoundDown = y - 1;
			break;
		}
	}

	//get the actual coordinates of the bounding square
	XMFLOAT3 leftLowerPoint = mVertexList[xBoundLeft + yBoundDown*vertexArrayWidth].pos;
	XMFLOAT3 leftUpperPoint = mVertexList[xBoundLeft + yBoundUp*vertexArrayWidth].pos;
	XMFLOAT3 rightUpperPoint = mVertexList[xBoundRight + yBoundUp*vertexArrayWidth].pos;
	XMFLOAT3 rightLowerPoint = mVertexList[xBoundRight + yBoundDown*vertexArrayWidth].pos;


	ll = leftLowerPoint;
	lu = leftUpperPoint;
	ru = rightUpperPoint;
	rl = rightLowerPoint;

	


	//flatten the points so they're all orthographic
	XMFLOAT3 flatLeftLowerPoint = leftLowerPoint;
	XMFLOAT3 flatLeftUpperPoint = leftUpperPoint;
	XMFLOAT3 flatRightUpperPoint = rightUpperPoint;
	XMFLOAT3 flatRightLowerPoint = rightLowerPoint;
	flatLeftLowerPoint.y = flatLeftUpperPoint.y = flatRightUpperPoint.y = flatRightLowerPoint.y = 0.0f;

	struct _playerTriangle
	{
		XMFLOAT3 points[3];
	}playerTriangle;


	XMFLOAT3 flatPlayerPos = playerPos;
	flatPlayerPos.y = 0.0f;

	//divide the square into two triangles and find which triangle the player is in
	float triOneArea = FindTriangleArea(flatLeftLowerPoint, flatLeftUpperPoint, flatRightLowerPoint);

	//find out if the player's position is in this triangle;
	float partialTri1Area = FindTriangleArea(flatLeftLowerPoint, flatLeftUpperPoint, flatPlayerPos);
	float partialTri2Area = FindTriangleArea(flatLeftUpperPoint, flatRightLowerPoint, flatPlayerPos);
	float partialTri3Area = FindTriangleArea(flatLeftLowerPoint, flatRightLowerPoint, flatPlayerPos);



	//check if the sum of the partial triangle areas equal the area of the overall triangle
	if ((partialTri1Area + partialTri2Area + partialTri3Area >= triOneArea - 5) && (partialTri1Area + partialTri2Area + partialTri3Area <= triOneArea + 5))
	{
		playerTriangle.points[0] = leftLowerPoint;
		playerTriangle.points[1] = leftUpperPoint;
		playerTriangle.points[2] = rightLowerPoint;
	}


	//repeat for the other triangle
	float triTwoArea = FindTriangleArea(flatLeftUpperPoint, flatRightUpperPoint, flatRightLowerPoint);

	//find out if the player's position is in this triangle;
	float partialTri1Area2 = FindTriangleArea(flatLeftUpperPoint, flatRightUpperPoint, flatPlayerPos);
	float partialTri2Area2 = FindTriangleArea(flatRightUpperPoint, flatRightLowerPoint, flatPlayerPos);
	float partialTri3Area2 = FindTriangleArea(flatRightLowerPoint, flatLeftUpperPoint, flatPlayerPos);



	//check if the sum of the partial triangle areas equal the area of the overall triangle
	if ((partialTri1Area2 + partialTri2Area2 + partialTri3Area2 >= triTwoArea - 5) && (partialTri1Area2 + partialTri2Area2 + partialTri3Area2 <= triTwoArea + 5))
	{
		playerTriangle.points[0] = leftUpperPoint;
		playerTriangle.points[1] = rightUpperPoint;
		playerTriangle.points[2] = rightLowerPoint;
	}


	//find the heighest and lowest vertices in the triangle

	//organize the triangle points by height (selection sort)
	for (int runCount = 0; runCount < 3; runCount++)
	{
		if (playerTriangle.points[0].y < playerTriangle.points[1].y)
		{
			if (playerTriangle.points[0].y < playerTriangle.points[2].y)
			{
				if (playerTriangle.points[1].y < playerTriangle.points[2].y)
					break;
				else
				{
					XMFLOAT3 temp = playerTriangle.points[1];
					playerTriangle.points[1] = playerTriangle.points[2];
					playerTriangle.points[2] = temp;


					break;
				}
			}
			else
			{
				XMFLOAT3 temp = playerTriangle.points[0];
				playerTriangle.points[0] = playerTriangle.points[2];
				playerTriangle.points[2] = temp;

				continue;
			}
		}
		else
		{
			XMFLOAT3 temp = playerTriangle.points[0];
			playerTriangle.points[0] = playerTriangle.points[1];
			playerTriangle.points[1] = temp;

			continue;
		}
	}

	//find the precentage of the way the player is up between the two points of the triangle 

	XMVECTOR triPointsVect[3];
	triPointsVect[0] = XMLoadFloat3(&playerTriangle.points[0]);
	triPointsVect[1] = XMLoadFloat3(&playerTriangle.points[1]);
	triPointsVect[2] = XMLoadFloat3(&playerTriangle.points[2]);

	XMFLOAT3 midpointf0, midpointf1;
	XMStoreFloat3(&midpointf0, triPointsVect[0]);
	XMStoreFloat3(&midpointf1, triPointsVect[1]);

	XMFLOAT3 baseMidpointf = FindMidPoint(midpointf0, midpointf1);
	XMVECTOR baseMidPoint = XMLoadFloat3(&baseMidpointf);

	XMVECTOR pt2ToMid = triPointsVect[2] - baseMidPoint;
	XMFLOAT3 pt2ToMidLenf;
	XMStoreFloat3(&pt2ToMidLenf, XMVector3Length(pt2ToMid));


	XMVECTOR playerPosV = XMLoadFloat3(&flatPlayerPos);
	XMVECTOR pt2ToPlayer = triPointsVect[2] - playerPosV;
	XMFLOAT3 pt2ToPlayerLenf;
	XMStoreFloat3(&pt2ToPlayerLenf, XMVector3Length(pt2ToPlayer));

	float percentageToGoal = pt2ToMidLenf.x / pt2ToPlayerLenf.x;

	XMVECTOR finalPlayerPos = baseMidPoint + pt2ToPlayer*percentageToGoal;
	XMFLOAT3 finalPlayerPosf;
	XMStoreFloat3(&finalPlayerPosf, finalPlayerPos);


	if (finalPlayerPosf.y != 0)
		int x = 0;

	**OutHeight = finalPlayerPosf.y;
	
	
/*

	int bmpHeight = mHeightMap->GetHeight();
	int bmpWidth = mHeightMap->GetWidth();

	float dimx_center = bmpWidth * mFloorScale / 2.0f;
	float dimy_center = bmpHeight * mFloorScale / 2.0f;

	//transform position to zero
	float normalizedX;
	float normalizedY;

	normalizedX = (playerPos.x + dimx_center) / (dimx_center * 2);
	normalizedY = (playerPos.y + dimy_center) / (dimy_center * 2);


	//make sure the player is within level limits
	if (normalizedX >= 1 || normalizedY >= 1 || normalizedX <= 0 || normalizedY <= 0)
	{

		return;
	}

	//find the fractional position
	float xPos = normalizedX * bmpWidth;
	float yPos = normalizedY * bmpHeight;



	UCHAR* bmpData = mHeightMap->GetData();
	float finalHeight = bmpData[int(xPos + 0.5f) + int(yPos + 0.5f)*mHeightMap->GetPitch()];
	**OutHeight = finalHeight;
	*/
}
	

void Terrain::GetBoundingCoordinates(XMFLOAT2 * lowerLeft, XMFLOAT2 * upperRight)
{
	*lowerLeft = mLowerLeftBound;
	*upperRight = mUpperRightBound;
}
