#include "ShadowBuffer.h"

#include "Externals/DirectXTex/d3dx12.h"

#include <cassert>

#include "Graphics.h"

void ShadowBuffer::Create(uint32_t width, uint32_t height, DXGI_FORMAT format) {
    Create(width, height, 1, format);
}

void ShadowBuffer::Create(uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format) {
    arraySize_ = arraySize;
    auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, UINT16(arraySize), 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
    auto clearValue = CD3DX12_CLEAR_VALUE(format, 1.0f, 0);
    CreateResource(resourceDesc, &clearValue);
}

void ShadowBuffer::CreateDepthStencilView(const DXGI_FORMAT* dsvFormat) {
    assert(resource_);

    if (arraySize_ == 1) {
        DepthBuffer::CreateDepthStencilView(dsvFormat);
        return;
    }

    auto& graphics = Graphics::Get();
    auto device = graphics.GetDevice();

    dsvFormat_ = (dsvFormat) ? *dsvFormat : format_;
    assert(IsValidDSVFormat(dsvFormat_));


    if (dsv_.IsNull()) {
        dsv_ = graphics.AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    }
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = dsvFormat_;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
    dsvDesc.Texture2DArray.ArraySize = arraySize_;
    device->CreateDepthStencilView(resource_.Get(), &dsvDesc, dsv_.CPU());
}

void ShadowBuffer::CreateShaderResourceView(const DXGI_FORMAT* srvFormat) {
    assert(resource_);

    if (arraySize_ == 1) {
        DepthBuffer::CreateShaderResourceView(srvFormat);
        return;
    }

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
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.ArraySize = arraySize_;
    srvDesc.Texture2DArray.MipLevels = 1;
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srv_.CPU());
}
