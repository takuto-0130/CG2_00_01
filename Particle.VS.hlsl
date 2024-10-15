#include "Particle.hlsli"

struct TransfomationMatrix
{
    float4x4 WVP;
    float4x4 World;
};

StructuredBuffer<TransfomationMatrix> gTransfomationMatrices : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransfomationMatrices[instanceId].WVP);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(input.normal, (float3x3) gTransfomationMatrices[instanceId].World));
    return output;
}