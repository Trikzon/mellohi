#pragma once

#include "mellohi/log.h"

namespace mellohi
{
    template<typename T>
    Vector3<T>::Vector3() : x(0), y(0), z(0) { }

    template<typename T>
    Vector3<T>::Vector3(T x, T y, T z) : x(x), y(y), z(z) { }

    template<typename T>
    template<typename U>
    Vector3<T>::Vector3(const Vector3<U>& v)
        : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)) { }

    template<typename T>
    T Vector3<T>::magnitude_squared() const
    {
        return x * x + y * y + z * z;
    }

    template<typename T>
    f32 Vector3<T>::magnitude() const
    {
        return std::sqrtf(magnitude_squared());
    }

    template<typename T>
    Vector3<f32> Vector3<T>::normalized() const
    {
        const f32 magnitude = Vector3::magnitude();
        MH_ASSERT(magnitude != 0, "Cannot normalize a zero vector {}.", *this);

        return { x / magnitude, y / magnitude, z / magnitude };
    }

    template<typename T>
    T Vector3<T>::dot(const Vector3 &rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    template<typename T>
    Vector3<T> Vector3<T>::cross(const Vector3 &rhs) const
    {
        return Vector3
        {
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        };
    }

    template<typename T>
    Vector3<T> Vector3<T>::operator+(const Vector3 &rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z };
    }

    template<typename T>
    Vector3<T> Vector3<T>::operator-(const Vector3 &rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z };
    }

    template<typename T>
    Vector3<T> Vector3<T>::operator-() const
    {
        return { -x, -y, -z };
    }

    template<typename T>
    Vector3<T> Vector3<T>::operator*(const T &scalar) const
    {
        return { x * scalar, y * scalar, z * scalar };
    }

    template <typename T>
    bool Vector3<T>::operator==(const Vector3& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    template <typename T>
    bool Vector3<T>::operator!=(const Vector3& rhs) const
    {
        return !(*this == rhs);
    }

    template<typename T>
    const T& Vector3<T>::operator[](usize i) const
    {
        switch (i)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            default:
                MH_ASSERT(false, "Cannot index Vector3 with index greater than 2 (i = {}).", i);
        }
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector3<T>& v)
    {
        return os << std::format("{}", v);
    }
}

template<typename T>
struct std::formatter<mellohi::Vector3<T>> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const mellohi::Vector3<T>& v, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
    }
};
