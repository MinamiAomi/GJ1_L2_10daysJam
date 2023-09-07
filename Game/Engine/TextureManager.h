#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include "Graphics/Texture.h"
#include "TextureHandle.h"

class TextureManager {
public:
    static TextureManager* GetInstance();

    TextureHandle Load(const std::filesystem::path& path);

    Texture& GetTexture(const TextureHandle& textureHandle) { return *textures_[textureHandle.index_].second; }

private:
    std::vector<std::pair<std::wstring, std::unique_ptr<Texture>>> textures_;
};