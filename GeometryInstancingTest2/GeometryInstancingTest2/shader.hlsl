cbuffer global
{
    matrix gWVP[10];
};

struct VS_IN
{
    float4 pos : POSITION;
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    uint id : COLOR;
};

VS_OUT VS(VS_IN input)  
{
    VS_OUT output;
    output.pos = mul(input.pos, gWVP[input.instanceID]);
    output.id = input.instanceID;
    return output;
};

float4 PS(VS_OUT input) : SV_Target
{
    float color = (input.id + 1) * 0.5f;
    return float4(color, color, color, 1);
};