#pragma once

#include <xar_engine/math/epsilon.hpp>
#include <xar_engine/math/vector.hpp>


namespace xar_engine::math
{
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