#pragma once
#include <cstdint>

struct MenuConfig {
    // Tab 1: Aimbot
    bool aimbotEnabled = false;
    bool aimbotVisibleOnly = true;
    bool aimbotAutoFire = false;
    int aimBone = 0; // 0: Dau, 1: Than, 2: Tay, 3: Chan
    float aimFov = 130.0f;
    float aimSmooth = 3.0f;

    // Tab 2: ESP + FOV
    bool espEnabled = false;
    bool espBox = true;
    bool espLine = true;
    bool espSkeleton = false;
    bool espHealth = true;
    bool espDistance = true;
    bool drawFovCircle = true;
    float fovColor[4] = {0.68f, 0.22f, 1.00f, 0.95f};
    float espColor[4] = {0.82f, 0.35f, 1.00f, 1.00f};

    // Tab 3: All Tinh nang
    bool noRecoil = false;
    bool fastReload = false;
    bool noSpread = false;
    bool speedHack = false;
    float speedMultiplier = 1.25f;
    bool highJump = false;
    bool whiteBody = false;
    bool nightMode = false;
    bool antiReport = true;
};

extern MenuConfig g_Config;

namespace Offsets {
    // // FILL: dien RVA thuc te tu dump libil2cpp.so theo ban OB hien tai
    constexpr uintptr_t GetBonePosition   = 0x0;
    constexpr uintptr_t SetPlayerRotation = 0x0;
    constexpr uintptr_t WeaponRecoil      = 0x0;
    constexpr uintptr_t WeaponSpread      = 0x0;
    constexpr uintptr_t FastReload        = 0x0;
    constexpr uintptr_t PlayerMoveSpeed   = 0x0;
    constexpr uintptr_t PlayerJumpHeight  = 0x0;
}
