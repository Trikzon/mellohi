cmake_minimum_required(VERSION 3.28)

project(Sandbox)

file(GLOB_RECURSE HEADER_FILES include/*.h include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(Sandbox ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(Sandbox PUBLIC include)

add_custom_target(remove_sandbox_resources ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory
        ${CMAKE_BINARY_DIR}/resources/sandbox
)
add_custom_target(copy_sandbox_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/sandbox
        ${CMAKE_BINARY_DIR}/resources/sandbox
)
add_dependencies(copy_sandbox_resources remove_sandbox_resources)
add_dependencies(Sandbox copy_sandbox_resources)

target_link_libraries(Sandbox PRIVATE Mellohi)
