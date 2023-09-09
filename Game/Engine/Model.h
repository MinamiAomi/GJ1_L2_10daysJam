#pragma once

#include <vector>
#include <memory>
#include <filesystem>

#include "Graphics/UploadBuffer.h"
#include "Graphics/RootSignature.h"
#include "Graphics/PipelineState.h"
#include "Graphics/Texture.h"
#include "Math/MathUtils.h"

class CommandContext;

class Model {
    friend class Model;
public:
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 texcoord;
    };

    static void CreatePipeline(DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);

    void LoadFromObjFile(const std::filesystem::path& path);
    void Draw(CommandContext& commandContext, const Matrix4x4& world, const Matrix4x4& camera);

private:
    struct Mesh {
        uint32_t vertexOffset;
        uint32_t indexOffset;
        uint32_t indexCount;
        uint32_t materialIndex;
    };
    struct Material {
        std::string name;
        Vector4 color;
        std::unique_ptr<Texture> texture;
    };

    static RootSignature rootSignature_;
    static PipelineState pipelineState_;


    UploadBuffer vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vbView_{};
    UploadBuffer indexBuffer_;
    D3D12_INDEX_BUFFER_VIEW ibView_{};
    std::vector<Mesh> meshes_;
    std::vector<Material> materials_;
};