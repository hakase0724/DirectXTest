cbuffer buffer
{
    matrix gWVP;
};

struct VS_IN
{
    float4 pos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    output.pos = mul(input.pos,gWVP);
    return output;
};

float4 PS(VS_OUT input) : SV_Target
{
    return float4(1, 1, 1, 1);
};