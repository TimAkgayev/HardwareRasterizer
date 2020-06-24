#include "SceneManager.h"
#include "Utility.h"


class index_comparator
{
public:
	bool operator()(const tinyobj::index_t& lhv, const tinyobj::index_t& rhv) const
	{
		return std::tie(lhv.vertex_index, lhv.normal_index, lhv.texcoord_index) < std::tie(rhv.vertex_index, rhv.normal_index, rhv.texcoord_index);
	}
};


SceneManager::SceneManager(ID3D11Device* device): mPlayer(nullptr), mTerrain(nullptr), mPhysicsEngine(nullptr), mDevice(device), mSkybox(nullptr)
{
	mDevice->GetImmediateContext(&mDeviceContext);
}

SceneManager::~SceneManager()
{
	mClearOldLevel();
}

CharacterController * SceneManager::GetPlayer() const
{
	return mPlayer;
}

Terrain * SceneManager::GetTerrain() const
{
	return mTerrain;
}

PhysicsEngine * SceneManager::GetPhysicsEngine() const
{
	return mPhysicsEngine;
}

void SceneManager::Update(float dt)
{
	mPlayer->Update(dt);
}

void SceneManager::LoadLevel(std::string filename)
{
	//clear the world
	mClearOldLevel();

	//create the physics engine 
	mPhysicsEngine = new PhysicsEngine();

	//parse the world
	std::string inputfile = filename;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	std::string base_dir = GetBaseDir(inputfile);


	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), base_dir.c_str());

	if (!warn.empty()) {

	}

	if (!err.empty()) {
		return;
	}

	if (!ret) {

	}

	struct object
	{
		Vertex::PosNormTex* vertexList;
		int numVertices;
		std::vector<unsigned int> indexList;
		std::string textureName;
	};

	//list of temporary world objects extracted from the file
	std::vector<object> objList;
	std::vector<object> collisionObjList;


	bool playerPosProvided = false;
	XMFLOAT3 providedPlayerPos;

	// Loop over shapes and extract each object
	for (unsigned int s = 0; s < shapes.size(); s++)
	{
		if (shapes[s].name.find("playerposition") != std::string::npos)
		{
			playerPosProvided = true;
			
			//take any vertex from the shape as the player's position
			tinyobj::real_t vx = attrib.vertices[3 * shapes[s].mesh.indices[0].vertex_index + 0];
			tinyobj::real_t vy = attrib.vertices[3 * shapes[s].mesh.indices[0].vertex_index + 1];
			tinyobj::real_t vz = attrib.vertices[3 * shapes[s].mesh.indices[0].vertex_index + 2];

			providedPlayerPos = { vx, vy, vz };

			continue;
		}
		else if (shapes[s].name.find("terrain") != std::string::npos)
		{
			if (!mTerrain)
			{
				//just pass the file on to the terrain's load method
				mTerrain = new Terrain(mDevice, mPhysicsEngine);
				mTerrain->LoadFromMemory(shapes[s], attrib, materials, inputfile);
				mPlayer->SetTerrain(mTerrain);

			}

			continue;
		}
		else if (shapes[s].name.find("collision") != std::string::npos)
		{
			object collision_obj;
			std::map<tinyobj::index_t, int, index_comparator> uniqueVertexMap;

			//go through each index and find unique entries
			for (tinyobj::index_t i : shapes[s].mesh.indices)
				uniqueVertexMap.insert(std::pair<tinyobj::index_t, int>(i, uniqueVertexMap.size()));


			//allocate space for the vertices
			collision_obj.vertexList = new Vertex::PosNormTex[uniqueVertexMap.size()];
			collision_obj.numVertices = uniqueVertexMap.size();

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

				collision_obj.vertexList[keyval.second] = vert;
			}

			//now re-index the old index list
			for (tinyobj::index_t i : shapes[s].mesh.indices)
				collision_obj.indexList.push_back(uniqueVertexMap[i]);

			//now save the texture name
			if (materials.size())
				collision_obj.textureName = materials[shapes[s].mesh.material_ids[0]].diffuse_texname;
			else
				collision_obj.textureName = "standin_texture.jpg";

			collisionObjList.push_back(collision_obj);
		}
		else //anything without a lable will just be loaded as a world object
		{

			object obj;
			std::map<tinyobj::index_t, int, index_comparator> uniqueVertexMap;

			//go through each index and find unique entries
			for (tinyobj::index_t i : shapes[s].mesh.indices)
				uniqueVertexMap.insert(std::pair<tinyobj::index_t, int>(i, uniqueVertexMap.size()));


			//allocate space for the vertices
			obj.vertexList = new Vertex::PosNormTex[uniqueVertexMap.size()];
			obj.numVertices = uniqueVertexMap.size();

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

				obj.vertexList[keyval.second] = vert;
			}

			//now re-index the old index list
			for (tinyobj::index_t i : shapes[s].mesh.indices)
				obj.indexList.push_back(uniqueVertexMap[i]);

			//now save the texture name
			if (materials.size())
				obj.textureName = materials[shapes[s].mesh.material_ids[0]].diffuse_texname;
			else
				obj.textureName = "standin_texture.jpg";

			objList.push_back(obj);
		}
	}

		
	//add the extracted objects to the world
	for (object obj : objList)
	{
		WorldObject* Obj = new WorldObject();
		std::string texturePath = base_dir + "/" + obj.textureName;
		std::wstring texturePathW = StringToWString(texturePath);
		Obj->Initialize(mDevice, XMFLOAT3(0.0f, 0.0f, 0.0f), obj.vertexList, obj.numVertices, obj.indexList, texturePathW, 1.0f);
		mWorldObjectList.push_back(Obj);

		delete obj.vertexList;

	}


	//add collision objects
	/*
	for (object obj : collisionObjList)
	{
		WorldObject* Obj = new WorldObject();
		std::string texturePath = base_dir + "/" + obj.textureName;
		std::wstring texturePathW = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(texturePath);
		Obj->Initialize(mDevice, XMFLOAT3(15.0f, 3.0f, 0.0f), obj.vertexList, obj.numVertices, obj.indexList, texturePathW, 3.0f);
		mWorldObjectList.push_back(Obj);

		delete obj.vertexList;

	}
	*/


	//create the player
	if (playerPosProvided)
		mPlayer = new CharacterController(mDeviceContext, mTerrain, mPhysicsEngine, &providedPlayerPos);
	else
		mPlayer = new CharacterController(mDeviceContext, mTerrain, mPhysicsEngine);


	//create the skybox
	mSkybox = new Skybox();
	mSkybox->init(mDevice, 2000.0f);

}

void SceneManager::Render()
{
	DirectXPipeline::ShadowMapShader::Render(mDeviceContext, mTerrain, (IDrawable**)&mWorldObjectList[0], mWorldObjectList.size());
	DirectXPipeline::SkyboxPipeline::Render(mDeviceContext, (WorldObject*)mSkybox);
}

void SceneManager::mClearOldLevel()
{
	if (mPlayer)
		delete mPlayer;

	if (mTerrain)
		delete mTerrain;

	if (mWorldObjectList.size())
		for (WorldObject* objPtr : mWorldObjectList)
			if (objPtr)
				delete objPtr;
	
	if (mPhysicsEngine)
		delete mPhysicsEngine;

	if (mSkybox)
		delete mSkybox;

	mPlayer = nullptr;
	mTerrain = nullptr;
	mPhysicsEngine = nullptr;
	mSkybox = nullptr;

	mWorldObjectList.clear();
	
}

