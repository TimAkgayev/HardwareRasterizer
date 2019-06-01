

Texture2D txDiffuse : register(t0);
TextureCube skyCubeMap : register(t1);

SamplerState triLinearSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


cbuffer ProjectionMatrices : register (b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct PS_INPUT_POS_TEX
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;

};

struct PS_INPUT_SKYBOX
{
	float4 Pos : SV_POSITION;
	float3 UV : TEXCOORD;
};

struct PS_INPUT_POS_COLOR
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
};


PS_INPUT_POS_TEX VS_SIMPLEPROJECTION(float4 Pos : POSITION, float2 UV : TEXCOORD0)
{

	PS_INPUT_POS_TEX psInput = (PS_INPUT_POS_TEX)0;

	psInput.Pos = mul(Pos, World);
	psInput.Pos = mul(psInput.Pos, View);
	psInput.Pos = mul(psInput.Pos, Projection);
	psInput.UV = UV;


	return psInput;
}


float4 PS_SIMPLETEXTURE(PS_INPUT_POS_TEX psInput) : SV_Target
{
	return txDiffuse.Sample(triLinearSampler, psInput.UV);
}

PS_INPUT_POS_COLOR VS_SIMPLEPROJECTIONCOLOR (float4 Pos: POSITION, float4 Color : COLOR)
{
	PS_INPUT_POS_COLOR psInput = (PS_INPUT_POS_COLOR)0;
	psInput.Pos = mul(Pos, World);
	psInput.Pos = mul(psInput.Pos, View);
	psInput.Pos = mul(psInput.Pos, Projection);
	psInput.Color = Color;

	return psInput;
}


float4 PS_SIMPLECOLOR(PS_INPUT_POS_COLOR psInput) : SV_Target
{
	return psInput.Color;
}


PS_INPUT_SKYBOX VS_SKYBOX(float3 Pos: POSITION)
{
	PS_INPUT_SKYBOX vsout = (PS_INPUT_SKYBOX)0;
	

	vsout.Pos = mul(float4(Pos,1.0f), World);
	vsout.Pos = mul(vsout.Pos, View);
	vsout.Pos = mul(vsout.Pos, Projection);
	vsout.UV = Pos;

	return vsout;

}

float4 PS_SKYBOX(PS_INPUT_SKYBOX psin) : SV_Target
{
	//return float4(255, 0, 0, 255);

	return skyCubeMap.Sample(triLinearSampler, psin.UV);
}