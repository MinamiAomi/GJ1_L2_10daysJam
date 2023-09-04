#pragma once

#include <map>
#include <memory>

#include "RootSignature.h"
#include "PipelineState.h"
#include "ColorBuffer.h"
#include "UploadBuffer.h"

class CommandContext;

class GaussianBlur {
public:
    // ルートシグネチャの生成を行う
    static void StaticInitialize();

    void Initialize(ColorBuffer* originalTexture);
    void Render(CommandContext& commandContext, float blurPower);

    ColorBuffer& GetResult() { return verticalBlurTexture_; }

private:
    static const uint32_t kNumWeights = 8;

    struct PipelineSet {
        PipelineState horizontalBlurPSO;
        PipelineState verticalBlurPSO;
    };

    static RootSignature rootSignature_;
    static std::map<DXGI_FORMAT, std::unique_ptr<PipelineSet>> pipelineStateMap_;

    static void CreatePipelineState(DXGI_FORMAT format);

    void UpdateWeightTable(float blurPower);

    ColorBuffer* originalTexture_;
    ColorBuffer horizontalBlurTexture_;
    ColorBuffer verticalBlurTexture_;
    UploadBuffer constantBuffer_;
    float weights_[kNumWeights]{};
};