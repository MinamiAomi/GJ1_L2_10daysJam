#include "SwapChain.h"

using namespace Microsoft::WRL;

#include "Application/GameWindow.h"
#include "Graphics.h"
#include "Helper.h"
#include "ColorBuffer.h"

SwapChain* SwapChain::GetInstance() {
    static SwapChain instance;
    return &instance;
}

void SwapChain::Initialize() {
    GameWindow* gameWindow = GameWindow::GetInstance();

    ComPtr<IDXGIFactory7> factory;
    ASSERT_IF_FAILED(CreateDXGIFactory(IID_PPV_ARGS(factory.GetAddressOf())));

    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Width = gameWindow->GetClientWidth(); // 画面幅
    desc.Height = gameWindow->GetClientHeight(); // 画面高
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色の形式
    desc.SampleDesc.Count = 1; // マルチサンプル市内
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画ターゲットとして利用する
    desc.BufferCount = kNumBuffers;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに移したら、中身を破棄

    ASSERT_IF_FAILED(factory->CreateSwapChainForHwnd(
        Graphics::GetInstance()->GetCommandQueue(),
        gameWindow->GetHWND(),
        &desc,
        nullptr,
        nullptr,
        reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf())));

    for (uint32_t i = 0; i < kNumBuffers; ++i) {
        ComPtr<ID3D12Resource> resource;
        ASSERT_IF_FAILED(swapChain_->GetBuffer(i, IID_PPV_ARGS(resource.GetAddressOf())));
        buffers_[i] = std::make_unique<ColorBuffer>();
        buffers_[i]->CreateFromSwapChain(L"SwapChainBuffer" + std::to_wstring(i), resource.Detach());
    }
}

void SwapChain::Present() {
    swapChain_->Present(1, 0);
    currentBufferIndex_ = (currentBufferIndex_ + 1) % kNumBuffers;
}
