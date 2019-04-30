cbuffer LightBuffer
{
    float4 diffuseColor;
    float4 lightDirection;
};

Texture2D normalMap : register(t0);
SamplerState sampleType;

struct PixelInputType
{
    float4 m_Position : SV_POSITION;
    float2 m_UV : TEXCOORD0;
    float3 m_Normal : NORMAL;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float3 lightDir;
    float lightIntensity;
    float4 color;

    float4 textureColor = float4(1.f, 1.f, 1.f, 1.f);
    
    lightDir = -lightDirection;
    lightDir = normalize(lightDir.xyz);

    float3 normalSample = normalMap.SampleLevel(sampleType, input.m_UV, 0).xzy;
    float subtract = 1;
    normalSample = (2 * normalSample) - subtract.xxx;
    normalSample = normalize(normalSample);

    float3 terrainNormal = normalize(input.m_Normal);
    float dotRes = dot(normalSample, lightDir.xyz);
    lightIntensity = saturate(dotRes);

    color = saturate(diffuseColor * lightIntensity);
    color = color * textureColor;
    
    return color;
}
