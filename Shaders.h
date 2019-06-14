#pragma once
#include "common_includes.h"

namespace Shaders
{
	extern ID3D11VertexShader*  VS_SimpleProjection;
	extern ID3D11PixelShader*   PS_SimpleTexture;

	extern ID3D11VertexShader*  VS_SimpleProjectionColor;
	extern ID3D11PixelShader*   PS_SimpleColor;

	extern ID3D11VertexShader*  VS_SkyBox;
	extern ID3D11PixelShader*   PS_SkyBox;

	extern ID3D11VertexShader*  VS_DirectionalLight;
	extern ID3D11PixelShader*   PS_DirectionalLight;



	extern ID3DBlob*	BLOB_VS_SimpleProjection;
	extern ID3DBlob*  BLOB_PS_SimpleTexture;

	extern ID3DBlob*	BLOB_VS_SimpleProjectionColor;
	extern ID3DBlob*  BLOB_PS_SimpleColor;

	extern ID3DBlob*	BLOB_VS_SkyBox;
	extern ID3DBlob*  BLOB_PS_SkyBox;

	extern ID3DBlob*	BLOB_VS_DirectionalLight;
	extern ID3DBlob*  BLOB_PS_DirectionalLight;


	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	void InitAll(ID3D11Device* device);
};
