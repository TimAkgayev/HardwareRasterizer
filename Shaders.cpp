#include "Shaders.h"

ID3D11VertexShader*  Shaders::VS_SimpleProjection = nullptr;
ID3D11PixelShader*   Shaders::PS_SimpleTexture = nullptr;

ID3D11VertexShader*  Shaders::VS_SimpleProjectionColor = nullptr;
ID3D11PixelShader*   Shaders::PS_SimpleColor = nullptr;

ID3D11VertexShader*  Shaders::VS_SkyBox = nullptr;
ID3D11PixelShader*   Shaders::PS_SkyBox = nullptr;

ID3D11VertexShader*  Shaders::VS_DirectionalLight = nullptr;
ID3D11PixelShader*   Shaders::PS_DirectionalLight = nullptr;

ID3DBlob*	Shaders::BLOB_VS_SimpleProjection = nullptr;
ID3DBlob*  Shaders::BLOB_PS_SimpleTexture = nullptr;

ID3DBlob*	Shaders::BLOB_VS_SimpleProjectionColor = nullptr;
ID3DBlob*  Shaders::BLOB_PS_SimpleColor = nullptr;

ID3DBlob*	Shaders::BLOB_VS_SkyBox = nullptr;
ID3DBlob*  Shaders::BLOB_PS_SkyBox = nullptr;


ID3DBlob*	Shaders::BLOB_VS_DirectionalLight = nullptr;
ID3DBlob*  Shaders::BLOB_PS_DirectionalLight = nullptr;

void Shaders::InitAll(ID3D11Device * device)
{
	//compile shaders ============================================

	//compile the mesh vertex shader
	HRESULT hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_TexturedVertex", "vs_4_0", &BLOB_VS_SimpleProjection);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Simple shader cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the mesh vertex shader
	hr = device->CreateVertexShader(BLOB_VS_SimpleProjection->GetBufferPointer(), BLOB_VS_SimpleProjection->GetBufferSize(), NULL, &VS_SimpleProjection);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_VS_SimpleProjection);
		return;
	}

	//================================================================

	// compile the mesh pixel shader
	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_TexturedVertex", "ps_4_0", &BLOB_PS_SimpleTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Simple texture shader cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the mesh pixel shader
	hr = device->CreatePixelShader(BLOB_PS_SimpleTexture->GetBufferPointer(), BLOB_PS_SimpleTexture->GetBufferSize(), NULL, &PS_SimpleTexture);
	
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_PS_SimpleTexture);
		return;
	}

	//================================================================
	

	hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_ColorVertex", "vs_4_0", &BLOB_VS_SimpleProjectionColor);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The ProjectionColor shader cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the line vertex shader
	hr = device->CreateVertexShader(BLOB_VS_SimpleProjectionColor->GetBufferPointer(), BLOB_VS_SimpleProjectionColor->GetBufferSize(), NULL, &VS_SimpleProjectionColor);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_VS_SimpleProjectionColor);
		return;
	}

	//================================================================

	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_ColorVertex", "ps_4_0", &BLOB_PS_SimpleColor);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the line pixel shader
	hr = device->CreatePixelShader(BLOB_PS_SimpleColor->GetBufferPointer(), BLOB_PS_SimpleColor->GetBufferSize(), NULL, &PS_SimpleColor);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_PS_SimpleColor);
		return;
	}


	//================================================================

	hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_Skybox", "vs_4_0", &BLOB_VS_SkyBox);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the skybox vertex shader
	hr = device->CreateVertexShader(BLOB_VS_SkyBox->GetBufferPointer(), BLOB_VS_SkyBox->GetBufferSize(), NULL, &VS_SkyBox);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_VS_SkyBox);
		return;
	}

	//================================================================

	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_Skybox", "ps_4_0", &BLOB_PS_SkyBox);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the skybox pixel shader
	hr = device->CreatePixelShader(BLOB_PS_SkyBox->GetBufferPointer(), BLOB_PS_SkyBox->GetBufferSize(), NULL, &PS_SkyBox);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_PS_SkyBox);
		return;
	}

	//================================================================

	hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_DirectionalLight", "vs_4_0", &BLOB_VS_DirectionalLight);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The shader VS_DirectionalLight cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the skybox vertex shader
	hr = device->CreateVertexShader(BLOB_VS_DirectionalLight->GetBufferPointer(), BLOB_VS_DirectionalLight->GetBufferSize(), NULL, &VS_DirectionalLight);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_VS_DirectionalLight);
		return;
	}

	//================================================================

	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_DirectionalLight", "ps_4_0", &BLOB_PS_DirectionalLight);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The shader PS_DirectionalLight cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the skybox pixel shader
	hr = device->CreatePixelShader(BLOB_PS_DirectionalLight->GetBufferPointer(), BLOB_PS_DirectionalLight->GetBufferSize(), NULL, &PS_DirectionalLight);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_PS_DirectionalLight);
		return;
	}
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

