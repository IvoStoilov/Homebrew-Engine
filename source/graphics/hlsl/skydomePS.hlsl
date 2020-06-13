cbuffer ColorBuffer
{
    float4 apexColor;
    float4 centerColor;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 domePosition : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float height;
    float4 outputColor;

    height = input.domePosition.y;

    if (height < 0.0)
    {
        height = 0.0f;
    }
    
    outputColor = lerp(centerColor, apexColor, height);
    
    return outputColor;
}