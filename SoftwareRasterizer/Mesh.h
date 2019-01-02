#pragma once
#include <vector>
#include "Vertex.h"
#include "ImageManager.h"

using namespace std;

typedef struct MESHOBJECT_TYP
{
	MESHOBJECT_TYP()
	{
		
	}

	MATRIX3D     worldTransform;
	vector<VERTEX2D> vertexBuffer;
	vector<int>      indexBuffer;
	Bitmap*   texture;


}MESHOBJECT, *MESHOBJECT_PTR;
