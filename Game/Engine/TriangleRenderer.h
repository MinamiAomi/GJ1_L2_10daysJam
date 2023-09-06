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
    static const uint32_t kMaxNumTriangles = 1024;

    static TriangleRenderer* GetInstance();

    void Initialize(DXGI_FORMAT rtvFormat);

    void Draw(CommandContext& commandContext, const Vector3& p0, uint32_t c0, const Vector3& p1, uint32_t c1, const Vector3& p2, uint32_t c2);
    void Draw(CommandContext& commandContext, const Vector3& p0, const Vector3& p1, const Vector3& p2, uint32_t c) {
        Draw(commandContext, p0, c, p1, c, p2, c);
    }
    void Draw(CommandContext& commandContext, const Vector3* vertices, uint32_t numTriangles, uint32_t);
    void Draw(CommandContext& commandContext, const std::vector<Vector3>& vertices, uint32_t c);
    void Reset();

    void SetBlendMode(BlendMode mode) { blendMode_ = mode; }

private:
    TriangleRenderer() = default;
    TriangleRenderer(const TriangleRenderer&) = delete;
    TriangleRenderer& operator=(const TriangleRenderer&) = delete;

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