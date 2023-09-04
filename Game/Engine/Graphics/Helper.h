#pragma once

#include "Externals/DirectXTex/d3dx12.h"

#include <winnt.h>

namespace Helper {

    static const D3D12_RASTERIZER_DESC RasterizerDefault;

    static const D3D12_BLEND_DESC BlendNoColorWrite;    
    static const D3D12_BLEND_DESC BlendDisable;     // ブレンド無効
    static const D3D12_BLEND_DESC BlendMultiply;    // 乗算合成
    static const D3D12_BLEND_DESC BlendAdditive;    // 加算合成

    static const D3D12_DEPTH_STENCIL_DESC DepthStateDisabled;
    static const D3D12_DEPTH_STENCIL_DESC DepthStateReadWrite;

    DXGI_FORMAT GetBaseFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetDepthFormat(DXGI_FORMAT format);
    DXGI_FORMAT GetStencilFormat(DXGI_FORMAT format);
    size_t GetBytePerPixel(DXGI_FORMAT format);

    void AssertIfFailed(HRESULT hr, const wchar_t* str);
}

#ifndef HELPER_DEFINES
#define HELPER_DEFINES

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define WIDE_(x) L##x
#define WIDE(x) WIDE_(x)

#define ASSERT_IF_FAILED(hr) Helper::AssertIfFailed(hr, WIDE(__FILE__) "(" STRINGIFY(__LINE__) ")\n" STRINGIFY(hr))

#ifdef _DEBUG
#define D3D12_OBJECT_SET_NAME(object, name) object->SetName(name)
#else
#define D3D12_OBJECT_SET_NAME(object, name) ((void)object),((void)name)
#endif // _DEBUG


#endif // HELPER_DEFINES