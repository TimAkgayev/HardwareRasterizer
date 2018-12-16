#pragma once
#include <vector>

enum {MESH_OBJECT};

class WorldObject
{
public:
	WorldObject();
	unsigned int ObjectType;
	static std::vector<WorldObject*> ObjectList;
};

