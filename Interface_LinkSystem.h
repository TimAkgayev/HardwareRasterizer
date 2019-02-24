#pragma once
#include <vector>
#include <map>
#include <DirectXMath.h>
using namespace DirectX;

class PositionLink
{
public: 
	PositionLink();
	virtual ~PositionLink();

	

	int GetLinkID();

	static void CreateOffsetPositionLink(PositionLink* Parent, PositionLink* Child);
	static void CreateOffsetPositionLink(int ParentID, int ChildID);
	static void _PositionLink_UpdateAllLinks();

private: 

	static std::vector<PositionLink*> mLinkList;
	static std::map<int, std::vector<int>> mLinkMap;
	static std::map<int, DirectX::XMVECTOR> mLastPositionMap;

	int mLinkId;

private:
	virtual void SetPosition(DirectX::XMVECTOR& position) = 0;
	virtual DirectX::XMVECTOR& GetPosition() = 0;
};
