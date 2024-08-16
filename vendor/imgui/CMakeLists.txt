# Define an ImGui target that fits our use case
add_library(imgui STATIC
        # Among the different backends available, we are interested in connecting
        # the GUI to GLFW andWebGPU:
        imgui/backends/imgui_impl_wgpu.h
        imgui/backends/imgui_impl_wgpu.cpp
        imgui/backends/imgui_impl_glfw.h
        imgui/backends/imgui_impl_glfw.cpp

        # Bonus to add some C++ specific features (the core ImGUi is a C library)
        imgui/misc/cpp/imgui_stdlib.h
        imgui/misc/cpp/imgui_stdlib.cpp

        # The core ImGui files
        imgui/imconfig.h
        imgui/imgui.h
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_internal.h
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
)

target_include_directories(imgui PUBLIC imgui)
target_link_libraries(imgui PUBLIC webgpu glfw)
