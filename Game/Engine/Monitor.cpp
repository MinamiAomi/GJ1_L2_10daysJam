#include "Monitor.h"

#include <filesystem>
#include <cassert>
#include <fstream>
#include <sstream>

#include "Graphics/Helper.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/CommandContext.h"

Monitor* Monitor::GetInstance() {
    static Monitor instance;
    return &instance;
}

void Monitor::Initilaize(uint32_t bufferWidth, uint32_t bufferHeight, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {
    {
        bufferWidth *= 2;
        bufferHeight *= 2;

        CD3DX12_DESCRIPTOR_RANGE ranges[1]{};
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

        CD3DX12_ROOT_PARAMETER rootParameters[2]{};
        rootParameters[0].InitAsDescriptorTable(_countof(ranges), ranges);
        rootParameters[1].InitAsConstantBufferView(0);

        CD3DX12_STATIC_SAMPLER_DESC staticSampler(
            0,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
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

        D3D12_INPUT_ELEMENT_DESC inputElements[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
        inputLayoutDesc.pInputElementDescs = inputElements;
        inputLayoutDesc.NumElements = _countof(inputElements);
        desc.InputLayout = inputLayoutDesc;

        auto vs = shaderManager->Compile(L"Resources/Shader/MonitorVS.hlsl", ShaderManager::kVertex);
        auto ps = shaderManager->Compile(L"Resources/Shader/MonitorPS.hlsl", ShaderManager::kPixel);
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
        pipelineState_.Create(L"Monitor PSO", desc);
    }
    colorBuffer_.Create(L"Monitor ColorBuffer", bufferWidth, bufferHeight, DXGI_FORMAT_R8G8B8A8_UNORM);

    {
        std::filesystem::path path = "Resources/Model/monitor.obj";
        std::ifstream file(path);
        assert(file.is_open());

        std::vector<Vector3> positions;
        std::vector<Vector2> texcoords;

        std::vector<Vertex> vertices;

        std::string line;
        while (std::getline(file, line)) {
            std::string identifier;
            std::istringstream iss(line);
            iss >> identifier;

            if (identifier == "#") {
                continue;
            }
            else if (identifier == "v") {
                Vector3& position = positions.emplace_back();
                iss >> position.x >> position.y >> position.z;
                // position.z = -position.z;
            }
            else if (identifier == "vt") {
                Vector2& texcoord = texcoords.emplace_back();
                iss >> texcoord.x >> texcoord.y;
                texcoord.y = 1.0f - texcoord.y;
            }
            else if (identifier == "f") {
                std::string vertexDefinitions[3];
                iss >> vertexDefinitions[0] >> vertexDefinitions[1] >> vertexDefinitions[2];

                for (uint32_t i = 0; i < 3; ++i) {
                    std::istringstream viss(vertexDefinitions[i]);
                    uint32_t elementIndices[2]{};
                    for (uint32_t j = 0; j < 2; ++j) {
                        std::string index;
                        std::getline(viss, index, '/');
                        if (!index.empty()) {
                            elementIndices[j] = static_cast<uint32_t>(std::stoi(index)) - 1;
                        }
                    }
                    auto& vertex = vertices.emplace_back();
                    vertex.position = positions[elementIndices[0]];
                    vertex.texcoord = texcoords[elementIndices[1]];
                }
            }
        }

        file.close();

        vertexBuffer_.Create(L"Monitor VertexBuffer", vertices.size(), sizeof(vertices[0]));
        vertexBuffer_.Copy(vertices.data(), vertices.size() * sizeof(vertices[0]));
        vbView_.BufferLocation = vertexBuffer_.GetGPUVirtualAddress();
        vbView_.SizeInBytes = UINT(vertexBuffer_.GetBufferSize());
        vbView_.StrideInBytes = UINT(sizeof(vertices[0]));
        vertexCount_ = uint32_t(vertices.size());
    }
}

void Monitor::BeginRender(CommandContext& commandContext) {
    commandContext.TransitionResource(colorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandContext.SetRenderTarget(colorBuffer_.GetRTV());
    commandContext.ClearColor(colorBuffer_);
    commandContext.SetViewportAndScissorRect(0, 0, colorBuffer_.GetWidth(), colorBuffer_.GetHeight());
}

void Monitor::Draw(CommandContext& commandContext, const Matrix4x4& world, const Matrix4x4& camera) {
    commandContext.TransitionResource(colorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandContext.SetRootSignature(rootSignature_);
    commandContext.SetPipelineState(pipelineState_);
    commandContext.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandContext.SetDescriptorTable(0, colorBuffer_.GetSRV());
    Matrix4x4 worldViewProj = world * camera;
    commandContext.SetDynamicConstantBufferView(1, sizeof(worldViewProj), &worldViewProj);
    commandContext.SetVertexBuffer(0, vbView_);
    commandContext.Draw(vertexCount_, 0);
}
