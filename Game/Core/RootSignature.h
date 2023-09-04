#pragma once

#include <d3d12.h>
#include <wrl/client.h>

class RootSignature {
public:
    void Create(const D3D12_ROOT_SIGNATURE_DESC& desc);

    Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() const { return rootSignature_; }

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
};