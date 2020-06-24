#include "Terrain.h"
#include <WICTextureLoader.h>
#include "ConstantBuffers.h"
#include "Vertex.h"
#include "Shaders.h"
#include "ConstantBuffers.h"
#include <limits>
#include <set>
#include <DirectXCollision.h>
#include "Utility.h"



struct _node
{
	//UINT start_box_index;
	//UINT end_box_index;
	
	std::vector<UINT> triangleIndices;
	_bounding_box bounding_box;

	_node* leftChild;
	_node* rightChild;

	bool isRoot;
};


_node* tree;


Terrain::Terrain(ID3D11Device * device, PhysicsEngine* physicsEngine)
{

	mD3DDevice = device;
	mPhysicsEngine = physicsEngine;

	device->GetImmediateContext(&mDeviceContext);

//	mDbgBox.Init(mD3DDevice, mDeviceContext);

	D3D11_RASTERIZER_DESC rasterizerStateWireframe;
	rasterizerStateWireframe.CullMode = D3D11_CULL_NONE;
	rasterizerStateWireframe.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerStateWireframe.FrontCounterClockwise = false;
	rasterizerStateWireframe.DepthBias = false;
	rasterizerStateWireframe.DepthBiasClamp = 0;
	rasterizerStateWireframe.SlopeScaledDepthBias = 0;
	rasterizerStateWireframe.DepthClipEnable = true;
	rasterizerStateWireframe.ScissorEnable = false;
	rasterizerStateWireframe.MultisampleEnable = false;
	rasterizerStateWireframe.AntialiasedLineEnable = true;
	mD3DDevice->CreateRasterizerState(&rasterizerStateWireframe, &mRasterizerStateWireframe);
	
}

Terrain::~Terrain()
{
	if (mHeightMap)
		delete mHeightMap;
	mHeightMap = nullptr;

}


void Terrain::Draw()
{

	//for (UINT i = 0; i < mBVHTree.size(); i++)
	//	mDbgBox.AddBox(mBVHTree[i].bounding_box, i);


	UINT stride = sizeof(Vertex::PosNormTex);
	UINT offset = 0;

	//update buffers
	ConstantBuffers::WorldMatrices worldBuffer;
	ConstantBuffers::Material material;


	worldBuffer.World = XMMatrixIdentity();

	material.Ka = 0.0f;
	material.Kd = 1.0f;
	material.Ks = 0.0f;
	material.A =  1.0f;


	mDeviceContext->UpdateSubresource(ConstantBuffers::MaterialBuffer, 0, NULL, &material, 0, 0);
	mDeviceContext->UpdateSubresource(ConstantBuffers::WorldMatrixBuffer, 0, NULL, &worldBuffer, 0, 0);

	//set shader resources
	mDeviceContext->PSSetShaderResources(0, 1, &mTextureResourceView);

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	mDeviceContext->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	//mDeviceContext->RSSetState(mRasterizerStateWireframe);

	mDeviceContext->VSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);
	mDeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);


	mDeviceContext->DrawIndexed(mNumIndices, 0, 0);

}


class index_comparator
{
public:
	bool operator()(const tinyobj::index_t& lhv, const tinyobj::index_t& rhv) const
	{
		return std::tie(lhv.vertex_index, lhv.normal_index, lhv.texcoord_index) < std::tie(rhv.vertex_index, rhv.normal_index, rhv.texcoord_index);
	}
};




bool IsPointInBox(XMVECTOR point,  const _bounding_box& box)
{
	XMFLOAT3 min = XMFLOAT3(box.center.x - box.x_extent, box.center.y - box.y_extent, box.center.z - box.z_extent);
	XMFLOAT3 max = XMFLOAT3(box.center.x + box.x_extent, box.center.y + box.y_extent, box.center.z + box.z_extent);

	XMFLOAT3 p;
	XMStoreFloat3(&p, point);

	if (min.x <= p.x && min.y <= p.y && min.z <= p.z && max.x >= p.x && max.y >= p.y && max.z >= p.z)
		return true;
	else
		return false;
}


bool _ray_box_intersect(const XMVECTOR& rayOrigin, const XMVECTOR& rayDirection, const _bounding_box& box)
{
	XMFLOAT3 rayDirectionF;
	XMStoreFloat3(&rayDirectionF, rayDirection);
	XMFLOAT3 rayInvDirF = XMFLOAT3(1.0f / rayDirectionF.x, 1.0f / rayDirectionF.y, 1.0f / rayDirectionF.z);

	if (rayDirectionF.x == 0)
		rayInvDirF.x = 0;
	if (rayDirectionF.y == 0)
		rayInvDirF.y = 0;
	if (rayDirectionF.z == 0)
		rayInvDirF.z = 0;

	XMVECTOR rayInvDirection = XMLoadFloat3(&rayInvDirF);

	XMFLOAT3 rayOriginF;
	XMStoreFloat3(&rayOriginF, rayOrigin);

	int sign[3];

	sign[0] = (rayInvDirF.x < 0);
	sign[1] = (rayInvDirF.y < 0);
	sign[2] = (rayInvDirF.z < 0);

	XMVECTOR boundsV[2];

	XMFLOAT3 boxExtentsF = XMFLOAT3(box.x_extent, box.y_extent, box.z_extent);
	boundsV[0] = XMLoadFloat3(&box.center) - XMLoadFloat3(&boxExtentsF);
	boundsV[1] = XMLoadFloat3(&box.center) + XMLoadFloat3(&boxExtentsF);

	XMFLOAT3 bounds[2];
	XMStoreFloat3(&bounds[0], boundsV[0]);
	XMStoreFloat3(&bounds[1], boundsV[1]);

	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	float t0 = -100000000.0f;
	float t1 = 100000000.0f;

	if (rayDirectionF.x >= 0) {
		tmin = (bounds[0].x - rayOriginF.x) / rayDirectionF.x;
		tmax = (bounds[1].x - rayOriginF.x) / rayDirectionF.x;
	}
	else {
		tmin = (bounds[1].x - rayOriginF.x) / rayDirectionF.x;
		tmax = (bounds[0].x - rayOriginF.x) / rayDirectionF.x;
	}
	if (rayDirectionF.y >= 0) {
		tymin = (bounds[0].y - rayOriginF.y) / rayDirectionF.y;
		tymax = (bounds[1].y - rayOriginF.y) / rayDirectionF.y;
	}
	else {
		tymin = (bounds[1].y - rayOriginF.y) / rayDirectionF.y;
		tymax = (bounds[0].y - rayOriginF.y) / rayDirectionF.y;
	}
	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	if (rayDirectionF.z >= 0) {
		tzmin = (bounds[0].z - rayOriginF.z) / rayDirectionF.z;
		tzmax = (bounds[1].z - rayOriginF.z) / rayDirectionF.z;
	}
	else {
		tzmin = (bounds[1].z - rayOriginF.z) / rayDirectionF.z;
		tzmax = (bounds[0].z - rayOriginF.z) / rayDirectionF.z;
	}
	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;


	return ((tmin < t1) && (tmax > t0));

}

