cmake_minimum_required(VERSION 3.28)

project(voxelfi)

file(GLOB_RECURSE HEADER_FILES include/*.h include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_library(voxelfi STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(voxelfi PUBLIC include)

add_custom_target(remove_voxelfi_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_BINARY_DIR}/assets/voxelfi
)
add_custom_target(copy_voxelfi_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/voxelfi
        ${CMAKE_BINARY_DIR}/assets/voxelfi
)
add_dependencies(copy_voxelfi_assets remove_voxelfi_assets)
add_dependencies(voxelfi copy_voxelfi_assets)

if (NOT EMSCRIPTEN)
    add_subdirectory(vendor/glfw)
else()
    # Emscripten has built-in support for GLFW but requires the `-sUSE_GLFW=3` link option:
    add_library(glfw INTERFACE)
    target_link_options(glfw INTERFACE -sUSE_GLFW=3)
endif()

add_subdirectory(vendor/glfw3webgpu)

add_subdirectory(vendor/dawn)

add_subdirectory(vendor/imgui)

target_link_libraries(voxelfi PUBLIC glfw glfw3webgpu webgpu imgui)
