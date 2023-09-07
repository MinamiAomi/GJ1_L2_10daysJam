#pragma once

#include <vector>

#include "Graphics/UploadBuffer.h"
#include "Graphics/RootSignature.h"
#include "Graphics/PipelineState.h"
#include "Graphics/CommandContext.h"

#include "Math/MathUtils.h"

class TriangleRenderer {
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
    };
    
    static const uint32_t kMaxNumTriangles = 1024;

    static TriangleRenderer* GetInstance();

    void Initialize(DXGI_FORMAT rtvFormat);
    void Draw(CommandContext& commandContext, const Vertex* vertices, uint32_t numTriangles);

    void Reset();

    void SetBlendMode(BlendMode mode) { blendMode_ = mode; }

private:
    TriangleRenderer() = default;
    TriangleRenderer(const TriangleRenderer&) = delete;
    TriangleRenderer& operator=(const TriangleRenderer&) = delete;


    RootSignature rootSignature_;
    PipelineState pipelineStates_[kNumBlendModes];

    UploadBuffer vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vbv_{};
    uint32_t triangleCount_ = 0;
    BlendMode blendMode_ = kBlendNormal;

};