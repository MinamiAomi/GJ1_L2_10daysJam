
#define MAX_LEVEL 5

Texture2D<float4> blurTexture_[MAX_LEVEL] : register(t0);
SamplerState sampler_ : register(s0);

struct Param {
    uint level;
};
ConstantBuffer<Param> param_ : register(b0);

struct PSInput {
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

PSOutput main(PSInput input) {
    PSOutput output = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    for (uint i = 0; i < param_.level; ++i) {
        output.color += blurTexture_[i].Sample(sampler_, input.texcoord);
    }
    
    output.color /= param_.level;
    output.color.a = 1.0f;
    
    return output;
}