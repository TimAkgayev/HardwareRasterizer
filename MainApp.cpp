#include "DirectXApplication.h"
#include "Terrain.h"
#include "Box.h"
#include "CharacterController.h"
#include "Shaders.h"
#include "ConstantBuffers.h"
#include "Skybox.h"
#include "WorldObject.h"
#include "Light.h"
#include "PhysicsEngine.h"
#include "SceneManager.h"
#define TINYOBJLOADER_IMPLEMENTATION 
#include "tinyobj\tinyobjloader\tiny_obj_loader.h"
#include <string>
#include <vector>
#include <tuple>
#include <map>



class MainApp : public DirectXApplication
{
public:
	MainApp(HINSTANCE hinstance);
	~MainApp();

	virtual void initApp() override;
	virtual void updateScene(float dt) override;
	virtual void drawScene() override;

	virtual LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
	
	void mInitResources();
	void mUpdateCamera(float dt);


private:

	SceneManager* mWorldManager;

	unsigned int mCurrentCamID;

	Camera mFreeCamera;
	Light* mDirectionalLight;
	Shader mShadingEngine;
	
};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif


	MainApp theApp(hInstance);

	theApp.initApp();

	return theApp.run();
}



void MainApp::mUpdateCamera(float dt)
{
	//update constant buffers
	Camera& currentCamera = (mCurrentCamID == 0) ? mWorldManager->GetPlayer()->GetCamera() : mFreeCamera;

	if (mCurrentCamID == 0)
	{
		mFreeCamera.Lock();
		mWorldManager->GetPlayer()->GetCamera().Unlock();
	}
	else if (mCurrentCamID == 1)
	{
		mWorldManager->GetPlayer()->GetCamera().Lock();
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

MainApp::MainApp(HINSTANCE hinstance):DirectXApplication(hinstance)
{
}

MainApp::~MainApp()
{

}

void MainApp::initApp()
{
	DirectXApplication::initApp();

	mInitResources();


	mWorldManager = new SceneManager(mD3DDevice);


	std::string highResTerrain = "C:\\terrain.obj";
	std::string lowResTerrain = "C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\Objects\\LowPolyTerrain.obj";
	std::string roadTerrain = "C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\Objects\\RoadTerrain.obj";
	std::string pathTerrain = "C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\Objects\\PathTerrain.obj";
	std::string cityLevel = "C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Projects\\HardwareRasterizer\\HardwareRasterizer\\Objects\\city.obj";


	std::wstring grassTexture = L"..\\HardwareRasterizer\\Textures\\grass.bmp";
	std::wstring roadTexture = L"..\\HardwareRasterizer\\Textures\\road.jpg";


	//terrain->LoadFromFile(roadTerrain);
	//terrain->SetTexture(roadTexture);

	mWorldManager->LoadLevel(cityLevel);

	mWorldManager->GetPlayer()->SetMoveSpeed(7.0f);


	XMFLOAT3 eye = { 40.0f, 30.0f, 40.0f};
	XMFLOAT3 at = { 0.0f, 0.0f, 0.0f};
	XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	mDirectionalLight = new Light(mDeviceContext, eye, at, color);


	mCurrentCamID = 0;
	
}

void MainApp::updateScene(float dt)
{

	mUpdateCamera(dt);

	DirectXApplication::updateScene(dt);
	
	mWorldManager->Update(dt);

	
	//rotate the light
	XMFLOAT3 lightPos;
	XMVECTOR destination = { sin(dt), cos(dt), tan(dt) };

	XMMATRIX mrot = XMMatrixRotationY(sin(dt*0.5f));
	XMStoreFloat3(&lightPos, XMVector3Transform(mDirectionalLight->GetPosition(), mrot));

	mDirectionalLight->SetPosition(lightPos);

}

void MainApp::drawScene()
{

	// Clear the back buffer 
	float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	mDeviceContext->ClearRenderTargetView(mD3D11RenderTargetView, clearColor);
	mDeviceContext->ClearDepthStencilView(mD3D11DepthStencilView, D3D11_CLEAR_DEPTH || D3D11_CLEAR_STENCIL, 1, 0);
	


	mWorldManager->Render();

	
	mD3D11SwapChain->Present(0, 0);

}

LRESULT MainApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
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

void MainApp::mInitResources()
{
	

	mShadingEngine.InitializeAllShaders(mD3DDevice, mD3D11RenderTargetView, mClientWidth, mClientHeight);
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
