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

float GetDegreeToRad(float degree)
{
    return degree * 3.14159265f / 180.f;
}

float3x3 GetRotateMatrixAxisY(float degree)
{
    float rad = GetDegreeToRad(degree);
    float3x3 rot = { cos(rad), 0.f, sin(rad),
                          0.f, 1.f, 0.f,
                    -sin(rad), 0.f, cos(rad) };
    return rot;
}

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    input.m_Position.w = 1.0f;
    //float4 heightMapSample = heightMap.SampleLevel(sampleType, input.m_UV, 0).xyzw;
    
    output.m_Position = mul(input.m_Position, worldMatrix);
    output.m_Position = mul(output.m_Position, viewMatrix);
    output.m_Position = mul(output.m_Position, projectionMatrix);

    output.m_UV = input.m_UV;
    float3 normalSample = normalMap.SampleLevel(sampleType, input.m_UV, 0).yzx;
    float subtract = 1;
    normalSample = (2 * normalSample) - subtract.xxx;
    output.m_Normal = mul(normalSample, worldMatrix);
    output.m_Normal = normalize(normalSample);
    
    return output;
}