void IntersectRayWithBVHTree(const std::vector<_heap_node> &tree, const XMVECTOR & rayOrigin, const XMVECTOR & rayDirection, std::vector<_bounding_box_tri> &outBoxList)
{

	//if tree is empty, do nothing
	if (!tree.size())
		return;

	std::vector<int> stack;
	stack.push_back(0);

	for (UINT i = 0; i < stack.size(); i++)
	{
		UINT node_index = stack[i];

		//check if the ray intersects the root box
		if (_ray_box_intersect(rayOrigin, rayDirection, tree[node_index].bounding_box))
		{
			//check if the point is in the left child
			if (tree[node_index].childIndices[0] != -1)
			{
				if (_ray_box_intersect(rayOrigin, rayDirection, tree[tree[node_index].childIndices[0]].bounding_box))
					stack.push_back(tree[node_index].childIndices[0]);
				
			}

			//check if the point is in the right child
			if (tree[node_index].childIndices[1] != -1)
			{
				if (_ray_box_intersect(rayOrigin, rayDirection, tree[tree[node_index].childIndices[1]].bounding_box))
					stack.push_back(tree[node_index].childIndices[1]);
				
			}

			//if this node doesn't have children just retrieve the triangles that are inside this node
			if (tree[node_index].childIndices[0] == -1 && tree[node_index].childIndices[1] == -1)
			{
				outBoxList.insert(outBoxList.end(), tree[node_index].triangleList.begin(), tree[node_index].triangleList.end());
			}


		}

	}
}


void FindPointInTree(const std::vector<_heap_node> &tree, const XMVECTOR &point, std::vector<_bounding_box_tri> &outBoxList )
{

	//if tree is empty, do nothing
	if (!tree.size())
		return;

	std::vector<int> stack;
	stack.push_back(0);

	for(UINT i = 0; i < stack.size(); i++)
	{
		UINT node_index = stack[i];

		//check if the point is in the root node
		if (IsPointInBox(point, tree[node_index].bounding_box))
		{
			//check if the point is in the left child
			if (tree[node_index].childIndices[0] != -1)
			{
				if (IsPointInBox(point, tree[tree[node_index].childIndices[0]].bounding_box))
				{

					stack.push_back(tree[node_index].childIndices[0]);
				}
			}

			//check if the point is in the right child
			if (tree[node_index].childIndices[1] != -1)
			{
				if (IsPointInBox(point, tree[tree[node_index].childIndices[1]].bounding_box))
				{
					stack.push_back(tree[node_index].childIndices[1]);
				}
			}

			//if this node doesn't have children just retrieve the triangles that are inside this node
			if (tree[node_index].childIndices[0] == -1 && tree[node_index].childIndices[1] == -1)
			{
				outBoxList.insert(outBoxList.end(), tree[node_index].triangleList.begin(), tree[node_index].triangleList.end());
			}

				
		}

	}
}




void FindBoundingBox(Vertex::PosNormTex* vertexList, UINT numVerts, _bounding_box_tri& outBox)
{
	//find the smallest and largest values
	std::vector<float> vertexXList;

	float smallest_x = 1000000.0f;
	float largest_x = -1000000.0f;

	float smallest_y = 1000000.0f;
	float largest_y = -1000000.0f;

	float smallest_z = 1000000.0f;
	float largest_z = -1000000.0f;

	for (UINT i = 0; i < numVerts; i++)
	{

		if (vertexList[i].pos.x < smallest_x)
			smallest_x = vertexList[i].pos.x;
		if (vertexList[i].pos.x > largest_x)
			largest_x = vertexList[i].pos.x;


		if (vertexList[i].pos.y < smallest_y)
			smallest_y = vertexList[i].pos.y;
		if (vertexList[i].pos.y > largest_y)
			largest_y = vertexList[i].pos.y;

		if (vertexList[i].pos.z < smallest_z)
			smallest_z = vertexList[i].pos.z;
		if (vertexList[i].pos.z > largest_z)
			largest_z = vertexList[i].pos.z;

	}

	outBox.center = XMFLOAT3((largest_x + smallest_x) / 2, (largest_y + smallest_y) / 2, (largest_z + smallest_z) / 2);
	outBox.x_extent = (largest_x - smallest_x) / 2;
	outBox.y_extent = (largest_y - smallest_y) / 2;
	outBox.z_extent = (largest_z - smallest_z) / 2;
}

_bounding_box FindBoundingBox(const std::vector<_bounding_box_tri>& boxList)
{

	std::vector<XMFLOAT3> minList;
	std::vector<XMFLOAT3> maxList;

	for (UINT i = 0; i < boxList.size(); i++)
	{
		XMVECTOR boxCenter = XMLoadFloat3(&boxList[i].center);
		
		
		XMFLOAT3 min, max;
		min = XMFLOAT3(boxList[i].center.x - boxList[i].x_extent, boxList[i].center.y - boxList[i].y_extent, boxList[i].center.z - boxList[i].z_extent);
		max = XMFLOAT3(boxList[i].center.x + boxList[i].x_extent, boxList[i].center.y + boxList[i].y_extent, boxList[i].center.z + boxList[i].z_extent);


		minList.push_back(min);
		maxList.push_back(max);
	
	}

	
	float smallest_x = 1000000.0f;
	float largest_x = -1000000.0f;

	float smallest_y = 1000000.0f;
	float largest_y = -1000000.0f;

	float smallest_z = 1000000.0f;
	float largest_z = -1000000.0f;

	for (UINT i = 0; i < boxList.size(); i++)
	{

		if (minList[i].x < smallest_x)
			smallest_x = minList[i].x;
		if (maxList[i].x > largest_x)
			largest_x = maxList[i].x;


		if (minList[i].y < smallest_y)
			smallest_y = minList[i].y;
		if (maxList[i].y > largest_y)
			largest_y = maxList[i].y;

		if (minList[i].z < smallest_z)
			smallest_z = minList[i].z;
		if (maxList[i].z > largest_z)
			largest_z = maxList[i].z;

	}

	_bounding_box outBox;
	outBox.center = XMFLOAT3((largest_x + smallest_x) / 2, (largest_y + smallest_y) / 2, (largest_z + smallest_z) / 2);
	outBox.x_extent = (largest_x - smallest_x) / 2;
	outBox.y_extent = (largest_y - smallest_y) / 2;
	outBox.z_extent = (largest_z - smallest_z) / 2;

	return outBox;
}

char FindLongestSide(_bounding_box& box)
{
	XMFLOAT3 min, max;
	min = XMFLOAT3(box.center.x - box.x_extent, box.center.y - box.y_extent, box.center.z - box.z_extent);
	max = XMFLOAT3(box.center.x + box.x_extent, box.center.y + box.y_extent, box.center.z + box.z_extent);

	float x_length = max.x - min.x;
	float y_length = max.y - min.y;
	float z_length = max.z - min.z;


	char side;

	if (x_length > y_length)
	{
		if (x_length > z_length) // x is the largest
			side = 'x';
		else //z is the largest
			side = 'z';

	}
	else
	{
		if (y_length > z_length) //y is the largest
			side = 'y';
		else //z is the larges
			side = 'z';

	}

	return side;

}

int compare_func_x(const void* a, const void* b)
{
	_bounding_box* mma = (_bounding_box*)a;
	_bounding_box* mmb = (_bounding_box*)b;

	if (mma->center.x < mmb->center.x)
		return -1;
	else if (mma->center.x > mmb->center.x)
		return 1;
	else return 0;
}
int compare_func_y(const void* a, const void* b)
{
	_bounding_box* mma = (_bounding_box*)a;
	_bounding_box* mmb = (_bounding_box*)b;

	if (mma->center.y < mmb->center.y)
		return -1;
	else if (mma->center.y > mmb->center.y)
		return 1;
	else return 0;
}
int compare_func_z(const void* a, const void* b)
{
	_bounding_box* mma = (_bounding_box*)a;
	_bounding_box* mmb = (_bounding_box*)b;

	if (mma->center.z < mmb->center.z)
		return -1;
	else if (mma->center.z > mmb->center.z)
		return 1;
	else return 0;
}

