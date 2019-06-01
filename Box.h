#pragma once
#include "d3dcommon.h"
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

	void init(ID3D10Device* device, float width, float height, XMFLOAT4 color);
	void init(ID3D10Device* device, float width, float height, std::wstring texturePath);
	void init(ID3D10Device* device, XMFLOAT3* eightPoints, XMFLOAT4 color);

	void SetIsWireframe(bool isWireframe);

	void draw();

private:

	DWORD mNumVertices;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10RasterizerState* mRasterizerStateWireframe;

	BOOL mIsWireframe;

	ID3D10ShaderResourceView* mTextureResourceView;
	XMFLOAT4 mColor;

};
