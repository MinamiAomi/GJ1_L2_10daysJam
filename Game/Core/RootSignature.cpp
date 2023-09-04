#include "RootSignature.h"

#include <cassert>

#include "Utility.h"
#include "Graphics.h"

void RootSignature::Create(const D3D12_ROOT_SIGNATURE_DESC& desc) {
    auto device = Graphics::Get().GetDevice();


    Microsoft::WRL::ComPtr<ID3DBlob> blob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    if (FAILED(D3D12SerializeRootSignature(
        &desc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        blob.GetAddressOf(),
        errorBlob.GetAddressOf()))) {
        OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }

    ASSERT_IF_FAILED(device->CreateRootSignature(
        0,
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        IID_PPV_ARGS(rootSignature_.GetAddressOf())));
}
