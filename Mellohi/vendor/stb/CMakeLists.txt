cmake_minimum_required(VERSION 3.28)

project(stb)
add_library(stb STATIC src/implementation.cpp)
target_include_directories(stb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
