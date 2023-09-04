#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>

#include "DescriptorHeap.h"

class GPUResource {
    friend class CommandList;
public:
    GPUResource() = default;
    GPUResource(Microsoft::WRL::ComPtr<ID3D12Resource> resource, D3D12_RESOURCE_STATES state) :
        resource_(resource), currentState_(state) {}

    Microsoft::WRL::ComPtr<ID3D12Resource> Get() const { return resource_; }
    D3D12_RESOURCE_STATES GetCurrentState() const { return currentState_; }

protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_COMMON;
};

class PixelBuffer : public GPUResource {
public:
    uint32_t GetWidth() const { return width_; }
    uint32_t GetHeight() const { return height_; }
    DXGI_FORMAT GetFormat() const { return format_; }

    DescriptorHandle GetShaderResourceView() const { return srv_; }
    DXGI_FORMAT GetSRVFormat() const { return srvFormat_; }

    virtual void CreateShaderResourceView(const DXGI_FORMAT* srvFormat = nullptr);

    void Resize(uint32_t width, uint32_t height);

    D3D12_CLEAR_VALUE* GetClearValue() const { return clearValue_.get(); }

protected:
    void CreateResource(const D3D12_RESOURCE_DESC& desc, const D3D12_CLEAR_VALUE* clearValue);

    uint32_t width_ = 0;
    uint32_t height_ = 0;
    DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;

    std::unique_ptr<D3D12_CLEAR_VALUE> clearValue_;

    DescriptorHandle srv_;
    DXGI_FORMAT srvFormat_ = DXGI_FORMAT_UNKNOWN;
};

class ColorBuffer : public PixelBuffer {
public:
    void Create(uint32_t width, uint32_t height, DXGI_FORMAT format, const float* clearColor = nullptr);
    void Create(Microsoft::WRL::ComPtr<ID3D12Resource> resource);
    void CreateRenderTargetView(const DXGI_FORMAT* rtvFormat = nullptr);

    void SetClearColor(const float* clearColor);

    DescriptorHandle GetRenderTargetView() const { return rtv_; }
    DXGI_FORMAT GetRTVFormat() const { return rtvFormat_; }

protected:
    DescriptorHandle rtv_;
    DXGI_FORMAT rtvFormat_ = DXGI_FORMAT_UNKNOWN;
};

class DepthBuffer : public PixelBuffer {
public:
    static bool IsValidDSVFormat(DXGI_FORMAT format);

    virtual void Create(uint32_t width, uint32_t height, DXGI_FORMAT format);
    virtual void CreateDepthStencilView(const DXGI_FORMAT* dsvFormat = nullptr);

    DescriptorHandle GetDepthStencilView() const { return dsv_; }
    DXGI_FORMAT GetDSVFormat() const { return dsvFormat_; }

protected:
    DescriptorHandle dsv_;
    DXGI_FORMAT dsvFormat_ = DXGI_FORMAT_UNKNOWN;
};

class UploadBuffer : public GPUResource {
public:
    ~UploadBuffer();

    void Create(size_t size);

    void Copy(const void* srcData, size_t size) const;
    template<class T>
    void Copy(const T& srcData) const { Copy(&srcData, sizeof(srcData)); }

    size_t GetBufferSize() const { return bufferSize_; }
    void* GetCPUData() const { return cpuData_; }

private:
    size_t bufferSize_ = 0;
    void* cpuData_ = nullptr;
};

class GPUBuffer : public GPUResource {
public:
    void Create(size_t size);
    void Upload(UploadBuffer& uploadBuffer);

    size_t GetBufferSize() const { return bufferSize_; }

private:
    size_t bufferSize_ = 0;
};