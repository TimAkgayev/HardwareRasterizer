#pragma once
#include "common_includes.h"
#include <string>
#include "Vertex.h"
#include "ConstantBuffers.h"
#include "Object.h"


using namespace DirectX;

class Box : public Object
{
public:
	Box();
	virtual ~Box();

	void Initialize(ID3D11Device* device, XMFLOAT3 position, float width, float height, XMFLOAT4 color);
	void Initialize(ID3D11Device* device, XMFLOAT3 position, float width, float height, std::wstring texturePath);
	void Initialize(ID3D11Device* device, const std::vector<XMVECTOR>& eightPoints, XMFLOAT4 color);

	void SetIsWireframe(bool isWireframe);

	virtual void Draw() override;

private:

	DWORD mNumVertices;
	DWORD mNumFaces;
	XMFLOAT3 mPosition;


	ID3D11RasterizerState* mRasterizerStateWireframe;

	BOOL mIsWireframe;
	BOOL mIsPretransformed;

	XMFLOAT4 mColor;

};
