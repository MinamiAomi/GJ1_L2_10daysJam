#include "GaussianBlur.h"

#include <cassert>
#include <cmath>

#include "Helper.h"
#include "ShaderManager.h"
#include "CommandContext.h"

RootSignature GaussianBlur::rootSignature_;
std::map<DXGI_FORMAT, std::unique_ptr<GaussianBlur::PipelineSet>> GaussianBlur::pipelineStateMap_;


void GaussianBlur::StaticInitialize() {
    assert(!rootSignature_);

    {
        CD3DX12_DESCRIPTOR_RANGE range{};
        range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER rootParameters[2]{};
        rootParameters[0].InitAsDescriptorTable(1, &range);
        rootParameters[1].InitAsConstantBufferView(0);

        CD3DX12_STATIC_SAMPLER_DESC staticSamplerDesc(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
        rootSignatureDesc.pParameters = rootParameters;
        rootSignatureDesc.NumParameters = _countof(rootParameters);
        rootSignatureDesc.pStaticSamplers = &staticSamplerDesc;
        rootSignatureDesc.NumStaticSamplers = 1;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_.Create(L"GaussianBlur RootSignature", rootSignatureDesc);
    }
}

void GaussianBlur::Initialize(ColorBuffer* originalTexture) {
    assert(originalTexture);

    originalTexture_ = originalTexture;
    horizontalBlurTexture_.Create(
        L"GaussianBlur HorizontalBlurTexture",
        originalTexture_->GetWidth() / 2,
        originalTexture_->GetHeight(),
        originalTexture_->GetFormat());
    verticalBlurTexture_.Create(
        L"GaussianBlur VerticalBlurTexture",
        originalTexture_->GetWidth() / 2,
        originalTexture_->GetHeight() / 2,
        originalTexture_->GetFormat());

    constantBuffer_.Create(L"GaussianBlur Constant", sizeof(weights_));

    CreatePipelineState(originalTexture->GetFormat());
}

void GaussianBlur::Render(CommandContext& commandContext, float blurPower) {
    UpdateWeightTable(blurPower);

    auto pipelineSet = pipelineStateMap_[originalTexture_->GetFormat()].get();

    commandContext.TransitionResource(*originalTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(horizontalBlurTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(horizontalBlurTexture_.GetRTV());
    commandContext.ClearColor(horizontalBlurTexture_);
    commandContext.SetViewportAndScissorRect(0, 0, horizontalBlurTexture_.GetWidth(), horizontalBlurTexture_.GetHeight());

    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineSet->horizontalBlurPSO);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(0, originalTexture_->GetSRV());
    commandContext.SetConstantBuffer(1, constantBuffer_.GetGPUVirtualAddress());
    commandContext.Draw(3);

    commandContext.TransitionResource(horizontalBlurTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.TransitionResource(verticalBlurTexture_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(verticalBlurTexture_.GetRTV());
    commandContext.ClearColor(verticalBlurTexture_);
    commandContext.SetViewportAndScissorRect(0, 0, verticalBlurTexture_.GetWidth(), verticalBlurTexture_.GetHeight());

    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineSet->verticalBlurPSO);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(0, horizontalBlurTexture_.GetSRV());
    commandContext.SetConstantBuffer(1, constantBuffer_.GetGPUVirtualAddress());
    commandContext.Draw(3);

    commandContext.TransitionResource(verticalBlurTexture_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void GaussianBlur::CreatePipelineState(DXGI_FORMAT format) {

    if (pipelineStateMap_.contains(format)) {
        auto psos = std::make_unique<PipelineSet>();

        auto shaderManager = ShaderManager::GetInstance();

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
        psoDesc.pRootSignature = rootSignature_;

        auto vs = shaderManager->Compile(L"Engine/Graphics/Shader/HorizontalGaussianBlurVS.hlsl", ShaderManager::kVertex);
        auto ps = shaderManager->Compile(L"Engine/Graphics/Shader/GaussianBlurPS.hlsl", ShaderManager::kPixel);
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
        psoDesc.BlendState = Helper::BlendDisable;
        psoDesc.RasterizerState = Helper::RasterizerDefault;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = format;
        psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.SampleDesc.Count = 1;

        psos->horizontalBlurPSO.Create(L"GaussianBlur HorizontalPSO", psoDesc);

        vs = shaderManager->Compile(L"Engine/Graphics/Shader/VerticalGaussianBlurVS.hlsl", ShaderManager::kVertex);
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        psos->verticalBlurPSO.Create(L"GaussianBlur VerticalPSO", psoDesc);

        pipelineStateMap_[format] = std::move(psos);
    }

}

void GaussianBlur::UpdateWeightTable(float blurPower) {
    float total = 0;
    for (uint32_t i = 0; i < kNumWeights; ++i) {
        weights_[i] = std::exp(-0.5f * float(i * i) / blurPower);
        total += 2.0f * weights_[i];
    }
    total = 1.0f / total;
    for (uint32_t i = 0; i < kNumWeights; ++i) {
        weights_[i] *= total;
    }
    constantBuffer_.Copy(weights_, sizeof(weights_));
}
