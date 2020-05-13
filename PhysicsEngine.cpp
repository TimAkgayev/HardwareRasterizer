#include "PhysicsEngine.h"

void PhysicsEngine::AddStaticCollisionBox(CollisionBox & box)
{
	mStaticCollisionBoxList.push_back(box);
}

bool PhysicsEngine::CheckForCollision(CollisionBox & playerCollisionBox, XMVECTOR fromLocation)
{
	for (CollisionBox current_box : mStaticCollisionBoxList)
	{

		//define world axes
		XMVECTOR xWorldAxis = { 1.0f, 0.0f, 0.0f };
		XMVECTOR yWorldAxis = { 0.0f, 1.0f, 0.0f };
		XMVECTOR zWorldAxis = { 0.0f, 0.0f, 1.0f };

		XMVECTOR current_box_Center = current_box.GetCenter();
		XMVECTOR current_box_XAxis = current_box.GetXAxis();
		XMVECTOR current_box_YAxis = current_box.GetYAxis();
		XMVECTOR current_box_ZAxis = current_box.GetZAxis();
		XMFLOAT3 current_box_Extents = current_box.GetExtents();

		//define our 15 projection axes
		XMVECTOR ProjectionAxes[15];
		ProjectionAxes[0] = xWorldAxis;
		ProjectionAxes[1] = yWorldAxis;
		ProjectionAxes[2] = zWorldAxis;
		ProjectionAxes[3] = current_box_XAxis;
		ProjectionAxes[4] = current_box_YAxis;
		ProjectionAxes[5] = current_box_ZAxis;
		ProjectionAxes[6] = XMVector3Cross(xWorldAxis, current_box_XAxis);
		ProjectionAxes[7] = XMVector3Cross(xWorldAxis, current_box_YAxis);
		ProjectionAxes[8] = XMVector3Cross(xWorldAxis, current_box_ZAxis);
		ProjectionAxes[9] = XMVector3Cross(yWorldAxis, current_box_XAxis);
		ProjectionAxes[10] = XMVector3Cross(yWorldAxis, current_box_YAxis);
		ProjectionAxes[11] = XMVector3Cross(yWorldAxis, current_box_ZAxis);
		ProjectionAxes[12] = XMVector3Cross(zWorldAxis, current_box_XAxis);
		ProjectionAxes[13] = XMVector3Cross(zWorldAxis, current_box_YAxis);
		ProjectionAxes[14] = XMVector3Cross(zWorldAxis, current_box_ZAxis);


		//for each axis, check if the projections overlap
		//if an axis is found where the projections don't overlap, that means the objects are not colliding
		//if all projections overlap along all 15 axes then the objects are touching

		bool doesThisBoxIntersectPlayer = true;
		for (int i = 0; i < 15; i++)
			if (_CheckIfProjectionsOverlapOnAxis(fromLocation, playerCollisionBox.GetXAxis(), playerCollisionBox.GetYAxis(), playerCollisionBox.GetZAxis(), playerCollisionBox.GetExtents(), current_box_Center, current_box_XAxis, current_box_YAxis, current_box_ZAxis, current_box_Extents, ProjectionAxes[i]) == false)
			{
				doesThisBoxIntersectPlayer = false;
				break;
			}
			
		if (doesThisBoxIntersectPlayer == true)
			return true;
		else
			continue;

	}

	return false;
}

