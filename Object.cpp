#include "Object.h"
#include <WICTextureLoader.h>
#include "ConstantBuffers.h"
#include "InputLayouts.h"


void Object::init(ID3D11Device * device, DirectX::XMFLOAT3 position, const Vertex::PosNormTex * vertexList, UINT numVertices, std::vector<UINT>& indexList, std::wstring texturePath, float scale)
{
	mPosition = position;

	device->GetImmediateContext(&mDeviceContext);
	mD3DDevice = device;


	Vertex::PosNormTex* scaledVertices = new Vertex::PosNormTex[numVertices];

	
	for (UINT i = 0; i < numVertices; i++)
	{

		scaledVertices[i].pos = XMFLOAT3(vertexList[i].pos.x * scale, vertexList[i].pos.y * scale, vertexList[i].pos.z * scale);
		scaledVertices[i].norm = vertexList[i].norm;
		scaledVertices[i].uv = vertexList[i].uv;
	}

	std::vector<Vertex::PosNormTex> test(scaledVertices, scaledVertices + numVertices);


	//create a vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosNormTex) * numVertices; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = scaledVertices;
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);

	delete scaledVertices;
	scaledVertices = nullptr;


	//creat the index buffer
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DWORD) * indexList.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	InitData.pSysMem = &indexList[0];
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);

	HRESULT hr = CreateWICTextureFromFile(mD3DDevice, texturePath.c_str(), NULL, &mTextureResourceView);

	mNumVertices = numVertices;
	mNumIndices = indexList.size();
}

void Object::draw()
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



	mDeviceContext->VSSetShader(Shaders::VS_DirectionalLight, NULL, 0);
	mDeviceContext->PSSetShader(Shaders::PS_DirectionalLight, NULL, 0);
	mDeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);
	mDeviceContext->PSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);
	mDeviceContext->VSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);
	mDeviceContext->PSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);
	mDeviceContext->VSSetConstantBuffers(2, 1, &ConstantBuffers::DirectionalLightBuffer);
	mDeviceContext->PSSetConstantBuffers(2, 1, &ConstantBuffers::DirectionalLightBuffer);
	mDeviceContext->PSSetShaderResources(0, 1, &mTextureResourceView);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->IASetInputLayout(InputLayout::PosNormTex);
	mDeviceContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	mDeviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	mDeviceContext->DrawIndexed(mNumIndices, 0, 0);

}

Object::~Object()
{

	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mTextureResourceView);

}
