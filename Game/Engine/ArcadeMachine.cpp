#include "ArcadeMachine.h"

#include "Graphics/Helper.h"
#include "Graphics/ShaderManager.h"

void ArcadeMachine::Initialize(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {
    CreatePipeline(rtvFormat, dsvFormat);
}

void ArcadeMachine::Update()
{
}

void ArcadeMachine::Draw(CommandContext& commandContext, const Matrix4x4& camera) {
    commandContext;
    camera;
}

void ArcadeMachine::CreatePipeline(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {

    {
        CD3DX12_DESCRIPTOR_RANGE ranges[1]{};
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER rootParameters[2]{};
        rootParameters[0].InitAsDescriptorTable(_countof(ranges), ranges);
        rootParameters[1].InitAsConstantBufferView(0);

        CD3DX12_STATIC_SAMPLER_DESC staticSampler(
            0,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_DEFAULT_MIP_LOD_BIAS, D3D12_DEFAULT_MAX_ANISOTROPY,
            D3D12_COMPARISON_FUNC_LESS_EQUAL,
            D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

        D3D12_ROOT_SIGNATURE_DESC desc{};
        desc.pParameters = rootParameters;
        desc.NumParameters = _countof(rootParameters);
        desc.pStaticSamplers = &staticSampler;
        desc.NumStaticSamplers = 1;
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_.Create(L"ArcadeMachine RootSignature", desc);
    }

    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
        auto shaderManager = ShaderManager::GetInstance();

        desc.pRootSignature = rootSignature_;

        D3D12_INPUT_ELEMENT_DESC inputElements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
        inputLayoutDesc.pInputElementDescs = inputElements;
        inputLayoutDesc.NumElements = _countof(inputElements);
        desc.InputLayout = inputLayoutDesc;

        auto vs = shaderManager->Compile(L"Engine/Shader/ModelVS.hlsl", ShaderManager::kVertex);
        auto ps = shaderManager->Compile(L"Engine/Shader/ModelPS.hlsl", ShaderManager::kPixel);
        desc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        desc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
        desc.BlendState = Helper::BlendDisable;
        desc.DepthStencilState = Helper::DepthStateReadWrite;
        desc.RasterizerState = Helper::RasterizerDefault;
        desc.NumRenderTargets = 1;
        desc.RTVFormats[0] = rtvFormat;
        desc.DSVFormat = dsvFormat;
        desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        desc.SampleDesc.Count = 1;
        pipelineState_.Create(L"ArcadeMachine PSO", desc);
    }
}

void ArcadeMachine::LoadModels() {

}
