#pragma once
#include <string>
#include "Mesh.h"

class Terrain : public Mesh
{
public:
	Terrain() {}
	virtual ~Terrain();
	void CreateFromHeightMap(std::wstring pathToHeightmap, float floorScale = 1.0f, float heightScale = 1.0f);
	void GetHeightAtPosition(float x, float y, float** height);

private:
	SoftwareBitmap::Bitmap* mHeightMap;
	float mFloorScale;
	float mHeightScale;
};