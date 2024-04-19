struct VertexShaderOutput
{
    float4 position : SV_POSITIONT;
};

struct VertexShaderInput
{
    float4 position : POSITIONT;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = input.position;
    return output;
}