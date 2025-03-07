#pragma once

#include <array>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::math
{
    template <typename T>
    struct TMatrix
    {
        using Type = T;

        union
        {
            std::array<TVector<T, 4>, 4> as_column_list;
            std::array<T, 16> as_scalar_list;
        };
    };

    using Matrix4x4f = TMatrix<float>;

    static_assert(sizeof(Matrix4x4f) == sizeof(Matrix4x4f::Type) * 16);


    Matrix4x4f make_identity_matrix();

    Matrix4x4f make_view_matrix(
        const math::Vector3f& eye_position,
        const math::Vector3f& look_at_position,
        const math::Vector3f& up_vector);

    Matrix4x4f make_projection_matrix(
        float field_of_view,
        float aspect,
        float near_z_plane,
        float far_z_plane);


    Matrix4x4f rotate_matrix(
        const Matrix4x4f& matrix,
        float angle,
        const math::Vector3f& axis);
}