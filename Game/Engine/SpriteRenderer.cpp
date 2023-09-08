#include "SpriteRenderer.h"

#include <cassert>

#include "Graphics/CommandContext.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Helper.h"

SpriteRenderer* SpriteRenderer::GetInstance() {
    static SpriteRenderer instance;
    return &instance;
}

void SpriteRenderer::Initialize(DXGI_FORMAT rtvFormat) {
    // ルートシグネチャを生成
    {
        CD3DX12_DESCRIPTOR_RANGE ranges[1]{};
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER rootParameters[1]{};
        rootParameters->InitAsDescriptorTable(1, ranges);

        CD3DX12_STATIC_SAMPLER_DESC staticSamplers[1]{};
        staticSamplers[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

        D3D12_ROOT_SIGNATURE_DESC desc{};
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        desc.pParameters = rootParameters;
        desc.NumParameters = _countof(rootParameters);
        desc.pStaticSamplers = staticSamplers;
        desc.NumStaticSamplers = _countof(staticSamplers);
        rootSignature_.Create(L"SpriteRenderer RootSignature", desc);
    }

    // パイプラインを生成
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};

        auto shaderManager = ShaderManager::GetInstance();

        desc.pRootSignature = rootSignature_;


        auto vs = shaderManager->Compile(L"Engine/Shader/SpriteVS.hlsl", ShaderManager::kVertex);
        auto ps = shaderManager->Compile(L"Engine/Shader/SpritePS.hlsl", ShaderManager::kPixel);
        desc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        desc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());

        D3D12_INPUT_ELEMENT_DESC inputElements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
        inputLayoutDesc.pInputElementDescs = inputElements;
        inputLayoutDesc.NumElements = _countof(inputElements);
        desc.InputLayout = inputLayoutDesc;

        desc.RasterizerState = Helper::RasterizerNoCull;
        desc.NumRenderTargets = 1;
        desc.RTVFormats[0] = rtvFormat;
        desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        desc.SampleDesc.Count = 1;

        desc.BlendState = Helper::BlendDisable;
        pipelineStates_[kBlendDisable].Create(L"SpriteRenderer BlendDisablePSO", desc);
        desc.BlendState = Helper::BlendAlpha;
        pipelineStates_[kBlendNormal].Create(L"SpriteRenderer BlendNormalPSO", desc);
        desc.BlendState = Helper::BlendAdditive;
        pipelineStates_[kBlendAdditive].Create(L"SpriteRenderer BlendAdditivePSO", desc);
        desc.BlendState = Helper::BlendSubtract;
        pipelineStates_[kBlendSubtract].Create(L"SpriteRenderer BlendSubtractPSO", desc);
    }

    vertexBuffer_.Create(L"TriangleRenderer VertexBuffer", kMaxNumTriangles * 3, sizeof(Vertex));
    vbv_.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
    vbv_.SizeInBytes = UINT(vertexBuffer_.GetBufferSize());
    vbv_.StrideInBytes = sizeof(Vertex);
    triangleCount_ = 0;
}

void SpriteRenderer::Reset() {
    triangleCount_ = 0;
}

void SpriteRenderer::Draw(CommandContext& commandContext, const Vertex* vertices, uint32_t numTriangles, D3D12_GPU_DESCRIPTOR_HANDLE srv) {
    assert(triangleCount_ + numTriangles <= kMaxNumTriangles);


    Vertex* dest = reinterpret_cast<Vertex*>(vertexBuffer_.GetCPUData());
    dest += triangleCount_ * 3;

    memcpy(dest, vertices, sizeof(Vertex) * numTriangles * 3);

    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineStates_[blendMode_]);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(0, srv);
    commandContext.SetVertexBuffer(0, vbv_);
    commandContext.Draw(numTriangles * 3, triangleCount_ * 3);

    triangleCount_ += numTriangles;
}
