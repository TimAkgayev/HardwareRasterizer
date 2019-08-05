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
	XMFLOAT3 GetExtents() const;
	XMVECTOR GetCenter() const;
	void     SetCenter(XMFLOAT3 pos);
	std::vector<XMVECTOR> GetVertices() const;

	

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
	
	std::vector<XMVECTOR> mProjectionAxesShape1;
	std::vector<XMVECTOR> mProjectionAxesShape2;

private:
	static std::vector<CollisionBox*> mCollisionBoxList;
	bool mCheckOverlapOnAxis(const XMVECTOR& boxCenter, const XMVECTOR& xAxis, const XMVECTOR& yAxis, const XMVECTOR& zAxis, XMFLOAT3 xyzExtents, const XMVECTOR& projectionAxis);

	bool mCheckForCollision(const CollisionBox& box);

	void mGetAxes(const CollisionBox& box1, const CollisionBox& box2);

};