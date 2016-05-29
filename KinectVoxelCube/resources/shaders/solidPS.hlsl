cbuffer cbSurfaceColor : register(b0)
{
    float4 surfaceColor;
}

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : TEXCOORD0;
};

float4 main(PSInput i) : SV_TARGET
{
    float light = abs(dot(normalize(i.normal), normalize(float3(1,0,-1))));
    return float4(light, light, light, 1);
}