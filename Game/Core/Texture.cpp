#include "Texture.h"

#include "Externals/DirectXTex/DirectXTex.h"
#include "Externals/DirectXTex/d3dx12.h"

#include "Utility.h"
#include "Graphics.h"
#include "CommandList.h"
#include "CommandQueue.h"

namespace {

    std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, D3D12_RESOURCE_DESC> CreateTexture(const DirectX::ScratchImage& image) {
        std::pair<Microsoft::WRL::ComPtr<ID3D12Resource>, D3D12_RESOURCE_DESC> texture;

        auto& graphics = Graphics::Get();
        auto device = graphics.GetDevice();
        
        DirectX::ScratchImage mipImages{};
        ASSERT_IF_FAILED(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages));

        auto& metadata = mipImages.GetMetadata();
        D3D12_RESOURCE_DESC resourceDesc{};
        resourceDesc.Width = UINT(metadata.width);
        resourceDesc.Height = UINT(metadata.height);
        resourceDesc.MipLevels = UINT16(metadata.mipLevels);
        resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
        resourceDesc.Format = metadata.format;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

        D3D12_HEAP_PROPERTIES heapPropeteies = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        ASSERT_IF_FAILED(device->CreateCommittedResource(
            &heapPropeteies,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(texture.first.ReleaseAndGetAddressOf())));

        texture.second = resourceDesc;

        auto& commandQueue = graphics.GetCommandQueue();
        CommandList commandList;
        commandList.Create();
        commandList.Reset();

        std::vector<D3D12_SUBRESOURCE_DATA> subresources;
        DirectX::PrepareUpload(device.Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
        uint64_t intermediateSize = GetRequiredIntermediateSize(texture.first.Get(), 0, UINT(subresources.size()));

        D3D12_RESOURCE_DESC intermediateResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(intermediateSize);
        heapPropeteies = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
        ASSERT_IF_FAILED(device->CreateCommittedResource(
            &heapPropeteies,
            D3D12_HEAP_FLAG_NONE,
            &intermediateResourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(intermediateResource.GetAddressOf())));

        UpdateSubresources(commandList.GetCommandList().Get(), texture.first.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
        commandList.TransitionBarrier(texture.first.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
        commandList.CommitResourceBarrier();
        commandList.Close();
        commandQueue.ExcuteCommandList(commandList);
        commandQueue.Signal();
        commandQueue.WaitForGPU();

        return texture;
    }

}

void Texture::CreateFromWICFile(const std::filesystem::path& path) {

    auto device = Graphics::Get().GetDevice();

    auto wname = path.wstring();

    DirectX::ScratchImage image{};
    ASSERT_IF_FAILED(DirectX::LoadFromWICFile(wname.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image));
    auto texture = CreateTexture(image);
    resource_ = texture.first;
    desc_ = texture.second;
    currentState_ = D3D12_RESOURCE_STATE_GENERIC_READ;
    CreateShaderResourceView();
}

void Texture::CreateShaderResourceView() {
    auto& graphics = Graphics::Get();
    auto device = graphics.GetDevice();

    if (srv_.IsNull()) {
        srv_ = graphics.AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
    desc.Format = desc_.Format;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipLevels = desc_.MipLevels;
    device->CreateShaderResourceView(resource_.Get(), &desc, srv_.CPU());
}
