#include "Skybox.h"
#include "Vertex.h"
#include "ConstantBuffers.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

Skybox::Skybox()
	:mSkyCubeMapSRV(0)
{

}

Skybox::~Skybox()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mSkyCubeMapSRV);
}

void Skybox::mSubdivide(std::vector<XMVECTOR>& vertices, std::vector<DWORD>& indices)
{
	std::vector<XMVECTOR> vin = vertices;
	std::vector<DWORD>  iin = indices;

	vertices.resize(0);
	indices.resize(0);


	UINT numTris = (UINT)iin.size() / 3;
	for (UINT i = 0; i < numTris; ++i)
	{
		XMVECTOR v0 = vin[iin[i * 3 + 0]];
		XMVECTOR v1 = vin[iin[i * 3 + 1]];
		XMVECTOR v2 = vin[iin[i * 3 + 2]];

		XMVECTOR m0 = 0.5f*(v0 + v1);
		XMVECTOR m1 = 0.5f*(v1 + v2);
		XMVECTOR m2 = 0.5f*(v0 + v2);

		vertices.push_back(v0); // 0
		vertices.push_back(v1); // 1
		vertices.push_back(v2); // 2
		vertices.push_back(m0); // 3
		vertices.push_back(m1); // 4
		vertices.push_back(m2); // 5

		indices.push_back(i * 6 + 0);
		indices.push_back(i * 6 + 3);
		indices.push_back(i * 6 + 5);

		indices.push_back(i * 6 + 3);
		indices.push_back(i * 6 + 4);
		indices.push_back(i * 6 + 5);

		indices.push_back(i * 6 + 5);
		indices.push_back(i * 6 + 4);
		indices.push_back(i * 6 + 2);

		indices.push_back(i * 6 + 3);
		indices.push_back(i * 6 + 1);
		indices.push_back(i * 6 + 4);
	}
}

void Skybox::mBuildGeoSphere(UINT numSubdivisions, float radius, std::vector<XMVECTOR>& vertices, std::vector<DWORD>& indices)
{


	// Put a cap on the number of subdivisions.
	numSubdivisions = Min(numSubdivisions, UINT(5));

	// Approximate a sphere by tesselating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;



	XMFLOAT3 posF[12] =
	{
		XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
	};

	XMVECTOR pos[12];
	for (int i = 0; i < 12; i++)
		pos[i] = XMLoadFloat3(&posF[i]);


	DWORD k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};


	vertices.resize(12);
	indices.resize(60);

	for (int i = 0; i < 12; ++i)
		vertices[i] = pos[i];

	for (int i = 0; i < 60; ++i)
		indices[i] = k[i];

	for (UINT i = 0; i < numSubdivisions; ++i)
		mSubdivide(vertices, indices);

	// Project vertices onto sphere and scale.
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		vertices[i] = XMVector3Normalize(vertices[i]);
		vertices[i] *= radius;
	}

}



void Skybox::init(ID3D11Device* device, float radius)
{
	mD3DDevice = device;
	mD3DDevice->GetImmediateContext(&mDeviceContext);
	
	std::wstring GrassLandCubeMapPath = L"C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\CubeMapLuna\\grassenvmap1024.dds";
	std::wstring CityCubeMapPath = L"C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\HardwareRasterizer\\Textures\\CityCubeMap.dds";
	std::wstring TropicalCubeMapPath = L"C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\HardwareRasterizer\\Textures\\TropicalCubeMap.dds";


	//create a skybox ============================================================================
	//HRESULT hr = D3DX11CreateShaderResourceViewFromFile(mD3DDevice, L"C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\CubeMapLuna\\grassenvmap1024.dds", 0, 0, &mSkyCubeMapSRV, 0);
	HRESULT hr = CreateDDSTextureFromFile(mD3DDevice, GrassLandCubeMapPath.c_str(), 0, &mSkyCubeMapSRV);

	std::vector<XMVECTOR> vertices;
	std::vector<DWORD> indices;

	mBuildGeoSphere(2, radius, vertices, indices);

	std::vector<Vertex::Skybox> skyVerts(vertices.size());
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		// Scale on y-axis to turn into an ellipsoid to make a flatter Sky surface
		XMStoreFloat3(&skyVerts[i].pos, 0.5f*vertices[i]);
		//skyVerts[i].color = XMFLOAT4(1.0f, 0, 0, 1.0f);
	}

	//set up rasterizer flags ============================================
	D3D11_RASTERIZER_DESC rasterizerStateSolid;
	rasterizerStateSolid.CullMode = D3D11_CULL_NONE;
	rasterizerStateSolid.FillMode = D3D11_FILL_SOLID;
	rasterizerStateSolid.FrontCounterClockwise = false;
	rasterizerStateSolid.DepthBias = false;
	rasterizerStateSolid.DepthBiasClamp = 0;
	rasterizerStateSolid.SlopeScaledDepthBias = 0;
	rasterizerStateSolid.DepthClipEnable = true;
	rasterizerStateSolid.ScissorEnable = false;
	rasterizerStateSolid.MultisampleEnable = false;
	rasterizerStateSolid.AntialiasedLineEnable = true;
	mD3DDevice->CreateRasterizerState(&rasterizerStateSolid, &mRasterizerStateNoCull);




	//float width = 500.0f;
	//float height = 500.0f;

	/*
	Vertex::PosTex mesh[] =
	{
		{ XMFLOAT3(-width / 2.0f, height / 2.0f, -width / 2.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(width / 2.0f, height / 2.0f, -width / 2.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(width / 2.0f, height / 2.0f, width / 2.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-width / 2.0f, height / 2.0f, width / 2.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-width / 2.0f, -height / 2.0f, -width / 2.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(width / 2.0f, -height / 2.0f, -width / 2.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(width / 2.0f, -height / 2.0f, width / 2.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-width / 2.0f, -height / 2.0f, width / 2.0f), XMFLOAT2(0.5f, 0.0f) }
	};


	//create index buffer
	DWORD indices[] =
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

	*/


	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Skybox) * (UINT)skyVerts.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &skyVerts[0];
	HandleError(mD3DDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	mNumIndices = (UINT)indices.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumIndices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
	HandleError(mD3DDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Skybox::Draw()
{

	// center Sky about eye in world space
	XMFLOAT3 eyePos = XMFLOAT3(1.0f, 2.0f, 3.0f);
	
	
	ConstantBuffers::WorldMatrices worldMat;
	XMMATRIX transl = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);
	worldMat.World = XMMatrixTranspose(transl);

	mDeviceContext->UpdateSubresource(ConstantBuffers::WorldMatrixBuffer, 0, NULL, &worldMat, 0, 0);


	UINT stride = sizeof(Vertex::Skybox);
	UINT offset = 0;

	ID3D11RasterizerState* oldState;
	mDeviceContext->RSGetState(&oldState);

	mDeviceContext->RSSetState(mRasterizerStateNoCull);

	mDeviceContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	mDeviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	
	mDeviceContext->PSSetShaderResources(1, 1, &mSkyCubeMapSRV);

	mDeviceContext->DrawIndexed(mNumIndices, 0, 0);
	
	mDeviceContext->RSSetState(oldState);
}
