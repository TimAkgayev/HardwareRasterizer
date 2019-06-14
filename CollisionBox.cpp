#include "CollisionBox.h"

std::vector<CollisionBox*> CollisionBox::mCollisionBoxList;

XMVECTOR vector_signum(const XMVECTOR& inVec)
{
	XMFLOAT3 inVecF;
	XMStoreFloat3(&inVecF, inVec);

	XMFLOAT3 outF = { (inVecF.x < 0) ? -1.0f : 1.0f, (inVecF.y < 0) ? -1.0f : 1.0f, (inVecF.z < 0) ? -1.0f : 1.0f };
	if (inVecF.x == 0)
		outF.x = 0;
	else if (inVecF.y == 0)
		outF.y = 0;
	else if (inVecF.z == 0)
		outF.z = 0;

	return XMLoadFloat3(&outF);
}


bool CollisionBox::mCheckOverlapOnAxis(const XMVECTOR& in_boxBCenter, const XMVECTOR& in_xAxis, const XMVECTOR& in_yAxis, const XMVECTOR& in_zAxis, XMFLOAT3 in_xyzExtents, const XMVECTOR& in_projectionAxis)
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
	XMVECTOR axesA[3] = { this->mXAxis, this->mYAxis, this->mZAxis };
	float    extentsA[3] = { this->mXExtent, this->mYExtent, this->mZExtent };
	XMVECTOR centerA = { 0.0f, 0.0f, 0.0f };


	//values of the passed in box
	XMVECTOR axesB[3] = {  in_xAxis, in_yAxis, in_zAxis };
	float  extentsB[3] = { in_xyzExtents.x, in_xyzExtents.y, in_xyzExtents.z };
	XMVECTOR centerB = { in_boxBCenter - mCenter }; //not sure if this should be done, but I figured if centerA is always taken to be zero, then centerB should be moved over as well


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
			XMVECTOR projDotAxesA = XMVector3Dot(in_projectionAxis, axesA[i]);
			XMVECTOR projDotAxesB = XMVector3Dot(in_projectionAxis, axesB[i]);

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
	XMVECTOR R = XMVector3Dot(in_projectionAxis, centerDistance);
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
	float xExtent = width  * 0.5f;
	float yExtent = height * 0.5f;
	float zExtent = width  * 0.5f;

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

	mCollisionBoxList.push_back(this);

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
	float zExtent = width  * 0.5f;

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

void CollisionBox::SetPosition(XMFLOAT3 newPos)
{
	mCenter = XMLoadFloat3(&newPos);

}


void CollisionBox::mGetAxes(const CollisionBox& box1, const CollisionBox& box2)
{
	std::vector<XMVECTOR> b1Vertices = box1.GetVertices();
	std::vector<XMVECTOR> b2Vertices = box2.GetVertices();



	for (int i = 0; i < b1Vertices.size(); i++)
	{
		XMVECTOR p1 = b1Vertices[i];

		int nextOffset = i + 1;
		if (nextOffset == b1Vertices.size())
			nextOffset = 0;

		XMVECTOR p2 = b1Vertices[nextOffset];
		XMVECTOR edge = p1 - p2;
		XMFLOAT3 edgeF;
		XMStoreFloat3(&edgeF, edge);
		XMFLOAT3 edgeNormalF = XMFLOAT3(-edgeF.y, edgeF.x, edgeF.z);
		XMVECTOR edgeNormal = XMLoadFloat3(&edgeNormalF);
		mProjectionAxesShape1.push_back(edgeNormal);
	}

	for (int i = 0; i < b2Vertices.size(); i++)
	{
		XMVECTOR p1 = b2Vertices[i];

		int nextOffset = i + 1;
		if (nextOffset == b1Vertices.size())
			nextOffset = 0;

		XMVECTOR p2 = b2Vertices[nextOffset];
		XMVECTOR edge = p1 - p2;
		XMFLOAT3 edgeF;
		XMStoreFloat3(&edgeF, edge);
		XMFLOAT3 edgeNormalF = XMFLOAT3(-edgeF.y, edgeF.x, edgeF.z);
		XMVECTOR edgeNormal = XMLoadFloat3(&edgeNormalF);
		mProjectionAxesShape2.push_back(edgeNormal);
	}
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
	//define world axes
	XMVECTOR xAxis = { 1.0f, 0.0f, 0.0f };
	XMVECTOR yAxis = { 0.0f, 1.0f, 0.0f };
	XMVECTOR zAxis = { 0.0f, 0.0f, 1.0f };

	XMVECTOR inCenter = box.GetCenter();
	XMVECTOR inXAxis = box.GetXAxis();
	XMVECTOR inYAxis = box.GetYAxis();
	XMVECTOR inZAxis = box.GetZAxis();
	XMFLOAT3 inExtents = box.GetExtents();

	//define our 15 projection axes
	XMVECTOR ProjectionAxes[15];
	ProjectionAxes[0] = xAxis;
	ProjectionAxes[1] = yAxis;
	ProjectionAxes[2] = zAxis;
	ProjectionAxes[3] = inXAxis;
	ProjectionAxes[4] = inYAxis;
	ProjectionAxes[5] = inZAxis;
	ProjectionAxes[6] = XMVector3Cross(xAxis, inXAxis);
	ProjectionAxes[7] = XMVector3Cross(xAxis, inYAxis);
	ProjectionAxes[8] = XMVector3Cross(xAxis, inZAxis);
	ProjectionAxes[9] = XMVector3Cross(yAxis, inXAxis);
	ProjectionAxes[10] = XMVector3Cross(yAxis, inYAxis);
	ProjectionAxes[11] = XMVector3Cross(yAxis, inZAxis);
	ProjectionAxes[12] = XMVector3Cross(zAxis, inXAxis);
	ProjectionAxes[13] = XMVector3Cross(zAxis, inYAxis);
	ProjectionAxes[14] = XMVector3Cross(zAxis, inZAxis);


	//for each axis, check if the projections overlap
	//if an axis is found where the projections don't overlap, that means the objects are not colliding
	//if all projections overlap along all 15 axes then the objects are touching
	for (int i = 0; i < 15; i++)
		if (mCheckOverlapOnAxis(inCenter, inXAxis, inYAxis, inZAxis, inExtents, ProjectionAxes[i]) == false)
			return false;

	return true;

}




