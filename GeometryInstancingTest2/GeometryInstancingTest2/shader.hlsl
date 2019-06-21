struct PerInstanceData
{
    matrix instanceMat;
    float4 color;
};

StructuredBuffer<PerInstanceData> g_pInstanceData : register(t0);

struct VS_IN
{
    float4 pos : POSITION;
    uint instanceID : SV_InstanceID;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    uint id : ID;
};

VS_OUT VS(VS_IN input)  
{
    VS_OUT output;
    output.pos = mul(input.pos, g_pInstanceData[input.instanceID].instanceMat);
    output.id = input.instanceID;
    return output;
};

float4 PS(VS_OUT input) : SV_Target
{
    return g_pInstanceData[input.id].color;
};