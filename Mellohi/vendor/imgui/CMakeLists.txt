cmake_minimum_required(VERSION 3.28)

file(GLOB HEADER_FILES imgui/*.h)
file(GLOB SOURCE_FILES imgui/*.cpp)

message(${HEADER_FILES})

project(imgui)
add_library(imgui STATIC
        ${SOURCE_FILES}
        ${HEADER_FILES}
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_glfw.h
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/backends/imgui_impl_opengl3.h
        imgui/misc/cpp/imgui_stdlib.cpp
        imgui/misc/cpp/imgui_stdlib.h
)
target_include_directories(imgui PUBLIC imgui)

target_link_libraries(imgui PRIVATE glfw)
