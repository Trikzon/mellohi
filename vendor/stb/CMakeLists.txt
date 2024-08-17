file(GLOB_RECURSE PUBLIC_SRC_FILES src/public/*.hpp src/public/*.cpp)
file(GLOB_RECURSE PRIVATE_SRC_FILES src/private/*.hpp src/private/*.cpp)

add_library(stb STATIC ${PUBLIC_SRC_FILES} ${PRIVATE_SRC_FILES})
target_include_directories(stb PUBLIC src/public)
