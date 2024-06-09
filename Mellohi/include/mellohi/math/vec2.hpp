#pragma once

#include "mellohi/log.h"

namespace mellohi
{
    template<typename T>
    Vector2<T>::Vector2() : x(0), y(0) { }

    template<typename T>
    Vector2<T>::Vector2(T x, T y) : x(x), y(y) { }

    template<typename T>
    template<typename U>
    Vector2<T>::Vector2(const Vector2<U> &v)
        : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) { }

    template<typename T>
    T Vector2<T>::magnitude_squared() const
    {
        return x * x + y * y;
    }

    template<typename T>
    f32 Vector2<T>::magnitude() const
    {
        return std::sqrtf(magnitude_squared());
    }

    template<typename T>
    Vector2<f32> Vector2<T>::normalized() const
    {
        const f32 magnitude = Vector2::magnitude();
        MH_ASSERT(magnitude != 0, "Cannot normalize a zero vector {}.", *this);

        return { x / magnitude, y / magnitude };
    }

    template<typename T>
    T Vector2<T>::dot(const Vector2 &rhs) const
    {
        return x * rhs.x + y * rhs.y;
    }

    template<typename T>
    Vector2<T> Vector2<T>::operator+(const Vector2 &rhs) const
    {
        return { x + rhs.x, y + rhs.y };
    }

    template<typename T>
    Vector2<T> Vector2<T>::operator-(const Vector2 &rhs) const
    {
        return { x - rhs.x, y - rhs.y };
    }

    template<typename T>
    Vector2<T> Vector2<T>::operator-() const
    {
        return { -x, -y };
    }

    template<typename T>
    Vector2<T> Vector2<T>::operator*(const T &scalar) const
    {
        return { x * scalar, y * scalar };
    }

    template<typename T>
    bool Vector2<T>::operator==(const Vector2 &rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    template<typename T>
    bool Vector2<T>::operator!=(const Vector2 &rhs) const
    {
        return !(*this == rhs);
    }

    template<typename T>
    const T & Vector2<T>::operator[](usize i) const
    {
        switch (i)
        {
            case 0:
                return x;
            case 1:
                return y;
            default:
                MH_ASSERT(false, "Cannot index Vector2 with index greater than 1 (i = {}).", i);
        }
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Vector2<T>& v)
    {
        return os << std::format("{}", v);
    }
}

template<typename T>
struct std::formatter<mellohi::Vector2<T>> : std::formatter<std::string_view>
{
    template<typename FormatContext>
    auto format(const mellohi::Vector2<T>& v, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "({}, {})", v.x, v.y);
    }
};
