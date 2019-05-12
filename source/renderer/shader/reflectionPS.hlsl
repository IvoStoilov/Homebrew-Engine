cbuffer DeltaTimeBuffer : register(b3)
{
    float4 moveFactor;
}

SamplerState SampleType;

Texture2D reflectionTexture : register(t9);
Texture2D refractionTexture : register(t10);
Texture2D dudvTexture : register(t11);
Texture2D normapMapTexture : register(t12);

#define WAVE_STRENGTH 0.02f
#define TILE_SCALE 4.f

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 globalPosition : POSITION0;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
    float4 refractionPosition : TEXCOORD2;
};

float2 ComputeDistortion(in float3 globalPosition)
{
    float2 globalPosSample = globalPosition.xz / TILE_SCALE;

    float2 globalPosSample1 = globalPosSample;
    globalPosSample1.x += moveFactor.x;
    float2 distortion1 = (dudvTexture.Sample(SampleType, globalPosSample1).rg * 2.f - 1.f) * WAVE_STRENGTH;

    float2 globalPosSample2;
    globalPosSample2.x = -globalPosSample.x;
    globalPosSample2.y =  globalPosSample.y + moveFactor.x;

    float2 distortion2 = (dudvTexture.Sample(SampleType, globalPosSample2).rg * 2.f - 1.f) * WAVE_STRENGTH;

    return distortion1 + distortion2;
}

float4 SampleRelfectionColor(in float4 reflectionPosition, in float2 distortion)
{
    float2 reflectTexCoord = reflectionPosition.xy / reflectionPosition.w / 2.0f + 0.5f;

    reflectTexCoord += distortion;
    reflectTexCoord = clamp(reflectTexCoord, 0.001f, 0.999f);

    return reflectionTexture.Sample(SampleType, reflectTexCoord);
}

float4 SampleRefractionColor(in float4 refractionPosition, in float2 distortion)
{
    float2 refractTexCoord;
    refractTexCoord.x = refractionPosition.x / refractionPosition.w / 2.f + 0.5;
    refractTexCoord.y = -refractionPosition.y / refractionPosition.w / 2.f + 0.5;
    
    refractTexCoord += distortion;
    refractTexCoord = clamp(refractTexCoord, 0.001f, 0.999f);

    return refractionTexture.Sample(SampleType, refractTexCoord);
}

float4 main(PixelInputType input) : SV_TARGET
{
    float2 distortion = ComputeDistortion(input.globalPosition);

    float4 reflectionColor = SampleRelfectionColor(input.reflectionPosition, distortion);
    float4 refractionColor = SampleRefractionColor(input.refractionPosition, distortion);

    float4 color = lerp(refractionColor, reflectionColor, 0.5f);
    color = lerp(color, float4(0.f, 0.3f, 0.5f, 1.f), 0.2f);

    return color;
}
