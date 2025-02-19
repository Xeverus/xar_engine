#pragma once

#include <cstdint>


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


    using Vector2f = Vector<float, 2>;
    using Vector3f = Vector<float, 3>;
    using Vector4f = Vector<float, 4>;

    static_assert(sizeof(Vector2f) == sizeof(float) * 2);
    static_assert(sizeof(Vector3f) == sizeof(float) * 3);
    static_assert(sizeof(Vector4f) == sizeof(float) * 4);


    using Vector2i32 = Vector<std::int32_t, 2>;
    using Vector3i32 = Vector<std::int32_t, 3>;
    using Vector4i32 = Vector<std::int32_t, 4>;

    static_assert(sizeof(Vector2i32) == sizeof(std::int32_t) * 2);
    static_assert(sizeof(Vector3i32) == sizeof(std::int32_t) * 3);
    static_assert(sizeof(Vector4i32) == sizeof(std::int32_t) * 4);
}