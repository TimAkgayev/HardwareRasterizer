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
	
	ID3D11ShaderResourceView* mSkyCubeMapSRV;



	void mInitResources();

private:

	Terrain terrain;
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

	terrain.CreateFromHeightMap(mD3DDevice, L"..\\HardwareRasterizer\\Heightmaps\\TestFloorSmall.bmp", 1000, 50);
	terrain.SetTexture(L"..\\HardwareRasterizer\\Textures\\grass.bmp");
	terrain.CreateCollisionBoxes();

	
	mCharacterController = new CharacterController(&terrain);
	mCharacterController->SetMoveSpeed(140.0f);

	mSkybox.init(mD3DDevice, 1000000.0f);


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
