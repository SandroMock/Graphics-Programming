struct Light
{
    float3 direction;
    float4 ambient;
    float4 diffuse;
};

cbuffer cbPerObject
{
    float4x4 WVP;
    float4x4 World;
};

cbuffer cbPerFrame
{
    Light light;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 normal : NORMAL;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
    VS_OUTPUT output;

    output.Pos = mul(inPos, WVP);

    output.normal = mul(normal, World);

    output.UV = inTexCoord;

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    input.normal = normalize(input.normal);

    float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.UV);

    float3 finalColor;

    finalColor = diffuse * light.ambient;
    finalColor += saturate(dot(light.direction, input.normal) * light.diffuse * diffuse);
    
    return float4(finalColor, diffuse.a);
}