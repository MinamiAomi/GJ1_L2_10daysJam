#include "Window.h"

#include "Externals/ImGui/imgui_impl_win32.h"

#include "Utility.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace {
    // ウィンドウプロシージャ
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }

        // メッセージに対してゲーム固有の処理を行う
        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
}

Window& Window::Get() {
    static Window instance;
    return instance;
}

void Window::Create(const std::string& name, uint32_t clientWidth, uint32_t clientHeight) {
    ASSERT_IF_FAILED(CoInitializeEx(0, COINIT_MULTITHREADED));

    // ウィンドウクラスを生成
    WNDCLASS wc{};
    wc.lpfnWndProc = WindowProc;	// ウィンドウプロシージャ
    wc.lpszClassName = L"CGWindowClass";	// ウィンドウクラス名
    wc.hInstance = GetModuleHandle(nullptr);	// インスタンスハンドル
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);	// カーソル
    RegisterClass(&wc);	// ウィンドウクラスを登録

    // ウィンドウサイズを表す構造体にクライアント領域を入れる
    RECT wrc{ 0,0,static_cast<LONG>(clientWidth),static_cast<LONG>(clientHeight) };
    // クライアント領域を元に実際のサイズにwrcを変更してもらう
    AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

    auto wname = Utility::ConvertString(name);

    // ウィンドウの生成
    hWnd_ = CreateWindow(
        wc.lpszClassName,		// 利用するクラス名
        wname.c_str(),				// タイトルバーの文字
        WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
        CW_USEDEFAULT,			// 表示X座標（WindowsOSに任せる）
        CW_USEDEFAULT,			// 表示Y座標（WindowsOSに任せる）
        wrc.right - wrc.left,	// ウィンドウ横幅
        wrc.bottom - wrc.top,	// ウィンドウ縦幅
        nullptr,				// 親ウィンドウハンドル
        nullptr,				// メニューハンドル
        wc.hInstance,			// インスタンスハンドル
        nullptr);				// オプション
    clientWidth_ = clientWidth;
    clientHeight_ = clientHeight;

    ShowWindow(hWnd_, SW_SHOW);
}

void Window::Destroy() {
    CloseWindow(hWnd_);
    CoUninitialize();
}

bool Window::ProcessMessage() const {
    MSG msg{};

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            break;
        }
    }
    return msg.message != WM_QUIT;
}
