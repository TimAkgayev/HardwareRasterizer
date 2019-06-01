#pragma once
#include "d3dcommon.h"
#include "Shaders.h"

namespace InputLayout
{
	extern ID3D10InputLayout* PosColor;
	extern ID3D10InputLayout* PosTex;
	extern ID3D10InputLayout* Skybox;


	extern D3D10_INPUT_ELEMENT_DESC PosUVVertexDesc[];
	extern D3D10_INPUT_ELEMENT_DESC PosColorVertexDesc[];
	extern D3D10_INPUT_ELEMENT_DESC SkyboxVertexDesc[];


	void InitAll(ID3D10Device* device);
	void DestroyAll();
};
