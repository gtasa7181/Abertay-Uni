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
    
    //MODIFIED WAVE CALCULATIONS
    // Experimented with reducing peak sharpness by dampening amplitude at wave crests

    // Large dominant waves - softened peaks
    float wave1_raw = sin((input.position.x * 0.5 + input.position.z * 0.3) * waveFrequency * 0.5 + time * waveSpeed * 0.7);
    float wave1 = wave1_raw * (1.0 - 0.25 * abs(wave1_raw)) * waveAmplitude * 0.5;

    // Medium choppy waves - softened peaks  
    float wave2_raw = cos((input.position.x * 0.8 - input.position.z * 1.2) * waveFrequency * 1.5 + time * waveSpeed * 1.1);
    float wave2 = wave2_raw * (1.0 - 0.25 * abs(wave2_raw)) * waveAmplitude * 0.3;

    // Small fast waves - softened peaks
    float wave3_raw = sin((input.position.x * 1.0 + input.position.z * 0.7) * waveFrequency * 2.0 + time * waveSpeed * 1.5);
    float wave3 = wave3_raw * (1.0 - 0.25 * abs(wave3_raw)) * waveAmplitude * 0.2;

    // Medium choppy waves - softened peaks
    float wave4_raw = cos((input.position.x * 0.8 - input.position.z * 1.2) * waveFrequency * 1.5 + time * waveSpeed * 1.1);
    float wave4 = wave4_raw * (1.0 - 0.25 * abs(wave4_raw)) * waveAmplitude * 0.2;

    // Small detail ripples - softened peaks
    float wave5_raw = sin((input.position.x * 2.0 + input.position.z * 1.5) * waveFrequency * 3.0 + time * waveSpeed * 2.0);
    float wave5 = wave5_raw * (1.0 - 0.25 * abs(wave5_raw)) * waveAmplitude * 0.15;

    // Combine all waves
    float displacement = wave1 + wave2 + wave3 + wave4 + wave5;


    
    // Apply displacement to Y position
    input.position.y += displacement;
    
    // Calculate wave normal for lighting (derivatives of all 5 waves)
    float dx = cos(input.position.x * waveFrequency * time * waveSpeed) * waveAmplitude * waveFrequency
         - sin((input.position.x * 0.8 - input.position.z * 1.2) * waveFrequency * 1.5 + time * waveSpeed * 1.1) * waveAmplitude * 0.2 * waveFrequency * 1.5 * 0.8
         + cos((input.position.x * 2.0 + input.position.z * 1.5) * waveFrequency * 3.0 + time * waveSpeed * 2.0) * waveAmplitude * 0.15 * waveFrequency * 3.0 * 2.0;
         
    float dz = -sin(input.position.z * waveFrequency * 0.7 * time * waveSpeed * 1.3) * waveAmplitude * 0.5 * waveFrequency * 0.7
         + sin((input.position.x * 0.8 - input.position.z * 1.2) * waveFrequency * 1.5 + time * waveSpeed * 1.1) * waveAmplitude * 0.2 * waveFrequency * 1.5 * 1.2
         + cos((input.position.x * 2.0 + input.position.z * 1.5) * waveFrequency * 3.0 + time * waveSpeed * 2.0) * waveAmplitude * 0.15 * waveFrequency * 3.0 * 1.5;

    float3 waveNormal = normalize(float3(-dx, 1.0, -dz));

    
    // Transform position
    output.position = mul(input.position, worldMatrix);
    output.worldPosition = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Pass through texture coordinates
    // Pass through texture coordinates with tiling
    output.tex = input.tex * 20.0f; // Repeat texture 20 times across the mesh

    
    // Transform normal
    output.normal = mul(waveNormal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    return output;
}
