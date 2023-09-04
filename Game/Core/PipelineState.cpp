#include "PipelineState.h"

#include "Utility.h"
#include "Graphics.h"

void PipelineState::Create(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc) {
    auto device = Graphics::Get().GetDevice();
    ASSERT_IF_FAILED(device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pipelineState_.ReleaseAndGetAddressOf())));
}
