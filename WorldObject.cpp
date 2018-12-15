#include "WorldObject.h"

WorldObject::WorldObject()
{
	ObjectList.push_back(this);
}

std::vector<WorldObject*> WorldObject::ObjectList;