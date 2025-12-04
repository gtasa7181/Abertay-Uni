// Water pixel shader
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
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
    
    // Sample the texture
    textureColor = shaderTexture.Sample(SampleType, input.tex);
    
    // Add blue tint for water
    textureColor *= float4(0.3, 0.5, 0.8, 1.0);
    
    // Calculate lighting
    lightDir = -lightDirection;
    lightIntensity = saturate(dot(input.normal, lightDir));
    
    // Combine ambient + diffuse lighting
    color = ambientColor + (diffuseColor * lightIntensity);
    color = saturate(color * textureColor);
    
    return color;
}
