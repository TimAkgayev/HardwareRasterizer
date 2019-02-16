#pragma once
#include <vector>

enum {MESH_OBJECT = 0, LINE_OBJECT};

class WorldObject
{
public:
	WorldObject();
	unsigned int ObjectType;
	static std::vector<WorldObject*> ObjectList;
};

