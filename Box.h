#pragma once
#include "common_includes.h"
#include "InputLayouts.h"
#include <string>
#include "Vertex.h"
#include "ConstantBuffers.h"


using namespace DirectX;

class Box
{
public:
	Box();
	~Box();

	void init(ID3D11Device* device, XMFLOAT3 position, float width, float height, XMFLOAT4 color);
	void init(ID3D11Device* device, XMFLOAT3 position, float width, float height, std::wstring texturePath);
	void init(ID3D11Device* device, const std::vector<XMVECTOR>& eightPoints, XMFLOAT4 color);

	void SetIsWireframe(bool isWireframe);

	void draw();

private:

	DWORD mNumVertices;
	DWORD mNumFaces;
	XMFLOAT3 mPosition;


	ID3D11Device* mD3DDevice;
	ID3D11DeviceContext* mDeviceContext;

	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	ID3D11RasterizerState* mRasterizerStateWireframe;

	BOOL mIsWireframe;
	BOOL mIsPretransformed;

	ID3D11ShaderResourceView* mTextureResourceView;
	XMFLOAT4 mColor;



};
