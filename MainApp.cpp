#include "DirectXApplication.h"
#include "Vertex.h"
#include "Terrain.h"
#include "Box.h"
#include "CharacterController.h"
#include "Shaders.h"
#include "ConstantBuffers.h"
#include "Skybox.h"
#include "Object.h"
#include "Light.h"
#include "PhysicsEngine.h"
#define TINYOBJLOADER_IMPLEMENTATION 
#include "tinyobj\tinyobjloader\tiny_obj_loader.h"
#include <string>
#include <vector>
#include <tuple>
#include <map>
//for converting std::string to std::wstring
#include <locale>
#include <codecvt>


class NewDXApp : public DirectXApplication
{
public:
	NewDXApp(HINSTANCE hinstance);
	~NewDXApp();

	void initApp() override;
	void updateScene(float dt) override;
	void drawScene() override;

	LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
	
	void mInitResources();
	void mLoadObject(std::string filename);
	void mLoadObjects();
	void mUpdateCamera(float dt);




private:

	unsigned int mCurrentCamID;
	ID3D11ShaderResourceView* mSkyCubeMapSRV;
	Terrain* terrain;
	CharacterController* mCharacterController;
	Skybox mSkybox;
	PhysicsEngine mPhysicsEngine;
	std::vector<Object*> mObjectList;
	
	Light* mDirectionalLight;
	Camera mFreeCamera;
	Box testBox;
};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


	NewDXApp theApp(hInstance);

	theApp.initApp();

	return theApp.run();
}



void NewDXApp::mUpdateCamera(float dt)
{
	//update constant buffers
	Camera& currentCamera = (mCurrentCamID == 0) ? mCharacterController->GetCamera() : mFreeCamera;

	if (mCurrentCamID == 0)
	{
		mFreeCamera.Lock();
		mCharacterController->GetCamera().Unlock();
	}
	else if (mCurrentCamID == 1)
	{
		mCharacterController->GetCamera().Lock();
		mFreeCamera.Unlock();
	}

	ConstantBuffers::ProjectionVariables projectionMatrices;
	projectionMatrices.View = XMMatrixTranspose(currentCamera.GetViewMatrix());
	projectionMatrices.Projection = XMMatrixTranspose(currentCamera.GetProjectionMatrix());
	mDeviceContext->UpdateSubresource(ConstantBuffers::ViewProjBuffer, 0, NULL, &projectionMatrices, 0, 0);

	ConstantBuffers::CameraPosition eyePos;
	eyePos.EyePosition = currentCamera.GetPosition();
	mDeviceContext->UpdateSubresource(ConstantBuffers::CameraPositionBuffer, 0, NULL, &eyePos, 0, 0);

	if (mCurrentCamID == 1)
	{
		float moveSpeed = 100.0f;
		
		if (GetAsyncKeyState(0x57)) //W key
			currentCamera.MoveAlongLookAt(dt* -1 * moveSpeed);

		if (GetAsyncKeyState(0x53)) //S key
			currentCamera.MoveAlongLookAt(dt * moveSpeed);



		if (GetAsyncKeyState(0x41)) //A key
			currentCamera.MoveAlongRight(dt* -1* moveSpeed);



		if (GetAsyncKeyState(0x44)) //D key
			currentCamera.MoveAlongRight(dt * moveSpeed);

	

		float turnSpeed = 200.0f;


		if (GetAsyncKeyState(VK_UP))
		{

			currentCamera.Pitch(-1 * dt*turnSpeed);
		}

		if (GetAsyncKeyState(VK_DOWN))
		{

			currentCamera.Pitch(1 * dt*turnSpeed);
		}

		if (GetAsyncKeyState(VK_LEFT))
		{

			currentCamera.Yaw(-1 * dt*turnSpeed);
		}

		if (GetAsyncKeyState(VK_RIGHT))
		{

			currentCamera.Yaw(1 * dt*turnSpeed);
		}

	}
}

NewDXApp::NewDXApp(HINSTANCE hinstance):DirectXApplication(hinstance)
{
}

NewDXApp::~NewDXApp()
{
	for (Object* o : mObjectList)
		delete o;
}

void NewDXApp::initApp()
{
	DirectXApplication::initApp();

	mInitResources();

	
	//terrain.CreateFromHeightMap(mD3DDevice, L"..\\HardwareRasterizer\\Heightmaps\\TestFloorSmall.bmp", 2.0f, 0.1);
	
	
	terrain = new Terrain(mD3DDevice, &mPhysicsEngine);

	std::string highResTerrain = "C:\\terrain.obj";
	std::string lowResTerrain = "C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\Objects\\LowPolyTerrain.obj";
	std::string roadTerrain = "C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\Objects\\RoadTerrain.obj";
	std::string pathTerrain = "C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\Objects\\PathTerrain.obj";


	std::wstring grassTexture = L"..\\HardwareRasterizer\\Textures\\grass.bmp";
	std::wstring roadTexture = L"..\\HardwareRasterizer\\Textures\\road.jpg";


	terrain->LoadFromFile(roadTerrain);
	terrain->SetTexture(roadTexture);

	
	mCharacterController = new CharacterController(mDeviceContext, terrain, &mPhysicsEngine);
	mCharacterController->SetMoveSpeed(7.0f);


	XMFLOAT3 eye = { 40.0f, 30.0f, 40.0f};
	XMFLOAT3 at = { 0.0f, 0.0f, 0.0f};
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	mDirectionalLight = new Light(mDeviceContext, eye, at, color);

	mSkybox.init(mD3DDevice, 2000.0f);
	mLoadObjects();

	
	mCurrentCamID = 0;
	
}

