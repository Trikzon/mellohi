cmake_minimum_required(VERSION 3.28)

project(Glad)
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
