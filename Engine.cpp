#include "Engine.h"

D3D10_INPUT_ELEMENT_DESC DX10VertexLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	
};

struct ShaderProjectionVars
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

void Engine::Initialization(Application* AppInstance)
{
	mApplicationInstance = AppInstance;
	mApplicationInstance->ApplicationInitialization();

}

int Engine::Loop()
{
	//Game update
	if (mApplicationInstance->ApplicationUpdate() == UPDATE_RESET)
	{
		
		//release window
		DestroyWindow(mMainWindowHandle);
		 

		return UPDATE_RESET;
	}

	//Render the frame

	// Update our time
	static float t = 0.0f;

	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();
	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;

	mWorldMatrix = XMMatrixRotationY(t);
	

	// Clear the back buffer 
	float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	mD3D10Device->ClearRenderTargetView(mD3D10RenderTargetView, clearColor);

	// Update shader variables
	ShaderProjectionVars shaderBuffer;
	shaderBuffer.mWorld = XMMatrixTranspose(mWorldMatrix);
	shaderBuffer.mView = XMMatrixTranspose(mViewMatrix);
	shaderBuffer.mProjection = XMMatrixTranspose(mProjectionMatrix);
	mD3D10Device->UpdateSubresource(mD3D10ConstantBuffer, 0, NULL, &shaderBuffer, 0, 0);

	// Render the mesh
	mD3D10Device->VSSetShader(mD3D10VertexShader);
	mD3D10Device->VSSetConstantBuffers(0, 1, &mD3D10ConstantBuffer);
	mD3D10Device->PSSetShader(mD3D10PixelShader);
	mD3D10Device->DrawIndexed(36, 0, 0);

	// Present the information rendered to the back buffer to the front buffer (the screen)
	mD3D10SwapChain->Present(0, 0);


	return UPDATE_NORMAL;
}

void Engine::Shutdown()
{

}

void Engine::CreateEngineWindow(const wchar_t* WindowClassName, HINSTANCE hInstance)
{
	HWND hwnd;
	POINT windowFrameDim = { 16, 39 };



	if (!(hwnd = CreateWindowEx(NULL,
		WindowClassName,     // class
		mApplicationInstance->GetWindowTitle(), // title
		WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,		//style
		0, 0,	  // initial x,y
		mApplicationInstance->GetClientWidth() + windowFrameDim.x, mApplicationInstance->GetClientHeight() + windowFrameDim.y,  // initial width, height
		NULL,		// handle to parent 
		NULL,		 // handle to menu
		hInstance,	// instance of this application
		NULL)))	// extra creation parms 
	{
	}
	
	// save main window handle
	mMainWindowHandle = hwnd;


	//setup swap chain ===================================================
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	//set buffer dimensions and format
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = mApplicationInstance->GetClientWidth();
	swapChainDesc.BufferDesc.Height = mApplicationInstance->GetClientHeight();
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


	//set refresh rate
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	//sampling settings
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;

	//output window handle
	swapChainDesc.OutputWindow = mMainWindowHandle;
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

	//set the render target
	mD3D10Device->OMSetRenderTargets(1, &mD3D10RenderTargetView, NULL);

	//set view port aka region of render target ===================================================
	mD3D10Viewport.Width = swapChainDesc.BufferDesc.Width;
	mD3D10Viewport.Height = swapChainDesc.BufferDesc.Height;
	mD3D10Viewport.MinDepth = 0.0f;
	mD3D10Viewport.MaxDepth = 1.0f;
	mD3D10Viewport.TopLeftX = 0;
	mD3D10Viewport.TopLeftY = 0;

	mD3D10Device->RSSetViewports(1, &mD3D10Viewport);




	//load the effects file and set up projections to be used ============================================

	//compile the vertex shader
	ID3DBlob* pVSBlob = NULL;
	HRESULT hr = CompileShaderFromFile(TEXT("DX10RendererEffect.fx"), "VS_MAIN", "vs_4_0", &pVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the vertex shader
	hr = mD3D10Device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &mD3D10VertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return;
	}

	// compile the pixel shader
	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(L"DX10RendererEffect.fx", "PS_MAIN", "ps_4_0", &pPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the pixel shader
	hr = mD3D10Device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), &mD3D10PixelShader);
	pPSBlob->Release();
	if (FAILED(hr))
		return;

	

	// Setup the projection matrix.
	float fieldOfView = (float)D3DX_PI / 4.0f;
	float screenAspect = (float)mApplicationInstance->GetClientWidth() / (float)mApplicationInstance->GetClientHeight();

	// Create the projection matrix 
	D3DXMATRIX projectionMatrix;
	D3DXMatrixPerspectiveFovLH(&projectionMatrix, fieldOfView, screenAspect, 0.1f, 100.0f);

	// Create the world matrix
	D3DXMATRIX worldMatrix;
	D3DXMatrixIdentity(&worldMatrix);

	// Creat the view matrix
	D3DXMATRIX viewMatrix;
	D3DXVECTOR3 eye(2, 3, 3);
	D3DXVECTOR3 at(0, 0, 0);
	D3DXVECTOR3 up(0, 1, 0);
	D3DXMatrixLookAtLH(&viewMatrix, &eye, &at, &up);



	//create input layout ============================================

	if (FAILED(mD3D10Device->CreateInputLayout(DX10VertexLayout, NUM_VERTEX_ELEMENTS, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &mD3D10InputLayout))) return;
	mD3D10Device->IASetInputLayout(mD3D10InputLayout);


	//set up rasterizer flags ============================================
	D3D10_RASTERIZER_DESC rasterizerState;
	rasterizerState.CullMode = D3D10_CULL_BACK;
	rasterizerState.FillMode = D3D10_FILL_SOLID;
	rasterizerState.FrontCounterClockwise = false;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = true;

	ID3D10RasterizerState* pRS;
	mD3D10Device->CreateRasterizerState(&rasterizerState, &pRS);
	mD3D10Device->RSSetState(pRS);


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


	//game functions
	mCreateSampleVertexIndexBuffer();



}

void Engine::mCreateSampleVertexIndexBuffer()
{

	Vertex mesh[] = 
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) },
	};


	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex)* 8; //total size of buffer in bytes
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = mesh;
	if (FAILED(mD3D10Device->CreateBuffer(&bufferDesc, &InitData, &mD3D10VertexBuffer))) return;


	// Set vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	mD3D10Device->IASetVertexBuffers(0, 1, &mD3D10VertexBuffer, &stride, &offset);


	//create index buffer
	WORD indices[] =
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

	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(WORD) * 36;
	bufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	InitData.pSysMem = indices;
	mD3D10Device->CreateBuffer(&bufferDesc, &InitData, &mD3D10IndexBuffer);

	mD3D10Device->IASetIndexBuffer(mD3D10IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	mD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//create a constant buffer for shader constant manipluation
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(ShaderProjectionVars);
	bufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	mD3D10Device->CreateBuffer(&bufferDesc, NULL, &mD3D10ConstantBuffer);

	//initialize matrices
	mWorldMatrix = XMMatrixIdentity();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mViewMatrix = XMMatrixLookAtLH(Eye, At, Up);

	// Initialize the projection matrix
	mProjectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, mD3D10Viewport.Width / (FLOAT)mD3D10Viewport.Height, 0.01f, 100.0f);


}

HRESULT Engine::CompileShaderFromFile(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut)
{
	
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX10CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);

	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;

}

