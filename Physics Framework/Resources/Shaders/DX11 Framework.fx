//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

struct SurfaceInfo
{
	float4 AmbientMtrl;
	float4 DiffuseMtrl;
	float4 SpecularMtrl;
};

struct Light
{
	float4 AmbientLight;
	float4 DiffuseLight;
	float4 SpecularLight;

	float SpecularPower;
	float3 LightVecW;
};

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;
	matrix View;
	matrix Projection;

	SurfaceInfo surface;
	Light light;

	float3 EyePosW;
	float HasTexture;
}

struct VS_INPUT
{
	float4 PosL : POSITION;
	float3 NormL : NORMAL;
	float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 PosH : SV_POSITION;
	float3 NormW : NORMAL;

	float3 PosW : POSITION;
	float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

	float4 posW = mul(input.PosL, World);
	output.PosW = posW.xyz;

	output.PosH = mul(posW, View);
	output.PosH = mul(output.PosH, Projection);
	output.Tex = input.Tex;

	float3 normalW = mul(float4(input.NormL, 0.0f), World).xyz;
	output.NormW = normalize(normalW);

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

Texture2D txDiffuse : TEXTURE : register(t0);
SamplerState samLinear : SAMPLER : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
	float3 normalW = normalize(input.NormW);

	float3 toEye = normalize(EyePosW - input.PosW);

	// Get texture data from file
	float4 textureColour = txDiffuse.Sample(samLinear, input.Tex);

	float3 ambient = float3(0.0f, 0.0f, 0.0f);
	float3 diffuse = float3(0.0f, 0.0f, 0.0f);
	float3 specular = float3(0.0f, 0.0f, 0.0f);

	float3 lightLecNorm = normalize(light.LightVecW);
	// Compute Colour

	// Compute the reflection vector.
	float3 r = reflect(-lightLecNorm, normalW);

	// Determine how much specular light makes it into the eye.
	float specularAmount = pow(max(dot(r, toEye), 0.0f), light.SpecularPower);

	// Determine the diffuse light intensity that strikes the vertex.
	float diffuseAmount = max(dot(lightLecNorm, normalW), 0.0f);

	// Only display specular when there is diffuse
	if (diffuseAmount <= 0.0f)
	{
		specularAmount = 0.0f;
	}

	// Compute the ambient, diffuse, and specular terms separately.
	specular += specularAmount * (surface.SpecularMtrl * light.SpecularLight).rgb;
	diffuse += diffuseAmount * (surface.DiffuseMtrl * light.DiffuseLight).rgb;
	ambient += (surface.AmbientMtrl * light.AmbientLight).rgb;

	// Sum all the terms together and copy over the diffuse alpha.
	float4 finalColour;

	if (HasTexture == 1.0f)
	{
		finalColour.rgb = (textureColour.rgb * (ambient + diffuse)) + specular;
        //finalColour.rgb = specular;
    }
	else
	{
		finalColour.rgb = ambient + diffuse + specular;
	}

	finalColour.a = surface.DiffuseMtrl.a;

	return finalColour;
}