void OrganizeListByAxis(char axis, std::vector<_bounding_box_tri>& boxList)
{


	switch (axis)
	{
	case 'x': {
		std::qsort(&boxList[0], boxList.size(), sizeof(_bounding_box_tri), compare_func_x);
	}break;
	case 'y': {
		std::qsort(&boxList[0], boxList.size(), sizeof(_bounding_box_tri), compare_func_y);
	}break;
	case 'z': {
		std::qsort(&boxList[0], boxList.size(), sizeof(_bounding_box_tri), compare_func_z);
	}break;

	default: break;
	}

}



std::vector<_heap_node> GenerateBVHTree(const std::vector<_bounding_box_tri>& terrainTriBBList, const UINT unitsPerLeaf = 100)
{

	std::vector<_heap_node> BVHTree;

	//if no triangles return an empty list
	if (!terrainTriBBList.size())
		return BVHTree;

	_heap_node root;

	//assign all triangles to root
	root.triangleList = terrainTriBBList;
	root.bounding_box = FindBoundingBox(terrainTriBBList);
	BVHTree.push_back(root);

	std::vector<int> stack;

	//if root has more triangles associated with it then the maximum allowed, split it
	if (root.triangleList.size() > unitsPerLeaf)
		stack.push_back(0);


	//go through each node that exceedes the max leaves and split
	for (unsigned int i = 0; i < stack.size(); i++)
	{
		int node_index = stack[i];

		//sort the triangles in this node before splitting
		std::vector<_bounding_box_tri>& sortedTriangles = BVHTree[node_index].triangleList;
		char longestAxis = FindLongestSide(BVHTree[node_index].bounding_box);
		OrganizeListByAxis(longestAxis, sortedTriangles);


		//split off the left side
		int split_start = 0;
		int split_end = int(sortedTriangles.size() / 2.0f + 0.5f);

		//create a sublist of triangle bounding boxes that are contained in this ﻿split
		std::vector<_bounding_box_tri>::const_iterator startIter = sortedTriangles.begin() + split_start;
		std::vector<_bounding_box_tri>::const_iterator endIter = sortedTriangles.begin() + split_end;
		std::vector<_bounding_box_tri> leftsideTriangleList(startIter, endIter);


		//avoid empty nodes
		if (leftsideTriangleList.size())
		{
			_heap_node leftChild;
			leftChild.bounding_box = FindBoundingBox(leftsideTriangleList);
			leftChild.triangleList = leftsideTriangleList;

			int leftChild_index = BVHTree.size();
			BVHTree[node_index].childIndices[0] = leftChild_index;
			
			//add child to tree
			BVHTree.push_back(leftChild);

			//if child needs splitting, do so
			if (leftsideTriangleList.size() > unitsPerLeaf)
				stack.push_back(leftChild_index);
		}


		//split off the right side
		split_start = split_end;
		split_end = BVHTree[node_index].triangleList.size();

		std::vector<_bounding_box_tri>& sortedTriangles2 = BVHTree[node_index].triangleList;

		//create a sublist of triangle bounding boxes that are contained in this ﻿split
		startIter = sortedTriangles2.begin() + split_start;
		endIter = sortedTriangles2.begin() + split_end;
		std::vector<_bounding_box_tri> rightsideTriangleList(startIter, endIter);

		//avoid empty nodes
		if (rightsideTriangleList.size())
		{
			_heap_node rightChild;
			rightChild.bounding_box = FindBoundingBox(rightsideTriangleList);
			rightChild.triangleList = rightsideTriangleList;

			int rightChild_index = BVHTree.size();
			BVHTree[node_index].childIndices[1] = rightChild_index;

			//add child to tree
			BVHTree.push_back(rightChild);

			//if child needs splitting, do so
			if (rightsideTriangleList.size() > unitsPerLeaf)
				stack.push_back(rightChild_index);
		}

	}


	return BVHTree;

	/* Proper implementation 

	std::vector<_heap_node> tree;


	//generate the root node
	_heap_node rootNode;


	//find the bounding box of all the bounding boxes in the terrain 
	rootNode.bounding_box = FindBoundingBox(terrainTriBBList);

	//assign the triangles associated with this box
	rootNode.triangleList = terrainTriBBList;

	//add root to tree
	tree.push_back(rootNode);

	std::vector<int> stack;

	if(rootNode.triangleList.size() > unitsPerLeaf)
		stack.push_back(0); // process the root node﻿﻿﻿﻿﻿﻿

	for (UINT i = 0; i < stack.size(); i++)
	{
		UINT parent_index = stack[i];
	
		//sort the parent box list
		std::vector<_bounding_box_tri>& sortedTriangles = tree[parent_index].triangleList;
		char axis = FindLongestSide(tree[parent_index].bounding_box);
		OrganizeListByAxis(axis, sortedTriangles);


		//generate the left branch
		_heap_node leftChild;
		
		UINT start_box_index = 0;
		UINT end_box_index = UINT(sortedTriangles.size() / 2.0f + 0.5f);

		//create a sublist of triangle bounding boxes that are contained in this ﻿split
		std::vector<_bounding_box_tri>::const_iterator startIter = sortedTriangles.begin() + start_box_index;
		std::vector<_bounding_box_tri>::const_iterator endIter = sortedTriangles.begin() + end_box_index;
		std::vector<_bounding_box_tri> leftsideTriangleList(startIter, endIter);

		//avoid empty nodes
		if (leftsideTriangleList.size())
		{
			UINT index_of_left_side = tree.size();

			//if left child has too many triangles, push it to the stack for further subdivision
			if (leftsideTriangleList.size() > unitsPerLeaf)
				stack.push_back(index_of_left_side);

			//find the bounding box around the split half
			leftChild.bounding_box = FindBoundingBox(leftsideTriangleList);

			//pass on the bounding box list
			leftChild.triangleList = leftsideTriangleList;

			//link the left side to parent node and add it to the tree
			tree[parent_index].childIndices[0] = index_of_left_side;
			tree.push_back(leftChild);

		}

	

		//generate the right branch
		_heap_node rightChild;


		//assign the other half to the right branch (1/2 list + 1 to end)
		start_box_index = end_box_index + 1; //start where left box has ended, just ofset by one
		end_box_index = tree[parent_index].triangleList.size(); //end at the end of the list﻿﻿﻿﻿﻿﻿﻿

		//create a sublist of bounding boxes ﻿to sort
		std::vector<_bounding_box_tri>& sortedParentCopyRight = tree[parent_index].triangleList;
		startIter = sortedParentCopyRight.begin() + start_box_index;
		endIter = sortedParentCopyRight.begin() + end_box_index;
		std::vector<_bounding_box_tri> rightsideTriangleList(startIter, endIter);

	
		//avoid empty nodes
		if (rightsideTriangleList.size())
		{
			UINT index_of_right_side = tree.size();

			//if right child has too many triangles, push it to the stack for further subdivision
			if (rightsideTriangleList.size() > unitsPerLeaf)
				stack.push_back(index_of_right_side);

			//find the bounding box around that split half
			rightChild.bounding_box = FindBoundingBox(rightsideTriangleList);

			//pass on the new bounding box list
			rightChild.triangleList = rightsideTriangleList;

			// link parent to right child and add it to the tree
			tree[parent_index].childIndices[1] = index_of_right_side;
			tree.push_back(rightChild);
		}

	}


	return tree;
	*/

}

