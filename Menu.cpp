#include "Menu.h"
#include "SDK.h"
#include "MemoryEngine.h"
#include "imgui.h"
#include <cstdio>

MenuConfig g_Config;
static bool g_MenuOpen = true;
static GLuint g_LogoTex = 0;
static float g_LogoAspect = 1.45f;

void Menu::SetLogoTexture(GLuint texId, int w, int h) {
    g_LogoTex = texId;
    if (w > 0 && h > 0) {
        g_LogoAspect = (float)w / (float)h;
    }
}

void Menu::ApplyTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 6.0f;
    style.WindowBorderSize = 1.4f;

    ImVec4* c = style.Colors;
    c[ImGuiCol_WindowBg]         = ImVec4(0.05f, 0.04f, 0.08f, 0.95f);
    c[ImGuiCol_Border]           = ImVec4(0.66f, 0.18f, 0.98f, 0.90f);
    c[ImGuiCol_TitleBg]          = ImVec4(0.12f, 0.04f, 0.20f, 1.00f);
    c[ImGuiCol_TitleBgActive]    = ImVec4(0.45f, 0.10f, 0.72f, 1.00f);
    c[ImGuiCol_Tab]              = ImVec4(0.13f, 0.10f, 0.20f, 1.00f);
    c[ImGuiCol_TabHovered]       = ImVec4(0.56f, 0.14f, 0.88f, 1.00f);
    c[ImGuiCol_TabActive]        = ImVec4(0.68f, 0.20f, 1.00f, 1.00f);
    c[ImGuiCol_CheckMark]        = ImVec4(0.82f, 0.42f, 1.00f, 1.00f);
    c[ImGuiCol_SliderGrab]       = ImVec4(0.68f, 0.20f, 1.00f, 1.00f);
    c[ImGuiCol_SliderGrabActive] = ImVec4(0.85f, 0.45f, 1.00f, 1.00f);
    c[ImGuiCol_FrameBg]          = ImVec4(0.12f, 0.10f, 0.18f, 1.00f);
    c[ImGuiCol_Button]           = ImVec4(0.54f, 0.14f, 0.84f, 1.00f);
    c[ImGuiCol_ButtonHovered]    = ImVec4(0.70f, 0.22f, 1.00f, 1.00f);
}

