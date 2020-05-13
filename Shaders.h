#pragma once
#include "common_includes.h"
#include "Object.h"
#include "ConstantBuffers.h"

namespace Shaders
{

	extern ID3D11VertexShader*  VS_DirectionalLight;
	extern ID3D11PixelShader*   PS_DirectionalLight;
	extern ID3D11PixelShader*   PS_ShadowSurfaceDirectionalLight;

	extern ID3DBlob*	BLOB_VS_DirectionalLight;
	extern ID3DBlob*   BLOB_PS_DirectionalLight;
	extern ID3DBlob*   BLOB_PS_ShadowSurfaceDirectionalLight;

	extern ID3D11InputLayout* InputLayout_DirectionalLight;

	extern ID3D11VertexShader*  VS_SkyBox;
	extern ID3D11PixelShader*   PS_SkyBox;
	extern ID3DBlob*	BLOB_VS_SkyBox;
	extern ID3DBlob*    BLOB_PS_SkyBox;
	extern ID3D11InputLayout* InputLayout_Skybox;

	extern ID3D11VertexShader*  VS_SimpleColor;
	extern ID3D11PixelShader*   PS_SimpleColor;
	extern ID3DBlob*	BLOB_VS_SimpleColor;
	extern ID3DBlob*   BLOB_PS_SimpleColor;
	extern ID3D11InputLayout* InputLayout_SimpleColor;

	extern ID3D11VertexShader*  VS_ShadowMap;
	extern ID3D11PixelShader*   PS_ShadowMap;
	extern ID3DBlob*	BLOB_VS_ShadowMap;
	extern ID3DBlob*   BLOB_PS_ShadowMap;
	extern ID3D11InputLayout* InputLayout_ShadowMap;


	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	void LoadAll(ID3D11Device* device);

	
	namespace SkyboxShader
	{

		void Render(ID3D11DeviceContext* context, IDrawable* obj);

	};

	namespace SimpleColorShader
	{

		void Render(ID3D11DeviceContext* context, IDrawable* obj);

	};

	namespace ShadowMapShader
	{
		void Render(ID3D11DeviceContext* context, IDrawable* shadowSurfaceObj, IDrawable** obj, UINT numObjects);
	};

	

};
