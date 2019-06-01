#pragma once
#include "d3dcommon.h"

namespace Shaders
{
	extern ID3D10VertexShader*  VS_SimpleProjection;
	extern ID3D10PixelShader*   PS_SimpleTexture;

	extern ID3D10VertexShader*  VS_SimpleProjectionColor;
	extern ID3D10PixelShader*   PS_SimpleColor;

	extern ID3D10VertexShader*  VS_SkyBox;
	extern ID3D10PixelShader*   PS_SkyBox;


	extern ID3D10Blob*	BLOB_VS_SimpleProjection;
	extern ID3D10Blob*  BLOB_PS_SimpleTexture;

	extern ID3D10Blob*	BLOB_VS_SimpleProjectionColor;
	extern ID3D10Blob*  BLOB_PS_SimpleColor;

	extern ID3D10Blob*	BLOB_VS_SkyBox;
	extern ID3D10Blob*  BLOB_PS_SkyBox;


	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	void InitAll(ID3D10Device* device);
};
