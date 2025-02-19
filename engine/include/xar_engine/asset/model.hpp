#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::asset
{
    struct TextureCoords
    {
        std::vector<float> coords;
        std::uint32_t channel_count;
    };

    struct Mesh
    {
        std::vector<math::Vector3f> vertices;
        std::vector<math::Vector3f> normals;
        std::vector<TextureCoords> tex_coords;

        std::vector<std::uint32_t> indices;
    };

    struct Model
    {
        std::string name;
        std::vector<Mesh> meshes;
    };
}