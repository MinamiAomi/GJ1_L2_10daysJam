#include "TextureManager.h"

std::shared_ptr<Texture> TextureManager::Load(const std::filesystem::path& path) {
    return Get().InternalLoad(path);
}

TextureManager& TextureManager::Get() {
    static TextureManager instance;
    return instance;
}

std::shared_ptr<Texture> TextureManager::InternalLoad(const std::filesystem::path& path) {
    std::string key = path.string();
    auto iter = textureMap_.find(key);
    if (iter != textureMap_.end()) {
        return iter->second;
    }

    auto texture = std::make_shared<Texture>();
    texture->CreateFromWICFile(path);
    textureMap_.emplace(key, texture);
    return texture;
}
