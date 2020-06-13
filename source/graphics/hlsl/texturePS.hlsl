Texture2D shaderTexture;
SamplerState sampleType;

struct PixelInputType
{
    float4 m_Position : SV_POSITION;
    float2 m_UV       : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor = shaderTexture.Sample(sampleType, input.m_UV);
    return textureColor;
}
