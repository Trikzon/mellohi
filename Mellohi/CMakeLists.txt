cmake_minimum_required(VERSION 3.28)

project(Mellohi)

file(GLOB_RECURSE HEADER_FILES include/*.h include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_library(Mellohi STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(Mellohi PUBLIC include)

add_custom_target(remove_mellohi_resources ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_BINARY_DIR}/resources/mellohi
)
add_custom_target(copy_mellohi_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/mellohi
        ${CMAKE_BINARY_DIR}/resources/mellohi
)
add_dependencies(copy_mellohi_resources remove_mellohi_resources)
add_dependencies(Mellohi copy_mellohi_resources)

add_subdirectory(vendor/glad)
target_link_libraries(Mellohi PRIVATE glad)

find_package(OpenGL REQUIRED)
target_link_libraries(Mellohi PRIVATE OpenGL::GL)

add_subdirectory(vendor/GLFW)
target_link_libraries(Mellohi PRIVATE glfw)

add_subdirectory(vendor/glm)
target_link_libraries(Mellohi PRIVATE glm)

add_subdirectory(vendor/imgui)
target_link_libraries(Mellohi PUBLIC imgui)

add_subdirectory(vendor/stb)
target_link_libraries(Mellohi PRIVATE stb)
