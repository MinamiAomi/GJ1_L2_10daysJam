
#define NUM_TEXTURES 4

Texture2D<float4> blurTexture0_ : register(t0);
Texture2D<float4> blurTexture1_ : register(t1);
Texture2D<float4> blurTexture2_ : register(t2);
Texture2D<float4> blurTexture3_ : register(t3);
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
    PSOutput output;
    output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    output.color += blurTexture0_.Sample(sampler_, input.texcoord);
    output.color += blurTexture1_.Sample(sampler_, input.texcoord);
    output.color += blurTexture2_.Sample(sampler_, input.texcoord);
    output.color += blurTexture3_.Sample(sampler_, input.texcoord);
    
    output.color /= NUM_TEXTURES;
    output.color.a = 1.0f;
    
    return output;
}