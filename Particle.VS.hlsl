#include "Particle.hlsli"

struct ParticleForGPU
{
    float4x4 WVP;
    float4x4 World;
    float4 color;
};

StructuredBuffer<ParticleForGPU> gParticles : register(t0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gParticles[instanceId].WVP);
    output.texCoord = input.texCoord;
    output.normal = normalize(mul(input.normal, (float3x3) gParticles[instanceId].World));
    output.color = gParticles[instanceId].color;
    return output;
}