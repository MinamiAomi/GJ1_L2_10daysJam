#pragma once

#include <memory>

#include "Graphics/RootSignature.h"
#include "Graphics/PipelineState.h"
#include "Graphics/UploadBuffer.h"
#include "Graphics/Texture.h"

#include "Math/MathUtils.h"

class CommandContext;

class ArcadeMachine {
public:
    void Initialize(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
    void Update();
    void Draw(CommandContext& commandContext, const Matrix4x4& camera);

private:
    struct Material {
        Vector4 color;
        std::unique_ptr<Texture> texture;
    };
    struct Mesh {
        uint32_t vertexOffset;
        uint32_t indexOffset;
        uint32_t indexCount;
    };
    
    RootSignature rootSignature_;
    PipelineState pipelineState_;

    UploadBuffer vertexBuffer_;
    UploadBuffer indexBuffer_;
};