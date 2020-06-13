struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 result = abs(input.color);
    return result;
}
