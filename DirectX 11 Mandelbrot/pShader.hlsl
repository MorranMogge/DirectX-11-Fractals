Texture2D<float4> _texture : register(t0);
SamplerState _sampler : register(s0);

struct VsOut
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

float4 main(VsOut input) : SV_Target
{
    return _texture.Sample(_sampler, input.uv);
}