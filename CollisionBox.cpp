#include "CollisionBox.h"

std::vector<CollisionBox*> CollisionBox::mCollisionBoxList;

void CollisionBox::init(DirectX::XMFLOAT2 startXZ, DirectX::XMFLOAT2 endXZ, float width, float height)
{
	XMVECTOR startXZv = XMLoadFloat3(&XMFLOAT3(startXZ.x, 0.0f, startXZ.y));
	XMVECTOR endXZv = XMLoadFloat3(&XMFLOAT3(endXZ.x, 0.0f, endXZ.y));

	//find the x axis 
	XMFLOAT3 temp;
	temp.x = endXZ.x - startXZ.x;
	temp.z = endXZ.y - startXZ.y;
	temp.y = 0.0f;
	XMVECTOR xAxis = XMLoadFloat3(&temp);

	//get the length
	XMVECTOR xAxisLength = XMVector3Length(xAxis);

	//make it a unit vector
	xAxis = XMVector3Normalize(xAxis);
	
	//y is always up
	XMVECTOR yAxis = { 0.0f, 1.0f, 0.0f };
	
	//get the z axis by crossing x and y
	XMVECTOR zAxis = XMVector3Cross(xAxis, yAxis);


	XMFLOAT3 xAxisF, yAxisF, zAxisF;
	XMFLOAT3 xAxisLengthF;
	XMStoreFloat3(&xAxisF, xAxis);
	XMStoreFloat3(&yAxisF, yAxis);
	XMStoreFloat3(&zAxisF, zAxis);
	XMStoreFloat3(&xAxisLengthF, xAxisLength);

	//get the x-extent (half length of the axis)
	float xExtent = xAxisLengthF.x * 0.5f;
	float yExtent = height * 0.5f;
	float zExtent = width  * 0.5f;

	//get the center
	XMVECTOR center = startXZv + xAxis * xExtent;
	XMFLOAT3 centerF;
	XMStoreFloat3(&centerF, center);

	//set all the member variables
	mXAxis = xAxis;
	mYAxis = yAxis;
	mZAxis = zAxis;
	mXExtent = xExtent;
	mYExtent = yExtent;
	mZExtent = zExtent;
	mCenter = center;

	mCollisionBoxList.push_back(this);
}

CollisionBox::CollisionBox()
{
}

void CollisionBox::init(XMFLOAT3 position, XMVECTOR xAxis, float width, float height)
{

	//get the length
	XMVECTOR xAxisLength = XMVector2Length(xAxis);

	//make it a unit vector
	xAxis = XMVector2Normalize(xAxis);

	//y is always up
	XMVECTOR yAxis = { 0.0f, 1.0f, 0.0f };

	//get the z axis by crossing x and y
	XMVECTOR zAxis = XMVector3Cross(xAxis, yAxis);


	XMFLOAT3 xAxisF, yAxisF, zAxisF;
	XMFLOAT3 xAxisLengthF;
	XMStoreFloat3(&xAxisF, xAxis);
	XMStoreFloat3(&yAxisF, yAxis);
	XMStoreFloat3(&zAxisF, zAxis);
	XMStoreFloat3(&xAxisLengthF, xAxisLength);

	//get the x-extent (half length of the axis)
	float xExtent = width  * 0.5f;
	float yExtent = height * 0.5f;
	float zExtent = width  * 0.5f;

	XMVECTOR positionV = XMLoadFloat3(&position);

	// The center can be computed as follows (assuming the endpoints lie on the
	// ground plane as in your images).
	XMVECTOR center = positionV;
	XMFLOAT3 centerF;
	XMStoreFloat3(&centerF, center);

	//set all the member variables
	mXAxis = xAxis;
	mYAxis = yAxis;
	mZAxis = zAxis;
	mXExtent = xExtent;
	mYExtent = yExtent;
	mZExtent = zExtent;
	mCenter = center;

	mCollisionBoxList.push_back(this);

}



CollisionBox::~CollisionBox()
{
	
}


XMVECTOR CollisionBox::GetXAxis() const
{
	return mXAxis;
}

XMVECTOR CollisionBox::GetYAxis() const
{
	return mYAxis;
}

XMVECTOR CollisionBox::GetZAxis() const
{
	return mZAxis;
}

XMVECTOR CollisionBox::GetExtents() const
{
	XMVECTOR extents = { mXExtent, mYExtent, mZExtent };
	return extents;
}

XMVECTOR CollisionBox::GetCenter() const
{
	return mCenter;
}

void CollisionBox::SetPosition(XMFLOAT3 newPos)
{
	mCenter = XMLoadFloat3(&newPos);

}


