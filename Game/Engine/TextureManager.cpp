#include "TextureManager.h"

TextureManager* TextureManager::GetInstance() {
    static TextureManager instance;
    return &instance;
}

TextureHandle TextureManager::Load(const std::filesystem::path& path) {
    TextureHandle handle;
    std::wstring wstr = path.wstring();
    // 読み込み済みか探す
    auto iter = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) { return texture.first == wstr; });
    // 読み込み済み
    if (iter != textures_.end()) {
        handle.index_ = std::distance(textures_.begin(), iter);
        return handle;
    }

    // 最後尾に読み込む
    handle.index_ = textures_.size();

    auto texture = std::make_unique<Texture>();
    texture->CreateFromWICFile(wstr);

    textures_.emplace_back(wstr, std::move(texture));
    return handle;
}
