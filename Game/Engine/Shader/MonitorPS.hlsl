Texture2D<float4> texture_ : register(t0);
SamplerState sampler_ : register(s0);

struct PSInput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

PSOutput main(PSInput input) {
    PSOutput output;

    float2 texcoord = input.texcoord;
    texcoord -= 0.5f;
    float power = pow(length(texcoord), 0.2f);
    texcoord *= power;
    texcoord *= 1.25f;
    texcoord += 0.5f;

    output.color = texture_.Sample(sampler_, texcoord);

    return output;
}
