#include "Terrain.h"
#include <WICTextureLoader.h>
#include "ConstantBuffers.h"
#include "Vertex.h"
#include "InputLayouts.h"
#include "Shaders.h"
#include "ConstantBuffers.h"
#include <limits>

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
	UINT stride = sizeof(Vertex::PosNormTex);
	UINT offset = 0;

	ConstantBuffers::WorldMatrices worldBuffer;
	worldBuffer.World = XMMatrixIdentity();
	mDeviceContext->UpdateSubresource(ConstantBuffers::WorldMatrixBuffer, 0, NULL, &worldBuffer, 0, 0);

	ConstantBuffers::DirectionalLight dirLight;
	XMVECTOR ldir = { -0.577f, 0.577f, -0.577f }; 
	ldir = XMVector3Normalize(ldir);
	XMStoreFloat3(&dirLight.LightDirection, ldir);

	dirLight.LightColor = { 1.0f, 0.0f, 0.0f, 1.0f };

	mDeviceContext->UpdateSubresource(ConstantBuffers::DirectionalLightBuffer, 0, NULL, &dirLight, 0, 0);

	
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->VSSetShader(Shaders::VS_DirectionalLight, NULL, 0);
	mDeviceContext->PSSetShader(Shaders::PS_DirectionalLight, NULL, 0);
	mDeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);
	mDeviceContext->PSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);
	mDeviceContext->VSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);
	mDeviceContext->PSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);
	mDeviceContext->VSSetConstantBuffers(2, 1, &ConstantBuffers::DirectionalLightBuffer);
	mDeviceContext->PSSetConstantBuffers(2, 1, &ConstantBuffers::DirectionalLightBuffer);
	mDeviceContext->PSSetShaderResources(0, 1, &mTextureResourceView);
	mDeviceContext->IASetInputLayout(InputLayout::PosNormTex);
	mDeviceContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	mDeviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	mDeviceContext->DrawIndexed(mNumIndices, 0, 0);

	mDebugLBound.draw();
	mDebugRBound.draw();
	mDebugUpperBound.draw();
	mDebugLowBound.draw();

	mDebugSmallLBound.draw();
	mDebugSmallRBound.draw();
	mDebugSmallUpperBound.draw();
	mDebugSmallLowBound.draw();

}

void GetCorrectHeightAtPoint(float x, float y, float** height)
{
}

