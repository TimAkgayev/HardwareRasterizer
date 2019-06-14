

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
	
	matrix View;
	matrix Projection;
}

cbuffer WorldMatrices : register (b1)
{
	matrix World;
}

cbuffer DirectionalLight : register (b2)
{
	float3 LightDirection;
	float4 LightColor;
};




struct TexturedVertex
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD0;

};

struct SkyboxVertex
{
	float4 Pos : SV_POSITION;
	float3 UV : TEXCOORD;
};

struct ColorVertex
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
};

struct TexturedLitVertex
{
	float4 Pos :   SV_POSITION;
	float3 Normal: NORMAL;
	float2 Uv:     TEXCOORD;
};


TexturedLitVertex VS_DirectionalLight(float4 Pos : POSITION, float3 Normal : NORMAL, float2 Uv : TEXCOORD0)
{
	TexturedLitVertex vsOut = (TexturedLitVertex)0;

	vsOut.Pos = mul(Pos, World);
	vsOut.Pos = mul(vsOut.Pos, View);
	vsOut.Pos = mul(vsOut.Pos, Projection);

	vsOut.Normal = mul(Normal, (float3x3)World);
	vsOut.Uv = Uv;
	
	return vsOut;
}

float4 PS_DirectionalLight(TexturedLitVertex psInput) : SV_Target
{
	psInput.Normal = normalize(psInput.Normal);

	float4 finalColor = 0;

	//do NdotL lighting
	float4 lcolor = { 1.0f, 1.0f, 1.0f, 1.0f };
	finalColor += saturate(dot((float3)LightDirection, psInput.Normal) * lcolor * txDiffuse.Sample(triLinearSampler, psInput.Uv));
	
	finalColor.a = 1;
	return finalColor;
	
}


TexturedVertex VS_TexturedVertex(float4 Pos : POSITION, float2 UV : TEXCOORD0)
{

	TexturedVertex psInput = (TexturedVertex)0;

	psInput.Pos = mul(Pos, World);
	psInput.Pos = mul(psInput.Pos, View);
	psInput.Pos = mul(psInput.Pos, Projection);
	psInput.UV = UV;


	return psInput;
}


float4 PS_TexturedVertex(TexturedVertex psInput) : SV_Target
{
	//return float4(255.0f, 0.0f, 0.0f, 255.0f);
	return txDiffuse.Sample(triLinearSampler, psInput.UV);
}

ColorVertex VS_ColorVertex (float4 Pos: POSITION, float4 Color : COLOR)
{
	ColorVertex psInput = (ColorVertex)0;
	psInput.Pos = mul(Pos, World);
	psInput.Pos = mul(psInput.Pos, View);
	psInput.Pos = mul(psInput.Pos, Projection);
	psInput.Color = Color;

	return psInput;
}


float4 PS_ColorVertex(ColorVertex psInput) : SV_Target
{
	return psInput.Color;
}


SkyboxVertex VS_Skybox(float3 Pos: POSITION)
{
	SkyboxVertex vsout = (SkyboxVertex)0;
	

	vsout.Pos = mul(float4(Pos,1.0f), World);
	vsout.Pos = mul(vsout.Pos, View);
	vsout.Pos = mul(vsout.Pos, Projection);
	vsout.UV = Pos;

	return vsout;

}

float4 PS_Skybox(SkyboxVertex psin) : SV_Target
{

	return skyCubeMap.Sample(triLinearSampler, psin.UV);
}