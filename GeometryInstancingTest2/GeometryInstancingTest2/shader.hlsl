cbuffer global
{
    matrix gWVP;
};

float4 VS(float4 pos : POSITION) : SV_POSITION
{
    return mul(pos, gWVP);
};

float4 PS() : SV_Target
{
    return float4(1, 1, 1, 1);
};