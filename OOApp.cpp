#include "DirectXApplication.h"
#include "Vertex.h"
#include "Terrain.h"
#include "Box.h"
#include "InputLayouts.h"
#include "CharacterController.h"
#include "Shaders.h"
#include "ConstantBuffers.h"
#include "Skybox.h"

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
	
	ID3D10ShaderResourceView* mSkyCubeMapSRV;



	void mInitResources();

private:

	Terrain terrain;
	Box box;
	CharacterController* mCharacterController;
	Skybox mSkybox;

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
}

void NewDXApp::initApp()
{
	DirectXApplication::initApp();

	mInitResources();

	terrain.CreateFromHeightMap(mD3D10Device, L"..\\HardwareRasterizer\\Heightmaps\\TestFloor.bmp", 100, 20);
	terrain.SetTexture(L"..\\HardwareRasterizer\\Textures\\Test.bmp");
	terrain.CreateCollisionBoxes();

	box.init(mD3D10Device, 20, 30, L"..\\HardwareRasterizer\\Textures\\Test.bmp");
	
	mCharacterController = new CharacterController(&terrain);
	mCharacterController->SetMoveSpeed(140.0f);

	mSkybox.init(mD3D10Device, 100000.0f);

}

void NewDXApp::updateScene(float dt)
{
	DirectXApplication::updateScene(dt);



	mCharacterController->Update(dt);

	projectionMatrices.View = XMMatrixTranspose(mCharacterController->GetCamera().GetViewMatrix());
	projectionMatrices.Projection = XMMatrixTranspose(mCharacterController->GetCamera().GetProjectionMatrix());
	projectionMatrices.World = XMMatrixTranspose(XMMatrixIdentity());

	mD3D10Device->UpdateSubresource(ConstantBuffers::ViewWorldProjBuffer, 0, NULL, &projectionMatrices, 0, 0);

}

void NewDXApp::drawScene()
{

	// Clear the back buffer 
	float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	mD3D10Device->ClearRenderTargetView(mD3D10RenderTargetView, clearColor);
	mD3D10Device->ClearDepthStencilView(mD3D10DepthStencilView, D3D10_CLEAR_DEPTH, 1, 1);


	terrain.draw();
	box.draw();
	mSkybox.draw(mCharacterController->GetCamera());


	mD3D10SwapChain->Present(0, 0);

}

LRESULT NewDXApp::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{

	return DirectXApplication::msgProc(msg, wParam, lParam);
}

void NewDXApp::mInitResources()
{
	

	Shaders::InitAll(mD3D10Device);
	InputLayout::InitAll(mD3D10Device);
	ConstantBuffers::InitAll(mD3D10Device);
	


}
