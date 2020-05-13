#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

class CollisionBox
{
public:

	CollisionBox();
	~CollisionBox();


	void init(DirectX::XMFLOAT3 position, XMVECTOR xAxis, float width, float height);

	//allow collision box to be defined in the x-z plane with width and height of the wall
	void init(DirectX::XMFLOAT2 startXZ, DirectX::XMFLOAT2 endXZ, float wdith = 1.0f, float height = 100.0f);


	XMVECTOR GetXAxis() const;
	XMVECTOR GetYAxis() const;
	XMVECTOR GetZAxis() const;
	XMFLOAT3 GetExtents() const;
	XMVECTOR GetCenter() const;
	void     SetCenter(XMFLOAT3 pos);
	std::vector<XMVECTOR> GetVertices() const;


protected:

	XMVECTOR mXAxis;
	XMVECTOR mYAxis;
	XMVECTOR mZAxis;

	float mXExtent;
	float mYExtent;
	float mZExtent;

	XMVECTOR mCenter;


};


class PhysicsEngine
{
public:
	void AddStaticCollisionBox(CollisionBox& box);

	//check if player collision box intersects any other static collision box
	bool CheckForCollision(CollisionBox& playerCollisionBox, XMVECTOR fromLocation);

private:
	
	std::vector<CollisionBox> mStaticCollisionBoxList;

	bool _CheckIfProjectionsOverlapOnAxis(const XMVECTOR& boxA_Center, const XMVECTOR& boxA_xAxis, const XMVECTOR& boxA_yAxis, const XMVECTOR& boxA_zAxis, XMFLOAT3 boxA_xyzExtents, const XMVECTOR& boxB_Center, const XMVECTOR& boxB_xAxis, const XMVECTOR& boxB_yAxis, const XMVECTOR& boxB_zAxis, XMFLOAT3 boxB_xyzExtents, const XMVECTOR& projectionAxis);

};