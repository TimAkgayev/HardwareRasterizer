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
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr1;
	if (FAILED(hr1 = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &swapChainDesc, &mD3D10SwapChain, &mD3D10Device)))
	{
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		MessageBox(NULL, TEXT("Failed to create device and swap chain"), TEXT("Error"), 0);
		return;
	}



	// create render target for merger state ===================================================
	ID3D10Texture2D* pBackBuffer;
	if (FAILED(mD3D10SwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer))) return;

	//try to create render target view
	if (FAILED(mD3D10Device->CreateRenderTargetView(pBackBuffer, NULL, &mD3D10RenderTargetView))) return;

	//release the back buffer
	pBackBuffer->Release();


	//create depth stencil texture
	D3D10_TEXTURE2D_DESC descDepth;

	descDepth.Width = mClientWidth;
	descDepth.Height = mClientHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	if (FAILED(mD3D10Device->CreateTexture2D(&descDepth, NULL, &mD3D10DepthStencilTexture))) return;

	// Create the depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	if (FAILED(mD3D10Device->CreateDepthStencilView(mD3D10DepthStencilTexture, &descDSV, &mD3D10DepthStencilView))) return;

	//set render targets
	mD3D10Device->OMSetRenderTargets(1, &mD3D10RenderTargetView, mD3D10DepthStencilView);


	//set view port aka region of render target ===================================================
	mD3D10Viewport.Width = swapChainDesc.BufferDesc.Width;
	mD3D10Viewport.Height = swapChainDesc.BufferDesc.Height;
	mD3D10Viewport.MinDepth = 0.0f;
	mD3D10Viewport.MaxDepth = 1.0f;
	mD3D10Viewport.TopLeftX = 0;
	mD3D10Viewport.TopLeftY = 0;

	mD3D10Device->RSSetViewports(1, &mD3D10Viewport);



	// Create the sampler state =============================================================
	D3D10_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D10_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D10_FLOAT32_MAX;
	mD3D10Device->CreateSamplerState(&sampDesc, &mD3D10SamplerState);
	mD3D10Device->PSSetSamplers(0, 1, &mD3D10SamplerState);

	//set up rasterizer flags ============================================
	D3D10_RASTERIZER_DESC rasterizerStateSolid;
	rasterizerStateSolid.CullMode = D3D10_CULL_BACK;
	rasterizerStateSolid.FillMode = D3D10_FILL_SOLID;
	rasterizerStateSolid.FrontCounterClockwise = false;
	rasterizerStateSolid.DepthBias = false;
	rasterizerStateSolid.DepthBiasClamp = 0;
	rasterizerStateSolid.SlopeScaledDepthBias = 0;
	rasterizerStateSolid.DepthClipEnable = true;
	rasterizerStateSolid.ScissorEnable = false;
	rasterizerStateSolid.MultisampleEnable = false;
	rasterizerStateSolid.AntialiasedLineEnable = true;
	mD3D10Device->CreateRasterizerState(&rasterizerStateSolid, &mRasterizerStateSolid);

	
	//set up alpha blending for transparent textures ============================================

	D3D10_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D10_BLEND_DESC));

	BlendState.BlendEnable[0] = TRUE;
	BlendState.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	BlendState.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	BlendState.BlendOp = D3D10_BLEND_OP_ADD;
	BlendState.SrcBlendAlpha = D3D10_BLEND_ZERO;
	BlendState.DestBlendAlpha = D3D10_BLEND_ZERO;
	BlendState.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	BlendState.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

	mD3D10Device->CreateBlendState(&BlendState, &mD3D10BlendState);
	mD3D10Device->OMSetBlendState(mD3D10BlendState, 0, 0xffffffff);

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