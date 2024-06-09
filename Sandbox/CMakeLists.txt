cmake_minimum_required(VERSION 3.28)

project(Sandbox)

file(GLOB_RECURSE HEADER_FILES include/*.h include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(Sandbox ${HEADER_FILES} ${SOURCE_FILES})
target_include_directories(Sandbox PUBLIC include)

target_link_libraries(Sandbox PRIVATE Mellohi)
