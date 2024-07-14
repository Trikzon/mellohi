cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)

project(mellohi)

file(GLOB_RECURSE HEADER_FILES include/*.h include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_library(mellohi STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(mellohi PUBLIC include)

add_custom_target(remove_mellohi_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_BINARY_DIR}/assets/mellohi
)
add_custom_target(copy_mellohi_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/mellohi
        ${CMAKE_BINARY_DIR}/assets/mellohi
)
add_dependencies(copy_mellohi_assets remove_mellohi_assets)
add_dependencies(mellohi copy_mellohi_assets)

if (NOT EMSCRIPTEN)
    add_subdirectory(vendor/glfw)
else()
    # Emscripten has built-in support for GLFW but requires the `-sUSE_GLFW=3` link option:
    add_library(glfw INTERFACE)
    target_link_options(glfw INTERFACE -sUSE_GLFW=3)
endif()

add_subdirectory(vendor/glfw3webgpu)
add_subdirectory(vendor/dawn)
add_subdirectory(vendor/flecs)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/tinyobjloader)

target_link_libraries(mellohi PUBLIC glfw glfw3webgpu webgpu flecs glm imgui tinyobjloader)
target_compile_definitions(mellohi PUBLIC
        GLM_FORCE_DEPTH_ZERO_TO_ONE
        GLM_FORCE_LEFT_HANDED
        GLM_ENABLE_EXPERIMENTAL
)

add_subdirectory(examples)
