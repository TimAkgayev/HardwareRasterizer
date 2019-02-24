#include "Interface_LinkSystem.h"

std::vector<PositionLink*> PositionLink::mLinkList;
std::map<int, std::vector<int>> PositionLink::mLinkMap;
std::map<int, DirectX::XMVECTOR> PositionLink::mLastPositionMap;

PositionLink::PositionLink()
{
	mLinkId = mLinkList.size();
	mLinkList.push_back(this);

}

PositionLink::~PositionLink()
{
	for (std::vector<PositionLink*>::iterator vIter; vIter < mLinkList.end(); vIter++)
	{
		if (*vIter == this)
			mLinkList.erase(vIter);
	}
}

int PositionLink::GetLinkID()
{
	return mLinkId;
}

void PositionLink::CreateOffsetPositionLink(PositionLink * Parent, PositionLink * Child)
{
	CreateOffsetPositionLink(Parent->GetLinkID(), Child->GetLinkID());
}

void PositionLink::CreateOffsetPositionLink(int ParentID, int ChildID)
{
	
	//check if parent map already exists
	auto search = mLinkMap.find(ParentID);
	if (search == mLinkMap.end()) {
		// not found
		std::vector<int> newChildList;
		newChildList.push_back(ChildID);
		mLinkMap.insert(make_pair(ParentID, newChildList));

		DirectX::XMVECTOR lastPos = mLinkList[ParentID]->GetPosition();
		mLastPositionMap.insert(std::make_pair(ParentID, lastPos));
	}
	else {
		// found
		search->second.push_back(ChildID);
	}

}

void PositionLink::_PositionLink_UpdateAllLinks()
{
	for(auto const& existingMap :  mLinkMap)
	{

		//get position delta
		DirectX::XMVECTOR posDelta = mLinkList[existingMap.first]->GetPosition() - mLastPositionMap[existingMap.first];

		//update the position of the children
		for(auto childID : existingMap.second)
			mLinkList[childID]->SetPosition(mLinkList[childID]->GetPosition() + posDelta);
		
		//update the last position of the parent
		auto search = mLastPositionMap.find(existingMap.first);
		if (search != mLastPositionMap.end()) {
			search->second = mLinkList[existingMap.first]->GetPosition();
		}
	}
}
