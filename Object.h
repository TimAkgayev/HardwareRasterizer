#pragma once
#include "common_includes.h"
#include "Vertex.h"

class Object
{
public:
	void init(ID3D11Device* device, DirectX::XMFLOAT3 position, const Vertex::PosNormTex* vertexList, UINT numVerteices, std::vector<UINT>& indexList, std::wstring texturePath, float scale = 1.0f);
	void draw();

	virtual ~Object();

private:

	UINT mNumVertices;
	UINT mNumIndices;

	XMFLOAT3 mPosition;


	ID3D11Device* mD3DDevice;
	ID3D11DeviceContext* mDeviceContext;

	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	ID3D11RasterizerState* mRasterizerStateWireframe;
	ID3D11ShaderResourceView* mTextureResourceView;

};