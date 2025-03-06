#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::asset
{
    struct Mesh
    {
        std::vector<math::Vector3f> position_list;
        std::vector<math::Vector3f> normal_list;
        std::vector<math::Vector2f> texture_coord_list;

        std::vector<std::uint32_t> index_list;
    };

    struct ModelMetadata
    {
        std::string name;
    };

    struct Model
    {
        ModelMetadata metadata;
        std::vector<Mesh> mesh_list;
    };
}