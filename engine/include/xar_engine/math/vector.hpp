#pragma once

#include <cstdint>

#include <xar_engine/math/epsilon.hpp>


namespace xar_engine::math
{
    template <typename T, std::uint32_t component_count>
    struct TVector;

    template <typename T>
    struct TVector<T, 2>
    {
        using Type = T;

        T x;
        T y;
    };

    template <typename T>
    struct TVector<T, 3>
    {
        using Type = T;

        T x;
        T y;
        T z;
    };

    template <typename T>
    struct TVector<T, 4>
    {
        using Type = T;

        T x;
        T y;
        T z;
        T w;
    };


    template <typename T>
    using Vector2 = TVector<T, 2>;

    template <typename T>
    using Vector3 = TVector<T, 3>;

    template <typename T>
    using Vector4 = TVector<T, 4>;

    using Vector2f = Vector2<float>;
    using Vector3f = Vector3<float>;
    using Vector4f = Vector4<float>;

    static_assert(sizeof(Vector2f) == sizeof(Vector2f::Type) * 2);
    static_assert(sizeof(Vector3f) == sizeof(Vector3f::Type) * 3);
    static_assert(sizeof(Vector4f) == sizeof(Vector4f::Type) * 4);


    using Vector2i32 = TVector<std::int32_t, 2>;
    using Vector3i32 = TVector<std::int32_t, 3>;
    using Vector4i32 = TVector<std::int32_t, 4>;

    static_assert(sizeof(Vector2i32) == sizeof(Vector2i32::Type) * 2);
    static_assert(sizeof(Vector3i32) == sizeof(Vector3i32::Type) * 3);
    static_assert(sizeof(Vector4i32) == sizeof(Vector4i32::Type) * 4);


    using Vector2u32 = TVector<std::uint32_t, 2>;
    using Vector3u32 = TVector<std::uint32_t, 3>;
    using Vector4u32 = TVector<std::uint32_t, 4>;

    static_assert(sizeof(Vector2u32) == sizeof(Vector2u32::Type) * 2);
    static_assert(sizeof(Vector3u32) == sizeof(Vector3u32::Type) * 3);
    static_assert(sizeof(Vector4u32) == sizeof(Vector4u32::Type) * 4);


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