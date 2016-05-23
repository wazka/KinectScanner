cbuffer cbWorld : register(b0)
{
    matrix worldMatrix;
};

cbuffer cbView : register(b1)
{
    matrix viewMatrix;
};

cbuffer cbProj : register(b2)
{
    matrix projMatrix;
};

struct VSInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : TEXCOORD0;
    float depth : TEXCOORD1;
};

PSInput main(VSInput i)
{
    PSInput o = (PSInput)0;
    matrix worldView = mul(viewMatrix, worldMatrix);
    float4 viewPos = float4(i.pos, 1.0f);
    viewPos = mul(worldView, viewPos);
    o.pos = mul(projMatrix, viewPos);
    o.normal = mul(worldView, i.normal);
    return o;
}