void Terrain::_CreateTerrainVertexBuffersAndGenerateBVHTree(tinyobj::shape_t& shape, tinyobj::attrib_t& attrib, std::vector<tinyobj::material_t>& materials, std::string filename)
{

	std::map<tinyobj::index_t, int, index_comparator> uniqueVertexMap;

	//go through each index and find unique entries
	for (tinyobj::index_t i : shape.mesh.indices)
		uniqueVertexMap.insert(std::pair<tinyobj::index_t, int>(i, uniqueVertexMap.size()));


	//allocate space for the vertices
	mVertices = new Vertex::PosNormTex[uniqueVertexMap.size()];
	mNumVertices = uniqueVertexMap.size();

	for (auto& keyval : uniqueVertexMap)
	{
		tinyobj::real_t vx = attrib.vertices[3 * keyval.first.vertex_index + 0];
		tinyobj::real_t vy = attrib.vertices[3 * keyval.first.vertex_index + 1];
		tinyobj::real_t vz = attrib.vertices[3 * keyval.first.vertex_index + 2];

		tinyobj::real_t nx = attrib.normals[3 * keyval.first.normal_index + 0];
		tinyobj::real_t ny = attrib.normals[3 * keyval.first.normal_index + 1];
		tinyobj::real_t nz = attrib.normals[3 * keyval.first.normal_index + 2];

		tinyobj::real_t tx = attrib.texcoords[2 * keyval.first.texcoord_index + 0];
		tinyobj::real_t ty = attrib.texcoords[2 * keyval.first.texcoord_index + 1];

		// Optional: vertex colors
		// tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
		// tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
		// tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];

		//
		// per-face material
		//shapes[s].mesh.material_ids[f];

		Vertex::PosNormTex vert;

		vert.pos.x = vx;
		vert.pos.y = vy;
		vert.pos.z = vz;

		vert.norm.x = nx;
		vert.norm.y = ny;
		vert.norm.z = nz;

		vert.uv.x = tx;
		vert.uv.y = ty;

		mVertices[keyval.second] = vert;
	}

	//now re-index the old index list
	for (tinyobj::index_t i : shape.mesh.indices)
		mIndexList.push_back(uniqueVertexMap[i]);


	//find boundaries
	float smallest_x = 100000000.0f;
	float smallest_y = 100000000.0f;
	float smallest_z = 100000000.0f;
	float largest_x = -100000000.0f;
	float largest_y = -100000000.0f;
	float largest_z = -100000000.0f;

	for (int i = 0; i < mNumVertices; i++)
	{
		Vertex::PosNormTex v = mVertices[i];
		if (v.pos.x < smallest_x)
			smallest_x = v.pos.x;
		if (v.pos.x > largest_x)
			largest_x = v.pos.x;
		if (v.pos.y < smallest_y)
			smallest_y = v.pos.y;
		if (v.pos.y > largest_y)
			largest_y = v.pos.y;
		if (v.pos.z < smallest_z)
			smallest_z = v.pos.z;
		if (v.pos.z > largest_z)
			largest_z = v.pos.z;
	}

	//load the texture
	std::string textureName = materials[shape.mesh.material_ids[0]].diffuse_texname;
	std::string base_dir = GetBaseDir(filename);
	std::string texturePath = base_dir + "/" + textureName;
	std::wstring texturePathW = StringToWString(texturePath);
	CreateWICTextureFromFile(mD3DDevice, texturePathW.c_str(), NULL, &mTextureResourceView);


	mBoundingRect.lowerLeft = XMFLOAT2(smallest_x, smallest_z);
	mBoundingRect.upperRight = XMFLOAT2(largest_x, largest_z);


	//adjust the y so it's on zero
	if (smallest_y != 0)
	{

		float neg_y = -smallest_y;
		for (int i = 0; i < mNumVertices; i++)
			mVertices[i].pos.y += neg_y;

	}


	//create the index buffer
	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(DWORD) * mIndexList.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &mIndexList[0];
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);

	mNumIndices = mIndexList.size();

	//create the vertex buffer
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosNormTex) * mNumVertices; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = mVertices;

	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);



	//generate a BVH

	//find the longest side of bounding box
	float x_length = largest_x - smallest_x;
	float y_length = largest_y - smallest_y;
	float z_length = largest_z - smallest_z;

	//find which side is the longest
	char side;

	if (x_length > y_length)
	{
		if (x_length > z_length) // x is the largest
			side = 'x';
		else //z is the largest
			side = 'z';

	}
	else
	{
		if (y_length > z_length) //y is the largest
			side = 'y';
		else //z is the larges
			side = 'z';

	}



	std::vector<_bounding_box_tri> triBoundingBoxes;


	for (UINT triangle = 0; triangle < mIndexList.size(); triangle += 3)
	{
		Vertex::PosNormTex v[3];

		v[0] = mVertices[mIndexList[triangle + 0]];
		v[1] = mVertices[mIndexList[triangle + 1]];
		v[2] = mVertices[mIndexList[triangle + 2]];


		_terrain_triangle t;
		t.vertices[0] = v[0];
		t.vertices[1] = v[1];
		t.vertices[2] = v[2];

		mTriangleList.push_back(t);


		_bounding_box_tri bb;
		FindBoundingBox(v, 3, bb);
		bb.tri_index = mTriangleList.size() - 1;

		triBoundingBoxes.push_back(bb);

	}

	mBVHTree = GenerateBVHTree(triBoundingBoxes, 10);
}



