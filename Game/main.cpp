#include <Windows.h>

#include "Engine/TOMATOsEngine.h"

#include "Engine/RenderManager.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

    TOMATOsEngine::Initialize();

    while (TOMATOsEngine::BeginFrame()) {

        ImGui::Begin("Color");

        static ImVec4 color{ };
        ImGui::ColorEdit4("c", &color.x);
        auto u32 = ImGui::ColorConvertFloat4ToU32(color);;


        TOMATOsEngine::DrawTriangle({ 500.0f,250.0f }, { 550.0f, 250.0f }, { 500.0f, 300.0f }, u32);
        TOMATOsEngine::DrawTriangle({ 10.0f,500.0f }, { 1000.0f, 500.0f }, { 300.0f, 600.0f }, 0xFFFFFFFF);
        TOMATOsEngine::DrawCircle({ 640.0f,360.0f }, 50.0f, u32);
        TOMATOsEngine::DrawRect({ 100.0f,100.0f }, {300.0f, -300.0f}, u32);
        TOMATOsEngine::DrawRectAngle({ 700.0f,600.0f }, { 300.0f, -300.0f }, {0.5f,0.5f}, 1.0f, u32);

        ImGui::End();
    
        static float bloomThreshold = 0.0f;
        ImGui::DragFloat("Bloom threshold", &bloomThreshold, 0.01f, 0.0f, 1.0f);
        RenderManager::GetInstance()->SetBloom(bloomThreshold);
    }

    TOMATOsEngine::Shutdown();

    return 0;
}