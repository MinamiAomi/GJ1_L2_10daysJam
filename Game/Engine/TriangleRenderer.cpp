#include "TriangleRenderer.h"

#include <cassert>

#include "Graphics/CommandContext.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/Helper.h"

TriangleRenderer* TriangleRenderer::GetInstance() {
    static TriangleRenderer instance;
    return &instance;
}

void TriangleRenderer::Initialize(DXGI_FORMAT rtvFormat) {
    // ルートシグネチャを生成
    {
        D3D12_ROOT_SIGNATURE_DESC desc{};
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        rootSignature_.Create(L"TriangleRenderer RootSignature", desc);
    }

    // パイプラインを生成
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};

        auto shaderManager = ShaderManager::GetInstance();

        desc.pRootSignature = rootSignature_;


        auto vs = shaderManager->Compile(L"Engine/Shader/TriangleVS.hlsl", ShaderManager::kVertex);
        auto ps = shaderManager->Compile(L"Engine/Shader/TrianglePS.hlsl", ShaderManager::kPixel);
        desc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        desc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());

        D3D12_INPUT_ELEMENT_DESC inputElements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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
        pipelineStates_[kBlendDisable].Create(L"TriangleRenderer BlendDisablePSO", desc);
        desc.BlendState = Helper::BlendAlpha;
        pipelineStates_[kBlendNormal].Create(L"TriangleRenderer BlendNormalPSO", desc);
        desc.BlendState = Helper::BlendAdditive;
        pipelineStates_[kBlendAdditive].Create(L"TriangleRenderer BlendAdditivePSO", desc);
        desc.BlendState = Helper::BlendSubtract;
        pipelineStates_[kBlendSubtract].Create(L"TriangleRenderer BlendSubtractPSO", desc);
    }

    vertexBuffer_.Create(L"TriangleRenderer VertexBuffer", kMaxNumTriangles, sizeof(Vertex));
    vbv_.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
    vbv_.SizeInBytes = UINT(vertexBuffer_.GetBufferSize());
    vbv_.StrideInBytes = sizeof(Vertex);
    triangleCount_ = 0;

}

void TriangleRenderer::Draw(CommandContext& commandContext, const Vector3& p0, uint32_t c0, const Vector3& p1, uint32_t c1, const Vector3& p2, uint32_t c2) {
    assert(triangleCount_ <= kMaxNumTriangles);

    Vertex* dest = reinterpret_cast<Vertex*>(vertexBuffer_.GetCPUData());
    dest += triangleCount_ * 3;

    dest[0].position = p0;
    dest[0].color = c0;
    dest[1].position = p1;
    dest[1].color = c1;
    dest[2].position = p2;
    dest[2].color = c2;

    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineStates_[blendMode_]);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetVertexBuffer(0, vbv_);
    commandContext.Draw(3, triangleCount_ * 3);

    ++triangleCount_;
}

void TriangleRenderer::Draw(CommandContext& commandContext, const Vector3* vertices, uint32_t numTriangles, uint32_t c) {
    assert(triangleCount_ + numTriangles <= kMaxNumTriangles);


    Vertex* dest = reinterpret_cast<Vertex*>(vertexBuffer_.GetCPUData());
    dest += triangleCount_ * 3;

    for (size_t i = 0; i < numTriangles; ++i) {
        dest[0].position = vertices[0];
        dest[0].color = c;
        dest[1].position = vertices[1];
        dest[1].color = c;
        dest[2].position = vertices[2];
        dest[2].color = c;
        dest += 3;
        vertices += 3;
    }

    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineStates_[blendMode_]);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetVertexBuffer(0, vbv_);
    commandContext.Draw(numTriangles * 3, triangleCount_ * 3);

    triangleCount_ += numTriangles;
}

void TriangleRenderer::Draw(CommandContext& commandContext, const std::vector<Vector3>& vertices, uint32_t c) {
    assert(vertices.size() % 3 == 0);

    Draw(commandContext, vertices.data(), uint32_t(vertices.size()), c);
}

void TriangleRenderer::Reset() {
    triangleCount_ = 0;
}

