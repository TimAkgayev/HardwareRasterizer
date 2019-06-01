#pragma once
#include <string>
#include <vector>
#include "d3dcommon.h"
#include "Vertex.h"
#include <Bitmap.h>
#include "InputLayouts.h"
#include "Shaders.h"
#include "ConstantBuffers.h"
#include "CollisionBox.h"
#include "Box.h"

class Terrain 
{
public:
	Terrain();
	virtual ~Terrain();

	void draw();
	

	void CreateFromHeightMap(ID3D10Device* device, std::wstring pathToHeightmap, float floorScale = 1.0f, float heightScale = 1.0f);
	void GetHeightAtPosition(XMFLOAT3& playerPos, float** height);

	void GetBoundingCoordinates(XMFLOAT2* lowerLeft, XMFLOAT2* upperRight);
	void GetBoundPoints(XMFLOAT3**, XMFLOAT3**, XMFLOAT3**, XMFLOAT3**);
	void CreateCollisionBoxes();

	void SetTexture(std::wstring texturePath);

private:
	SoftwareBitmap::Bitmap* mHeightMap;
	float mFloorScale;
	float mHeightScale;

	int mNumIndices;

	std::vector<Vertex::PosTex> mVertexList;

	XMFLOAT2 mLowerLeftBound;
	XMFLOAT2 mUpperRightBound;

	XMFLOAT3 ll, lu, ru, rl;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	ID3D10ShaderResourceView* mTextureResourceView;

	CollisionBox mBoxLeftBound;
	CollisionBox mBoxRightBound;
	CollisionBox mBoxLowBound;
	CollisionBox mBoxUpperBound;

	Box mDebugLBound;
	Box mDebugRBound;
	Box mDebugLowBound;
	Box mDebugUpperBound;


};

