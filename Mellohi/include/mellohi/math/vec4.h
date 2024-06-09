#pragma once

#include <iostream>

#include "mellohi/types.h"

namespace mellohi
{
    template<typename T>
    struct Vector4
    {
        static_assert(std::is_arithmetic_v<T>, "Template parameter of Vector4 must be a numeric type.");

        union { T x, r, s; };
        union { T y, g, t; };
        union { T z, b, p; };
        union { T w, a, q; };

        Vector4();
        Vector4(T x, T y, T z, T w);

        template<typename U>
        explicit Vector4(const Vector4<U>& v);

        T magnitude_squared() const;
        f32 magnitude() const;

        [[nodiscard]] Vector4<f32> normalized() const;

        T dot(const Vector4& rhs) const;

        Vector4 operator+(const Vector4& rhs) const;
        Vector4 operator-(const Vector4& rhs) const;
        Vector4 operator-() const;
        Vector4 operator*(const T& scalar) const;

        bool operator==(const Vector4& rhs) const;
        bool operator!=(const Vector4& rhs) const;

        const T& operator[](usize i) const;

        template<typename U>
        friend std::ostream& operator<<(std::ostream& os, const Vector4<U>& v);
    };

    using vec4 = Vector4<f32>;
    using ivec4 = Vector4<i32>;
    using uvec4 = Vector4<u32>;
}

#include "vec4.hpp"
