// water_vs.hlsl - Ocean wave vertex shader
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer TimeBuffer : register(b1)
{
    float time;
    float waveAmplitude;
    float waveFrequency;
    float waveSpeed;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

OutputType main(InputType input)
{
    OutputType output;
    
    // Calculate wave displacement
    float wave1 = sin(input.position.x * waveFrequency + time * waveSpeed) * waveAmplitude;
    float wave2 = cos(input.position.z * waveFrequency * 0.7 + time * waveSpeed * 1.3) * waveAmplitude * 0.5;
    
    // Combine waves
    float displacement = wave1 + wave2;
    
    // Apply displacement to Y position
    input.position.y += displacement;
    
    // Calculate wave normal for lighting
    float dx = cos(input.position.x * waveFrequency + time * waveSpeed) * waveAmplitude * waveFrequency;
    float dz = -sin(input.position.z * waveFrequency * 0.7 + time * waveSpeed * 1.3) * waveAmplitude * 0.5 * waveFrequency * 0.7;
    
    float3 waveNormal = normalize(float3(-dx, 1.0, -dz));
    
    // Transform position
    output.position = mul(input.position, worldMatrix);
    output.worldPosition = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Pass through texture coordinates
    output.tex = input.tex;
    
    // Transform normal
    output.normal = mul(waveNormal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    return output;
}
