cmake_minimum_required(VERSION 3.28)

project(Sandbox)

file(GLOB_RECURSE HEADER_FILES include/*.h include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(Sandbox ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(Sandbox PUBLIC include)

add_custom_target(remove_sandbox_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_BINARY_DIR}/assets/sandbox
)
add_custom_target(copy_sandbox_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/sandbox
        ${CMAKE_BINARY_DIR}/assets/sandbox
)
add_dependencies(copy_sandbox_assets remove_sandbox_assets)
add_dependencies(Sandbox copy_sandbox_assets)

target_link_libraries(Sandbox PRIVATE Mellohi)
