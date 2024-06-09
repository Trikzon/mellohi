#pragma once

#include "mellohi/log.h"

namespace mellohi
{
    template<typename T>
    Vector4<T>::Vector4() : x(0), y(0), z(0), w(0) { }

    template<typename T>
    Vector4<T>::Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { }

    template<typename T>
    template<typename U>
    Vector4<T>::Vector4(const Vector4<U> &v)
        : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)), w(static_cast<T>(v.w)) { }

    template<typename T>
    T Vector4<T>::magnitude_squared() const
    {
        return x * x + y * y + z * z + w * w;
    }

    template<typename T>
    f32 Vector4<T>::magnitude() const
    {
        return std::sqrtf(magnitude_squared());
    }

    template<typename T>
    Vector4<f32> Vector4<T>::normalized() const
    {
        const f32 magnitude = Vector4::magnitude();
        MH_ASSERT(magnitude != 0, "Cannot normalize a zero vector {}.", *this);

        return { x / magnitude, y / magnitude, z / magnitude, w / magnitude };
    }

    template<typename T>
    T Vector4<T>::dot(const Vector4 &rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
    }

    template<typename T>
    Vector4<T> Vector4<T>::operator+(const Vector4 &rhs) const
    {
        return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }

    template<typename T>
    Vector4<T> Vector4<T>::operator-(const Vector4 &rhs) const
    {
        return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }

    template<typename T>
    Vector4<T> Vector4<T>::operator-() const
    {
        return { -x, -y, -z, -w };
    }

    template<typename T>
    Vector4<T> Vector4<T>::operator*(const T &scalar) const
    {
        return { x * scalar, y * scalar, z * scalar, w * scalar };
    }

    template<typename T>
    bool Vector4<T>::operator==(const Vector4 &rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    template<typename T>
    bool Vector4<T>::operator!=(const Vector4 &rhs) const
    {
        return !(*this == rhs);
    }

    template<typename T>
    const T & Vector4<T>::operator[](usize i) const
    {
        switch (i)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
            default:
                MH_ASSERT(false, "Cannot index Vector4 with index greater than 3 (i = {}).", i);
        }
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector4<T>& v)
    {
        return os << std::format("{}", v);
    }
}

template<typename T>
struct std::formatter<mellohi::Vector4<T>> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const mellohi::Vector4<T>& v, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "({}, {}, {}, {})", v.x, v.y, v.z, v.w);
    }
};
