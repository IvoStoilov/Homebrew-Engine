cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


//Texture2D heightMap : register (t0);
Texture2D normalMap : register (t0);
SamplerState sampleType;

struct VertexInputType
{
    float4 m_Position : POSITION0;
    float2 m_UV : TEXCOORD0;
};

struct PixelInputType
{
    float4 m_Position : SV_POSITION;
    float2 m_UV       : TEXCOORD0;
    float3 m_Normal   : NORMAL;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    input.m_Position.w = 1.0f;
    //float4 heightMapSample = heightMap.SampleLevel(sampleType, input.m_UV, 0).xyzw;
    
    output.m_Position = mul(input.m_Position, worldMatrix);
    output.m_Position = mul(output.m_Position, viewMatrix);
    output.m_Position = mul(output.m_Position, projectionMatrix);

    output.m_UV = input.m_UV;
    //We need to invert the order in which we sample the texture, because it was exported with the following settings :
    //  - z is height in the texture, in our engine y is height, hence the normal sample is .xzy
    //  - -y is forward, but in our case y is forward, this means that the UV.y coord had to be inverted ( 1 - y)
    output.m_UV.y = 1.f - output.m_UV.y;

    float3 normalSample = normalMap.SampleLevel(sampleType, output.m_UV, 0).xzy;
    float subtract = 1;
    normalSample = (2 * normalSample) - subtract.xxx;
    output.m_Normal = mul(normalSample, worldMatrix);
    output.m_Normal = normalize(normalSample);
    
    return output;
}