#pragma once
#include "common_includes.h"
#include <vector>
#include "WorldObject.h"


using namespace DirectX;

class Skybox : public WorldObject
{
public:
	Skybox();
	~Skybox();

	void init(ID3D11Device* device, float radius);
	virtual void Draw() override;


private:

	//prohibit copying
	Skybox(const Skybox& rhs);
	Skybox& operator=(const Skybox& rhs);

private:

	ID3D11ShaderResourceView* mSkyCubeMapSRV;
	ID3D11RasterizerState* mRasterizerStateNoCull;


	void mSubdivide(std::vector<XMVECTOR>& vertices, std::vector<DWORD>& indices);
	void mBuildGeoSphere(UINT numSubdivisions, float radius, std::vector<XMVECTOR>& vertices, std::vector<DWORD>& indices);

};