void Terrain::CreateFromHeightMap(ID3D11Device* device, std::wstring pathToHeightmap, float floorScale, float heightScale)
{
	mD3DDevice = device;
	mD3DDevice->GetImmediateContext(&mDeviceContext);

	mHeightMap = new SoftwareBitmap::Bitmap(pathToHeightmap);
	int bmpHeight = mHeightMap->GetHeight();
	int bmpWidth = mHeightMap->GetWidth();

	//create the vertices
	int numVertices = bmpWidth*bmpHeight;
	Vertex::PosNormTex* floorMesh = new Vertex::PosNormTex[numVertices];
	UCHAR* source_mem = (UCHAR*)mHeightMap->GetData();

	//position the terrain so it's in the middle of the level
	float dimx_center = bmpWidth * floorScale / 2.0f;
	float dimy_center = bmpHeight * floorScale / 2.0f;

	
	//create a grid
	for (int zdim = 0; zdim < bmpHeight; zdim++)
	{
		for (int xdim = 0; xdim < bmpWidth; xdim++)
		{
			float height = source_mem[xdim + zdim*mHeightMap->GetPitch()];
			floorMesh[xdim + zdim*bmpWidth] = { XMFLOAT3((float)xdim*floorScale - dimx_center, height * heightScale , (float)zdim*floorScale - dimy_center), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(float(zdim) / bmpHeight,float(xdim) / bmpWidth) };
		}
	}

	//get the true bounds
	float smallestX = -std::numeric_limits<float>::lowest();
	float largestX = std::numeric_limits<float>::lowest();
	float smallestZ = -std::numeric_limits<float>::lowest();
	float largestZ = std::numeric_limits<float>::lowest();
	for (int i = 0; i < numVertices; i++)
	{

		if (floorMesh[i].pos.x < smallestX)
			smallestX = floorMesh[i].pos.x;
		
		if (floorMesh[i].pos.x > largestX)
			largestX = floorMesh[i].pos.x;

		if (floorMesh[i].pos.z < smallestZ)
			smallestZ = floorMesh[i].pos.z;

		if (floorMesh[i].pos.z > largestZ)
			largestZ = floorMesh[i].pos.z;

	}

	mBoundingRect.lowerLeft = XMFLOAT2(smallestX, smallestZ);
	mBoundingRect.upperRight = XMFLOAT2(largestX, largestZ);


	//save the vertex list so that we don't have to lock the vertex buffer constantly
	std::vector<Vertex::PosNormTex>vertexList = std::vector<Vertex::PosNormTex>(floorMesh, floorMesh + bmpWidth*bmpHeight);
	

	//create the indices
	int numXCells = bmpWidth - 1;
	int numYCells = bmpHeight - 1;
	int vertsPerRow = bmpWidth;


	std::vector<DWORD> indices;

	for (int cellX = 0; cellX < numXCells; cellX++)
	{
		for (int cellY = 0; cellY < numYCells; cellY++)
		{
			int	index0 = cellY * vertsPerRow + cellX;
			int	index1 = index0 + 1;
			int	index2 = (cellY + 1) * vertsPerRow + cellX;
			int	index3 = index2 + 1;


			indices.push_back(index0);
			indices.push_back(index3);
			indices.push_back(index1);

			indices.push_back(index0);
			indices.push_back(index2);
			indices.push_back(index3);

		}
	}


	
	//create the index buffer
	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(DWORD) * indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &indices[0];
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);

	//calculate the normals
	for (int vertIndex = 0; vertIndex < numVertices; vertIndex++)
	{
		XMVECTOR normTotal = { 0.0f, 0.0f, 0.0f };
		
		
		//for each index triplet
		int faceCount = 0;
		for (UINT indIndex = 0; indIndex < mNumIndices; indIndex += 3)
		{
			
			if ((indices[indIndex + 0] == vertIndex) || (indices[indIndex + 1] == vertIndex) || (indices[indIndex + 2] == vertIndex))
			{
				XMVECTOR v0_pos = XMLoadFloat3(&floorMesh[indices[indIndex + 0]].pos);
				XMVECTOR v1_pos = XMLoadFloat3(&floorMesh[indices[indIndex + 1]].pos);
				XMVECTOR v2_pos = XMLoadFloat3(&floorMesh[indices[indIndex + 2]].pos);

				XMVECTOR sideA = v1_pos - v0_pos;
				XMVECTOR sideB = v2_pos - v0_pos;

				XMVECTOR faceNormal = XMVector3Cross(sideA, sideB);

				normTotal += faceNormal;
				faceCount++;

				//there could only be 6 faces that a single vertex can influence, so quit early to save time
				if (faceCount == 6)
					break;
			}
		}
		

		//get the average of all the normals (avg = (n1 + n2 + n3..) / length(n1 + n2 + n3)
		normTotal = normTotal / XMVector3Length(normTotal);
	
		XMStoreFloat3(&floorMesh[vertIndex].norm, normTotal);

	

	}


	//create the vertex buffer
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosNormTex) * vertexList.size(); //total size of buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = floorMesh;

	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);

	delete floorMesh;
	floorMesh = nullptr;

}


XMFLOAT3 FindMidPoint(XMFLOAT3 p1, XMFLOAT3 p2)
{
	//divide the square into two triangles and find which triangle the player is in
	XMVECTOR point0 = XMLoadFloat3(&p1);
	XMVECTOR point1 = XMLoadFloat3(&p2);

	XMVECTOR baseMidpoint = (point1 + point0) / 2;
	XMFLOAT3 outFloat;
	XMStoreFloat3(&outFloat, baseMidpoint);

	return outFloat;
}



float FindTriangleArea(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2)
{
	XMVECTOR point0 = XMLoadFloat3(&p0);
	XMVECTOR point1 = XMLoadFloat3(&p1);
	XMVECTOR point2 = XMLoadFloat3(&p2);

	XMVECTOR BA = point1 - point0;
	XMVECTOR BC = point1 - point2;

	XMVECTOR BAlen, BClen;
	XMFLOAT3 BAlenf, BClenf;

	BAlen = XMVector3Length(BA);
	BClen = XMVector3Length(BC);

	XMStoreFloat3(&BAlenf, BAlen);
	XMStoreFloat3(&BClenf, BClen);


	XMVECTOR angle = XMVector3AngleBetweenVectors(BA, BC);
	XMFLOAT3 anglef;
	XMStoreFloat3(&anglef, angle);


	return 0.5f * BAlenf.x * BClenf.x * sin(anglef.x);
}




void Terrain::_CreateBoundaryCollisionBoxes()
{

	//create default bounding boxes around the edges
	XMFLOAT2 ll, ur, ul, lr;
	ll = mBoundingRect.lowerLeft;
	ur = mBoundingRect.upperRight;

	ul.x = ll.x;
	ul.y = ur.y;

	lr.x = ur.x;
	lr.y = ll.y;

	CollisionBox leftCB, rightCB, lowCB, upperCB;

	leftCB.init(ll, ul);
	rightCB.init(ul, ur);
	lowCB.init(ur, lr);
	upperCB.init(lr, ll);

	mPhysicsEngine->AddStaticCollisionBox(leftCB);
	mPhysicsEngine->AddStaticCollisionBox(rightCB);
	mPhysicsEngine->AddStaticCollisionBox(lowCB);
	mPhysicsEngine->AddStaticCollisionBox(upperCB);
}

void Terrain::_CreateCollisionBoxes(std::vector<tinyobj::shape_t>& shapes, tinyobj::attrib_t& attrib, std::vector<UINT>& collisionBoxIndexList)
{
	//create the default bounding boxes along the edge of the terrain
	_CreateBoundaryCollisionBoxes();


	std::vector<frect_t> collisionBoxList;

	if (collisionBoxIndexList.size() > 0)
	{
		for (UINT collisionShapeIndex : collisionBoxIndexList)
		{
			std::map<tinyobj::index_t, int, index_comparator> uniqueVertexMap;

			//go through each index and find unique entries
			for (tinyobj::index_t i : shapes[collisionShapeIndex].mesh.indices)
				uniqueVertexMap.insert(std::pair<tinyobj::index_t, int>(i, uniqueVertexMap.size()));


			//allocate space for the vertices
			Vertex::PosNormTex* collisionBoxVerts = new Vertex::PosNormTex[uniqueVertexMap.size()];
			UINT numCollisionBoxVertices = uniqueVertexMap.size();

			for (auto& keyval : uniqueVertexMap)
			{

				Vertex::PosNormTex vert;

				//get the position of each vertex
				tinyobj::real_t vx = attrib.vertices[3 * keyval.first.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * keyval.first.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * keyval.first.vertex_index + 2];

				//set the positions 
				vert.pos.x = vx;
				vert.pos.y = vy;
				vert.pos.z = vz;

				//save the vertex
				collisionBoxVerts[keyval.second] = vert;
			}


			//find boundaries
			float smallest_x = 100000000.0f;
			float smallest_y = 100000000.0f;
			float smallest_z = 100000000.0f;
			float largest_x = -100000000.0f;
			float largest_y = -100000000.0f;
			float largest_z = -100000000.0f;

			for (int i = 0; i < numCollisionBoxVertices; i++)
			{
				Vertex::PosNormTex v = collisionBoxVerts[i];
				if (v.pos.x < smallest_x)
					smallest_x = v.pos.x;
				if (v.pos.x > largest_x)
					largest_x = v.pos.x;
				if (v.pos.y < smallest_y)
					smallest_y = v.pos.y;
				if (v.pos.y > largest_y)
					largest_y = v.pos.y;
				if (v.pos.z < smallest_z)
					smallest_z = v.pos.z;
				if (v.pos.z > largest_z)
					largest_z = v.pos.z;
			}

			frect_t collisionBoundingRect;

			collisionBoundingRect.lowerLeft = XMFLOAT2(smallest_x, smallest_z);
			collisionBoundingRect.upperRight = XMFLOAT2(largest_x, largest_z);

			collisionBoxList.push_back(collisionBoundingRect);

			//release the memory
			delete collisionBoxVerts;
			collisionBoxVerts = nullptr;
		}
	}


	for (frect_t& collisionBox : collisionBoxList)
	{
		CollisionBox cb;
		cb.init(collisionBox.lowerLeft, collisionBox.upperRight);
		mPhysicsEngine->AddStaticCollisionBox(cb);
	}
}