void Menu::Draw() {
    ImGuiIO& io = ImGui::GetIO();

    if (g_Config.drawFovCircle) {
        ImDrawList* bg = ImGui::GetBackgroundDrawList();
        ImVec2 center(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
        ImU32 col = ImGui::ColorConvertFloat4ToU32(
            ImVec4(g_Config.fovColor[0], g_Config.fovColor[1], g_Config.fovColor[2], g_Config.fovColor[3])
        );
        bg->AddCircle(center, g_Config.aimFov, col, 72, 2.0f);
    }

    ImGui::SetNextWindowPos(ImVec2(36, 110), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin("##7A4Bubble", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    if (g_LogoTex != 0) {
        if (ImGui::ImageButton("##LogoBtn", (ImTextureID)(intptr_t)g_LogoTex, ImVec2(92, 92 / g_LogoAspect))) {
            g_MenuOpen = !g_MenuOpen;
        }
    } else {
        if (ImGui::Button(g_MenuOpen ? "7A4 [AN]" : "7A4 [MO]", ImVec2(110, 42))) {
            g_MenuOpen = !g_MenuOpen;
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();

    if (!g_MenuOpen) return;

    ImGui::SetNextWindowSize(ImVec2(480, 420), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(150, 110), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("7A4 // FREE FIRE VIP HUB", &g_MenuOpen, ImGuiWindowFlags_NoCollapse)) {
        if (g_LogoTex != 0) {
            float bannerH = 74.0f;
            float bannerW = bannerH * g_LogoAspect;
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - bannerW) * 0.5f);
            ImGui::Image((ImTextureID)(intptr_t)g_LogoTex, ImVec2(bannerW, bannerH));
        }

        if (MemoryEngine::IsAttached()) {
            ImGui::TextColored(ImVec4(0.30f, 1.0f, 0.45f, 1.0f),
                "[CONNECTED] PID: %d | libil2cpp: 0x%llX",
                MemoryEngine::GetPid(), (unsigned long long)MemoryEngine::GetLibBase());
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "[DISCONNECTED] Chua tim thay Free Fire");
            ImGui::SameLine();
            if (ImGui::SmallButton("Ket noi Game")) {
                MemoryEngine::AttachGame();
            }
        }

        ImGui::Separator();

        if (ImGui::BeginTabBar("7A4Tabs")) {
            if (ImGui::BeginTabItem("Aimbot")) {
                ImGui::Spacing();
                ImGui::Checkbox("Kich hoat Aimbot", &g_Config.aimbotEnabled);
                ImGui::Checkbox("Chi khoa muc tieu lo dien (Visible Check)", &g_Config.aimbotVisibleOnly);
                ImGui::Checkbox("Tu dong ban khi khoa (Auto Fire)", &g_Config.aimbotAutoFire);

                ImGui::Separator();
                ImGui::Text("Vung khoa muc tieu (Hitbox):");
                const char* bones[] = { "Dau (Head)", "Than (Body)", "Tay (Arms)", "Chan (Legs)" };
                ImGui::Combo("##AimBone", &g_Config.aimBone, bones, IM_ARRAYSIZE(bones));

                ImGui::SliderFloat("Do muot (Smooth)", &g_Config.aimSmooth, 1.0f, 15.0f, "%.1f");
                ImGui::SliderFloat("Ban kinh FOV Aim", &g_Config.aimFov, 30.0f, 450.0f, "%.0f px");
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("ESP + FOV")) {
                ImGui::Spacing();
                ImGui::Checkbox("Kich hoat ESP", &g_Config.espEnabled);
                ImGui::Checkbox("ESP Khung 2D (Box)", &g_Config.espBox);
                ImGui::Checkbox("ESP Duong ke (Line)", &g_Config.espLine);
                ImGui::Checkbox("ESP Xuong (Skeleton)", &g_Config.espSkeleton);
                ImGui::Checkbox("ESP Thanh mau (Health)", &g_Config.espHealth);
                ImGui::Checkbox("ESP Khoang cach (Distance)", &g_Config.espDistance);
                ImGui::ColorEdit4("Mau ESP", g_Config.espColor, ImGuiColorEditFlags_NoInputs);

                ImGui::Separator();
                ImGui::Checkbox("Hien thi vong tron FOV", &g_Config.drawFovCircle);
                ImGui::SliderFloat("Kich thuoc FOV", &g_Config.aimFov, 30.0f, 450.0f, "%.0f px");
                ImGui::ColorEdit4("Mau vong FOV", g_Config.fovColor, ImGuiColorEditFlags_NoInputs);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("All Tinh Nang")) {
                ImGui::Spacing();
                ImGui::Checkbox("Khong giat (No Recoil)", &g_Config.noRecoil);
                ImGui::Checkbox("Dan thang tam (No Spread)", &g_Config.noSpread);
                ImGui::Checkbox("Thay dan nhanh (Fast Reload)", &g_Config.fastReload);
                ImGui::Checkbox("Tang toc chay (Speed Hack)", &g_Config.speedHack);
                if (g_Config.speedHack) {
                    ImGui::SliderFloat("Toc do nhan", &g_Config.speedMultiplier, 1.0f, 2.5f, "%.2fx");
                }
                ImGui::Checkbox("Nhay cao (High Jump)", &g_Config.highJump);
                ImGui::Checkbox("Nguoi trang (White Body)", &g_Config.whiteBody);
                ImGui::Checkbox("Che do ban dem (Night Mode)", &g_Config.nightMode);
                ImGui::Separator();
                ImGui::Checkbox("Chong bao cao (Anti-Report)", &g_Config.antiReport);

                MemoryEngine::SyncFeatures();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}
