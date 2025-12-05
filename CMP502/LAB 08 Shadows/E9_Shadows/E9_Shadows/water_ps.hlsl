// water_ps.hlsl - Ocean water pixel shader
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(b0)
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
    // Ocean water color (deep blue-green)
    float4 waterColor = float4(0.15, 0.55, 0.75, 1.0);
    
    // Sample texture
    float4 textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    // Blend texture with water color
    float4 color = waterColor * 0.85 + textureColor * 0.15;
    
    // Calculate lighting
    float3 lightDir = normalize(-lightDirection);
    float lightIntensity = saturate(dot(input.normal, lightDir));
    
    // Apply ambient and diffuse lighting
    float4 finalColor = ambientColor * 1.2;
    finalColor += (diffuseColor * lightIntensity);
    
    // Multiply by color
    finalColor = finalColor * color * 1.3;
    finalColor.a = 1.0;
    
    return finalColor;
}
