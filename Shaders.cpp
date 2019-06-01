#include "Shaders.h"

ID3D10VertexShader*  Shaders::VS_SimpleProjection = nullptr;
ID3D10PixelShader*   Shaders::PS_SimpleTexture = nullptr;

ID3D10VertexShader*  Shaders::VS_SimpleProjectionColor = nullptr;
ID3D10PixelShader*   Shaders::PS_SimpleColor = nullptr;

ID3D10VertexShader*  Shaders::VS_SkyBox = nullptr;
ID3D10PixelShader*   Shaders::PS_SkyBox = nullptr;


ID3D10Blob*	Shaders::BLOB_VS_SimpleProjection = nullptr;
ID3D10Blob*  Shaders::BLOB_PS_SimpleTexture = nullptr;

ID3D10Blob*	Shaders::BLOB_VS_SimpleProjectionColor = nullptr;
ID3D10Blob*  Shaders::BLOB_PS_SimpleColor = nullptr;

ID3D10Blob*	Shaders::BLOB_VS_SkyBox = nullptr;
ID3D10Blob*  Shaders::BLOB_PS_SkyBox = nullptr;



void Shaders::InitAll(ID3D10Device * device)
{
	//compile shaders ============================================

	//compile the mesh vertex shader
	HRESULT hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_SIMPLEPROJECTION", "vs_4_0", &BLOB_VS_SimpleProjection);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Simple projection shader cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the mesh vertex shader
	hr = device->CreateVertexShader(BLOB_VS_SimpleProjection->GetBufferPointer(), BLOB_VS_SimpleProjection->GetBufferSize(), &VS_SimpleProjection);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_VS_SimpleProjection);
		return;
	}

	//================================================================

	// compile the mesh pixel shader
	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_SIMPLETEXTURE", "ps_4_0", &BLOB_PS_SimpleTexture);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Simple texture shader cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the mesh pixel shader
	hr = device->CreatePixelShader(BLOB_PS_SimpleTexture->GetBufferPointer(), BLOB_PS_SimpleTexture->GetBufferSize(), &PS_SimpleTexture);
	
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_PS_SimpleTexture);
		return;
	}

	//================================================================
	

	hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_SIMPLEPROJECTIONCOLOR", "vs_4_0", &BLOB_VS_SimpleProjectionColor);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The ProjectionColor shader cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the line vertex shader
	hr = device->CreateVertexShader(BLOB_VS_SimpleProjectionColor->GetBufferPointer(), BLOB_VS_SimpleProjectionColor->GetBufferSize(), &VS_SimpleProjectionColor);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_VS_SimpleProjectionColor);
		return;
	}

	//================================================================

	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_SIMPLECOLOR", "ps_4_0", &BLOB_PS_SimpleColor);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the line pixel shader
	hr = device->CreatePixelShader(BLOB_PS_SimpleColor->GetBufferPointer(), BLOB_PS_SimpleColor->GetBufferSize(), &PS_SimpleColor);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_PS_SimpleColor);
		return;
	}


	//================================================================

	hr = CompileShaderFromFile(TEXT("DX10HardwareRendererEffect.fx"), "VS_SKYBOX", "vs_4_0", &BLOB_VS_SkyBox);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled", L"Error", MB_OK);
		return;
	}

	//create the skybox vertex shader
	hr = device->CreateVertexShader(BLOB_VS_SkyBox->GetBufferPointer(), BLOB_VS_SkyBox->GetBufferSize(), &VS_SkyBox);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_VS_SkyBox);
		return;
	}

	//================================================================

	hr = CompileShaderFromFile(L"DX10HardwareRendererEffect.fx", "PS_SKYBOX", "ps_4_0", &BLOB_PS_SkyBox);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"The FX file cannot be compiled.", L"Error", MB_OK);
		return;
	}

	// create the skybox pixel shader
	hr = device->CreatePixelShader(BLOB_PS_SkyBox->GetBufferPointer(), BLOB_PS_SkyBox->GetBufferSize(), &PS_SkyBox);
	if (FAILED(hr))
	{
		ReleaseCOM(BLOB_PS_SkyBox);
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
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX10CompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);

	if (FAILED(hr))
	{
		char* err = (char*)pErrorBlob->GetBufferPointer();
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;

}

