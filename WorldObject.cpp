#include "WorldObject.h"
#include <WICTextureLoader.h>
#include "ConstantBuffers.h"
#include "tinyobj\tinyobjloader\tiny_obj_loader.h"


class index_comparator
{
public:
	bool operator()(const tinyobj::index_t& lhv, const tinyobj::index_t& rhv) const
	{
		return std::tie(lhv.vertex_index, lhv.normal_index, lhv.texcoord_index) < std::tie(rhv.vertex_index, rhv.normal_index, rhv.texcoord_index);
	}
};

WorldObject::WorldObject()
{

	mNoDoubleBlendDSS = nullptr;
	mD3DDevice = nullptr;
	mDeviceContext = nullptr;

	mVB = nullptr;
	mIB = nullptr;

	mRasterizerStateWireframe = nullptr;
	mTextureResourceView = nullptr;

}

void WorldObject::Initialize(ID3D11Device * device, DirectX::XMFLOAT3 position, const Vertex::PosNormTex * vertexList, UINT numVertices, std::vector<UINT>& indexList, std::wstring texturePath, float scale)
{
	mScale = scale;
	mPosition = position;

	device->GetImmediateContext(&mDeviceContext);
	mD3DDevice = device;


	//create a vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosNormTex) * numVertices; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertexList;
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);


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


	D3D11_DEPTH_STENCIL_DESC noDoubleBlendDesc;
	noDoubleBlendDesc.DepthEnable = true;
	noDoubleBlendDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlendDesc.DepthFunc = D3D11_COMPARISON_LESS;
	noDoubleBlendDesc.StencilEnable = true;
	noDoubleBlendDesc.StencilReadMask = 0xff;
	noDoubleBlendDesc.StencilWriteMask = 0xff;
	noDoubleBlendDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	// We are not rendering backfacing polygons, so these settings do not matter.
	noDoubleBlendDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	mD3DDevice->CreateDepthStencilState(&noDoubleBlendDesc, &mNoDoubleBlendDSS);

}

void WorldObject::Draw()
{
	UINT stride = sizeof(Vertex::PosNormTex);
	UINT offset = 0;



	
	ConstantBuffers::WorldMatrices worldBuffer;
	worldBuffer.World = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	XMMATRIX scale = XMMatrixScaling(mScale, mScale, mScale);
	worldBuffer.World = XMMatrixTranspose(XMMatrixMultiply(scale, worldBuffer.World));
	mDeviceContext->UpdateSubresource(ConstantBuffers::WorldMatrixBuffer, 0, NULL, &worldBuffer, 0, 0);


	mDeviceContext->PSSetShaderResources(0, 1, &mTextureResourceView);

	mDeviceContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	mDeviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

//	mDeviceContext->DrawIndexed(mNumIndices, 0, 0);

}

UINT WorldObject::GetNumberOfIndices()
{
	return mNumIndices;
}

WorldObject::~WorldObject()
{

	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
	ReleaseCOM(mTextureResourceView);

}

