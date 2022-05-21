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
    float3 camPos;
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
    float2 UV : TEXCOORD0;
    float3 normal : NORMAL;
};


VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD0, float3 normal : NORMAL)
{
    VS_OUTPUT output;
    
    normal = normalize(normal);

    output.Pos = mul(inPos, WVP);

    output.UV = inTexCoord;
    
    output.normal = normalize(mul(float4(normal, 0), World).xyz);
    

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    
    float3 normalDir = normalize(input.normal);
    float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.UV);
    
    float3 viewDir = normalize(camPos);
    float attenuation = 1.2;
    float3 lightDir = normalize(light.direction);
    float3 ambient = light.ambient * diffuse.rgba;
    float3 diffRefl = attenuation * light.diffuse * max(0, dot(normalDir, lightDir)) * diffuse.a;
    float4 specColor = float4(0, 0, 0, 0);

    float3 specRefl = attenuation * light.diffuse.a * specColor.rgb * 
        pow(max(0, dot(reflect(-lightDir, normalDir), viewDir)), 1);
               
    return float4(ambient + diffRefl + specRefl, 1);
}
