
Texture2D<float4> src_ : register(t0);
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
    
    output.color = src_.Sample(sampler_, input.texcoord);
    
    float luminance = dot(output.color.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    
    clip(luminance - 1.0f);

    return output;
}
