#pragma once

#include "GaussianBlur.h"

class CommandContext;

class Bloom {
public:
    static const uint32_t kMaxLevel = 4;
    
    void Initialize(ColorBuffer* originalTexture);
    void Render(CommandContext& commandContext, uint32_t level = kMaxLevel);

    ColorBuffer& GetResult() { return *originalTexture_; }

private:
    RootSignature rootSignature_;
    PipelineState luminacePipelineState_;
    PipelineState additivePipelineState_;

    ColorBuffer* originalTexture_;
    ColorBuffer luminanceTexture_;
    GaussianBlur gaussianBlurs_[kMaxLevel];
};