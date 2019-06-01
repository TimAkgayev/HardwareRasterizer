#pragma once
#include "d3dcommon.h"
#include <vector>
#include "Camera.h"
#include "InputLayouts.h"

using namespace DirectX;

class Skybox
{
public:
	Skybox();
	~Skybox();

	void init(ID3D10Device* device, float radius);
	void draw(const Camera& camera);


private:

	//prohibit copying
	Skybox(const Skybox& rhs);
	Skybox& operator=(const Skybox& rhs);

private:
	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10ShaderResourceView* mSkyCubeMapSRV;
	ID3D10RasterizerState* mRasterizerStateNoCull;

	UINT mNumIndices;

	void mSubdivide(std::vector<XMVECTOR>& vertices, std::vector<DWORD>& indices);
	void mBuildGeoSphere(UINT numSubdivisions, float radius, std::vector<XMVECTOR>& vertices, std::vector<DWORD>& indices);


};