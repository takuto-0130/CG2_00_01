#include "Object3d.hlsli"

struct TransfomationMatrix
{
    float4x4 WVP;
};
ConstantBuffer<TransfomationMatrix> gTransfomationMatrix : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texCoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransfomationMatrix.WVP);
    output.texCoord = input.texCoord;
    return output;
}