// check if there's a separating plane in between the selected axes
bool CollisionBox::mGetSeparatingPlane(const XMVECTOR& RPos, const XMVECTOR& Plane, const XMVECTOR& AxisX, const XMVECTOR& AxisY, const XMVECTOR& AxisZ, const XMVECTOR& Extents)
{
	XMFLOAT3 ExtentsF;
	XMStoreFloat3(&ExtentsF, Extents);

	XMVECTOR PosPlaneDot = XMVector3Dot(RPos, Plane);
	XMFLOAT3 PosPlaneDotF;
	XMStoreFloat3(&PosPlaneDotF, PosPlaneDot);

	XMVECTOR XAxisPlaneDotSelf = XMVector3Dot(mXAxis*mXExtent, Plane);
	XMFLOAT3 XAxisPlaneDotSelfF;
	XMStoreFloat3(&XAxisPlaneDotSelfF, XAxisPlaneDotSelf);

	XMVECTOR YAxisPlaneDotSelf = XMVector3Dot(mYAxis*mYExtent, Plane);
	XMFLOAT3 YAxisPlaneDotSelfF;
	XMStoreFloat3(&YAxisPlaneDotSelfF, YAxisPlaneDotSelf);

	XMVECTOR ZAxisPlaneDotSelf = XMVector3Dot(mZAxis*mZExtent, Plane);
	XMFLOAT3 ZAxisPlaneDotSelfF;
	XMStoreFloat3(&ZAxisPlaneDotSelfF, ZAxisPlaneDotSelf);

	XMVECTOR XAxisPlaneDotOther = XMVector3Dot(AxisX*ExtentsF.x, Plane);
	XMFLOAT3 XAxisPlaneDotOtherF;
	XMStoreFloat3(&XAxisPlaneDotOtherF, XAxisPlaneDotOther);

	XMVECTOR YAxisPlaneDotOther = XMVector3Dot(AxisY*ExtentsF.y, Plane);
	XMFLOAT3 YAxisPlaneDotOtherF;
	XMStoreFloat3(&YAxisPlaneDotOtherF, YAxisPlaneDotOther);

	XMVECTOR ZAxisPlaneDotOther = XMVector3Dot(AxisZ*ExtentsF.z, Plane);
	XMFLOAT3 ZAxisPlaneDotOtherF;
	XMStoreFloat3(&ZAxisPlaneDotOtherF, ZAxisPlaneDotOther);


	return (fabs(PosPlaneDotF.x) >
		(fabs(XAxisPlaneDotSelfF.x) +
			fabs(YAxisPlaneDotSelfF.x) +
			fabs(ZAxisPlaneDotSelfF.x) +
			fabs(XAxisPlaneDotOtherF.x) +
			fabs(YAxisPlaneDotOtherF.x) +
			fabs(ZAxisPlaneDotOtherF.x)));
}

bool CollisionBox::checkForCollision(XMVECTOR fromLocation)
{

	for (CollisionBox* box : mCollisionBoxList)
	{
		if (box == this)
			continue;

		XMVECTOR oldCenter = mCenter;
		mCenter = fromLocation;
		if (mCheckForCollision(*box))
		{
			mCenter = oldCenter;
			return true;
		}
		mCenter = oldCenter;
	}

	return false;
}

bool CollisionBox::checkForCollision()
{

	for (CollisionBox* box : mCollisionBoxList)
	{
		if (box == this)
			continue;

		
		if (mCheckForCollision(*box))
			return true;
	}

	return false;
}


bool CollisionBox::mCheckForCollision(const CollisionBox& box)
{

	//static XMVECTOR RPos;
	
	XMVECTOR RPos;
	RPos = box.GetCenter() - mCenter;

	XMVECTOR XXCross = XMVector3Cross(mXAxis, box.GetXAxis());
	XMVECTOR XYCross = XMVector3Cross(mXAxis, box.GetYAxis());
	XMVECTOR XZCross = XMVector3Cross(mXAxis, box.GetZAxis());
	XMVECTOR YXCross = XMVector3Cross(mYAxis, box.GetXAxis());
	XMVECTOR YYCross = XMVector3Cross(mYAxis, box.GetYAxis());
	XMVECTOR YZCross = XMVector3Cross(mYAxis, box.GetZAxis());
	XMVECTOR ZXCross = XMVector3Cross(mZAxis, box.GetXAxis());
	XMVECTOR ZYCross = XMVector3Cross(mZAxis, box.GetYAxis());
	XMVECTOR ZZCross = XMVector3Cross(mZAxis, box.GetZAxis());


	return !(mGetSeparatingPlane(RPos, mXAxis, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, mYAxis, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, mZAxis, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, box.GetXAxis(), box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, box.GetYAxis(), box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, box.GetZAxis(), box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, XXCross, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, XYCross, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, XZCross, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, YXCross, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, YYCross, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, YZCross, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, ZXCross, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, ZYCross, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()) ||
		mGetSeparatingPlane(RPos, ZZCross, box.GetXAxis(), box.GetYAxis(), box.GetZAxis(), box.GetExtents()));
	
}




