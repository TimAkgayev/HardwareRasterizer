TextureCube gSkyCubeMap : register(t0);
cbuffer ProjectionMatrices : register(b0)
{
	matrix TotalTransform;
};

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 texC : TEXCOORD0;
};

PS_INPUT VS_MAIN(float3 Pos : POSITION)
{
	PS_INPUT psInput = (PS_INPUT)0;

	psInput.Pos = mul(float4(Pos, 1.0f), TotalTransform).xyww;
	psInput.texC = Pos;

	return psInput;
}

float4 PS_MAIN(PS_INPUT pIn) : SV_Target
{
	return gSkyCubeMap.Sample(gTriLinearSam, pIn.texC);
}