#pragma once

#include <cstdint>

#include <xar_engine/math/epsilon.hpp>


namespace xar_engine::math
{
    template <typename T, std::uint32_t component_count>
    struct Vector;

    template <typename T>
    struct Vector<T, 2>
    {
        T x;
        T y;
    };

    template <typename T>
    struct Vector<T, 3>
    {
        T x;
        T y;
        T z;
    };

    template <typename T>
    struct Vector<T, 4>
    {
        T x;
        T y;
        T z;
        T w;
    };


    template <typename T>
    using Vector2 = Vector<T, 2>;

    template <typename T>
    using Vector3 = Vector<T, 3>;

    template <typename T>
    using Vector4 = Vector<T, 4>;

    using Vector2f = Vector2<float>;
    using Vector3f = Vector3<float>;
    using Vector4f = Vector4<float>;

    static_assert(sizeof(Vector2f) == sizeof(float) * 2);
    static_assert(sizeof(Vector3f) == sizeof(float) * 3);
    static_assert(sizeof(Vector4f) == sizeof(float) * 4);


    using Vector2i32 = Vector<std::int32_t, 2>;
    using Vector3i32 = Vector<std::int32_t, 3>;
    using Vector4i32 = Vector<std::int32_t, 4>;

    static_assert(sizeof(Vector2i32) == sizeof(std::int32_t) * 2);
    static_assert(sizeof(Vector3i32) == sizeof(std::int32_t) * 3);
    static_assert(sizeof(Vector4i32) == sizeof(std::int32_t) * 4);


    template <typename T>
    bool operator==(
        const Vector2<T>& left,
        const Vector2<T>& right)
    {
        return
            epsilon::equal(
                left.x,
                right.x) &&
            epsilon::equal(
                left.y,
                right.y);
    }

    template <typename T>
    bool operator!=(
        const Vector2<T>& left,
        const Vector2<T>& right)
    {
        return !(left == right);
    }

    template <typename T>
    bool operator==(
        const Vector3<T>& left,
        const Vector3<T>& right)
    {
        return
            epsilon::equal(
                left.x,
                right.x) &&
            epsilon::equal(
                left.y,
                right.y) &&
            epsilon::equal(
                left.z,
                right.z);
    }

    template <typename T>
    bool operator!=(
        const Vector3<T>& left,
        const Vector3<T>& right)
    {
        return !(left == right);
    }

    template <typename T>
    bool operator==(
        const Vector4<T>& left,
        const Vector4<T>& right)
    {
        return
            epsilon::equal(
                left.x,
                right.x) &&
            epsilon::equal(
                left.y,
                right.y) &&
            epsilon::equal(
                left.z,
                right.z) &&
            epsilon::equal(
                left.w,
                right.w);
    }

    template <typename T>
    bool operator!=(
        const Vector4<T>& left,
        const Vector4<T>& right)
    {
        return !(left == right);
    }
}