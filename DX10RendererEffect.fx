


cbuffer ProjectionMatrices : register (b0)
{
	matrix World;
	matrix View;
	matrix Projection;
}

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;

};


PS_INPUT VS_MAIN(float4 Pos : POSITION, float4 Color : COLOR)
{

	PS_INPUT psInput;

	psInput.Pos = mul(Pos, World);
	psInput.Pos = mul(psInput.Pos, View);
	psInput.Pos = mul(psInput.Pos, Projection);
	psInput.Color = Color;


	return psInput;
}


float4 PS_MAIN(PS_INPUT psInput) : SV_Target
{
	return psInput.Color;
}



