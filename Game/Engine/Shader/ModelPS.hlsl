struct Material {
    float4 color;
    uint useTexture;
};

ConstantBuffer<Material> material_ : register(b1);

Texture2D<float4> texture_ : register(t0);
SamplerState sampler_ : register(s0);

struct PSInput {
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};

struct PSOutput {
    float4 color : SV_TARGET0;
};

float ShadeColor(float3 normal, float3 direction) {
    float NdotL = dot(normalize(normal), -direction);
    return pow(NdotL * 0.5f + 0.5f, 2.0f);
}

PSOutput main(PSInput input) {
    PSOutput output;
    
    float4 textureColor = material_.color;
    if (material_.useTexture != 0) {
        textureColor = texture_.Sample(sampler_, input.texcoord);
    }
    
    output.color = textureColor;
    
    float3 direction = normalize(float3(0.0f, -0.1f, 1.0f));
    //output.color *= ShadeColor(input.normal, direction) * 1.0f;
    //direction = normalize(float3(0.0f, -0.1f, 1.0f));
    output.color *= ShadeColor(input.normal, direction);
   //output.color.xyz += float3(0.1f, 0.1f, 0.1f);
    
    return output;
}