#include "mellohi/math/mat4.h"

namespace mellohi
{
    mat4::mat4(const f32 diagonal_scalar) : rows
        {
            vec4{diagonal_scalar, 0, 0, 0},
            vec4{0, diagonal_scalar, 0, 0},
            vec4{0, 0, diagonal_scalar, 0},
            vec4{0, 0, 0, diagonal_scalar}
        }
    { }

    mat4::mat4(const vec4& row0, const vec4& row1, const vec4& row2, const vec4& row3)
        : rows{ row0, row1, row2, row3 } { }

    mat4 mat4::transposed() const
    {
        return mat4
        {
            vec4{ rows[0][0], rows[1][0], rows[2][0], rows[3][0] },
            vec4{ rows[0][1], rows[1][1], rows[2][1], rows[3][1] },
            vec4{ rows[0][2], rows[1][2], rows[2][2], rows[3][2] },
            vec4{ rows[0][3], rows[1][3], rows[2][3], rows[3][3] }
        };
    }

    mat4 mat4::translated(const vec3& v) const
    {
        const mat4 translation_matrix
        {
            vec4(1.0, 0.0, 0.0, v.x),
            vec4(0.0, 1.0, 0.0, v.y),
            vec4(0.0, 0.0, 1.0, v.z),
            vec4(0.0, 0.0, 0.0, 1.0),
        };

        return *this * translation_matrix;
    }

    mat4 mat4::operator+(const mat4& rhs) const
    {
        return mat4
        {
            rows[0] + rhs.rows[0],
            rows[1] + rhs.rows[1],
            rows[2] + rhs.rows[2],
            rows[3] + rhs.rows[3]
        };
    }

    mat4 mat4::operator-(const mat4& rhs) const
    {
        return mat4
        {
            rows[0] - rhs.rows[0],
            rows[1] - rhs.rows[1],
            rows[2] - rhs.rows[2],
            rows[3] - rhs.rows[3]
        };
    }

    mat4 mat4::operator-() const
    {
        return mat4{ -rows[0], -rows[1], -rows[2], -rows[3] };
    }

    mat4 mat4::operator*(const f32& scalar) const
    {
        return mat4
        {
            rows[0] * scalar,
            rows[1] * scalar,
            rows[2] * scalar,
            rows[3] * scalar
        };
    }

    mat4 mat4::operator*(const mat4& rhs) const
    {
        const mat4 rhs_t = rhs.transposed();

        return mat4
        {
            vec4{ rows[0].dot(rhs_t[0]), rows[0].dot(rhs_t[1]), rows[0].dot(rhs_t[2]), rows[0].dot(rhs_t[3]) },
            vec4{ rows[1].dot(rhs_t[0]), rows[1].dot(rhs_t[1]), rows[1].dot(rhs_t[2]), rows[1].dot(rhs_t[3]) },
            vec4{ rows[2].dot(rhs_t[0]), rows[2].dot(rhs_t[1]), rows[2].dot(rhs_t[2]), rows[2].dot(rhs_t[3]) },
            vec4{ rows[3].dot(rhs_t[0]), rows[3].dot(rhs_t[1]), rows[3].dot(rhs_t[2]), rows[3].dot(rhs_t[3]) }
        };
    }

    bool mat4::operator==(const mat4& rhs) const
    {
        return rows[0] == rhs.rows[0] && rows[1] == rhs.rows[1] && rows[2] == rhs.rows[2] && rows[3] == rhs.rows[3];
    }

    bool mat4::operator!=(const mat4& rhs) const
    {
        return !(*this == rhs);
    }

    const vec4& mat4::operator[](const usize m) const
    {
        MH_ASSERT(m < 4, "Cannot index mat4 with index greater than 3 (m = {}).", m);

        return rows[m];
    }

    const f32 &mat4::operator[](const usize m, const usize n) const
    {
        return (*this)[m][n];
    }

    // Reference: https://www.songho.ca/opengl/gl_projectionmatrix.html
    mat4 mat4::orthographic(const f32 left, const f32 right,
                            const f32 bottom, const f32 top,
                            const f32 near, const f32 far)
    {
        const f32 µ_x = 2.0f / (right - left);
        const f32 ß_x = -(right + left) / (right - left);
        const f32 µ_y = 2.0f / (top - bottom);
        const f32 ß_y = -(top + bottom) / (top - bottom);
        const f32 µ_z = -2.0f / (far - near);
        const f32 ß_z = -(far + near) / (far - near);

        return mat4
        {
            vec4{ µ_x, 0.0, 0.0, ß_x },
            vec4{ 0.0, µ_y, 0.0, ß_y },
            vec4{ 0.0, 0.0, µ_z, ß_z },
            vec4{ 0.0, 0.0, 0.0, 1.0 }
        };
    }
}
