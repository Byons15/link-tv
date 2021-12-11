Texture2D tx : register(t0);
SamplerState samLinear : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 tex0 : SV_Target0;
    float4 tex1 : SV_Target1;
};

PS_OUTPUT CanvasPSMain(PS_INPUT frag) : SV_Target
{
    PS_OUTPUT output;
    output.tex0 = tx.Sample(samLinear, frag.Tex);
    output.tex1 = output.tex0;
    return output;
}