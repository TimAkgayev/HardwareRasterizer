#include "Shaders.h"

//shadow variables
ID3D11Texture2D* mShadowMap;
ID3D11DepthStencilView* mShadowMapView;
ID3D11RasterizerState* mBackCullRasterizerState;
ID3D11RasterizerState* mFrontCullRasterizerState;
D3D11_VIEWPORT mShadowViewport;
ID3D11ShaderResourceView* mShadowMapSRV;
ID3D11SamplerState* mShadowSamplerState;

void CreateShadowMap(ID3D11Device* device)
{
	ID3D11DeviceContext* context;
	device->GetImmediateContext(&context);

	D3D11_VIEWPORT currentVP;
	UINT numVP = 1;
	context->RSGetViewports(&numVP, &currentVP);

	D3D11_TEXTURE2D_DESC shadowMapDesc;
	ZeroMemory(&shadowMapDesc, sizeof(D3D11_TEXTURE2D_DESC));
	shadowMapDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	shadowMapDesc.MipLevels = 1;
	shadowMapDesc.ArraySize = 1;
	shadowMapDesc.SampleDesc.Count = 1;
	shadowMapDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	shadowMapDesc.Height = static_cast<UINT>(currentVP.Height);
	shadowMapDesc.Width = static_cast<UINT>(currentVP.Width);

	HRESULT hr = device->CreateTexture2D(&shadowMapDesc, nullptr, &mShadowMap);


	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	hr = device->CreateDepthStencilView(mShadowMap, &depthStencilViewDesc, &mShadowMapView);

	hr = device->CreateShaderResourceView(mShadowMap, &shaderResourceViewDesc, &mShadowMapSRV);

	D3D11_SAMPLER_DESC comparisonSamplerDesc;
	ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSamplerDesc.BorderColor[0] = 1.0f;
	comparisonSamplerDesc.BorderColor[1] = 1.0f;
	comparisonSamplerDesc.BorderColor[2] = 1.0f;
	comparisonSamplerDesc.BorderColor[3] = 1.0f;
	comparisonSamplerDesc.MinLOD = 0.f;
	comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	comparisonSamplerDesc.MipLODBias = 0.f;
	comparisonSamplerDesc.MaxAnisotropy = 0;
	comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

	// Point filtered shadows can be faster, and may be a good choice when
	// rendering on hardware with lower feature levels. This sample has a
	// UI option to enable/disable filtering so you can see the difference
	// in quality and speed.


	device->CreateSamplerState(&comparisonSamplerDesc, &mShadowSamplerState);


	D3D11_RASTERIZER_DESC drawingRenderStateDesc;
	ZeroMemory(&drawingRenderStateDesc, sizeof(D3D11_RASTERIZER_DESC));
	drawingRenderStateDesc.CullMode = D3D11_CULL_BACK;
	drawingRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	drawingRenderStateDesc.DepthClipEnable = true; // Feature level 9_1 requires DepthClipEnable == true

	device->CreateRasterizerState(&drawingRenderStateDesc, &mBackCullRasterizerState);


	D3D11_RASTERIZER_DESC shadowRenderStateDesc;
	ZeroMemory(&shadowRenderStateDesc, sizeof(D3D11_RASTERIZER_DESC));
	shadowRenderStateDesc.CullMode = D3D11_CULL_FRONT;
	shadowRenderStateDesc.FillMode = D3D11_FILL_SOLID;
	shadowRenderStateDesc.DepthClipEnable = true;

	device->CreateRasterizerState(&shadowRenderStateDesc, &mFrontCullRasterizerState);


	// Init viewport for shadow rendering
	ZeroMemory(&mShadowViewport, sizeof(D3D11_VIEWPORT));
	mShadowViewport.Height = (float)currentVP.Height;
	mShadowViewport.Width = (float)currentVP.Width;
	mShadowViewport.MinDepth = 0.f;
	mShadowViewport.MaxDepth = 1.f;
}


