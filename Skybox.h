#pragma once
#include "common_includes.h"
#include <vector>
#include "Camera.h"
#include "InputLayouts.h"

using namespace DirectX;

class Skybox
{
public:
	Skybox();
	~Skybox();

	void init(ID3D11Device* device, float radius);
	void draw(const Camera& camera);


private:

	//prohibit copying
	Skybox(const Skybox& rhs);
	Skybox& operator=(const Skybox& rhs);

private:
	ID3D11Device* mD3DDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;

	ID3D11ShaderResourceView* mSkyCubeMapSRV;
	ID3D11RasterizerState* mRasterizerStateNoCull;

	UINT mNumIndices;

	void mSubdivide(std::vector<XMVECTOR>& vertices, std::vector<DWORD>& indices);
	void mBuildGeoSphere(UINT numSubdivisions, float radius, std::vector<XMVECTOR>& vertices, std::vector<DWORD>& indices);


};