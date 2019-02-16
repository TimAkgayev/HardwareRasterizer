

Texture2D txDiffuse : register(t0);
SamplerState linearSampler : register(s0);

cbuffer ProjectionMatrices : register (b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct PS_INPUT_MESH
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;

};

struct PS_INPUT_LINE
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
};


PS_INPUT_MESH VS_MAIN_MESH(float4 Pos : POSITION, float2 UV : TEXCOORD0)
{

	PS_INPUT_MESH psInput = (PS_INPUT_MESH)0;

	psInput.Pos = mul(Pos, World);
	psInput.Pos = mul(psInput.Pos, View);
	psInput.Pos = mul(psInput.Pos, Projection);
	psInput.UV = UV;


	return psInput;
}


float4 PS_MAIN_MESH(PS_INPUT_MESH psInput) : SV_Target
{
	return txDiffuse.Sample(linearSampler, psInput.UV);
}

PS_INPUT_LINE VS_MAIN_LINE(float4 Pos: POSITION, float4 Color : COLOR)
{
	PS_INPUT_LINE psInput = (PS_INPUT_LINE)0;
	psInput.Pos = mul(Pos, World);
	psInput.Pos = mul(psInput.Pos, View);
	psInput.Pos = mul(psInput.Pos, Projection);
	psInput.Color = Color;

	return psInput;
}


float4 PS_MAIN_LINE(PS_INPUT_LINE psInput) : SV_Target
{
	return psInput.Color;
}
