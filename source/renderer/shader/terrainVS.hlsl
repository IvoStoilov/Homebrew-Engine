cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

Texture2D heightMap : register (t0);
SamplerState sampleType;

struct VertexInputType
{
    float4 m_Position : POSITION0;
    float2 m_UV : TEXCOORD0;
};

struct PixelInputType
{
    float4 m_FinalPosition  : SV_POSITION;
    float4 m_GlobalPosition : POSITION0;
    float2 m_UV             : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    output.m_UV = input.m_UV;
    //We need to invert the order in which we sample the texture, because it was exported with the following settings :
    //  - -y is forward, but in our case y is forward, this means that the UV.y coord had to be inverted ( 1 - y)
    output.m_UV.y = 1.f - output.m_UV.y;

    input.m_Position.z = heightMap.SampleLevel(sampleType, output.m_UV, 0).x * 35.f;
    input.m_Position.w = 1.0f;
    
    output.m_FinalPosition = mul(input.m_Position, worldMatrix);
    output.m_GlobalPosition = output.m_FinalPosition;
    output.m_FinalPosition = mul(output.m_FinalPosition, viewMatrix);
    output.m_FinalPosition = mul(output.m_FinalPosition, projectionMatrix);

    

    return output;
}