#include <xar_engine/math/matrix.hpp>

#include <cstring>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace xar_engine::math
{
    namespace
    {
        Matrix4x4f to_engine_matrix(const glm::mat4x4& glm_matrix)
        {
            Matrix4x4f engine_matrix;
            memcpy(
                engine_matrix.as_scalar_list.data(),
                glm::value_ptr(glm_matrix),
                sizeof(engine_matrix));

            return engine_matrix;
        }

        glm::mat4x4 to_glm_matrix(const Matrix4x4f engine_matrix)
        {
            glm::mat4x4 glm_matrix;
            memcpy(
                glm::value_ptr(glm_matrix),
                engine_matrix.as_scalar_list.data(),
                sizeof(engine_matrix));

            return glm_matrix;
        }

        glm::vec3 to_glm_vector(const math::Vector3f& engine_vector)
        {
            return glm::vec3{engine_vector.x, engine_vector.y, engine_vector.z};
        }
    }


    Matrix4x4f make_identity_matrix()
    {
        return to_engine_matrix(glm::mat4x4(1.0f));
    }

    Matrix4x4f make_view_matrix(
        const math::Vector3f& eye_position,
        const math::Vector3f& look_at_position,
        const math::Vector3f& up_vector)
    {
        return to_engine_matrix(
            glm::lookAt(
                to_glm_vector(eye_position),
                to_glm_vector(look_at_position),
                to_glm_vector(up_vector)));
    }

    Matrix4x4f make_projection_matrix(
        const float field_of_view,
        const float aspect,
        const float near_z_plane,
        const float far_z_plane)
    {
        return to_engine_matrix(
            glm::perspective(
                glm::radians(field_of_view),
                aspect,
                near_z_plane,
                far_z_plane));
    }

    Matrix4x4f rotate_matrix(
        const Matrix4x4f& matrix,
        const float angle,
        const math::Vector3f& axis)
    {
        return to_engine_matrix(
            glm::rotate(
                to_glm_matrix(matrix),
                glm::radians(angle),
                to_glm_vector(axis)));
    }
}
