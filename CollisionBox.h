#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;



class CollisionBox 
{
public:
	
	CollisionBox();
	~CollisionBox();


	//allow collision box to be defined in the x-z plane with width and height of the wall
	void init(DirectX::XMFLOAT3 position, XMVECTOR xAxis, float width, float height);
	void init(DirectX::XMFLOAT2 startXZ, DirectX::XMFLOAT2 endXZ, float wdith = 1.0f, float height = 100.0f);
	




	XMVECTOR GetXAxis() const;
	XMVECTOR GetYAxis() const;
	XMVECTOR GetZAxis() const;
	XMVECTOR GetExtents() const;
	XMVECTOR GetCenter() const;

	void SetPosition(XMFLOAT3 pos);

public:

	bool checkForCollision();
	bool checkForCollision(XMVECTOR fromLocation);


protected:

	XMVECTOR mXAxis;
	XMVECTOR mYAxis;
	XMVECTOR mZAxis;

	float mXExtent;
	float mYExtent;
	float mZExtent;

	XMVECTOR mCenter;
	
private:
	static std::vector<CollisionBox*> mCollisionBoxList;

	bool mCheckForCollision(const CollisionBox& box);
	bool mGetSeparatingPlane(const XMVECTOR& RPos, const XMVECTOR& Plane, const XMVECTOR& AxisX, const XMVECTOR& AxisY, const XMVECTOR& AxisZ, const XMVECTOR& Extents);

};