#pragma once
#include "common_includes.h"
#include <SoftwareBitmap.h>
#include "CollisionBox.h"
#include "Box.h"



class Terrain : public Object
{
public:
	Terrain();
	virtual ~Terrain();

	virtual void Draw() override;

	void CreateFromHeightMap(ID3D11Device* device, std::wstring pathToHeightmap, float floorScale = 1.0f, float heightScale = 1.0f);
	void GetHeightAtPosition(XMFLOAT3& playerPos, float& outHeight);

	void GetBoundingCoordinates(XMFLOAT2* lowerLeft, XMFLOAT2* upperRight);
	void GetBoundPoints(XMFLOAT3**, XMFLOAT3**, XMFLOAT3**, XMFLOAT3**);
	void CreateCollisionBoxes();

	void SetHeightMap(std::wstring heightmapPath);
	void SetTexture(std::wstring texturePath);
	void RenderShadowMap();

private:

	void mDrawSceneForShadowMap();

	SoftwareBitmap::Bitmap* mHeightMap;
	float mFloorScale;
	float mHeightScale;

	std::vector<Vertex::PosNormTex> mVertexList;

	XMFLOAT2 mLowerLeftBound;
	XMFLOAT2 mUpperRightBound;

	XMFLOAT3 ll, lu, ru, rl;

	CollisionBox mBoxLeftBound;
	CollisionBox mBoxRightBound;
	CollisionBox mBoxLowBound;
	CollisionBox mBoxUpperBound;

	Box mDebugLBound;
	Box mDebugRBound;
	Box mDebugLowBound;
	Box mDebugUpperBound;

	Box mDebugSmallLBound;
	Box mDebugSmallRBound;
	Box mDebugSmallLowBound;
	Box mDebugSmallUpperBound;

	



};

