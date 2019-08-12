#include "Box.h"
#include <WICTextureLoader.h>

Box::Box()
	:mIsWireframe(false), mIsPretransformed(false)
{
	
}

Box::~Box()
{
	
}


void Box::Initialize(ID3D11Device* device, XMFLOAT3 position, float width, float height, XMFLOAT4 color)
{

	
	mPosition = position;


	device->GetImmediateContext(&mDeviceContext);
	mD3DDevice = device;

	Vertex::PosColor mesh[] =
	{
		
		{ XMFLOAT3(-width / 2.0f, height / 2.0f, -width / 2.0f), color },
		{ XMFLOAT3(width / 2.0f, height / 2.0f, -width / 2.0f), color },
		{ XMFLOAT3(width / 2.0f, height / 2.0f, width / 2.0f), color },
		{ XMFLOAT3(-width / 2.0f, height / 2.0f, width / 2.0f), color },
		{ XMFLOAT3(-width / 2.0f, -height / 2.0f, -width / 2.0f), color },
		{ XMFLOAT3(width / 2.0f, -height / 2.0f, -width / 2.0f), color },
		{ XMFLOAT3(width / 2.0f, -height / 2.0f, width / 2.0f), color },
		{ XMFLOAT3(-width / 2.0f, -height / 2.0f, width / 2.0f), color }
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

	mNumIndices = sizeof(indices) / sizeof(DWORD);


	//create a vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosColor) * 8; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &mesh;
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);


	//creat the index buffer
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DWORD) * 36;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	InitData.pSysMem = &(indices);
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);

	ReleaseCOM(mTextureResourceView);
	mColor = color;

}

void Box::Initialize(ID3D11Device * device, XMFLOAT3 position, float width, float height, std::wstring texturePath)
{
	mPosition = position;

	device->GetImmediateContext(&mDeviceContext);
	mD3DDevice = device;

	Vertex::PosNormTex mesh[] =
	{
		{ XMFLOAT3(-width / 2.0f, height / 2.0f, -width / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(width / 2.0f, height / 2.0f, -width / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(width / 2.0f, height / 2.0f, width / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-width / 2.0f, height / 2.0f, width / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-width / 2.0f, -height / 2.0f, -width / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(width / 2.0f, -height / 2.0f, -width / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(width / 2.0f, -height / 2.0f, width / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-width / 2.0f, -height / 2.0f, width / 2.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),XMFLOAT2(0.5f, 0.0f) }
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

	mNumIndices = sizeof(indices) / sizeof(DWORD);


	//create a vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosNormTex) * 8; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &mesh;
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);


	//creat the index buffer
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DWORD) * 36;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	InitData.pSysMem = &(indices);
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);
	
	HRESULT hr = CreateWICTextureFromFile(mD3DDevice, texturePath.c_str(), NULL, &mTextureResourceView);

}

void Box::Initialize(ID3D11Device * device, const std::vector<XMVECTOR>& eightPoints, XMFLOAT4 color)
{
	mIsPretransformed = true;

	device->GetImmediateContext(&mDeviceContext);
	mD3DDevice = device;

	XMFLOAT3 points[8];
	for (int i = 0; i < 8; i++)
		XMStoreFloat3(&points[i], eightPoints[i]);

	Vertex::PosColor mesh[] =
	{

		{ points[0], color },
		{ points[1], color },
		{ points[2], color },
		{ points[3], color },
		{ points[4], color },
		{ points[5], color },
		{ points[6], color },
		{ points[7], color }
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

	mNumIndices = sizeof(indices) / sizeof(DWORD);

	//create a vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosColor) * 8; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &mesh;
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);


	//creat the index buffer
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DWORD) * 36;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	InitData.pSysMem = &(indices);
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);

	ReleaseCOM(mTextureResourceView);
	mColor = color;



	D3D11_RASTERIZER_DESC rasterizerStateWireframe;
	rasterizerStateWireframe.CullMode = D3D11_CULL_NONE;
	rasterizerStateWireframe.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerStateWireframe.FrontCounterClockwise = false;
	rasterizerStateWireframe.DepthBias = false;
	rasterizerStateWireframe.DepthBiasClamp = 0;
	rasterizerStateWireframe.SlopeScaledDepthBias = 0;
	rasterizerStateWireframe.DepthClipEnable = true;
	rasterizerStateWireframe.ScissorEnable = false;
	rasterizerStateWireframe.MultisampleEnable = false;
	rasterizerStateWireframe.AntialiasedLineEnable = true;
	mD3DDevice->CreateRasterizerState(&rasterizerStateWireframe, &mRasterizerStateWireframe);


}

void Box::SetIsWireframe(bool isWireframe)
{
	mIsWireframe = isWireframe;
}


void Box::Draw()
{


	UINT stride;
	UINT offset = 0;
	

	if (mTextureResourceView)
	{
		stride = sizeof(Vertex::PosNormTex);
		mDeviceContext->PSSetShaderResources(0, 1, &mTextureResourceView);
	

	}
	else
	{
		stride = sizeof(Vertex::PosColor);

	}

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	mDeviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	if (!mIsPretransformed)
	{
		ConstantBuffers::WorldMatrices worldMat;
		worldMat.World = XMMatrixTranspose(XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z));
		mDeviceContext->UpdateSubresource(ConstantBuffers::WorldMatrixBuffer, 0, NULL, &worldMat, 0, 0);
	}


	ID3D11RasterizerState* oldRasterizerState;
	if (mIsWireframe)
	{
		mDeviceContext->RSGetState(&oldRasterizerState);
		mDeviceContext->RSSetState(mRasterizerStateWireframe);
	}



	mDeviceContext->DrawIndexed(mNumIndices, 0, 0);

	if (mIsWireframe)
		mDeviceContext->RSSetState(oldRasterizerState);

		
}