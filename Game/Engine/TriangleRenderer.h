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
        uint32_t color{};
    };

    static TriangleRenderer* GetInstance();

    void Initialize(DXGI_FORMAT rtvFormat);
    void Draw(CommandContext& commandContext, const Vertex* vertices, uint32_t numTriangles);

    void SetBlendMode(BlendMode mode) { blendMode_ = mode; }

private:
    TriangleRenderer() = default;
    TriangleRenderer(const TriangleRenderer&) = delete;
    TriangleRenderer& operator=(const TriangleRenderer&) = delete;


    RootSignature rootSignature_;
    PipelineState pipelineStates_[kNumBlendModes];

    BlendMode blendMode_ = kBlendNormal;

};