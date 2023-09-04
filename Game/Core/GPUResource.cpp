#include "GPUResource.h"

#include "Externals/DirectXTex/d3dx12.h"

#include <cassert>

#include "Graphics.h"
#include "Utility.h"
#include "CommandList.h"
#include "CommandQueue.h"

void PixelBuffer::CreateShaderResourceView(const DXGI_FORMAT* srvFormat) {
    assert(resource_);

    auto& graphics = Graphics::Get();
    auto device = graphics.GetDevice();

    auto resDesc = resource_->GetDesc();
    srvFormat_ = (srvFormat) ? *srvFormat : resDesc.Format;

    if (srv_.IsNull()) {
        srv_ = graphics.AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = srvFormat_;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = resDesc.MipLevels;
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srv_.CPU());
}

void PixelBuffer::Resize(uint32_t width, uint32_t height) {
    assert(resource_);

    auto resDesc = resource_->GetDesc();
    resDesc.Width = width;
    resDesc.Height = height;

    CreateResource(resDesc, clearValue_.get());
}

void PixelBuffer::CreateResource(const D3D12_RESOURCE_DESC& desc, const D3D12_CLEAR_VALUE* clearValue) {
    width_ = uint32_t(desc.Width);
    height_ = desc.Height;
    format_ = desc.Format;

    if (clearValue) {
        clearValue_ = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
    }
    else {
        clearValue_ = nullptr;
    }

    D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    auto device = Graphics::Get().GetDevice();
    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COMMON,
        clearValue_.get(),
        IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())));
}

void ColorBuffer::Create(uint32_t width, uint32_t height, DXGI_FORMAT format, const float* clearColor) {
    auto device = Graphics::Get().GetDevice();

    auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

    D3D12_CLEAR_VALUE* pClearValue = nullptr;
    D3D12_CLEAR_VALUE clearValue{};
    if (clearColor) {
        clearValue = CD3DX12_CLEAR_VALUE(format, clearColor);
        pClearValue = &clearValue;
    }
    CreateResource(resDesc, pClearValue);
}

void ColorBuffer::Create(Microsoft::WRL::ComPtr<ID3D12Resource> resource) {
    assert(resource);

    resource_ = resource;

    auto resDesc = resource_->GetDesc();
    width_ = uint32_t(resDesc.Width);
    height_ = resDesc.Height;
    format_ = resDesc.Format;
}

void ColorBuffer::CreateRenderTargetView(const DXGI_FORMAT* rtvFormat) {
    assert(resource_);

    auto& graphics = Graphics::Get();
    auto device = graphics.GetDevice();

    rtvFormat_ = (rtvFormat) ? *rtvFormat : format_;

    if (rtv_.IsNull()) {
        rtv_ = graphics.AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    rtvDesc.Format = rtvFormat_;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    device->CreateRenderTargetView(resource_.Get(), &rtvDesc, rtv_.CPU());
}

void ColorBuffer::SetClearColor(const float* clearColor) {
    if (clearColor) {
        if (!clearValue_) {
            D3D12_CLEAR_VALUE v{};
            v.Format = format_;
            clearValue_ = std::make_unique<D3D12_CLEAR_VALUE>(v);
        }
        memcpy(clearValue_->Color, clearColor, sizeof(clearValue_->Color));
    }
    else {
        clearValue_ = nullptr;
    }
}

bool DepthBuffer::IsValidDSVFormat(DXGI_FORMAT format) {
    switch (format) {
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_UNKNOWN:
        return true;
    }
    return false;
}

void DepthBuffer::Create(uint32_t width, uint32_t height, DXGI_FORMAT format) {
    auto device = Graphics::Get().GetDevice();

    auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
    auto clearValue = CD3DX12_CLEAR_VALUE(format, 1.0f, 0);
    CreateResource(resDesc, &clearValue);
}

void DepthBuffer::CreateDepthStencilView(const DXGI_FORMAT* dsvFormat) {
    assert(resource_);

    auto& graphics = Graphics::Get();
    auto device = graphics.GetDevice();

    dsvFormat_ = (dsvFormat) ? *dsvFormat : format_;
    assert(IsValidDSVFormat(dsvFormat_));


    if (dsv_.IsNull()) {
        dsv_ = graphics.AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    }
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = dsvFormat_;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsv_.CPU());
}

UploadBuffer::~UploadBuffer() {
    if (cpuData_ && resource_) {
        resource_->Unmap(0, nullptr);
        cpuData_ = nullptr;
    }
}

void UploadBuffer::Create(size_t size) {
    auto device = Graphics::Get().GetDevice();

    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(size));
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())));

    currentState_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    bufferSize_ = size;
    ASSERT_IF_FAILED(resource_->Map(0, nullptr, &cpuData_));
}

void UploadBuffer::Copy(const void* srcData, size_t size) const {
    assert(size <= bufferSize_);
    memcpy(cpuData_, srcData, size);
}

void GPUBuffer::Create(size_t size) {
    auto device = Graphics::Get().GetDevice();

    auto desc = CD3DX12_RESOURCE_DESC::Buffer(UINT64(size));
    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    ASSERT_IF_FAILED(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf())));

    currentState_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    bufferSize_ = size;
}

void GPUBuffer::Upload(UploadBuffer& uploadBuffer) {
    auto& commandQueue = Graphics::Get().GetCommandQueue();
    CommandList commandList;
    commandList.Create();
    commandList.Reset();

    commandList.TransitionBarrier(*this, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList.CommitResourceBarrier();
    commandList.CopyResource(*this, uploadBuffer);
    commandList.TransitionBarrier(*this, D3D12_RESOURCE_STATE_GENERIC_READ);
    commandList.CommitResourceBarrier();
    commandList.Close();
    commandQueue.ExcuteCommandList(commandList);
    commandQueue.Signal();
    commandQueue.WaitForGPU();
}