void Shaders::ShadowMapShader::Render(ID3D11DeviceContext * context, Object* shadowSurfaceObj, Object ** obj, UINT numObjects)
{

	//generate the shadow map

	ID3D11RenderTargetView* oldRenderTarget;
	ID3D11DepthStencilView* oldDepthStencil;
	context->OMGetRenderTargets(1, &oldRenderTarget, &oldDepthStencil);

	// Only bind the ID3D11DepthStencilView for output.
	context->OMSetRenderTargets(0, nullptr, mShadowMapView);

	ID3D11RasterizerState* oldRenderState;
	D3D11_VIEWPORT oldViewPort;
	UINT numViewport = 1;

	context->RSGetState(&oldRenderState);
	context->RSGetViewports(&numViewport, &oldViewPort);

	context->RSSetState(mFrontCullRasterizerState);
	context->RSSetViewports(1, &mShadowViewport);

	context->ClearDepthStencilView(mShadowMapView, D3D11_CLEAR_DEPTH, 1.0f, 0);


	context->VSSetShader(Shaders::VS_ShadowMap, NULL, 0);
	context->PSSetShader(Shaders::PS_ShadowMap, NULL, 0);
	context->IASetInputLayout(Shaders::InputLayout_ShadowMap);

	context->VSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);
	context->VSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//draw shadow maps
	shadowSurfaceObj->Draw();

	for(UINT i = 0; i < numObjects; i++)
		obj[i]->Draw();

	context->RSSetState(oldRenderState);
	context->RSSetViewports(1, &oldViewPort);
	context->OMSetRenderTargets(1, &oldRenderTarget, oldDepthStencil);

	//render the actual geometry
	//------------------------------------------------------------------------------------

	context->VSSetShader(VS_DirectionalLight, NULL, 0);
	context->PSSetShader(PS_ShadowSurfaceDirectionalLight, NULL, 0);
	context->IASetInputLayout(InputLayout_DirectionalLight);


	context->VSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);
	context->PSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);

	context->VSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);
	context->PSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);


	context->VSSetConstantBuffers(2, 1, &ConstantBuffers::DirectionalLightBuffer);
	context->PSSetConstantBuffers(2, 1, &ConstantBuffers::DirectionalLightBuffer);


	context->VSSetConstantBuffers(3, 1, &ConstantBuffers::MaterialBuffer);
	context->PSSetConstantBuffers(3, 1, &ConstantBuffers::MaterialBuffer);


	context->VSSetConstantBuffers(4, 1, &ConstantBuffers::LightVariablesBuffer);
	context->PSSetConstantBuffers(4, 1, &ConstantBuffers::LightVariablesBuffer);


	context->VSSetConstantBuffers(5, 1, &ConstantBuffers::CameraPositionBuffer);
	context->PSSetConstantBuffers(5, 1, &ConstantBuffers::CameraPositionBuffer);

	context->PSSetShaderResources(2, 1, &mShadowMapSRV);
	context->PSSetSamplers(0, 1, &mShadowSamplerState);

	shadowSurfaceObj->Draw();


	context->PSSetShader(PS_DirectionalLight, NULL, 0);

	for (UINT i = 0; i < numObjects; i++)
		obj[i]->Draw();

}

namespace Shaders {
	ID3D11VertexShader*  VS_DirectionalLight = nullptr;
	ID3D11PixelShader*   PS_DirectionalLight = nullptr;
	ID3D11PixelShader*   PS_ShadowSurfaceDirectionalLight = nullptr;

	ID3DBlob*	BLOB_VS_DirectionalLight = nullptr;
	ID3DBlob*   BLOB_PS_DirectionalLight = nullptr;
	ID3DBlob*   BLOB_PS_ShadowSurfaceDirectionalLight = nullptr;

	ID3D11InputLayout* InputLayout_DirectionalLight = nullptr;

	ID3D11VertexShader*  VS_SkyBox = nullptr;
	ID3D11PixelShader*   PS_SkyBox = nullptr;
	ID3DBlob*	BLOB_VS_SkyBox = nullptr;
	ID3DBlob*    BLOB_PS_SkyBox = nullptr;
	ID3D11InputLayout* InputLayout_Skybox = nullptr;

