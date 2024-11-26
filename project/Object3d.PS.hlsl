#include "Object3d.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransforam;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct Camera
{
    float3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b1);

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b2);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texCoord, 0.0f, 1.0f), gMaterial.uvTransforam);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    if (gMaterial.enableLighting == 2)
    {
        float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else if (gMaterial.enableLighting == 1)
    {
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}