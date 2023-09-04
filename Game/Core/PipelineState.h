#pragma once

#include <d3d12.h>
#include <wrl/client.h>

class PipelineState {
public:
    void Create(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);

    Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPipelineState() const { return pipelineState_; }

private:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
};