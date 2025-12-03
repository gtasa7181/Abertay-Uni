// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

#define MAX_LIGHTS 3
static const int bnumLights = 3;
struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 position;
    float specularPower;

	
    float attenuationConst;
    float attenuationLinear;
    float attenuationQuad;
    float padding1;
};

cbuffer LightBuffer : register(b0)
{
    PointLight lights[MAX_LIGHTS];
    //int numLights;
    //float3 padding2;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 ldiffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(ldiffuse * intensity);
    return colour;
}

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(specularColour * specularIntensity);
}


float4 main(InputType input) : SV_TARGET
{
	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    float4 totalLightColour = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    for (int i = 0; i < bnumLights; i++)
    {
        float3 lightVector;
        float attenuation = 1.0f;
        
        if (i == 0) // Directional Light
        {
            lightVector = normalize(-lights[i].position);
        }
        else
        {
            lightVector = normalize(lights[i].position - input.worldPosition);
            float distance = length(lights[i].position - input.worldPosition);
        
            attenuation = 1.0f / (lights[i].attenuationConst + (lights[i].attenuationLinear * distance) + (lights[i].attenuationQuad * distance * distance));
        }
        
        float4 diffuseLight = calculateLighting(lightVector, input.normal, lights[i].diffuse);
        float4 specularLight = calcSpecular(lightVector, input.normal, input.viewVector, lights[i].specular, lights[i].specularPower);
        
        totalLightColour += lights[i].ambient;
        totalLightColour += (diffuseLight + specularLight) * attenuation;
    }
    return saturate(totalLightColour * textureColour);
}