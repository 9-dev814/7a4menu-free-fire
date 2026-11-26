#include <jni.h>
#include <GLES3/gl3.h>
#include <android/asset_manager_jni.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "TextureLoader.h"
#include "Menu.h"
#include "MemoryEngine.h"

static bool g_Initialized = false;
static int g_Width = 0;
static int g_Height = 0;

extern "C" {

JNIEXPORT void JNICALL
Java_com_hub7a4_ffmenu_OverlayService_nativeInit(JNIEnv* env, jclass, jobject assetManager, jint width, jint height) {
    g_Width = width;
    g_Height = height;

    AAssetManager* mgr = AAssetManager_fromJava(env, assetManager);

    if (!g_Initialized) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.DisplaySize = ImVec2((float)width, (float)height);

        Menu::ApplyTheme();
        ImGui_ImplOpenGL3_Init("#version 300 es");
        MemoryEngine::AttachGame();
        g_Initialized = true;
    }

    LogoTexture logo = LoadAssetTexture(mgr, "logo.png");
    if (logo.id != 0) {
        Menu::SetLogoTexture(logo.id, logo.width, logo.height);
    }
}

JNIEXPORT void JNICALL
Java_com_hub7a4_ffmenu_OverlayService_nativeRender(JNIEnv*, jclass) {
    if (!g_Initialized) return;

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)g_Width, (float)g_Height);

    glViewport(0, 0, g_Width, g_Height);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    Menu::Draw();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

JNIEXPORT void JNICALL
Java_com_hub7a4_ffmenu_OverlayService_nativeTouch(JNIEnv*, jclass, jint action, jfloat x, jfloat y) {
    if (!g_Initialized) return;
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
    if (action == 0 || action == 5) {
        io.AddMouseButtonEvent(0, true);
    } else if (action == 1 || action == 6) {
        io.AddMouseButtonEvent(0, false);
    }
}

JNIEXPORT jboolean JNICALL
Java_com_hub7a4_ffmenu_OverlayService_nativeWantCaptureMouse(JNIEnv*, jclass) {
    if (!g_Initialized) return JNI_FALSE;
    return ImGui::GetIO().WantCaptureMouse ? JNI_TRUE : JNI_FALSE;
}

}
