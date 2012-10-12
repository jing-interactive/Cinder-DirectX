//=============================================================================
// Lighting.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Transforms and lights geometry.
//=============================================================================

#include "LightHelper.fx"
 
cbuffer cbPerFrame
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	float3 gEyePosW;
};

Texture2D gDiffuseMap;
Texture2D gNormalMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

RasterizerState CullFront
{
//	FILLMODE = WIREFRAME;
	CullMode = FRONT;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	//float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	Material gMaterial;
};

struct VS_INPUT
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex		: TEXCOORD;
    float3 TangentL : TANGENT;
};

struct VS_OUTPUT
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 Tex		: TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(input.PosL, 1.0f), gWorld).xyz;
	//vout.NormalW = mul(input.NormalL, (float3x3)gWorldInvTranspose);
	vout.NormalW = mul(input.NormalL, (float3x3)gWorld);
    vout.TangentW = mul(input.TangentL, (float3x3)gWorld);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(input.PosL, 1.0f), gWorldViewProj);
	
	vout.Tex = input.Tex;
	
	return vout;
}
  
float4 PS(VS_OUTPUT input) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	input.NormalW = normalize(input.NormalW);
    
	float4 texColor = gDiffuseMap.Sample( samLinear, input.Tex );

	// Interpolating normal can unnormalize it, so normalize it.
    input.NormalW = normalize(input.NormalW);
    
	//
	// Normal mapping
	//
    float3 normalT = gNormalMap.Sample( samLinear, input.Tex );
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalT, input.NormalW, input.TangentW);
    
	float3 toEyeW = normalize(gEyePosW - input.PosW);

	// Start with a sum of zero. 
	float4 ambient = float4(0.2f, 0.2f, 0.2f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.
	float4 A, D, S;
	ComputeDirectionalLight(gMaterial, gDirLight, bumpedNormalW, toEyeW, A, D, S);
	ambient += A;  
	diffuse += D;
	spec    += S;

	float4 litColor = texColor*(ambient + diffuse);

	// Common to take alpha from diffuse material.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;
}

technique11 LightTech
{
    pass P0
    {
//		SetRasterizerState( CullFront ); 
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
