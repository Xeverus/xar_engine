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
        std::vector<math::Vector3f> positions;
        std::vector<math::Vector3f> normals;
        std::vector<TextureCoords> texture_coords;

        std::vector<std::uint32_t> indices;
    };

    struct ModelMetadata
    {
        std::string name;
    };

    struct Model
    {
        ModelMetadata metadata;
        std::vector<Mesh> meshes;
    };
}