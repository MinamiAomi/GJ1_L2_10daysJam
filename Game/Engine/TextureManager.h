#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include "Graphics/Texture.h"

class TextureHandle {
    friend class TextureManager;
public:
    Texture& GetTexture() const;
    bool IsValid() const { return index_ != ((size_t)-1); }
    size_t GetIndex() const { return index_; }
private:
    size_t index_ = ((size_t)-1);
};

class TextureManager {
public:
    static TextureManager* GetInstance();

    TextureHandle Load(const std::filesystem::path& path);

    Texture& GetTexture(const TextureHandle& textureHandle) { return *textures_[textureHandle.index_].second; }

private:
    std::vector<std::pair<std::wstring, std::unique_ptr<Texture>>> textures_;
};