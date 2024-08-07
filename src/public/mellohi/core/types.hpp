#pragma once

#include <deque>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace mellohi
{
    using isize = std::ptrdiff_t;
    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using usize = std::size_t;
    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using f32 = float;
    using f64 = double;

    using vec2i = glm::ivec2;
    using vec3i = glm::ivec3;
    using vec4i = glm::ivec4;

    using vec2u = glm::uvec2;
    using vec3u = glm::uvec3;
    using vec4u = glm::uvec4;

    using vec2f = glm::fvec2;
    using vec3f = glm::fvec3;
    using vec4f = glm::fvec4;

    using mat2x2f = glm::fmat2x2;
    using mat2x3f = glm::fmat2x3;
    using mat2x4f = glm::fmat2x4;
    using mat3x2f = glm::fmat3x2;
    using mat3x3f = glm::fmat3x3;
    using mat3x4f = glm::fmat3x4;
    using mat4x2f = glm::fmat4x2;
    using mat4x3f = glm::fmat4x3;
    using mat4x4f = glm::fmat4x4;

    using quat = glm::quat;

    using string = std::string;
    using str = std::string_view;

    template<typename T>
    using s_ptr = std::shared_ptr<T>;
    template<typename T>
    using u_ptr = std::unique_ptr<T>;
    template<typename T>
    using w_ptr = std::weak_ptr<T>;

    using std::deque;
    template<typename K, typename V>
    using hash_map = std::unordered_map<K, V>;
    using std::optional;
    using std::filesystem::path;
    using std::tuple;
    using std::vector;
}
