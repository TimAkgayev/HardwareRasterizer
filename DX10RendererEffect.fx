

Texture2D txDiffuse : register(t0);
SamplerState linearSampler : register(s0);

cbuffer ProjectionMatrices : register (b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;

};


PS_INPUT VS_MAIN(float4 Pos : POSITION, float2 UV : TEXCOORD0)
{

	PS_INPUT psInput = (PS_INPUT)0;

	psInput.Pos = mul(Pos, World);
	psInput.Pos = mul(psInput.Pos, View);
	psInput.Pos = mul(psInput.Pos, Projection);
	psInput.UV = UV;


	return psInput;
}


float4 PS_MAIN(PS_INPUT psInput) : SV_Target
{
	return txDiffuse.Sample(linearSampler, psInput.UV);
}