void NewDXApp::updateScene(float dt)
{

	mUpdateCamera(dt);

	DirectXApplication::updateScene(dt);
	mCharacterController->Update(dt);

	XMVECTOR playerPos = XMLoadFloat3(&mCharacterController->GetPosition());

	XMFLOAT3 lightPos;
	XMVECTOR destination = { sin(dt), cos(dt), tan(dt) };

	XMMATRIX mrot = XMMatrixRotationY(sin(dt*0.1f));

	XMStoreFloat3(&lightPos, XMVector3Transform(mDirectionalLight->GetPosition(), mrot));

	mDirectionalLight->SetPosition(lightPos);

}

void NewDXApp::drawScene()
{

	// Clear the back buffer 
	float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	mDeviceContext->ClearRenderTargetView(mD3D11RenderTargetView, clearColor);
	mDeviceContext->ClearDepthStencilView(mD3D11DepthStencilView, D3D11_CLEAR_DEPTH || D3D11_CLEAR_STENCIL, 1, 0);
	


	Shaders::ShadowMapShader::Render(mDeviceContext, terrain, (IDrawable**)&mObjectList[0], mObjectList.size());
	Shaders::SkyboxShader::Render(mDeviceContext, (Object*)&mSkybox);

	
	mD3D11SwapChain->Present(0, 0);

}

LRESULT NewDXApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
		{
			switch (wParam)
			{
				case VK_F1:
				{
					if (mCurrentCamID == 0)
						mCurrentCamID = 1;
					else mCurrentCamID = 0;
				}break;

				default: break;
			}
		}break;

		default: break;

	}// end switch


	return DirectXApplication::msgProc(msg, wParam, lParam);
}

void NewDXApp::mInitResources()
{
	

	Shaders::LoadAll(mD3DDevice);
	ConstantBuffers::InitAll(mD3DDevice);
	

}


class index_comparator
{
public:
	bool operator()(const tinyobj::index_t& lhv, const tinyobj::index_t& rhv) const
	{
		return std::tie(lhv.vertex_index, lhv.normal_index, lhv.texcoord_index) < std::tie(rhv.vertex_index, rhv.normal_index, rhv.texcoord_index);
	}
};

static std::string GetBaseDir(const std::string& filepath) {
	if (filepath.find_last_of("/\\") != std::string::npos)
		return filepath.substr(0, filepath.find_last_of("/\\"));
	return "";
}

void NewDXApp::mLoadObject(std::string filename)
{
	//load a test object
	std::string inputfile = filename;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	struct object
	{
		Vertex::PosNormTex*		  vertexList;
		int numVertices;
		std::vector<unsigned int> indexList;
		std::string textureName;
	};


	std::string base_dir = GetBaseDir(inputfile);



	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str(), base_dir.c_str());

	if (!warn.empty()) {

	}

	if (!err.empty()) {

	}

	if (!ret) {

	}

	//list of objects extracted from the file
	std::vector<object> objList;

	// Loop over shapes and extract an object list
	for (unsigned int s = 0; s < shapes.size(); s++)
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

	for (object obj : objList)
	{
		Object* Obj = new Object();
		std::string texturePath = base_dir + "/" + obj.textureName;
		std::wstring texturePathW = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(texturePath);
		Obj->Initialize(mD3DDevice, XMFLOAT3(15.0f, 3.0f, 0.0f), obj.vertexList, obj.numVertices, obj.indexList, texturePathW , 3.0f);
		mObjectList.push_back(Obj);



		delete obj.vertexList;
		

	}

}

void NewDXApp::mLoadObjects()
{

	std::string box = "C:/Users/Tim/Documents/Visual Studio 2017/Projects/HardwareRasterizer/HardwareRasterizer/Objects/boxobj.obj";
	std::string building = "C:/Users/Tim/Documents/Visual Studio 2017/Projects/HardwareRasterizer/HardwareRasterizer/Objects/building.obj";
	std::string city = "C:/Users/Tim/Documents/Visual Studio 2017/Projects/HardwareRasterizer/HardwareRasterizer/Objects/city.obj";


	//mLoadObject(box);
	//mLoadObject(building);

	mLoadObject(city);

	

}


