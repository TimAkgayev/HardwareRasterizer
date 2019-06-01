#include "Box.h"


Box::Box()
	: mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0), mIsWireframe(false), mTextureResourceView(nullptr)
{
	
}

Box::~Box()
{
	if (mVB)
	{
		mVB->Release();
		mVB = nullptr;
	}
	if (mIB)
	{
		mIB->Release();
		mIB = nullptr;
	}
}


void Box::init(ID3D10Device* device, float width, float height, XMFLOAT4 color)
{

	md3dDevice = device;

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



	//create a vertex buffer
	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosColor) * 8; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &mesh;
	md3dDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);


	//creat the index buffer
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DWORD) * 36;
	bufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	InitData.pSysMem = &(indices);
	md3dDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);

	ReleaseCOM(mTextureResourceView);
	mColor = color;

}

void Box::init(ID3D10Device * device, float width, float height, std::wstring texturePath)
{

	md3dDevice = device;

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



	//create a vertex buffer
	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosTex) * 8; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &mesh;
	md3dDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);


	//creat the index buffer
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DWORD) * 36;
	bufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	InitData.pSysMem = &(indices);
	md3dDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);
	
	D3DX10CreateShaderResourceViewFromFile(md3dDevice, texturePath.c_str(), NULL, NULL, &mTextureResourceView, NULL);
}

void Box::init(ID3D10Device * device, XMFLOAT3 * eightPoints, XMFLOAT4 color)
{


	md3dDevice = device;

	Vertex::PosColor mesh[] =
	{

		{ eightPoints[0], color },
		{ eightPoints[1], color },
		{ eightPoints[2], color },
		{ eightPoints[3], color },
		{ eightPoints[4], color },
		{ eightPoints[5], color },
		{ eightPoints[6], color },
		{ eightPoints[7], color }
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



	//create a vertex buffer
	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosColor) * 8; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &mesh;
	md3dDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);


	//creat the index buffer
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DWORD) * 36;
	bufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	InitData.pSysMem = &(indices);
	md3dDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);

	ReleaseCOM(mTextureResourceView);
	mColor = color;



	D3D10_RASTERIZER_DESC rasterizerStateWireframe;
	rasterizerStateWireframe.CullMode = D3D10_CULL_NONE;
	rasterizerStateWireframe.FillMode = D3D10_FILL_WIREFRAME;
	rasterizerStateWireframe.FrontCounterClockwise = false;
	rasterizerStateWireframe.DepthBias = false;
	rasterizerStateWireframe.DepthBiasClamp = 0;
	rasterizerStateWireframe.SlopeScaledDepthBias = 0;
	rasterizerStateWireframe.DepthClipEnable = true;
	rasterizerStateWireframe.ScissorEnable = false;
	rasterizerStateWireframe.MultisampleEnable = false;
	rasterizerStateWireframe.AntialiasedLineEnable = true;
	md3dDevice->CreateRasterizerState(&rasterizerStateWireframe, &mRasterizerStateWireframe);


}

void Box::SetIsWireframe(bool isWireframe)
{
	mIsWireframe = isWireframe;
}


void Box::draw()
{
	UINT stride;
	UINT offset = 0;
	
	if (mTextureResourceView)
	{
		stride = sizeof(Vertex::PosTex);
		
		md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		md3dDevice->VSSetShader(Shaders::VS_SimpleProjection);
		md3dDevice->PSSetShader(Shaders::PS_SimpleTexture);
		md3dDevice->VSSetConstantBuffers(0, 1, &ConstantBuffers::ViewWorldProjBuffer);
		md3dDevice->PSSetConstantBuffers(0, 1, &ConstantBuffers::ViewWorldProjBuffer);
		md3dDevice->PSSetShaderResources(0, 1, &mTextureResourceView);
		md3dDevice->IASetInputLayout(InputLayout::PosTex);
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);


	}
	else
	{
		stride = sizeof(Vertex::PosColor);

		md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		md3dDevice->IASetInputLayout(InputLayout::PosColor);
		md3dDevice->VSSetShader(Shaders::VS_SimpleProjectionColor);
		md3dDevice->PSSetShader(Shaders::PS_SimpleColor);
		md3dDevice->VSSetConstantBuffers(0, 1, &ConstantBuffers::ViewWorldProjBuffer);
		md3dDevice->PSSetConstantBuffers(0, 1, &ConstantBuffers::ViewWorldProjBuffer);
		md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
		md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	}

	ID3D10RasterizerState* oldRasterizerState;
	if (mIsWireframe)
	{
		md3dDevice->RSGetState(&oldRasterizerState);
		md3dDevice->RSSetState(mRasterizerStateWireframe);
	}

	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);


	md3dDevice->DrawIndexed(36, 0, 0);

	if (mIsWireframe)
		md3dDevice->RSSetState(oldRasterizerState);
}