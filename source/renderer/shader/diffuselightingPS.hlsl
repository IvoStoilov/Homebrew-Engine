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
    float4 globalPosition : POSITION0;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 GetTriPlanarBlend(float3 worldPos, float3 worldNormal)
{
    float texScale = 1.0;
    float3 blendWeights = abs(worldNormal);

    blendWeights = (blendWeights - 0.2) * 7;
    blendWeights = max(blendWeights, 0);

    blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z).xxx;

    float2 coord1 = worldPos.yz * texScale;
    float2 coord2 = worldPos.xz * texScale;
    float2 coord3 = worldPos.xy * texScale;

    float4 color1 = shaderTexture.Sample(sampleType, coord1);
    float4 color2 = shaderTexture.Sample(sampleType, coord2);
    float4 color3 = shaderTexture.Sample(sampleType, coord3);

    return  color1.xyzw * blendWeights.xxxx +
        color2.xyzw * blendWeights.yyyy +
        color3.xyzw * blendWeights.zzzz;
}

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float4 lightDir;
    float lightIntensity;
    float4 color;

    float3 terrainNormal = normalize(input.normal);

    if (hasTexture.x > 0)
    {
        textureColor = GetTriPlanarBlend(input.globalPosition.xyz, input.normal.xyz);
        //textureColor = shaderTexture.Sample(sampleType, input.globalPosition.xz);
    }
    else
        textureColor = (1.f, 1.f, 1.f, 1.f);

    //Invert for calculations
    lightDir = -lightDirection;
    lightDir = normalize(lightDir);
    lightIntensity = saturate(dot(terrainNormal, lightDir.xyz));

    color = saturate(diffuseColor * lightIntensity);
    color = color * textureColor;
    //color = textureColor;

    return color;
}
