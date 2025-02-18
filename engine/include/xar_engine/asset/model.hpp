#pragma once

#include <cstdint>
#include <vector>


namespace xar_engine::asset
{
    struct Float3
    {
        float x;
        float y;
        float z;
    };

    static_assert(sizeof(Float3) == sizeof(float) * 3);

    struct Mesh
    {
        std::vector<Float3> vertices;
        std::vector<std::uint32_t> indices;

        std::vector<std::vector<float>> tex_coords;
        std::vector<std::uint32_t> tex_coords_channel_count;
    };

    struct Model
    {
        std::vector<Mesh> meshes;
    };
}