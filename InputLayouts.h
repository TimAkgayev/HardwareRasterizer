#pragma once
#include "common_includes.h"
#include "Shaders.h"

namespace InputLayout
{
	extern ID3D11InputLayout* PosColor;
	extern ID3D11InputLayout* PosTex;
	extern ID3D11InputLayout* PosNormTex;
	extern ID3D11InputLayout* Skybox;


	extern D3D11_INPUT_ELEMENT_DESC PosUvVertexDesc[];
	extern D3D11_INPUT_ELEMENT_DESC PosColorVertexDesc[];
	extern D3D11_INPUT_ELEMENT_DESC PosNormalUvVertexDesc[];
	extern D3D11_INPUT_ELEMENT_DESC SkyboxVertexDesc[];


	void InitAll(ID3D11Device* device);
	void DestroyAll();
};
