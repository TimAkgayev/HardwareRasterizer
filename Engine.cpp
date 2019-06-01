#include "Engine.h"

D3D10_INPUT_ELEMENT_DESC MeshVertexLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	
};

D3D10_INPUT_ELEMENT_DESC LineVertexLayout[] =
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


int Engine::Loop()
{

	// Clear the back buffer 
	float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	mD3D10Device->ClearRenderTargetView(mD3D10RenderTargetView, clearColor);
	mD3D10Device->ClearDepthStencilView(mD3D10DepthStencilView, D3D10_CLEAR_DEPTH, 1, 1);


	//Game update

	PositionLink::_PositionLink_UpdateAllLinks();
	AutoUpdate::_AutoUpdate_UpdateAll();

	if (mApplicationInstance->ApplicationUpdate() == UPDATE_RESET)
	{
		
		//release window
		DestroyWindow(mMainWindowHandle);
		 

		return UPDATE_RESET;
	}



	//load any meshes into memory
	for (WorldObject* obj : WorldObject::ObjectList)
	{
		if (obj->ObjectType == MESH_OBJECT)
		{

			//make sure that this mesh is not already loaded
			bool meshExists = false;
			for (MeshDescriptor& desc : mLoadedMeshes)
				if (desc.MeshObjectPtr == obj)
				{
					meshExists = true;
					break;
				}

			if (meshExists)
				continue;

			MeshDescriptor meshDesc;
			meshDesc.MeshObjectPtr = obj;

			Mesh* mesh = (Mesh*)obj;

			//make sure the mesh is not empty
			if (mesh->GetVertexList().size() == 0)
				break;


			//create a vertex buffer
			D3D10_BUFFER_DESC bufferDesc;
			bufferDesc.Usage = D3D10_USAGE_DEFAULT;
			bufferDesc.ByteWidth = sizeof(D3DMeshVertex) * mesh->GetVertexList().size(); //total size of buffer in bytes
			bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;

			D3D10_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = &(mesh->GetVertexList()[0]);
			if (FAILED(mD3D10Device->CreateBuffer(&bufferDesc, &InitData, &meshDesc.VertexBuffer))) return UPDATE_NORMAL;

			//create an index buffer if it's not a line list


			bufferDesc.Usage = D3D10_USAGE_DEFAULT;
			bufferDesc.ByteWidth = sizeof(WORD) * mesh->GetIndexList().size();
			bufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;

			InitData.pSysMem = &(mesh->GetIndexList()[0]);
			mD3D10Device->CreateBuffer(&bufferDesc, &InitData, &meshDesc.IndexBuffer);


			//Create texture views
			D3DX10CreateShaderResourceViewFromFile(mD3D10Device, mesh->GetTexturePath().c_str(), NULL, NULL, &meshDesc.mTextureResourceView, NULL);


			mLoadedMeshes.push_back(meshDesc);
		}

		else if (obj->ObjectType == LINE_OBJECT)
		{

			//make sure that this mesh is not already loaded
			bool objExists = false;
			for (LineDescriptor& desc : mLoadedLines)
			{
				if (desc.LineObjectPtr == obj)
				{
					//update
					desc.VertexBuffer->Release();

					Line* line = (Line*)obj;

					//create a vertex buffer
					D3D10_BUFFER_DESC bufferDesc;
					bufferDesc.Usage = D3D10_USAGE_DEFAULT;
					bufferDesc.ByteWidth = sizeof(D3DLineVertex) * line->GetVertexList().size(); //total size of buffer in bytes
					bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
					bufferDesc.CPUAccessFlags = 0;
					bufferDesc.MiscFlags = 0;

					D3D10_SUBRESOURCE_DATA InitData;
					ZeroMemory(&InitData, sizeof(InitData));
					InitData.pSysMem = &(line->GetVertexList()[0]);
					if (FAILED(mD3D10Device->CreateBuffer(&bufferDesc, &InitData, &desc.VertexBuffer))) return UPDATE_NORMAL;

					objExists = true;
					break;
				}
			}

			if (objExists)
				continue;


			LineDescriptor lineDesc;
			lineDesc.LineObjectPtr = obj;

			Line* line = (Line*)obj;

			//create a vertex buffer
			D3D10_BUFFER_DESC bufferDesc;
			bufferDesc.Usage = D3D10_USAGE_DEFAULT;
			bufferDesc.ByteWidth = sizeof(D3DLineVertex) * line->GetVertexList().size(); //total size of buffer in bytes
			bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;

			D3D10_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = &(line->GetVertexList()[0]);
			if (FAILED(mD3D10Device->CreateBuffer(&bufferDesc, &InitData, &lineDesc.VertexBuffer))) return UPDATE_NORMAL;

			mLoadedLines.push_back(lineDesc);
		}
	}

	// Update shader variables
	ShaderProjectionVars shaderBuffer;
	shaderBuffer.mProjection = XMMatrixTranspose(mProjectionMatrix);

	// Update our time
	static float t = 0.0f;

	static DWORD dwTimeStart = 0;
	DWORD dwTimeCur = GetTickCount();
	if (dwTimeStart == 0)
		dwTimeStart = dwTimeCur;
	t = (dwTimeCur - dwTimeStart) / 1000.0f;

	//interpolate the view matrices
	XMMATRIX diffMatrix = mNewViewMatrix - mViewMatrix;
	XMMATRIX partialMatrix = diffMatrix / 30.0f;


	//mViewMatrix += partialMatrix;
	mViewMatrix = mNewViewMatrix;

		
	
	shaderBuffer.mView = XMMatrixTranspose(mViewMatrix);
	
	mD3D10Device->PSSetSamplers(0, 1, &mD3D10SamplerState);
	

	// Render the mesh list

	mD3D10Device->IASetInputLayout(mD3D10MeshInputLayout);
	mD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mD3D10Device->VSSetShader(mMeshVertexShader);
	mD3D10Device->PSSetShader(mMeshPixelShader);

	for (UINT objIndex : mMeshDrawList)
	{
		//set the objects world position
		shaderBuffer.mWorld = XMMatrixTranspose(mLoadedMeshes[objIndex].WorldMatrix);
		mD3D10Device->UpdateSubresource(mD3D10ConstantBuffer, 0, NULL, &shaderBuffer, 0, 0);

		//draw mesh objects
		mD3D10Device->VSSetConstantBuffers(0, 1, &mD3D10ConstantBuffer);
		mD3D10Device->PSSetShaderResources(0, 1, &mLoadedMeshes[objIndex].mTextureResourceView);



		// Set vertex and index buffer
		UINT stride = sizeof(D3DMeshVertex);
		UINT offset = 0;
		mD3D10Device->IASetVertexBuffers(0, 1, &mLoadedMeshes[objIndex].VertexBuffer, &stride, &offset);
		mD3D10Device->IASetIndexBuffer(mLoadedMeshes[objIndex].IndexBuffer, DXGI_FORMAT_R16_UINT, 0);


		//draw
		D3D10_BUFFER_DESC tempDesc;
		mLoadedMeshes[objIndex].IndexBuffer->GetDesc(&tempDesc);
		UINT numIndices = tempDesc.ByteWidth / sizeof(WORD);

		mD3D10Device->DrawIndexed(numIndices, 0, 0);
		
	}


	mD3D10Device->IASetInputLayout(mD3D10LineInputLayout);
	mD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	mD3D10Device->VSSetShader(mLineVertexShader);
	mD3D10Device->PSSetShader(mLinePixelShader);
	for (UINT objIndex : mLineDrawList)
	{
		//set the objects world position
		shaderBuffer.mWorld = XMMatrixTranspose(mLoadedLines[objIndex].WorldMatrix);
		mD3D10Device->UpdateSubresource(mD3D10ConstantBuffer, 0, NULL, &shaderBuffer, 0, 0);

		//draw mesh objects
	

		mD3D10Device->VSSetConstantBuffers(0, 1, &mD3D10ConstantBuffer);


		// Set vertex and index buffer
		UINT stride = sizeof(D3DLineVertex);
		UINT offset = 0;
		mD3D10Device->IASetVertexBuffers(0, 1, &mLoadedLines[objIndex].VertexBuffer, &stride, &offset);
		


		//draw
		mD3D10Device->Draw(2, 0);
	}

	// Present the information rendered to the back buffer to the front buffer (the screen)
	mD3D10SwapChain->Present(0, 0);

	//empty the lists
	mMeshDrawList.clear();
	mLineDrawList.clear();

	return UPDATE_NORMAL;
}

