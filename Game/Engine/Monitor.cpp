#include "Monitor.h"

#include "Graphics/Helper.h"
#include "Graphics/ShaderManager.h"

void Monitor::Initilaize(uint32_t width, uint32_t height, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {

    {
        CD3DX12_DESCRIPTOR_RANGE ranges[1]{};
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER rootParameters[2]{};
        rootParameters[0].InitAsDescriptorTable(_countof(ranges), ranges);
        rootParameters[1].InitAsConstantBufferView(0);

        CD3DX12_STATIC_SAMPLER_DESC staticSampler(
            0,
            D3D12_FILTER_MIN_MAG_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            D3D12_TEXTURE_ADDRESS_MODE_BORDER,
            D3D12_DEFAULT_MIP_LOD_BIAS, D3D12_DEFAULT_MAX_ANISOTROPY,
            D3D12_COMPARISON_FUNC_LESS_EQUAL,
            D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

        D3D12_ROOT_SIGNATURE_DESC desc{};
        desc.pParameters = rootParameters;
        desc.NumParameters = _countof(rootParameters);
        desc.pStaticSamplers = &staticSampler;
        desc.NumStaticSamplers = 1;
        desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        rootSignature_.Create(L"Monitor RootSignature", desc);
    }

    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
        auto shaderManager = ShaderManager::GetInstance();

        desc.pRootSignature = rootSignature_;

        auto vs = shaderManager->Compile(L"Engine/Shader/MonitorVS.hlsl", ShaderManager::kVertex);
        auto ps = shaderManager->Compile(L"Engine/Shader/MonitorPS.hlsl", ShaderManager::kPixel);
        desc.VS = CD3DX12_SHADER_BYTECODE(vs->GetBufferPointer(), vs->GetBufferSize());
        desc.PS = CD3DX12_SHADER_BYTECODE(ps->GetBufferPointer(), ps->GetBufferSize());
        desc.BlendState = Helper::BlendDisable;
        desc.DepthStencilState = Helper::DepthStateReadWrite;
        desc.RasterizerState = Helper::RasterizerDefault;
        desc.NumRenderTargets = 1;
        desc.RTVFormats[0] = rtvFormat;
        desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
        desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        desc.SampleDesc.Count = 1;
        pipelineState_.Create(L"Monitor PSO", desc);
    }

    colorBuffer_.Create(L"Monitor ColorBuffer", width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

}

void Monitor::BeginRender(CommandContext& commandContext) {
    commandContext.TransitionResource(colorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(colorBuffer_.GetRTV());
    commandContext.ClearColor(colorBuffer_);
    commandContext.SetViewportAndScissorRect(0, 0, colorBuffer_.GetWidth(), colorBuffer_.GetHeight());
}

void Monitor::Draw(CommandContext& commandContext, const Matrix4x4& camera) {
    commandContext.TransitionResource(colorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(0, colorBuffer_.GetSRV());
    commandContext.SetDynamicConstantBufferView(1, sizeof(camera), &camera);

    //D3D12_VERTEX_BUFFER_VIEW vbv{};
    //commandContext.SetVertexBuffer(0, vbv);
    //commandContext.SetIndexBuffer();


    struct Vertex {
        Vector3 pos;
        Vector2 tex;
    };

    Vertex vertices[6] = {
        {{ -0.8f, -0.8f, 0.0f }, {0.0f, 1.0f}},
        {{ -0.8f,  0.8f, 0.0f }, {0.0f, 0.0f}},
        {{  0.8f,  0.8f, 0.0f }, {1.0f, 0.0f}},
        
        {{ -0.8f, -0.8f, 0.0f }, {0.0f, 1.0f}},
        {{  0.8f,  0.8f, 0.0f }, {1.0f, 0.0f}},
        {{  0.8f, -0.8f, 0.0f }, {1.0f, 1.0f}},
    };
    commandContext.SetDynamicVertexBuffer(0, 6, sizeof(Vertex), vertices);
    commandContext.Draw(6, 0);
}
