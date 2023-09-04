#pragma once
#include "GPUResource.h"

#include <cstdint>
#include "Math/MathUtils.h"

class ShadowBuffer : public DepthBuffer {
public:
    void Create(uint32_t width, uint32_t height, DXGI_FORMAT format) override;
    void Create(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format);
    void CreateDepthStencilView(const DXGI_FORMAT* dsvFormat = nullptr) override;
    void CreateShaderResourceView(const DXGI_FORMAT* srvFormat = nullptr) override;

    uint32_t GetArraySize() { return arraySize_; }

private:
    uint32_t arraySize_ = 1;
};