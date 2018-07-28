Texture2D shaderTexture;
SamplerState sampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float4 lightDirection;
    float4 hasTexture;
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

    if (hasTexture.x > 0)
        textureColor = shaderTexture.Sample(sampleType, input.tex);
    else
        textureColor = (1.f, 1.f, 1.f, 1.f);

    //Invert for calculations
    lightDir = -lightDirection;
    lightIntensity = saturate(dot(input.normal, lightDir));

    color = saturate(diffuseColor * lightIntensity);
    color = color * textureColor;

    return color;
}