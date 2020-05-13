#include "DirectXApplication.h"



LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static DirectXApplication* app = 0;

	switch (msg)
	{
	case WM_CREATE:
	{
		// Get the 'this' pointer we passed to CreateWindow via the lpParam parameter.
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		app = (DirectXApplication*)cs->lpCreateParams;
		return 0;
	}
	}

	// Don't start processing messages until after WM_CREATE.
	if (app)
		return app->msgProc(msg, wParam, lParam);
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);
}




DirectXApplication::DirectXApplication(HINSTANCE hInstance)
{
	mhAppInst = hInstance;
	mhMainWnd = 0;
	mAppPaused = false;
	mClientWidth = 800;
	mClientHeight = 600;
	mMainWndCaption = L"Example Application";
}

DirectXApplication::~DirectXApplication()
{

}


int DirectXApplication::run()
{
	MSG msg = { 0 };
	mTimer.reset();

	while (msg.message != WM_QUIT)
	{
		
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		else
		{
			mTimer.tick();

			if (mTimer.getDeltaTime() < 0.016666f)
				Sleep(1);

			if (!mAppPaused)
				updateScene(mTimer.getDeltaTime());
			else
				Sleep(50);

			drawScene();
		}
	}
	return (int)msg.wParam;

}



void DirectXApplication::initApp()
{

	//initialize COM interface to use the WIC imagine functions

#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	if (FAILED(initialize))
		return;// error
#else
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		// error
#endif
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);


	initMainWindow();
	initDirectX();
}

void DirectXApplication::updateScene(float dt)
{
	
}

void DirectXApplication::drawScene()
{

}


void DirectXApplication::initMainWindow()
{

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"DXAppWndClass";
	

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"Failed to register the window class!", 0, 0);
		PostQuitMessage(0);
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"DXAppWndClass", 
		                     mMainWndCaption.c_str(),
						     WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
						     width, height, 
							 0, 0, 
						     mhAppInst, 
							 this);
	if (!mhMainWnd)
	{
		MessageBox(0, L"Failed to create the main window!", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);


}



void DirectXApplication::initDirectX()
{



	//setup swap chain ===================================================
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	//set buffer dimensions and format
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = mClientWidth;
	swapChainDesc.BufferDesc.Height = mClientHeight;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


	//set refresh rate
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	//sampling settings
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;

	//output window handle
	swapChainDesc.OutputWindow = mhMainWnd;
	swapChainDesc.Windowed = true;


	//create device ===================================================
	UINT createDeviceFlags = 0;
#ifdef _DEBUG 
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL d3dFL;

	HRESULT hr1;
	if (FAILED(hr1 = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &swapChainDesc, &mD3D11SwapChain, &mD3DDevice, &d3dFL, &mDeviceContext)))
	{
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		MessageBox(NULL, TEXT("Failed to create device and swap chain"), TEXT("Error"), 0);
		return;
	}



	// create render target for merger state ===================================================
	ID3D11Texture2D* pBackBuffer;
	if (FAILED(mD3D11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer))) return;

	//try to create render target view
	if (FAILED(mD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &mD3D11RenderTargetView))) return;

	//release the back buffer
	pBackBuffer->Release();


	//create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;

	descDepth.Width = mClientWidth;
	descDepth.Height = mClientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	if (FAILED(mD3DDevice->CreateTexture2D(&descDepth, NULL, &mD3D11DepthStencilTexture))) return;
	if (FAILED(mD3DDevice->CreateDepthStencilView(mD3D11DepthStencilTexture, 0, &mD3D11DepthStencilView))) return;

	//set render targets
	mDeviceContext->OMSetRenderTargets(1, &mD3D11RenderTargetView, mD3D11DepthStencilView);


	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	mD3DDevice->CreateDepthStencilState(&dsDesc, &mDepthStencilState);
	mDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);

	//set view port aka region of render target ===================================================
	mD3D11Viewport.Width = (float)swapChainDesc.BufferDesc.Width;
	mD3D11Viewport.Height = (float)swapChainDesc.BufferDesc.Height;
	mD3D11Viewport.MinDepth = 0.0f;
	mD3D11Viewport.MaxDepth = 1.0f;
	mD3D11Viewport.TopLeftX = 0;
	mD3D11Viewport.TopLeftY = 0;

	mDeviceContext->RSSetViewports(1, &mD3D11Viewport);



	// Create the sampler state =============================================================
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	mD3DDevice->CreateSamplerState(&sampDesc, &mD3D11SamplerState);
	mDeviceContext->PSSetSamplers(0, 1, &mD3D11SamplerState);

	//set up rasterizer flags ============================================
	D3D11_RASTERIZER_DESC rasterizerStateSolid;
	rasterizerStateSolid.CullMode = D3D11_CULL_BACK;
	rasterizerStateSolid.FillMode = D3D11_FILL_SOLID;
	rasterizerStateSolid.FrontCounterClockwise = false;
	rasterizerStateSolid.DepthBias = false;
	rasterizerStateSolid.DepthBiasClamp = 0;
	rasterizerStateSolid.SlopeScaledDepthBias = 0;
	rasterizerStateSolid.DepthClipEnable = true;
	rasterizerStateSolid.ScissorEnable = false;
	rasterizerStateSolid.MultisampleEnable = false;
	rasterizerStateSolid.AntialiasedLineEnable = true;
	mD3DDevice->CreateRasterizerState(&rasterizerStateSolid, &mRasterizerStateSolid);

	
	//set up alpha blending for transparent textures ============================================

	
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(D3D11_BLEND_DESC));

	

	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	mD3DDevice->CreateBlendState(&BlendDesc, &mD3D11BlendState);
	mDeviceContext->OMSetBlendState(mD3D11BlendState, 0, 0xffffffff);

	
}


LRESULT DirectXApplication::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.start();
		}
		return 0;
	}


		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	}// end switch

	return DefWindowProc(mhMainWnd, msg, wParam, lParam);
}