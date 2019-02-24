#include "Physics.h"

std::vector<CollisionBox*> CollisionBox::mCollisionBoxList;

CollisionBox::~CollisionBox()
{
	for (std::vector<CollisionBox*>::iterator vIter; vIter < mCollisionBoxList.end(); vIter++)
	{
		if (*vIter == this)
			mCollisionBoxList.erase(vIter);
	}
}

CollisionBox::CollisionBox() 
{
	mPosition = { 0.0f, 0.0f, 0.0f, 0.0f };
	mWidth = 0;
	mHeight = 0;

	mCollisionBoxList.push_back(this);
}

CollisionBox::CollisionBox(XMVECTOR position, float width, float height)
{
	mPosition = position;
	mWidth = width;
	mHeight = height;

	mCollisionBoxList.push_back(this);

}


void CollisionBox::SetPosition(XMVECTOR & position)
{
	mPosition = position;
}

XMVECTOR & CollisionBox::GetPosition()
{
	return mPosition;
}