void Terrain::LoadFromMemory(tinyobj::shape_t & shape, tinyobj::attrib_t & attrib, std::vector<tinyobj::material_t>& materials, std::string filename)
{
	std::map<tinyobj::index_t, int, index_comparator> uniqueVertexMap;

	//go through each index and find unique entries
	for (tinyobj::index_t i : shape.mesh.indices)
		uniqueVertexMap.insert(std::pair<tinyobj::index_t, int>(i, uniqueVertexMap.size()));


	//allocate space for the vertices
	mVertices = new Vertex::PosNormTex[uniqueVertexMap.size()];
	mNumVertices = uniqueVertexMap.size();

	for (auto& keyval : uniqueVertexMap)
	{
		tinyobj::real_t vx = attrib.vertices[3 * keyval.first.vertex_index + 0];
		tinyobj::real_t vy = attrib.vertices[3 * keyval.first.vertex_index + 1];
		tinyobj::real_t vz = attrib.vertices[3 * keyval.first.vertex_index + 2];

		tinyobj::real_t nx = attrib.normals[3 * keyval.first.normal_index + 0];
		tinyobj::real_t ny = attrib.normals[3 * keyval.first.normal_index + 1];
		tinyobj::real_t nz = attrib.normals[3 * keyval.first.normal_index + 2];

		tinyobj::real_t tx = attrib.texcoords[2 * keyval.first.texcoord_index + 0];
		tinyobj::real_t ty = attrib.texcoords[2 * keyval.first.texcoord_index + 1];

		// Optional: vertex colors
		// tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
		// tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
		// tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];

		//
		// per-face material
		//shapes[s].mesh.material_ids[f];

		Vertex::PosNormTex vert;

		vert.pos.x = vx;
		vert.pos.y = vy;
		vert.pos.z = vz;

		vert.norm.x = nx;
		vert.norm.y = ny;
		vert.norm.z = nz;

		vert.uv.x = tx;
		vert.uv.y = ty;

		mVertices[keyval.second] = vert;
	}

	//now re-index the old index list
	for (tinyobj::index_t i : shape.mesh.indices)
		mIndexList.push_back(uniqueVertexMap[i]);


	//find boundaries
	float smallest_x = 100000000.0f;
	float smallest_y = 100000000.0f;
	float smallest_z = 100000000.0f;
	float largest_x = -100000000.0f;
	float largest_y = -100000000.0f;
	float largest_z = -100000000.0f;

	for (int i = 0; i < mNumVertices; i++)
	{
		Vertex::PosNormTex v = mVertices[i];
		if (v.pos.x < smallest_x)
			smallest_x = v.pos.x;
		if (v.pos.x > largest_x)
			largest_x = v.pos.x;
		if (v.pos.y < smallest_y)
			smallest_y = v.pos.y;
		if (v.pos.y > largest_y)
			largest_y = v.pos.y;
		if (v.pos.z < smallest_z)
			smallest_z = v.pos.z;
		if (v.pos.z > largest_z)
			largest_z = v.pos.z;
	}

	//load the texture
	std::string textureName = materials[shape.mesh.material_ids[0]].diffuse_texname;
	std::string base_dir = GetBaseDir(filename);
	std::string texturePath = base_dir + "/" + textureName;
	std::wstring texturePathW = StringToWString(texturePath);
	CreateWICTextureFromFile(mD3DDevice, texturePathW.c_str(), NULL, &mTextureResourceView);


	mBoundingRect.lowerLeft = XMFLOAT2(smallest_x, smallest_z);
	mBoundingRect.upperRight = XMFLOAT2(largest_x, largest_z);

/*
	//adjust the y so it's on zero
	if (smallest_y != 0)
	{

		float neg_y = -smallest_y;
		for (int i = 0; i < mNumVertices; i++)
			mVertices[i].pos.y += neg_y;

	}

	*/


	//create the index buffer
	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.ByteWidth = sizeof(DWORD) * mIndexList.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;


	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &mIndexList[0];
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mIB);

	mNumIndices = mIndexList.size();

	//create the vertex buffer
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosNormTex) * mNumVertices; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = mVertices;

	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mVB);



	//generate a BVH

	//find the longest side of bounding box
	float x_length = largest_x - smallest_x;
	float y_length = largest_y - smallest_y;
	float z_length = largest_z - smallest_z;

	//find which side is the longest
	char side;

	if (x_length > y_length)
	{
		if (x_length > z_length) // x is the largest
			side = 'x';
		else //z is the largest
			side = 'z';

	}
	else
	{
		if (y_length > z_length) //y is the largest
			side = 'y';
		else //z is the larges
			side = 'z';

	}



	std::vector<_bounding_box_tri> triBoundingBoxes;


	for (UINT triangle = 0; triangle < mIndexList.size(); triangle += 3)
	{
		Vertex::PosNormTex v[3];

		v[0] = mVertices[mIndexList[triangle + 0]];
		v[1] = mVertices[mIndexList[triangle + 1]];
		v[2] = mVertices[mIndexList[triangle + 2]];


		_terrain_triangle t;
		t.vertices[0] = v[0];
		t.vertices[1] = v[1];
		t.vertices[2] = v[2];

		mTriangleList.push_back(t);


		_bounding_box_tri bb;
		FindBoundingBox(v, 3, bb);
		bb.tri_index = mTriangleList.size() - 1;

		triBoundingBoxes.push_back(bb);

	}

	mBVHTree = GenerateBVHTree(triBoundingBoxes, 10);
}

void Terrain::SetTexture(std::wstring texturePath)
{
	HRESULT hr = CreateWICTextureFromFile(mD3DDevice, texturePath.c_str(), NULL, &mTextureResourceView);
}

