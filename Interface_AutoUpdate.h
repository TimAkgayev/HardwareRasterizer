#pragma once
#include <vector>


class AutoUpdate
{
public: 
	AutoUpdate();
	virtual ~AutoUpdate();

	static void _AutoUpdate_UpdateAll();

private:
	virtual void Update() = 0;
	static std::vector<AutoUpdate*> mAutoUpdateList;
};