#pragma once

#include <d3d12.h>
#include <dxcapi.h>
#include <wrl/client.h>

#include <filesystem>

class ShaderCompiler {
public:
    static ShaderCompiler& Get();
    static Microsoft::WRL::ComPtr<IDxcBlob> Compile(const std::filesystem::path& path, const std::wstring& profile);

    void Create();

private:
    ShaderCompiler() = default;
    ShaderCompiler(const ShaderCompiler&) = delete;
    ShaderCompiler& operator=(const ShaderCompiler&) = delete;

    Microsoft::WRL::ComPtr<IDxcBlob> InternalCompile(const std::filesystem::path& path, const std::wstring& profile);

    Microsoft::WRL::ComPtr<IDxcUtils> utils_;
    Microsoft::WRL::ComPtr<IDxcCompiler3> compiler_;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
};