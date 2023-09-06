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
    static const uint32_t kMaxNumSprites = 1024;

    static SpriteRenderer* GetInstance();

    void Initialize(DXGI_FORMAT rtvFormat);

    void Reset();


private:
    SpriteRenderer() = default;
    SpriteRenderer(const SpriteRenderer&) = delete;
    SpriteRenderer& operator=(const SpriteRenderer&) = delete;

    struct Vertex {
        Vector3 position;
        uint32_t color;
    };

    RootSignature rootSignature_;
    PipelineState pipelineStates_[kNumBlendModes];

    UploadBuffer vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vbv_{};
    uint32_t triangleCount_ = 0;
    BlendMode blendMode_ = kBlendNormal;

};