void Terrain::GetHeightAtPosition(XMFLOAT3& playerPos, float& OutHeight)
{
	//OutHeight = 0;
	//playerPos = XMFLOAT3(16.34f, 5.75f, 9.86f);

	playerPos.y += 1;

	
	XMVECTOR rayOrigin = XMLoadFloat3(&playerPos);
	XMVECTOR rayDirection = { 0.0f, -1.0f, 0.0f };


	//find where in the space tree where located
	std::vector<_bounding_box_tri> trianglesToCheck;
	IntersectRayWithBVHTree(mBVHTree, rayOrigin, rayDirection, trianglesToCheck);
	//FindPointInTree(mBVHTree, rayOrigin, trianglesToCheck);

	//if point is outside of the tree, find the nearest location above the tree
	if (!trianglesToCheck.size())
	{
		rayDirection = { 0.0f, 1.0f, 0.0f };
		IntersectRayWithBVHTree(mBVHTree, rayOrigin, rayDirection, trianglesToCheck);

		if (!trianglesToCheck.size())
			return;
	}

	struct _playerTriangle
	{
		XMFLOAT3 points[3];
	}playerTriangle;

	bool intersectionFound = false;

	for (UINT triangle = 0; triangle < trianglesToCheck.size(); triangle++)
	{
		_terrain_triangle tri = mTriangleList[trianglesToCheck[triangle].tri_index];
		XMVECTOR v0 = XMLoadFloat3(&tri.vertices[0].pos);
		XMVECTOR v1 = XMLoadFloat3(&tri.vertices[1].pos);
		XMVECTOR v2 = XMLoadFloat3(&tri.vertices[2].pos);


		float distanceToTri;
		if (TriangleTests::Intersects(rayOrigin, rayDirection, v0, v1, v2, distanceToTri))
		{
			XMStoreFloat3(&playerTriangle.points[0], v0);
			XMStoreFloat3(&playerTriangle.points[1], v1);
			XMStoreFloat3(&playerTriangle.points[2], v2);

			intersectionFound = true;
			break;
		}
	}

	if (intersectionFound == false)
	{
		rayDirection = { 0.0f, 1.0f, 0.0f };
		for (UINT triangle = 0; triangle < trianglesToCheck.size(); triangle++)
		{
			_terrain_triangle tri = mTriangleList[trianglesToCheck[triangle].tri_index];
			XMVECTOR v0 = XMLoadFloat3(&tri.vertices[0].pos);
			XMVECTOR v1 = XMLoadFloat3(&tri.vertices[1].pos);
			XMVECTOR v2 = XMLoadFloat3(&tri.vertices[2].pos);


			float distanceToTri;
			if (TriangleTests::Intersects(rayOrigin, rayDirection, v0, v1, v2, distanceToTri))
			{
				XMStoreFloat3(&playerTriangle.points[0], v0);
				XMStoreFloat3(&playerTriangle.points[1], v1);
				XMStoreFloat3(&playerTriangle.points[2], v2);

				intersectionFound = true;
				break;
			}
		}
	}

	


	//find the heighest and lowest vertices in the triangle

	//organize the triangle points by height (selection sort)
	for (int runCount = 0; runCount < 3; runCount++)
	{
		if (playerTriangle.points[0].y < playerTriangle.points[1].y)
		{
			if (playerTriangle.points[0].y < playerTriangle.points[2].y)
			{
				if (playerTriangle.points[1].y < playerTriangle.points[2].y)
					break;
				else
				{
					XMFLOAT3 temp = playerTriangle.points[1];
					playerTriangle.points[1] = playerTriangle.points[2];
					playerTriangle.points[2] = temp;


					break;
				}
			}
			else
			{
				XMFLOAT3 temp = playerTriangle.points[0];
				playerTriangle.points[0] = playerTriangle.points[2];
				playerTriangle.points[2] = temp;

				continue;
			}
		}
		else
		{
			XMFLOAT3 temp = playerTriangle.points[0];
			playerTriangle.points[0] = playerTriangle.points[1];
			playerTriangle.points[1] = temp;

			continue;
		}
	}

	//find the precentage of the way the player is up between the two points of the triangle 

	XMVECTOR triPointsVect[3];
	triPointsVect[0] = XMLoadFloat3(&playerTriangle.points[0]);
	triPointsVect[1] = XMLoadFloat3(&playerTriangle.points[1]);
	triPointsVect[2] = XMLoadFloat3(&playerTriangle.points[2]);

	XMFLOAT3 midpointf0, midpointf1;
	XMStoreFloat3(&midpointf0, triPointsVect[0]);
	XMStoreFloat3(&midpointf1, triPointsVect[1]);

	XMFLOAT3 baseMidpointf = FindMidPoint(midpointf0, midpointf1);
	XMVECTOR baseMidPoint = XMLoadFloat3(&baseMidpointf);

	XMVECTOR pt2ToMid = triPointsVect[2] - baseMidPoint;
	XMFLOAT3 pt2ToMidLenf;
	XMStoreFloat3(&pt2ToMidLenf, XMVector3Length(pt2ToMid));

	XMFLOAT3 flatPlayerPos = playerPos;
	flatPlayerPos.y = 0.0f;

	XMVECTOR playerPosV = XMLoadFloat3(&flatPlayerPos);
	XMVECTOR pt2ToPlayer = triPointsVect[2] - playerPosV;
	XMFLOAT3 pt2ToPlayerLenf;
	XMStoreFloat3(&pt2ToPlayerLenf, XMVector3Length(pt2ToPlayer));

	float percentageToGoal = pt2ToMidLenf.x / pt2ToPlayerLenf.x;

	XMVECTOR finalPlayerPos = baseMidPoint + pt2ToPlayer*percentageToGoal;
	XMFLOAT3 finalPlayerPosf;
	XMStoreFloat3(&finalPlayerPosf, finalPlayerPos);


	

	if (finalPlayerPosf.y != 0)
		int x = 0;

	OutHeight = finalPlayerPosf.y;
	
	if (OutHeight < -100)
		int x = 0;


/*

	int bmpHeight = mHeightMap->GetHeight();
	int bmpWidth = mHeightMap->GetWidth();

	float dimx_center = bmpWidth * mFloorScale / 2.0f;
	float dimy_center = bmpHeight * mFloorScale / 2.0f;

	//transform position to zero
	float normalizedX;
	float normalizedY;

	normalizedX = (playerPos.x + dimx_center) / (dimx_center * 2);
	normalizedY = (playerPos.y + dimy_center) / (dimy_center * 2);


	//make sure the player is within level limits
	if (normalizedX >= 1 || normalizedY >= 1 || normalizedX <= 0 || normalizedY <= 0)
	{

		return;
	}

	//find the fractional position
	float xPos = normalizedX * bmpWidth;
	float yPos = normalizedY * bmpHeight;



	UCHAR* bmpData = mHeightMap->GetData();
	float finalHeight = bmpData[int(xPos + 0.5f) + int(yPos + 0.5f)*mHeightMap->GetPitch()];
	**OutHeight = finalHeight;
	*/
}

void Terrain::GetBoundingRect(XMFLOAT2 & outLowerLeft, XMFLOAT2 & outUpperRight) const
{
	outLowerLeft = mBoundingRect.lowerLeft;
	outUpperRight = mBoundingRect.upperRight;
}

DebugBox::DebugBox()
{
	
}

