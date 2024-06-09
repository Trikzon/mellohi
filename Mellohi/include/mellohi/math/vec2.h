#pragma once

#include <iostream>

#include "mellohi/types.h"

namespace mellohi
{
    template<typename T>
    struct Vector2
    {
        static_assert(std::is_arithmetic_v<T>, "Template parameter of Vector2 must be a numeric type.");

        union { T x, s; };
        union { T y, t; };

        Vector2();
        Vector2(T x, T y);

        template<typename U>
        explicit Vector2(const Vector2<U>& v);

        T magnitude_squared() const;
        f32 magnitude() const;

        [[nodiscard]] Vector2<f32> normalized() const;

        T dot(const Vector2& rhs) const;

        Vector2 operator+(const Vector2& rhs) const;
        Vector2 operator-(const Vector2& rhs) const;
        Vector2 operator-() const;
        Vector2 operator*(const T& scalar) const;

        bool operator==(const Vector2& rhs) const;
        bool operator!=(const Vector2& rhs) const;

        const T& operator[](usize i) const;

        template<typename U>
        friend std::ostream& operator<<(std::ostream& os, const Vector2<U>& v);
    };

    using vec2 = Vector2<f32>;
    using ivec2 = Vector2<i32>;
    using uvec2 = Vector2<u32>;
}

#include "vec2.hpp"
