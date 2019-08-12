

Texture2D txDiffuse : register(t0);
TextureCube skyCubeMap : register(t1);
Texture2D shadowMap : register(t2);

SamplerComparisonState pointSampler : register (s0);

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


cbuffer LightingVariables : register (b4)
{
	float4 AmbientLight;	
};

cbuffer CameraPosition : register (b5)
{
	float3 EyePosition;
	float _camera_position_padding;
};


cbuffer DirectionalLight : register (b2)
{
	float3   LightDirection;
	float    _directional_light_padding;
	float4   LightColor;
	float4x4 LightWorldMatrix;
	float4x4 LightViewMatrix;
	float4x4 LightProjectionMatrix;
	float4   LightPosition;
};

cbuffer Material : register (b3)
{
	float Ka, Kd, Ks, A;
};

struct DirectionalLightPSInput
{
	float4 Pos :   SV_POSITION;
	float4 LightSpacePos : LIGHTSPACEPOS;
	float2 Uv:     TEXCOORD;
	float3 LightRay: LIGHTRAY;
	float3 CameraRay: CAMRAY;
	float3 Normal: NORMAL;
	float3 HalfAngle: HALFANGLE;
};

float4 calcBlinnPhongLighting(float MaterialKa, float MaterialKd, float MaterialKs, float MaterialA, float4 LColor, float3 N, float3 L, float3 H)
{
	float4 Ia = MaterialKa * AmbientLight;
	float4 Id = MaterialKd * saturate(dot(N, L));
	float4 Is = MaterialKs * pow(saturate(dot(N, H)), MaterialA);

	float4 finalColor = Ia + (Id + Is) * LColor;
	finalColor.a = 1.0f;

	return finalColor;
}


DirectionalLightPSInput VS_DirectionalLight(float4 Pos : POSITION, float3 Normal : NORMAL, float2 Uv : TEXCOORD0)
{
	DirectionalLightPSInput output = (DirectionalLightPSInput)0;

	//transform model to clip space
	float4 modelPos = mul(Pos, World);
	output.Pos = mul(modelPos, View);
	output.Pos = mul(output.Pos, Projection);

	//transform the model into light space
	float4 lightSpacePos = mul(modelPos, LightViewMatrix);
	lightSpacePos = mul(lightSpacePos, LightProjectionMatrix);
	output.LightSpacePos = lightSpacePos;

	//find the lightray 
	output.LightRay = LightPosition.xyz - modelPos.xyz;

	//find the camera ray
	output.CameraRay = EyePosition.xyz - modelPos.xyz;

	//pass on the texture coordinates
	output.Uv = Uv;


	float3 N = normalize(mul(Normal, (float3x3)World));
	float3 V = normalize(EyePosition - (float3)modelPos);
	float3 H = normalize(LightDirection + V);

	output.Normal = N;
	output.HalfAngle = H;

	return output;
}

float4 PS_ShadowSurfaceDirectionalLight(DirectionalLightPSInput input) : SV_Target
{

	//renormalize interpolated vectors
	input.Normal = normalize(input.Normal);
	input.HalfAngle = normalize(input.HalfAngle);

	// Compute texture coordinates for the current point's location on the shadow map.
	float2 shadowTexCoords;
	shadowTexCoords.x = 0.5f + (input.LightSpacePos.x / input.LightSpacePos.w * 0.5f);
	shadowTexCoords.y = 0.5f - (input.LightSpacePos.y / input.LightSpacePos.w * 0.5f);
	float pixelDepth = input.LightSpacePos.z / input.LightSpacePos.w;

	float lighting = 1;

	// Check if the pixel texture coordinate is in the view frustum of the 
	// light before doing any shadow work.
	if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) && (saturate(shadowTexCoords.y) == shadowTexCoords.y) && (pixelDepth > 0))
	{

		// Use an offset value to mitigate shadow artifacts due to imprecise 
		// floating-point values (shadow acne).
		//
		// This is an approximation of epsilon * tan(acos(saturate(NdotL))):
		float margin = acos(saturate(dot(input.Normal, LightDirection)));
#ifdef LINEAR
		// The offset can be slightly smaller with smoother shadow edges.
		float epsilon = 0.0005 / margin;
#else
		float epsilon = 0.001 / margin;
#endif
		// Clamp epsilon to a fixed range so it doesn't go overboard.
		epsilon = clamp(epsilon, 0, 0.1);

		// Use the SampleCmpLevelZero Texture2D method (or SampleCmp) to sample from 
		// the shadow map, just as you would with Direct3D feature level 10_0 and
		// higher.  Feature level 9_1 only supports LessOrEqual, which returns 0 if
		// the pixel is in the shadow.
		lighting = float(shadowMap.SampleCmpLevelZero(pointSampler, shadowTexCoords, pixelDepth + epsilon));
	}

	//pixel not in shadow
	if (lighting == 1)
		return calcBlinnPhongLighting(Ka, Kd, Ks, A, LightColor, input.Normal, LightDirection, input.HalfAngle) * txDiffuse.Sample(triLinearSampler, input.Uv);
	else
		return AmbientLight * txDiffuse.Sample(triLinearSampler, input.Uv);
	

}


float4 PS_DirectionalLight(DirectionalLightPSInput input) : SV_Target
{

	return calcBlinnPhongLighting(Ka, Kd, Ks, A, LightColor, input.Normal, LightDirection, input.HalfAngle) * txDiffuse.Sample(triLinearSampler, input.Uv);
}

struct ColorVertex
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR0;
};

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



struct SkyboxVertex
{
	float4 Pos : SV_POSITION;
	float3 UV : TEXCOORD;
};


SkyboxVertex VS_Skybox(float3 Pos: POSITION)
{
	SkyboxVertex vsout = (SkyboxVertex)0;
	
	//build a transpose matrix based on the camera position
	float4x4 world2 = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		EyePosition.x, EyePosition.y, EyePosition.z, 1.0f

	};

	vsout.Pos = mul(float4(Pos,1.0f), world2);
	vsout.Pos = mul(vsout.Pos, View);
	vsout.Pos = mul(vsout.Pos, Projection);
	vsout.UV = Pos;

	return vsout;

}

float4 PS_Skybox(SkyboxVertex psin) : SV_Target
{

	return skyCubeMap.Sample(triLinearSampler, psin.UV);
}


struct ShadowMapPSInput
{
	float4 pos : SV_POSITION;
};

ShadowMapPSInput VS_ShadowMap(float4 Pos : POSITION, float3 Normal : NORMAL, float2 Uv : TEXCOORD0)
{
	ShadowMapPSInput output;
	output.pos = mul(Pos, World);
	output.pos = mul(output.pos, LightViewMatrix);
	output.pos = mul(output.pos, LightProjectionMatrix);

	return output;
}

void PS_ShadowMap(ShadowMapPSInput input) 
{
	
}