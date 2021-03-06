cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer ReflectionBuffer : register(b2)
{
    matrix reflectionMatrix;
    matrix projectionMatrix1;
}

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 globalPosition : POSITION0;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
    float4 refractionPosition : TEXCOORD2;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.globalPosition = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;

    // Create the reflection projection world matrix.
    matrix reflectProjectWorld;
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(viewMatrix, reflectProjectWorld);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

    // Calculate the input position against the reflectProjectWorld matrix.
    output.reflectionPosition = mul(input.position, reflectProjectWorld);

    matrix viewProjectWorld;
    viewProjectWorld = mul(viewMatrix, projectionMatrix);
    viewProjectWorld = mul(worldMatrix, viewProjectWorld);

    // Calculate the input position against the reflectProjectWorld matrix.
    output.refractionPosition = mul(input.position, viewProjectWorld);
    
    return output;
}
