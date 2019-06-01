#include "InputLayouts.h"

ID3D10InputLayout* InputLayout::PosTex = nullptr;
ID3D10InputLayout* InputLayout::PosColor = nullptr;
ID3D10InputLayout* InputLayout::Skybox = nullptr;


D3D10_INPUT_ELEMENT_DESC InputLayout::PosUVVertexDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
};

D3D10_INPUT_ELEMENT_DESC InputLayout::PosColorVertexDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
};


D3D10_INPUT_ELEMENT_DESC InputLayout::SkyboxVertexDesc[] =
{
	{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 }
};



void InputLayout::InitAll(ID3D10Device* device)
{
	device->CreateInputLayout(PosUVVertexDesc, 2, Shaders::BLOB_VS_SimpleProjection->GetBufferPointer(), Shaders::BLOB_VS_SimpleProjection->GetBufferSize(), &PosTex);
	device->CreateInputLayout(PosColorVertexDesc, 2, Shaders::BLOB_VS_SimpleProjectionColor->GetBufferPointer(), Shaders::BLOB_VS_SimpleProjectionColor->GetBufferSize(), &PosColor);
	device->CreateInputLayout(SkyboxVertexDesc, 1, Shaders::BLOB_VS_SkyBox->GetBufferPointer(), Shaders::BLOB_VS_SkyBox->GetBufferSize(), &Skybox);
}


void InputLayout::DestroyAll()
{
	ReleaseCOM(PosTex);
	ReleaseCOM(PosColor);
	ReleaseCOM(Skybox);
}