	ID3D11VertexShader*  VS_SimpleColor = nullptr;
	ID3D11PixelShader*   PS_SimpleColor = nullptr;
	ID3DBlob*	BLOB_VS_SimpleColor = nullptr;
	ID3DBlob*   BLOB_PS_SimpleColor = nullptr;
	ID3D11InputLayout* InputLayout_SimpleColor = nullptr;

	ID3D11VertexShader*  VS_ShadowMap = nullptr;
	ID3D11PixelShader*   PS_ShadowMap = nullptr;
	ID3DBlob*	BLOB_VS_ShadowMap = nullptr;
	ID3DBlob*   BLOB_PS_ShadowMap = nullptr;
	ID3D11InputLayout* InputLayout_ShadowMap = nullptr;
};

void Shaders::LoadAll(ID3D11Device * device)
{
	
	//SimpleColor shader
	//================================================================

	HRESULT hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_ColorVertex", "vs_4_0", & BLOB_VS_SimpleColor);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The SimpleColor shader cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the line vertex shader
	hr = device->CreateVertexShader( BLOB_VS_SimpleColor->GetBufferPointer(),  BLOB_VS_SimpleColor->GetBufferSize(), NULL, & VS_SimpleColor);
	if (FAILED(hr))
	{
		ReleaseCOM( BLOB_VS_SimpleColor);
		return;
	}


	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_ColorVertex", "ps_4_0", & BLOB_PS_SimpleColor);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the line pixel shader
	hr = device->CreatePixelShader( BLOB_PS_SimpleColor->GetBufferPointer(),  BLOB_PS_SimpleColor->GetBufferSize(), NULL, & PS_SimpleColor);
	if (FAILED(hr))
	{
		ReleaseCOM( BLOB_PS_SimpleColor);
		return;
	}

	D3D11_INPUT_ELEMENT_DESC PosColorVertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(PosColorVertexDesc, 2,  BLOB_VS_SimpleColor->GetBufferPointer(),  BLOB_VS_SimpleColor->GetBufferSize(), & InputLayout_SimpleColor);



	//Skybox
	//================================================================

	hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_Skybox", "vs_4_0", & BLOB_VS_SkyBox);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the skybox vertex shader
	hr = device->CreateVertexShader( BLOB_VS_SkyBox->GetBufferPointer(),  BLOB_VS_SkyBox->GetBufferSize(), NULL, & VS_SkyBox);
	if (FAILED(hr))
	{
		ReleaseCOM( BLOB_VS_SkyBox);
		return;
	}


	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_Skybox", "ps_4_0", & BLOB_PS_SkyBox);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the skybox pixel shader
	hr = device->CreatePixelShader( BLOB_PS_SkyBox->GetBufferPointer(),  BLOB_PS_SkyBox->GetBufferSize(), NULL, & PS_SkyBox);
	if (FAILED(hr))
	{
		ReleaseCOM( BLOB_PS_SkyBox);
		return;
	}


	D3D11_INPUT_ELEMENT_DESC SkyboxVertexDesc[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	device->CreateInputLayout(SkyboxVertexDesc, 1,  BLOB_VS_SkyBox->GetBufferPointer(),  BLOB_VS_SkyBox->GetBufferSize(), & InputLayout_Skybox);



	//DirectionalLight
	//================================================================

	hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_DirectionalLight", "vs_4_0", & BLOB_VS_DirectionalLight);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The shader VS_DirectionalLight cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the skybox vertex shader
	hr = device->CreateVertexShader( BLOB_VS_DirectionalLight->GetBufferPointer(),  BLOB_VS_DirectionalLight->GetBufferSize(), NULL, & VS_DirectionalLight);
	if (FAILED(hr))
	{
		ReleaseCOM( BLOB_VS_DirectionalLight);
		return;
	}

	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_DirectionalLight", "ps_4_0", & BLOB_PS_DirectionalLight);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The shader PS_DirectionalLight cannot be compiled.", L"Error", MB_OK);
		return;
	}

	hr = device->CreatePixelShader( BLOB_PS_DirectionalLight->GetBufferPointer(),  BLOB_PS_DirectionalLight->GetBufferSize(), NULL, & PS_DirectionalLight);
	if (FAILED(hr))
	{
		ReleaseCOM( BLOB_PS_DirectionalLight);
		return;
	}

	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_ShadowSurfaceDirectionalLight", "ps_4_0", &BLOB_PS_ShadowSurfaceDirectionalLight);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The shader PS_ShadowSurfaceDirectionalLight cannot be compiled.", L"Error", MB_OK);
		return;
	}

	hr = device->CreatePixelShader(BLOB_PS_ShadowSurfaceDirectionalLight->GetBufferPointer(), BLOB_PS_ShadowSurfaceDirectionalLight->GetBufferSize(), NULL, &PS_ShadowSurfaceDirectionalLight);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_PS_ShadowSurfaceDirectionalLight);
		return;
	}


	D3D11_INPUT_ELEMENT_DESC PosNormalUvVertexDesc[] =
	{
		{ "POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	device->CreateInputLayout(PosNormalUvVertexDesc, 3,  BLOB_VS_DirectionalLight->GetBufferPointer(),  BLOB_VS_DirectionalLight->GetBufferSize(), & InputLayout_DirectionalLight);

	//ShadowMap
	//================================================================

	hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_ShadowMap", "vs_4_0", &BLOB_VS_ShadowMap);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The shader VS_ShadowMap cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the skybox vertex shader
	hr = device->CreateVertexShader(BLOB_VS_ShadowMap->GetBufferPointer(), BLOB_VS_ShadowMap->GetBufferSize(), NULL, &VS_ShadowMap);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_VS_ShadowMap);
		return;
	}

	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_ShadowMap", "ps_4_0", &BLOB_PS_ShadowMap);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The shader PS_ShadowMap cannot be compiled.", L"Error", MB_OK);
		return;
	}

	hr = device->CreatePixelShader(BLOB_PS_ShadowMap->GetBufferPointer(), BLOB_PS_ShadowMap->GetBufferSize(), NULL, &PS_ShadowMap);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_PS_ShadowMap);
		return;
	}


	device->CreateInputLayout(PosNormalUvVertexDesc, 3, BLOB_VS_ShadowMap->GetBufferPointer(), BLOB_VS_ShadowMap->GetBufferSize(), &InputLayout_ShadowMap);



	//create showmap variables
	CreateShadowMap(device);
}
HRESULT Shaders::CompileShaderFromFile(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut)
{

	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PREFER_FLOW_CONTROL;
#endif


	ID3DBlob* errors;

	hr = D3DCompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &errors);

	if (errors)
	{
		OutputDebugStringA(reinterpret_cast<const char*>(errors->GetBufferPointer()));
		errors->Release();
	}


	return hr;

}



void Shaders::SkyboxShader::Render(ID3D11DeviceContext * context, Object * obj)
{
	context->VSSetShader(VS_SkyBox, NULL, 0);
	context->PSSetShader(PS_SkyBox, NULL, 0);
	context->IASetInputLayout(InputLayout_Skybox);

	context->VSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);
	context->PSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);

	context->VSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);
	context->PSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);

	context->VSSetConstantBuffers(5, 1, &ConstantBuffers::CameraPositionBuffer);
	context->PSSetConstantBuffers(5, 1, &ConstantBuffers::CameraPositionBuffer);

	obj->Draw();
}

void Shaders::SimpleColorShader::Render(ID3D11DeviceContext * context, Object * obj)
{
	
	context->VSSetShader(VS_SimpleColor, NULL, 0);
	context->PSSetShader(PS_SimpleColor, NULL, 0);
	context->IASetInputLayout(InputLayout_SimpleColor);

	context->VSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);
	context->PSSetConstantBuffers(0, 1, &ConstantBuffers::ViewProjBuffer);

	context->VSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);
	context->PSSetConstantBuffers(1, 1, &ConstantBuffers::WorldMatrixBuffer);


	obj->Draw();
	
}

