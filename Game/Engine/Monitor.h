#pragma once

#include "Graphics/ColorBuffer.h"
#include "Graphics/RootSignature.h"
#include "Graphics/PipelineState.h"

#include "Math/MathUtils.h"

class CommandContext;

class Monitor {
public:

    void Initilaize(uint32_t width, uint32_t height, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
    void BeginRender(CommandContext& commandContext);
    void Draw(CommandContext& commandContext, const Matrix4x4& camera);

    ColorBuffer& GetColorBuffer() { return colorBuffer_; }

private:
    RootSignature rootSignature_;
    PipelineState pipelineState_;
    ColorBuffer colorBuffer_;
    UploadBuffer vertexBuffer_;
    UploadBuffer indexBuffer_;
};