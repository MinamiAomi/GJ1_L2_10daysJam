#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <string>

#include "Texture.h"

class TextureManager {
public:
    static std::shared_ptr<Texture> Load(const std::filesystem::path& path);

private:
    static TextureManager& Get();

    TextureManager() = default;
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    std::shared_ptr<Texture> InternalLoad(const std::filesystem::path& path);

    std::map<std::string, std::shared_ptr<Texture>> textureMap_;
};