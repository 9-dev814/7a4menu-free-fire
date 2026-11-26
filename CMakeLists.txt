cmake_minimum_required(VERSION 3.22.1)
project(hub7a4)

file(GLOB IMGUI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/imgui_*.cpp")

add_library(hub7a4 SHARED
    main.cpp
    Menu.cpp
    MemoryEngine.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(hub7a4 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hub7a4
    android
    log
    EGL
    GLESv3
)
