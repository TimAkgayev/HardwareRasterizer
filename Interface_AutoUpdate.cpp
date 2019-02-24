#include "Interface_AutoUpdate.h"

std::vector<AutoUpdate*> AutoUpdate::mAutoUpdateList;

AutoUpdate::AutoUpdate()
{
	mAutoUpdateList.emplace_back(this);
}

AutoUpdate::~AutoUpdate()
{
	for (std::vector<AutoUpdate*>::iterator vIter; vIter < mAutoUpdateList.end(); vIter++)
	{
		if (*vIter == this)
			mAutoUpdateList.erase(vIter);
	}
}

void AutoUpdate::_AutoUpdate_UpdateAll()
{
	for (AutoUpdate* obj : mAutoUpdateList)
	{
		obj->Update();
	}
}
