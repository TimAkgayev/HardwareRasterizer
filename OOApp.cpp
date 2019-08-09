#include "DirectXApplication.h"
#include "Vertex.h"
#include "Terrain.h"
#include "Box.h"
#include "InputLayouts.h"
#include "CharacterController.h"
#include "Shaders.h"
#include "ConstantBuffers.h"
#include "Skybox.h"
#include "Object.h"
#define TINYOBJLOADER_IMPLEMENTATION 
#include "tinyobj\tinyobjloader\tiny_obj_loader.h"
#include <string>
#include <vector>
#include <tuple>
#include <map>

ConstantBuffers::ProjectionVariables projectionMatrices;

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
	
	ID3D11ShaderResourceView* mSkyCubeMapSRV;
	void mInitResources();
	void mLoadObjects();
private:

	Terrain terrain;
	CharacterController* mCharacterController;
	Skybox mSkybox;
	std::vector<Object*> mObjectList;
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

	terrain.CreateFromHeightMap(mD3DDevice, L"..\\HardwareRasterizer\\Heightmaps\\TestFloorSmall.bmp", 1000, 50);
	terrain.SetTexture(L"..\\HardwareRasterizer\\Textures\\grass.bmp");
	terrain.CreateCollisionBoxes();

	
	mCharacterController = new CharacterController(&terrain);
	mCharacterController->SetMoveSpeed(140.0f);

	mSkybox.init(mD3DDevice, 1000000.0f);

	mLoadObjects();

}

void NewDXApp::updateScene(float dt)
{

	DirectXApplication::updateScene(dt);
	mCharacterController->Update(dt);

	projectionMatrices.View = XMMatrixTranspose(mCharacterController->GetCamera().GetViewMatrix());
	projectionMatrices.Projection = XMMatrixTranspose(mCharacterController->GetCamera().GetProjectionMatrix());


	mDeviceContext->UpdateSubresource(ConstantBuffers::ViewProjBuffer, 0, NULL, &projectionMatrices, 0, 0);

}

void NewDXApp::drawScene()
{

	// Clear the back buffer 
	float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	mDeviceContext->ClearRenderTargetView(mD3D11RenderTargetView, clearColor);
	mDeviceContext->ClearDepthStencilView(mD3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
	mDeviceContext->ClearDepthStencilView(mD3D11DepthStencilView, D3D11_CLEAR_STENCIL, 1, 1);


	terrain.draw();
	mSkybox.draw(mCharacterController->GetCamera());

	for (Object* obj : mObjectList)
		obj->draw();


	mD3D11SwapChain->Present(0, 0);

}

LRESULT NewDXApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{

	return DirectXApplication::msgProc(msg, wParam, lParam);
}

void NewDXApp::mInitResources()
{
	

	Shaders::InitAll(mD3DDevice);
	InputLayout::InitAll(mD3DDevice);
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


void NewDXApp::mLoadObjects()
{


	std::string inputfile = "Objects/boxobj.obj";
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
	};

	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

	if (!warn.empty()) {

	}

	if (!err.empty()) {

	}

	if (!ret) {

	}

	//list of objects extracted from the file
	std::vector<object> objList;

	// Loop over shapes
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

		for ( auto& keyval : uniqueVertexMap)
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

		objList.push_back(obj);
	}

	for (object obj : objList)
	{
		Object* Obj = new Object();
		Obj->init(mD3DDevice, XMFLOAT3(0.0f, 0.0f, 0.0f), obj.vertexList, obj.numVertices, obj.indexList, L"..\\HardwareRasterizer\\Textures\\Brick.bmp", 1000.0f);
		mObjectList.push_back(Obj);



		delete obj.vertexList;
		break;

	}


	

}
