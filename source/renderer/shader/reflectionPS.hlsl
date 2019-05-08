SamplerState SampleType;

Texture2D reflectionTexture : register(t9);
Texture2D albedoTexture     : register(t10);

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor = albedoTexture.Sample(SampleType, input.tex);

    // Calculate the projected reflection texture coordinates.
    float2 reflectTexCoord;
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

    float4 reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);
    float4 color = lerp(textureColor, reflectionColor, 0.15f);
    
    return color;
}
