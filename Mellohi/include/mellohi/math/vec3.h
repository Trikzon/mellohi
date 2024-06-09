#pragma once

#include <iostream>

#include "mellohi/types.h"

namespace mellohi
{
    template<typename T>
    struct Vector3
    {
        static_assert(std::is_arithmetic_v<T>, "Template parameter of Vector3 must be a numeric type.");

        union { T x, r, s; };
        union { T y, g, t; };
        union { T z, b, p; };

        Vector3();
        Vector3(T x, T y, T z);

        template<typename U>
        explicit Vector3(const Vector3<U>& v);

        T magnitude_squared() const;
        f32 magnitude() const;

        [[nodiscard]] Vector3<f32> normalized() const;

        T dot(const Vector3& rhs) const;
        Vector3 cross(const Vector3& rhs) const;

        Vector3 operator+(const Vector3& rhs) const;
        Vector3 operator-(const Vector3& rhs) const;
        Vector3 operator-() const;
        Vector3 operator*(const T& scalar) const;

        bool operator==(const Vector3& rhs) const;
        bool operator!=(const Vector3& rhs) const;

        const T& operator[](usize i) const;

        template<typename U>
        friend std::ostream& operator<<(std::ostream& os, const Vector3<U>& v);
    };

    using vec3 = Vector3<f32>;
    using ivec3 = Vector3<i32>;
    using uvec3 = Vector3<u32>;
}

#include "vec3.hpp"
