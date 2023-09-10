#pragma once

#include "Graphics/ColorBuffer.h"
#include "Graphics/RootSignature.h"
#include "Graphics/PipelineState.h"
#include "Graphics/UploadBuffer.h"

#include "Math/MathUtils.h"

class CommandContext;

class Monitor {
public:
    static Monitor* GetInstance();

    void Initilaize(uint32_t bufferWidth, uint32_t bufferHeight, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
    void BeginRender(CommandContext& commandContext);
    void Draw(CommandContext& commandContext, const Matrix4x4& world, const Matrix4x4& camera);

    ColorBuffer& GetColorBuffer() { return colorBuffer_; }

private:
    struct Vertex {
        Vector3 position;
        Vector2 texcoord;
    };

    RootSignature rootSignature_;
    PipelineState pipelineState_;
    ColorBuffer colorBuffer_;
    UploadBuffer vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vbView_{};
    uint32_t vertexCount_ = 0;
};