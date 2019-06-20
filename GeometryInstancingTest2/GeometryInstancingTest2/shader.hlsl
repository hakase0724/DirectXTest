cbuffer global
{
    matrix gWVP[10];
};

struct PerInstanceData
{
    matrix instanceMat;
};

StructuredBuffer<PerInstanceData> g_pInstanceData : register(t0);

struct VS_IN
{
    float4 pos : POSITION;
    matrix instancePos : MATRIX;
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
};

VS_OUT VS(VS_IN input)  
{
    VS_OUT output;
    output.pos = mul(input.pos, g_pInstanceData[input.instanceID].instanceMat);
    return output;
};

float4 PS(VS_OUT input) : SV_Target
{
    return float4(1, 1, 1, 1);
};