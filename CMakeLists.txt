cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(mellohi)

option(MH_GRAPHICS_VULKAN "Enable Vulkan graphics backend" ON)
option(MH_PLATFORM_GLFW   "Enable GLFW platform backend"   ON)

set(INCLUDES
    include/mellohi/assets/asset.hpp
    include/mellohi/assets/asset_cache.hpp
    include/mellohi/assets/asset_path.hpp
    include/mellohi/assets/asset_registry.hpp
    include/mellohi/assets/toml_asset.hpp
    include/mellohi/core/color.hpp
    include/mellohi/core/config_assets.hpp
    include/mellohi/core/engine.hpp
    include/mellohi/core/logger.hpp
    include/mellohi/core/types.hpp
    include/mellohi/events/event.hpp
    include/mellohi/events/event_dispatcher.hpp
    include/mellohi/platform/platform.hpp
    include/mellohi/platform/window.hpp
)

set(SOURCES
    src/mellohi/assets/asset.cpp
    src/mellohi/assets/asset_cache.cpp
    src/mellohi/assets/asset_path.cpp
    src/mellohi/assets/asset_registry.cpp
    src/mellohi/assets/toml_asset.cpp
    src/mellohi/core/color.cpp
    src/mellohi/core/config_assets.cpp
    src/mellohi/core/engine.cpp
    src/mellohi/events/event_dispatcher.cpp
    src/mellohi/platform/window.cpp
)

if(MH_PLATFORM_GLFW)
    list(APPEND INCLUDES
        include/mellohi/platform/glfw/glfw.hpp
        include/mellohi/platform/glfw/glfw_platform.hpp
        include/mellohi/platform/glfw/glfw_window.hpp
    )
    
    list(APPEND SOURCES
        src/mellohi/platform/glfw/glfw.cpp
        src/mellohi/platform/glfw/glfw_platform.cpp
        src/mellohi/platform/glfw/glfw_window.cpp
    )
endif()

add_library(mellohi STATIC ${SOURCES} ${INCLUDES})
target_include_directories(mellohi PUBLIC include)

target_compile_definitions(mellohi PUBLIC MH_ENGINE_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(mellohi PUBLIC MH_DEBUG_MODE)
endif()

if(MH_GRAPHICS_VULKAN)
    find_package(Vulkan REQUIRED)
    find_library(SHADERC_COMBINED_LIB shaderc_combined)
    if (NOT SHADERC_COMBINED_LIB)
        message(FATAL_ERROR "shaderc_combined not found.")
    endif()

    target_link_libraries(mellohi PUBLIC Vulkan::Vulkan ${SHADERC_COMBINED_LIB})
    target_compile_definitions(mellohi PUBLIC MH_GRAPHICS_VULKAN)
    target_compile_definitions(mellohi PUBLIC GLFW_INCLUDE_VULKAN)
endif()

if(MH_PLATFORM_GLFW)
    add_subdirectory(external/glfw)
    target_link_libraries(mellohi PUBLIC glfw)
    target_compile_definitions(mellohi PUBLIC MH_PLATFORM_GLFW)
endif()

add_subdirectory(external/glm)
target_link_libraries(mellohi PUBLIC glm)
target_compile_definitions(mellohi PUBLIC GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_ENABLE_EXPERIMENTAL)

add_subdirectory(external/tomlplusplus)
target_link_libraries(mellohi PUBLIC tomlplusplus::tomlplusplus)

add_subdirectory(games)
