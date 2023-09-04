#include "ShaderCompiler.h"

#include <cassert>

#include "Utility.h"

#pragma comment(lib,"dxcompiler.lib")

using namespace Microsoft::WRL;

ShaderCompiler& ShaderCompiler::Get() {
    static ShaderCompiler instance;
    return instance;
}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderCompiler::Compile(const std::filesystem::path& path, const std::wstring& profile) {
    return Get().InternalCompile(path, profile);
}

void ShaderCompiler::Create() {

    ASSERT_IF_FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils_.GetAddressOf())));
    ASSERT_IF_FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler_.GetAddressOf())));
    ASSERT_IF_FAILED(utils_->CreateDefaultIncludeHandler(includeHandler_.GetAddressOf()));


}

Microsoft::WRL::ComPtr<IDxcBlob> ShaderCompiler::InternalCompile(const std::filesystem::path& path, const std::wstring& profile) {
    OutputDebugStringW(std::format(L"Begin CompileShader, path:{}, profile:{}\n", path.wstring(), profile).c_str());

    //MessageBoxW(nullptr, path.wstring().c_str(), L"Cap", S_OK);

    ComPtr<IDxcBlobEncoding> shaderSource;
    ASSERT_IF_FAILED(utils_->LoadFile(path.wstring().c_str(), nullptr, shaderSource.GetAddressOf()));

    DxcBuffer shader_source_buffer{};
    shader_source_buffer.Ptr = shaderSource->GetBufferPointer();
    shader_source_buffer.Size = shaderSource->GetBufferSize();
    shader_source_buffer.Encoding = DXC_CP_UTF8;

    LPCWSTR arguments[] = {
        path.c_str(),
        L"-E", L"main",
        L"-T", profile.c_str(),
        L"-Zi", L"-Qembed_debug",
        L"-Od",
        L"-Zpr"
    };

    ComPtr<IDxcResult> shaderResult;
    ASSERT_IF_FAILED(compiler_->Compile(
        &shader_source_buffer,
        arguments,
        _countof(arguments),
        includeHandler_.Get(),
        IID_PPV_ARGS(shaderResult.GetAddressOf())));

    ComPtr<IDxcBlobUtf8> shaderError = nullptr;
    ASSERT_IF_FAILED(shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(shaderError.GetAddressOf()), nullptr));

    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
        std::string errorMsg = shaderError->GetStringPointer();
        OutputDebugStringA(errorMsg.c_str());
        assert(false);
    }

    ComPtr<IDxcBlob> blob;
    ASSERT_IF_FAILED(shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(blob.GetAddressOf()), nullptr));

    OutputDebugStringW(std::format(L"Compile Succeeded, path:{}, profile:{}\n", path.wstring(), profile).c_str());
    return blob;
}