void Terrain::CreateFromHeightMap(ID3D11Device* device, std::wstring pathToHeightmap, float floorScale, float heightScale)
{
	mD3DDevice = device;
	mD3DDevice->GetImmediateContext(&mDeviceContext);

	mHeightMap = new SoftwareBitmap::Bitmap(pathToHeightmap);
	int bmpHeight = mHeightMap->GetHeight();
	int bmpWidth = mHeightMap->GetWidth();

	mFloorScale = floorScale;
	mHeightScale = heightScale;

	//create the vertices
	int numVertices = bmpWidth*bmpHeight;
	Vertex::PosNormTex* floorMesh = new Vertex::PosNormTex[numVertices];
	UCHAR* source_mem = (UCHAR*)mHeightMap->GetData();

	//position the terrain so it's in the middle of the level
	float dimx_center = bmpWidth * floorScale / 2.0f;
	float dimy_center = bmpHeight * floorScale / 2.0f;

	

	//create a grid
	for (int zdim = 0; zdim < bmpHeight; zdim++)
	{
		for (int xdim = 0; xdim < bmpWidth; xdim++)
		{
			float height = source_mem[xdim + zdim*mHeightMap->GetPitch()];
			floorMesh[xdim + zdim*bmpWidth] = { XMFLOAT3((float)xdim*floorScale - dimx_center, height * heightScale , (float)zdim*floorScale - dimy_center), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(float(zdim) / bmpHeight,float(xdim) / bmpWidth) };
		}
	}

	//get the true bounds
	float smallestX = -std::numeric_limits<float>::lowest();
	float largestX = std::numeric_limits<float>::lowest();
	float smallestZ = -std::numeric_limits<float>::lowest();
	float largestZ = std::numeric_limits<float>::lowest();
	for (int i = 0; i < numVertices; i++)
	{

		if (floorMesh[i].pos.x < smallestX)
			smallestX = floorMesh[i].pos.x;
		
		if (floorMesh[i].pos.x > largestX)
			largestX = floorMesh[i].pos.x;

		if (floorMesh[i].pos.z < smallestZ)
			smallestZ = floorMesh[i].pos.z;

		if (floorMesh[i].pos.z > largestZ)
			largestZ = floorMesh[i].pos.z;

	}


	//save bounds
	mUpperRightBound = { largestX, largestZ };
	mLowerLeftBound = { smallestX, smallestZ };



	//save the vertex list so that we don't have to lock the vertex buffer constantly
	mVertexList = std::vector<Vertex::PosNormTex>(floorMesh, floorMesh + bmpWidth*bmpHeight);
	

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

	mNumIndices = indices.size();

	
	//create the index buffer
	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(DWORD) * indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &indices[0];
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);

	//calculate the normals
	for (int vertIndex = 0; vertIndex < numVertices; vertIndex++)
	{
		XMVECTOR normTotal = { 0.0f, 0.0f, 0.0f };
		
		
		//for each index triplet
		int faceCount = 0;
		for (int indIndex = 0; indIndex < mNumIndices; indIndex += 3)
		{
			
			if ((indices[indIndex + 0] == vertIndex) || (indices[indIndex + 1] == vertIndex) || (indices[indIndex + 2] == vertIndex))
			{
				XMVECTOR v0_pos = XMLoadFloat3(&floorMesh[indices[indIndex + 0]].pos);
				XMVECTOR v1_pos = XMLoadFloat3(&floorMesh[indices[indIndex + 1]].pos);
				XMVECTOR v2_pos = XMLoadFloat3(&floorMesh[indices[indIndex + 2]].pos);

				XMVECTOR sideA = v1_pos - v0_pos;
				XMVECTOR sideB = v2_pos - v0_pos;

				XMVECTOR faceNormal = XMVector3Cross(sideA, sideB);

				normTotal += faceNormal;
				faceCount++;

				//there could only be 6 faces that a single vertex can influence, so quit early to save time
				if (faceCount == 6)
					break;
			}
		}
		

		//get the average of all the normals (avg = (n1 + n2 + n3..) / length(n1 + n2 + n3)
		normTotal = normTotal / XMVector3Length(normTotal);
	
		XMStoreFloat3(&floorMesh[vertIndex].norm, normTotal);

	

	}


	//create the vertex buffer
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosNormTex) * mVertexList.size(); //total size of buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = floorMesh;

	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);



	delete floorMesh;
	floorMesh = nullptr;
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


	mBoxLeftBound.init(ll, ul, 200, 1500);
	mBoxRightBound.init(ul, ur, 200, 1500);
	mBoxLowBound.init(ur, lr, 200, 1500);
	mBoxUpperBound.init(lr, ll, 200, 1500);

	XMFLOAT3 wll, wur, wul, wlr;
	wll.x = ll.x;
	wll.y = 0.0f;
	wll.z = ll.y;

	wur.x = ur.x;
	wur.y = 0.0f;
	wur.z = ur.y;

	wul.x = ul.x;
	wul.y = 0.0f;
	wul.z = ul.y;

	wlr.x = lr.x;
	wlr.y = 0.0f;
	wlr.z = lr.y;
	

	mDebugLBound.init(mD3DDevice, mBoxLeftBound.GetVertices(), XMFLOAT4(0.0f, 1.0f, 1.0f, 255.0f));
	mDebugUpperBound.init(mD3DDevice, mBoxUpperBound.GetVertices(), XMFLOAT4(255.0f, 0.0, 0.0f, 255.0f));
	mDebugRBound.init(mD3DDevice, mBoxRightBound.GetVertices(), XMFLOAT4(255.0f, 0.0, 0.0f, 255.0f));
	mDebugLowBound.init(mD3DDevice, mBoxLowBound.GetVertices(), XMFLOAT4(0.0f, 1.0f, 1.0f, 255.0f));

	mDebugSmallLBound.init(mD3DDevice, wll, 50.0f, 100.0f, XMFLOAT4(0.0f, 1.0f, 1.0f, 255.0f));
	mDebugSmallUpperBound.init(mD3DDevice, wur, 50.0f, 100.0f, L"..\\HardwareRasterizer\\Textures\\grass.bmp");
	mDebugSmallRBound.init(mD3DDevice, wul, 50.0f, 100.0f, L"..\\HardwareRasterizer\\Textures\\grass.bmp");
	mDebugSmallLowBound.init(mD3DDevice, wlr, 50.0f, 100.0f, L"..\\HardwareRasterizer\\Textures\\grass.bmp");
}

void Terrain::SetTexture(std::wstring texturePath)
{
	HRESULT hr = CreateWICTextureFromFile(mD3DDevice, texturePath.c_str(), NULL, &mTextureResourceView);
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


	float triOneErrorMargin = triOneArea*0.002f;

	//check if the sum of the partial triangle areas equal the area of the overall triangle
	if ((partialTri1Area + partialTri2Area + partialTri3Area >= triOneArea - triOneErrorMargin) && (partialTri1Area + partialTri2Area + partialTri3Area <= triOneArea + triOneErrorMargin))
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

	float triTwoErrorMargin = triTwoArea*0.002f;


	//check if the sum of the partial triangle areas equal the area of the overall triangle
	if ((partialTri1Area2 + partialTri2Area2 + partialTri3Area2 >= triTwoArea - triTwoErrorMargin) && (partialTri1Area2 + partialTri2Area2 + partialTri3Area2 <= triTwoArea + triTwoErrorMargin))
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
