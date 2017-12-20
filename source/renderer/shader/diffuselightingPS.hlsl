Texture2D shaderTexture;
SamplerState sampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float4 lightDirection;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float4 lightDir;
    float lightIntensity;
    float4 color;

    textureColor = shaderTexture.Sample(sampleType, input.tex);

    //Invert for calculations
    lightDir = -lightDirection;
    lightIntensity = saturate(dot(input.normal, lightDir));

    color = saturate(diffuseColor * lightIntensity);
    color = color * textureColor;

    return color;
}