#pragma once

#include "vec3.h"
#include "vec4.h"

namespace mellohi
{
    struct mat4
    {
        const vec4 rows[4];

        explicit mat4(f32 diagonal_scalar = 1.0f);
        explicit mat4(const vec4& row0, const vec4& row1, const vec4& row2, const vec4& row3);

        [[nodiscard]] mat4 transposed() const;

        [[nodiscard]] mat4 translated(const vec3& v) const;

        mat4 operator+(const mat4& rhs) const;
        mat4 operator-(const mat4& rhs) const;
        mat4 operator-() const;
        mat4 operator*(const f32& scalar) const;
        mat4 operator*(const mat4& rhs) const;

        bool operator==(const mat4& rhs) const;
        bool operator!=(const mat4& rhs) const;

        const vec4& operator[](usize m) const;
        const f32& operator[](usize m, usize n) const;

        static mat4 orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);
    };
}