void Engine::Shutdown()
{

}

void Engine::CreateEngineWindow(const wchar_t* WindowClassName, HINSTANCE hInstance, Application* appInstance)
{
	HWND hwnd;
	POINT windowFrameDim = { 16, 39 };

	mApplicationInstance = appInstance;

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


	//create depth stencil texture
	D3D10_TEXTURE2D_DESC descDepth;

	descDepth.Width = mApplicationInstance->GetClientWidth();
	descDepth.Height = mApplicationInstance->GetClientHeight();
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




	//load the effects file and set up projections to be used ============================================

	//compile the mesh vertex shader
	ID3DBlob* pMeshVSBlob = NULL;
	HRESULT hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_MAIN_MESH", "vs_4_0", &pMeshVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the mesh vertex shader
	hr = mD3D10Device->CreateVertexShader(pMeshVSBlob->GetBufferPointer(), pMeshVSBlob->GetBufferSize(), &mMeshVertexShader);
	if (FAILED(hr))
	{
		pMeshVSBlob->Release();
		return;
	}

	// compile the mesh pixel shader
	ID3DBlob* pMeshPSBlob = NULL;
	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_MAIN_MESH", "ps_4_0", &pMeshPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the mesh pixel shader
	hr = mD3D10Device->CreatePixelShader(pMeshPSBlob->GetBufferPointer(), pMeshPSBlob->GetBufferSize(), &mMeshPixelShader);
	pMeshPSBlob->Release();
	if (FAILED(hr))
		return;



	//compile the line vertex shader
	ID3DBlob* pLineVSBlob = NULL;
	hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_MAIN_LINE", "vs_4_0", &pLineVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the line vertex shader
	hr = mD3D10Device->CreateVertexShader(pLineVSBlob->GetBufferPointer(), pLineVSBlob->GetBufferSize(), &mLineVertexShader);
	if (FAILED(hr))
	{
		pLineVSBlob->Release();
		return;
	}

	// compile the line pixel shader
	ID3D10Blob* pLinePSBlob = NULL;
	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_MAIN_LINE", "ps_4_0", &pLinePSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the line pixel shader
	hr = mD3D10Device->CreatePixelShader(pLinePSBlob->GetBufferPointer(), pLinePSBlob->GetBufferSize(), &mLinePixelShader);
	pLinePSBlob->Release();
	if (FAILED(hr))
		return;

	//create a constant buffer for shader constant manipluation
	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.MiscFlags = 0;
	bufferDesc.Usage = D3D10_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(ShaderProjectionVars);
	bufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	mD3D10Device->CreateBuffer(&bufferDesc, NULL, &mD3D10ConstantBuffer);
	

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mViewMatrix = XMMatrixLookAtLH(Eye, At, Up);
	mNewViewMatrix = mViewMatrix;

	// Initialize the projection matrix
	mProjectionMatrix = XMMatrixPerspectiveFovLH(D3DX_PI/4, mD3D10Viewport.Width / (FLOAT)mD3D10Viewport.Height, 0.01f, 100000.0f);


	// Create input layouts ============================================
	if (FAILED(mD3D10Device->CreateInputLayout(MeshVertexLayout, NUM_MESH_VERTEX_ELEMENTS, pMeshVSBlob->GetBufferPointer(), pMeshVSBlob->GetBufferSize(), &mD3D10MeshInputLayout))) return;
	if (FAILED(mD3D10Device->CreateInputLayout(LineVertexLayout, NUM_LINE_VERTEX_ELEMENTS, pLineVSBlob->GetBufferPointer(), pLineVSBlob->GetBufferSize(), &mD3D10LineInputLayout))) return;

	mD3D10Device->IASetInputLayout(mD3D10MeshInputLayout);

	// Create the sample state
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


	//compile the skybox vertex shader
	ID3DBlob* pSkyBoxVSBlob = NULL;
	hr = CompileShaderFromFile(TEXT("SkyCubeMap.fx"), "VS_MAIN", "vs_4_0", &pSkyBoxVSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the skybox vertex shader
	hr = mD3D10Device->CreateVertexShader(pSkyBoxVSBlob->GetBufferPointer(), pSkyBoxVSBlob->GetBufferSize(), &mSkyBoxVertexShader);
	pSkyBoxVSBlob->Release();
	if (FAILED(hr))
		return;


	// compile the skybox pixel shader
	ID3DBlob* pSkyBoxPSBlob = NULL;
	hr = CompileShaderFromFile(L"SkyCubeMap.fx", "PS_MAIN", "ps_4_0", &pSkyBoxPSBlob);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the skybox pixel shader
	hr = mD3D10Device->CreatePixelShader(pSkyBoxPSBlob->GetBufferPointer(), pSkyBoxPSBlob->GetBufferSize(), &mSkyBoxPixelShader);
	pSkyBoxPSBlob->Release();
	if (FAILED(hr))
		return;

	//create a skybox ============================================================================
	D3DX10CreateShaderResourceViewFromFile(mD3D10Device, L"C:\\Users\\Tim\\Documents\\Visual Studio 2017\\Pojects\\HardwareRasterizer\\HardwareRasterizer\\Textures\\SkyCubeMap.dds", 0, 0, &mSkyCubeMapSRV, 0);

	
	
	

	//initialize application
	mApplicationInstance->ApplicationInitialization(mMainWindowHandle, hInstance);
	mApplicationInstance->SetRasterizer((RasterizerInterface*)this);


}

void Engine::SetViewMatrix(const XMMATRIX & view)
{
	mNewViewMatrix = view;
}


void Engine::DrawWorldObject(WorldObject * obj)
{
	if (obj->ObjectType == MESH_OBJECT)
	{
		UINT count = 0;
		for (MeshDescriptor desc : mLoadedMeshes)
		{
			if (desc.MeshObjectPtr == obj) //find the object that was loaded
			{
				mLoadedMeshes[count].WorldMatrix = obj->GetTransform();
				mMeshDrawList.push_back(count);
				break;
			}

			count++;
		}
	}
	else if (obj->ObjectType == LINE_OBJECT)
	{
		UINT count = 0;
		for (LineDescriptor desc : mLoadedLines)
		{
			if (desc.LineObjectPtr == obj) //find the object that was loaded
			{
				mLoadedLines[count].WorldMatrix = obj->GetTransform();
				mLineDrawList.push_back(count);
				break;
			}

			count++;
		}

	}
}



void Engine::DrawWorldObject(WorldObject * obj, XMMATRIX& worldMatrix)
{
	if (obj->ObjectType == MESH_OBJECT)
	{
		UINT count = 0;
		for (MeshDescriptor desc : mLoadedMeshes)
		{
			if (desc.MeshObjectPtr == obj) //find the object that was loaded
			{
				mLoadedMeshes[count].WorldMatrix = worldMatrix;
				mMeshDrawList.push_back(count);
				break;
			}	

			count++;
		}
	}
	else if (obj->ObjectType == LINE_OBJECT)
	{
		UINT count = 0;
		for (LineDescriptor desc : mLoadedLines)
		{
			if (desc.LineObjectPtr == obj) //find the object that was loaded
			{
				mLoadedLines[count].WorldMatrix = worldMatrix;
				mLineDrawList.push_back(count);
				break;
			}

			count++;
		}

	}
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

