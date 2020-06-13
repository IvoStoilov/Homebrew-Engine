cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 m_Position : POSITION0;
    float2 m_UV       : TEXCOORD0;
};

struct PixelInputType
{
    float4 m_Position : SV_POSITION;
    float2 m_UV       : TEXCOORD0;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    input.m_Position.w = 1.0f;

    output.m_Position = mul(input.m_Position, worldMatrix);
    output.m_Position = mul(output.m_Position, viewMatrix);
    output.m_Position = mul(output.m_Position, projectionMatrix);

    output.m_UV = input.m_UV;

    return output;
}