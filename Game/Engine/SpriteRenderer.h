#pragma once

#include "Graphics/UploadBuffer.h"
#include "Graphics/RootSignature.h"
#include "Graphics/PipelineState.h"
#include "Graphics/CommandContext.h"

#include "Math/MathUtils.h"

#include "TextureManager.h"

class SpriteRenderer {
public:
    enum BlendMode {
        kBlendDisable,
        kBlendNormal,
        kBlendAdditive,
        kBlendSubtract,

        kNumBlendModes
    };
    struct Vertex {
        Vector3 position;
        uint32_t color;
        Vector2 texcoord;
        float pad[2];
    };

    static SpriteRenderer* GetInstance();

    void Initialize(DXGI_FORMAT rtvFormat);

    void Draw(CommandContext& commandContext, const Vertex* vertices, uint32_t numTriangles, D3D12_GPU_DESCRIPTOR_HANDLE srv);

    void SetBlendMode(BlendMode mode) { blendMode_ = mode; }

private:
    SpriteRenderer() = default;
    SpriteRenderer(const SpriteRenderer&) = delete;
    SpriteRenderer& operator=(const SpriteRenderer&) = delete;


    RootSignature rootSignature_;
    PipelineState pipelineStates_[kNumBlendModes];

    BlendMode blendMode_ = kBlendNormal;

};