#pragma once
#include "common_includes.h"
#include "Vertex.h"

class IDrawable
{
public:
	virtual void Draw() = 0;
};

class Object : public IDrawable
{
public:
	Object();
	virtual void Initialize(ID3D11Device* device, DirectX::XMFLOAT3 position, const Vertex::PosNormTex* vertexList, UINT numVerteices, std::vector<UINT>& indexList, std::wstring texturePath, float scale = 1.0f);
	virtual void Draw() override;
	virtual UINT GetNumberOfIndices();

	virtual ~Object();

protected:

	UINT mNumVertices;
	UINT mNumIndices;

	XMFLOAT3 mPosition;
	float mScale;

	ID3D11DepthStencilState* mNoDoubleBlendDSS;
	ID3D11Device* mD3DDevice;
	ID3D11DeviceContext* mDeviceContext;

	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	ID3D11RasterizerState*    mRasterizerStateWireframe;
	ID3D11ShaderResourceView* mTextureResourceView;

};