// Water vertex shader with wave manipulation
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer TimeBuffer : register(b1)
{
    float time;
    float amplitude;
    float frequency;
    float speed;
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
    
    // Wave manipulation - Create ocean waves
    float wave1 = sin((input.position.x * frequency) + (time * speed)) * amplitude;
    float wave2 = cos((input.position.z * frequency * 0.8) + (time * speed * 0.7)) * amplitude * 0.5;
    
    // Apply wave displacement to Y position
    input.position.y += wave1 + wave2;
    
    // Calculate new normal for correct lighting on waves
    // Derivatives for tangent and bitangent
    float dx = cos((input.position.x * frequency) + (time * speed)) * amplitude * frequency;
    float dz = -sin((input.position.z * frequency * 0.8) + (time * speed * 0.7)) * amplitude * frequency * 0.4;
    
    float3 tangent = normalize(float3(1.0, dx, 0.0));
    float3 bitangent = normalize(float3(0.0, dz, 1.0));
    float3 newNormal = cross(tangent, bitangent);
    
    // Transform to world space
    output.position = mul(input.position, worldMatrix);
    output.worldPosition = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;
    output.normal = mul(newNormal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    return output;
}
