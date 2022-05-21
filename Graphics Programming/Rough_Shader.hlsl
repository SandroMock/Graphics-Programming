Texture2D ObjTexture;
SamplerState ObjSamplerState;
Texture2D ObjNormalMap;
SamplerState ObjNormalSampler;

struct Light
{
    float3 direction;
    float4 ambient;
    float4 diffuse;
    float intensity;
};

cbuffer cbPerFrame
{
    Light light;
};

cbuffer cbPerObject
{
    float4x4 WVP;
    float4x4 World;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
    float3 normal : NORMAL;
};

VS_OUTPUT VS(float4 inPos : SV_POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL)
{
    VS_OUTPUT output;
    
    output.Pos = mul(inPos, WVP);
    
    output.UV = inTexCoord * float2(1, 1) + float2(0, 0);
    output.normal = mul(normal, World);

    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 tex = ObjTexture.Sample(ObjSamplerState, input.UV);
    float4 normalSample = ObjNormalMap.Sample(ObjSamplerState, input.UV);
    
    float3 normal = -normalize(normalSample * 2 - 1);
    
    // ambient light from directional light
    float3 ambientLight = light.ambient.rgb;
    
    float3 finalColor = tex.rgb * ambientLight;
    
    float3 lightDir = normalize(-light.direction.xyz);
    finalColor += saturate(dot(-lightDir, normal) * light.diffuse * tex);
    
    return float4(finalColor, tex.a);
}


	

