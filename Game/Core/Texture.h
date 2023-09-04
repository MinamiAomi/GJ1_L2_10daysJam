#pragma once
#include "GPUResource.h"

#include <d3d12.h>
#include <wrl/client.h>

#include <filesystem>

#include "DescriptorHandle.h"

class Texture : public GPUResource {
public:
    void CreateFromWICFile(const std::filesystem::path& path);

    const D3D12_RESOURCE_DESC& GetDesc() const { return desc_; }
    const DescriptorHandle& GetSRV() const { return srv_; }

private:
    void CreateShaderResourceView();

    D3D12_RESOURCE_DESC desc_{};
    DescriptorHandle srv_;
};