void DebugBox::Init(ID3D11Device* D3DDevice, ID3D11DeviceContext* DeviceContext)
{
	mD3DDevice = D3DDevice;
	mDeviceContext = DeviceContext;


	//creat the index buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(DWORD) * 36;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	


	DWORD indices[36] =
	{
		3,1,0,
		2,1,3,

		0,5,4,
		1,5,0,

		3,4,7,
		0,4,3,

		1,6,5,
		2,6,1,

		2,7,6,
		3,7,2,

		6,4,5,
		7,4,6,
	};

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = indices;
	//mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &mDebugBoxIB);


	D3D11_RASTERIZER_DESC rasterizerStateWireframe;
	rasterizerStateWireframe.CullMode = D3D11_CULL_NONE;
	rasterizerStateWireframe.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerStateWireframe.FrontCounterClockwise = false;
	rasterizerStateWireframe.DepthBias = false;
	rasterizerStateWireframe.DepthBiasClamp = 0;
	rasterizerStateWireframe.SlopeScaledDepthBias = 0;
	rasterizerStateWireframe.DepthClipEnable = true;
	rasterizerStateWireframe.ScissorEnable = false;
	rasterizerStateWireframe.MultisampleEnable = false;
	rasterizerStateWireframe.AntialiasedLineEnable = true;
	mD3DDevice->CreateRasterizerState(&rasterizerStateWireframe, &mRasterizerStateWireframe);

}

DebugBox::~DebugBox()
{
	for (unsigned int i = 0; i < mDebugBoxVB.size(); i++)
	{
		if (mDebugBoxVB[i])
		{
			mDebugBoxVB[i]->Release();
			mDebugBoxVB[i] = nullptr;
		}
	}

	if (mDebugBoxIB)
	{
		mDebugBoxIB->Release();
		mDebugBoxIB = nullptr;
	}
}

void DebugBox::AddBox(const _bounding_box & box, UINT id)
{
	//prevent repeat creation 
	if (id < mDebugBoxVB.size())
		return;

	//create a vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex::PosColor) * 24; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	/*
	Vertex::PosColor vertices[] =
	{
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	};

	*/

	Vertex::PosColor vertices[24] =
	{
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f),XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(-0.5f, 0.5f, -0.5f),XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },


		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(-0.5f, 0.5f, -0.5f),XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },

		{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },


		
	};


	ID3D11Buffer* vertex_buffer;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &vertex_buffer);

	mDebugBoxVB.push_back(vertex_buffer);
	mPositionList.push_back(box.center);
	mScaleList.push_back(XMFLOAT3(box.x_extent * 2, box.y_extent * 2, box.z_extent * 2));

}

void DebugBox::Draw()
{

	//save old inputs
	ID3D11RasterizerState* oldRasterizerState;
	mDeviceContext->RSGetState(&oldRasterizerState);
	D3D_PRIMITIVE_TOPOLOGY oldTopology;
	mDeviceContext->IAGetPrimitiveTopology(&oldTopology);

	//set new inputs
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//mDeviceContext->IASetIndexBuffer(mDebugBoxIB, DXGI_FORMAT_R32_UINT, 0);
	//mDeviceContext->RSSetState(mRasterizerStateWireframe);

	UINT stride = sizeof(Vertex::PosColor);
	UINT offset = 0;

	static UINT box_count = 0;

	if (GetAsyncKeyState(VK_END))
	{
		box_count++;
		Sleep(100);
	}
	/*
	for (UINT i = 9; i < 11; i++)
	{
		ConstantBuffers::WorldMatrices worldBuffer;
		XMMATRIX scale;

		worldBuffer.World = XMMatrixTranslation(mPositionList[i].x, mPositionList[i].y, mPositionList[i].z);
		scale = XMMatrixScaling(mScaleList[i].x, mScaleList[i].y, mScaleList[i].z);
		worldBuffer.World = XMMatrixTranspose(XMMatrixMultiply(scale, worldBuffer.World));

		mDeviceContext->UpdateSubresource(ConstantBuffers::WorldMatrixBuffer, 0, NULL, &worldBuffer, 0, 0);

		mDeviceContext->IASetVertexBuffers(0, 1, &mDebugBoxVB[i], &stride, &offset);
		mDeviceContext->Draw(24, 0);
	
	}
	*/

	ConstantBuffers::WorldMatrices worldBuffer;
	XMMATRIX scale;

	worldBuffer.World = XMMatrixTranslation(mPositionList[4].x, mPositionList[4].y, mPositionList[4].z);
	scale = XMMatrixScaling(mScaleList[4].x, mScaleList[4].y, mScaleList[4].z);
	worldBuffer.World = XMMatrixTranspose(XMMatrixMultiply(scale, worldBuffer.World));

	mDeviceContext->UpdateSubresource(ConstantBuffers::WorldMatrixBuffer, 0, NULL, &worldBuffer, 0, 0);

	mDeviceContext->IASetVertexBuffers(0, 1, &mDebugBoxVB[4], &stride, &offset);
	mDeviceContext->Draw(24, 0);

	//restore old inputs
	mDeviceContext->RSSetState(oldRasterizerState);
	mDeviceContext->IASetPrimitiveTopology(oldTopology);

}	


/*
ObjectSet::ObjectSet()
{
}

void ObjectSet::SetMasterListAndDevices(std::vector<_terrain_triangle>* masterList, ID3D11Device* device)
{
	mMasterList = masterList;
	mD3DDevice = device;
	mD3DDevice->GetImmediateContext(&mContext);
}

void ObjectSet::CreateNewSet(unsigned int Id)
{
	for (auto i_set : mSetList)
		if (i_set.setId == Id)
			return;

	_set newSet;
	newSet.setId = Id;

	newSet.SetDevice(mD3DDevice);

	mSetList.push_back(newSet);
}

void ObjectSet::AddObjectToSet(UINT setId, UINT objectOffset)
{
	for (_set& i_set : mSetList)
		if (i_set.setId == setId)
		{
			if (i_set.isBaked)
				return;

			i_set.offsetList.push_back(objectOffset);
			break;
		}
			
}

void ObjectSet::AddSetToDrawList(UINT setId)
{
	mDrawList.push_back(setId);
}

void ObjectSet::Draw()
{
	//bake the sets and draw them
	for (auto& i_set : mSetList)
	{
		if (!i_set.isBaked)
		{

			if (i_set.offsetList.size() == 0)
				return;

			UINT numVerts = i_set.offsetList.size() * 3;
			Vertex::PosColor* vertices = new Vertex::PosColor[numVerts];

		

			for (UINT i = 0; i < i_set.offsetList.size(); i++)
			{
				_terrain_triangle tri = (*mMasterList)[i_set.offsetList[i]];
				vertices[i*3 + 0] = { tri.vertices->pos, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) };
				vertices[i*3 + 1] = { tri.vertices->pos, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) };
				vertices[i*3 + 2] = { tri.vertices->pos, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) };
			}

			//create a vertex buffer
			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.ByteWidth = sizeof(Vertex::PosColor) * numVerts; //total size of buffer in bytes
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = vertices;
			mD3DDevice->CreateBuffer(&bufferDesc, &InitData, &i_set.VB);

			i_set.vertexCount = numVerts;
			i_set.isBaked = true;
		}

		//if on the draw list then draw the set
		for(auto i_drawlist : mDrawList)
			if(i_drawlist == i_set.setId)
				Shaders::SimpleColorShader::Render(mContext, &i_set);

	}
}

_set::_set()
{
	isBaked = false;
	VB = nullptr;
	vertexCount = 0;
	mDevice = nullptr;
	mContext = nullptr;
}

void _set::SetDevice(ID3D11Device * device)
{
	mDevice = device;
	mDevice->GetImmediateContext(&mContext);
}

void _set::Draw()
{
	UINT stride = sizeof(Vertex::PosColor);
	UINT offset = 0;

	mContext->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	mContext->Draw(vertexCount, 0);
}

*/