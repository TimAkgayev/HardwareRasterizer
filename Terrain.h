#pragma once
#include "common_includes.h"
#include <SoftwareBitmap.h>
#include "PhysicsEngine.h"
#include "Box.h"
#include "tinyobj/tinyobjloader/tiny_obj_loader.h"

struct _bounding_box_tri
{
	XMFLOAT3 center;
	float    x_extent;
	float    y_extent;
	float    z_extent;

	UINT tri_index;
};

struct _bounding_box
{
	XMFLOAT3 center;
	float    x_extent;
	float    y_extent;
	float    z_extent;

};



struct _terrain_triangle
{
	Vertex::PosNormTex vertices[3];
};

struct _heap_node
{

	//the bounding box of this volume
	_bounding_box bounding_box;

	//the list of triangles this volume encompasses
	std::vector<_bounding_box_tri> triangleList;

	//left and right branches
	int childIndices[2];

	_heap_node()
	{
		childIndices[0] = -1;
		childIndices[1] = -1;
	}

};

/*
class _set : public IDrawable
{
public:
	_set();
	void SetDevice(ID3D11Device* device);
	virtual void Draw() override;


public:
	UINT setId;
	std::vector<UINT> offsetList;
	ID3D11Buffer* VB;
	UINT vertexCount;
	bool isBaked;

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mContext;
};


class ObjectSet
{
public:
	ObjectSet();
	void SetMasterListAndDevices(std::vector<_terrain_triangle>* masterList, ID3D11Device* device);
	void CreateNewSet(unsigned int Id);
	void AddObjectToSet(UINT setId, UINT objectOffset);
	void AddSetToDrawList(UINT setId);
	void Draw();

private:

	ID3D11Device* mD3DDevice;
	ID3D11DeviceContext* mContext;
	std::vector<_terrain_triangle>* mMasterList;
	std::vector<_set> mSetList;
	std::vector<UINT> mDrawList;
};

*/

class DebugBox : public Object
{
public:
	DebugBox();
	~DebugBox();

	void AddBox(const _bounding_box& box, UINT id);
	virtual void Draw() override;
	void Init(ID3D11Device* D3DDevice, ID3D11DeviceContext* DeviceContext);

private:
	std::vector<ID3D11Buffer*> mDebugBoxVB;
	ID3D11Buffer* mDebugBoxIB;

	std::vector<XMFLOAT3> mPositionList;
	std::vector<XMFLOAT3> mScaleList;
};

class Terrain : public Object
{
public:
	Terrain(ID3D11Device* device, PhysicsEngine* physicsEngine);
	virtual ~Terrain();

	virtual void Draw() override;

	void CreateFromHeightMap(ID3D11Device* device, std::wstring pathToHeightmap, float floorScale = 1.0f, float heightScale = 1.0f);
	void LoadFromFile(std::string filename);
	void GetHeightAtPosition(XMFLOAT3& playerPos, float& outHeight);
	void GetBoundingRect(XMFLOAT2& outLowerLeft, XMFLOAT2& outUpperRight) const;

	void SetTexture(std::wstring texturePath);

private:
	
	//ObjectSet mObjectSet;
	DebugBox mDbgBox;
	SoftwareBitmap::Bitmap* mHeightMap;

	struct frect_t
	{
		XMFLOAT2 lowerLeft;
		XMFLOAT2 upperRight;
	}mBoundingRect;

	PhysicsEngine* mPhysicsEngine;
	
	std::vector<_heap_node> mBVHTree;
	Vertex::PosNormTex*		mVertices;
	int					    mNumVertices;
	std::vector<DWORD>      mIndexList;
	std::vector<_terrain_triangle> mTriangleList;

	void _CreateTerrainVertexBuffersAndGenerateBVHTree(tinyobj::shape_t& shape , tinyobj::attrib_t& attrib);
	void _CreateBoundaryCollisionBoxes();
	void _CreateCollisionBoxes(std::vector<tinyobj::shape_t>& shapes, tinyobj::attrib_t& attrib, std::vector<UINT>& collisionBoxIndexList);
};

