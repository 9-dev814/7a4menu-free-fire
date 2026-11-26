#pragma once
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct LogoTexture {
    GLuint id = 0;
    int width = 0;
    int height = 0;
};

inline LogoTexture LoadAssetTexture(AAssetManager* mgr, const char* filename) {
    LogoTexture tex{};
    if (!mgr) return tex;

    AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_BUFFER);
    if (!asset) return tex;

    size_t len = AAsset_getLength(asset);
    std::vector<unsigned char> buf(len);
    AAsset_read(asset, buf.data(), len);
    AAsset_close(asset);

    int channels = 0;
    unsigned char* pixels = stbi_load_from_memory(buf.data(), (int)len, &tex.width, &tex.height, &channels, 4);
    if (!pixels) return tex;

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.width, tex.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);
    return tex;
}
