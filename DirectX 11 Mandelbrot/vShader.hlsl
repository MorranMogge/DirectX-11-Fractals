cbuffer ConstantBuffer : register(b0)
{
    matrix ViewProjMatrix;
}

struct VsIn
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct VsOut
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

VsOut main(VsIn input)
{
    VsOut output;
    
    output.pos = float4(input.pos, 1.0); 
    output.pos = mul(output.pos, ViewProjMatrix);
    output.uv = input.uv;
    output.normal = input.normal;
    
    return output;
}