bool PhysicsEngine::_CheckIfProjectionsOverlapOnAxis(const XMVECTOR & boxA_Center, const XMVECTOR & boxA_xAxis, const XMVECTOR & boxA_yAxis, const XMVECTOR & boxA_zAxis, XMFLOAT3 boxA_xyzExtents, const XMVECTOR & boxB_Center, const XMVECTOR & boxB_xAxis, const XMVECTOR & boxB_yAxis, const XMVECTOR & boxB_zAxis, XMFLOAT3 boxB_xyzExtents, const XMVECTOR & projectionAxis)
{
	//sign vector used to get all the 8 vertices
	//this is to accomodate the Sign() in the equations
	int sign[8][3];
	sign[0][0] = 1;
	sign[0][1] = 1;
	sign[0][2] = 1;

	sign[1][0] = 1;
	sign[1][1] = 1;
	sign[1][2] = -1;

	sign[2][0] = -1;
	sign[2][1] = 1;
	sign[2][2] = -1;

	sign[3][0] = -1;
	sign[3][1] = 1;
	sign[3][2] = 1;

	sign[4][0] = 1;
	sign[4][1] = -1;
	sign[4][2] = 1;

	sign[5][0] = 1;
	sign[5][1] = -1;
	sign[5][2] = -1;

	sign[6][0] = -1;
	sign[6][1] = -1;
	sign[6][2] = -1;

	sign[7][0] = -1;
	sign[7][1] = -1;
	sign[7][2] = 1;

	//values of the box in this class
	XMVECTOR axesA[3] = { boxA_xAxis, boxA_yAxis, boxA_zAxis };
	float    extentsA[3] = { boxA_xyzExtents.x, boxA_xyzExtents.y, boxA_xyzExtents.z };
	XMVECTOR centerA = { 0.0f, 0.0f, 0.0f };


	//values of the passed in box
	XMVECTOR axesB[3] = { boxB_xAxis, boxB_yAxis, boxB_zAxis };
	float  extentsB[3] = { boxB_xyzExtents.x, boxB_xyzExtents.y, boxB_xyzExtents.z };
	XMVECTOR centerB = { boxB_Center - boxA_Center }; //not sure if this should be done, but I figured if centerA is always taken to be zero, then centerB should be moved over as well


	//start with radius of zero for comparison
	float radiusA = 0.0f;
	float radiusB = 0.0f;

	for (int vertexCount = 0; vertexCount < 8; vertexCount++)
	{
		float vertexRadiusA = 0.0f;
		float vertexRadiusB = 0.0f;
		for (int i = 0; i < 3; i++)
		{
			//project each vertex onto the plane and find the distance to each one
			XMVECTOR projDotAxesA = XMVector3Dot(projectionAxis, axesA[i]);
			XMVECTOR projDotAxesB = XMVector3Dot(projectionAxis, axesB[i]);

			XMFLOAT3 projDotAxesAF, projDotAxesBF;
			XMStoreFloat3(&projDotAxesAF, projDotAxesA);
			XMStoreFloat3(&projDotAxesBF, projDotAxesB);


			vertexRadiusA += abs(extentsA[i] * sign[vertexCount][i] * projDotAxesAF.x);
			vertexRadiusB += abs(extentsB[i] * sign[vertexCount][i] * projDotAxesBF.x);
		}


		//find the vertex with the largest distance and set that as the radius of the cicle
		if (vertexRadiusA > radiusA)
			radiusA = vertexRadiusA;

		if (vertexRadiusB > radiusB)
			radiusB = vertexRadiusB;

	}


	//find the distance between two box's centers
	XMVECTOR centerDistance = centerB - centerA;

	//L*D
	XMVECTOR R = XMVector3Dot(projectionAxis, centerDistance);
	XMFLOAT3 Rf;
	XMStoreFloat3(&Rf, R);


	//find if two circle shadows overlap or not
	if (abs(Rf.x) > radiusA + radiusB)
		return false;
	else
		return true;
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
	float xExtent = width * 0.5f;
	float yExtent = height * 0.5f;
	float zExtent = width * 0.5f;

	XMVECTOR positionV = XMLoadFloat3(&position);


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

}


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
	XMVECTOR zAxis = -XMVector3Cross(xAxis, yAxis);


	XMFLOAT3 xAxisF, yAxisF, zAxisF;
	XMFLOAT3 xAxisLengthF;
	XMStoreFloat3(&xAxisF, xAxis);
	XMStoreFloat3(&yAxisF, yAxis);
	XMStoreFloat3(&zAxisF, zAxis);
	XMStoreFloat3(&xAxisLengthF, xAxisLength);

	//get the x-extent (half length of the axis)
	float xExtent = xAxisLengthF.x * 0.5f;
	float yExtent = height * 0.5f;
	float zExtent = width * 0.5f;

	//get the center
	XMVECTOR center = startXZv + xAxis * xExtent + yAxis * yExtent + zAxis * zExtent;
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

XMFLOAT3 CollisionBox::GetExtents() const
{
	XMFLOAT3 extents = { mXExtent, mYExtent, mZExtent };
	return extents;
}

XMVECTOR CollisionBox::GetCenter() const
{
	return mCenter;
}

std::vector<XMVECTOR> CollisionBox::GetVertices() const
{

	XMVECTOR vertex[8];

	int sign[8][3];
	sign[0][0] = 1;
	sign[0][1] = 1;
	sign[0][2] = 1;

	sign[1][0] = 1;
	sign[1][1] = 1;
	sign[1][2] = -1;

	sign[2][0] = -1;
	sign[2][1] = 1;
	sign[2][2] = -1;

	sign[3][0] = -1;
	sign[3][1] = 1;
	sign[3][2] = 1;

	sign[4][0] = 1;
	sign[4][1] = -1;
	sign[4][2] = 1;

	sign[5][0] = 1;
	sign[5][1] = -1;
	sign[5][2] = -1;

	sign[6][0] = -1;
	sign[6][1] = -1;
	sign[6][2] = -1;

	sign[7][0] = -1;
	sign[7][1] = -1;
	sign[7][2] = 1;

	float extents[3] = { mXExtent, mYExtent, mZExtent };
	XMVECTOR axes[3] = { mXAxis, mYAxis, mZAxis };


	for (int vertexCount = 0; vertexCount < 8; vertexCount++)
	{
		XMVECTOR sum = { 0.0f, 0.0f, 0.0f };
		for (int i = 0; i < 3; i++)
			sum += sign[vertexCount][i] * extents[i] * axes[i];

		vertex[vertexCount] = mCenter + sum;
	}


	return std::vector<XMVECTOR>(vertex, vertex + 8);
}

void CollisionBox::SetCenter(XMFLOAT3 newPos)
{
	mCenter = XMLoadFloat3(&newPos);

}
