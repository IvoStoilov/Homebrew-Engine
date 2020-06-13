cbuffer LightBuffer
{
    float4 diffuseColor;
    float4 lightDirection;
    float4 ambientLight;
};

Texture2D normalMap   : register(t0);
Texture2D hiteMap     : register(t1);
Texture2D snowAlbedo  : register(t2);
Texture2D rockAlbedo  : register(t3);
Texture2D grassAlbedo : register(t4);
Texture2D sandAlbedo  : register(t5);
Texture2D sandHeight  : register(t6);
Texture2D rockHeight  : register(t7);
Texture2D snowHeight  : register(t8);

SamplerState sampleType;

struct PixelInputType
{
    float4 m_FinalPosition  : SV_POSITION;
    float4 m_GlobalPosition : POSITION0;
    float2 m_UV             : TEXCOORD0;
    float3 m_VertexNormal   : NORMAL;
    float4 m_Tangent        : TANGENT;

    float m_Clip : SV_ClipDistance0;
};

float4 GetTriPlanarBlend(Texture2D textureToSample, float3 worldPos, float3 worldNormal)
{
    float texScale = 1.0;
    float3 blendWeights = abs(worldNormal);

    blendWeights = (blendWeights - 0.2) * 7;
    blendWeights = max(blendWeights, 0);
    blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z).xxx;

    float2 coord1 = worldPos.yz * texScale;
    float2 coord2 = worldPos.xz * texScale;
    float2 coord3 = worldPos.xy * texScale;

    float4 color1 = textureToSample.Sample(sampleType, coord1);
    float4 color2 = textureToSample.Sample(sampleType, coord2);
    float4 color3 = textureToSample.Sample(sampleType, coord3);

    return  color1.xyzw * blendWeights.xxxx +
            color2.xyzw * blendWeights.yyyy +
            color3.xyzw * blendWeights.zzzz;
}

float3 SampleNormal(in float2 uv, in float3 tan, in float3 biNorm, in float3 norm)
{
    float3 normalSample = normalMap.SampleLevel(sampleType, uv, 0).xzy;
    normalSample = (normalSample * 2.f) - 1;
    normalSample = (normalSample.x * tan) + (normalSample.y * norm) + (normalSample.z * biNorm);
    normalSample = normalize(normalSample);
    return normalSample;
}

inline float4 BlendMaterials(in float3 globalPos, in float3 globalNormal, 
                             in Texture2D albedo1, in float height1, 
                             in Texture2D albedo2, in float height2, float biasTowards2)
{
    return height1 > (height2 + biasTowards2) ? GetTriPlanarBlend(albedo1, globalPos, globalNormal) : GetTriPlanarBlend(albedo2, globalPos, globalNormal);
}

inline float4 BlendMaterials(in float3 globalPos, in float3 globalNormal, 
                             in Texture2D albedo1, in Texture2D height1, 
                             in Texture2D albedo2, in float height2, float biasTowards2)
{
    float h1 = GetTriPlanarBlend(height1, globalPos, globalNormal).x;
    return  BlendMaterials(globalPos, globalNormal, albedo1, h1, albedo2, height2, biasTowards2);
}

float4 BlendMaterials(in float3 globalPos, in float3 globalNormal, 
                      in Texture2D albedo1, in Texture2D height1, 
                      in Texture2D albedo2, in Texture2D height2, float biasTowards2)
{
    float h1 =  GetTriPlanarBlend(height1, globalPos, globalNormal).x;
    float h2 =  GetTriPlanarBlend(height2, globalPos, globalNormal).x;
    return BlendMaterials(globalPos, globalNormal, albedo1, h1, albedo2, h2, biasTowards2);
}

float4 SampleSandAlbedo(float3 worldPosition, float3 worldNormal)
{
    return GetTriPlanarBlend(sandAlbedo, worldPosition, worldNormal);
}

float4 SampleGrassAlbedo(float3 worldPosition, float3 worldNormal)
{
    return GetTriPlanarBlend(grassAlbedo, worldPosition, worldNormal);
}

float4 SampleRockAlbedo(float3 worldPosition, float3 worldNormal)
{
    return GetTriPlanarBlend(rockAlbedo, worldPosition, worldNormal);
}

float4 SampleSnowAlbedo(float3 worldPosition, float3 worldNormal)
{
    return GetTriPlanarBlend(snowAlbedo, worldPosition, worldNormal);
}

inline float4 ComputTextureColor(float4 globalPosition, float2 uv, float3 globalNormal)
{
    float slope = 1.f - globalNormal.y;
    float4 hiteMapWeights = hiteMap.SampleLevel(sampleType, uv, 0);

    if (slope >= 0.8f + hiteMapWeights.z)
    {
        return SampleRockAlbedo(globalPosition.xyz, globalNormal);
    }

    if (hiteMapWeights.x + hiteMapWeights.y + hiteMapWeights.z < 0.1f)
    {
        return SampleSandAlbedo(globalPosition.xyz, globalNormal);
    }

    if (hiteMapWeights.z == 0.f && hiteMapWeights.r > 0.1f)
    {
        return BlendMaterials(globalPosition.xyz, globalNormal, sandAlbedo, sandHeight, grassAlbedo, 0.4f, 0.f);
    }

    if (hiteMapWeights.z == 0.f && hiteMapWeights.g > 0.1f)
    {
        return BlendMaterials(globalPosition.xyz, globalNormal, snowAlbedo, snowHeight, grassAlbedo, 0.4f, -hiteMapWeights.g * 0.13f);
    }

    if (hiteMapWeights.r == 0.f)
    {
        return BlendMaterials(globalPosition.xyz, globalNormal, rockAlbedo, rockHeight, snowAlbedo, snowHeight, hiteMapWeights.z);
    }

    return float4(1.f, 1.f, 0.f, 1.f);

}

float4 main(PixelInputType input) : SV_TARGET
{
    input.m_VertexNormal = normalize(input.m_VertexNormal);
    input.m_Tangent = normalize(input.m_Tangent);

    //return float4(input.m_VertexNormal, 1.f);
    float3 biNormal = cross(input.m_VertexNormal, input.m_Tangent.xyz);
    float3 terrainNormal = SampleNormal(input.m_UV, input.m_Tangent.xyz, biNormal, input.m_VertexNormal);
    //return float4(terrainNormal, 1.f);
    float4 textureColor = ComputTextureColor(input.m_GlobalPosition, input.m_UV, terrainNormal);
    
    float3 lightDir = -lightDirection.xyz;
    lightDir = normalize(lightDir);
    float dotRes = dot(terrainNormal, lightDir);
    float lightIntensity = saturate(dotRes);
    
    float4 color = saturate(diffuseColor * lightIntensity);
    color += ambientLight;
    
    color = color * textureColor;
    return color;
}
