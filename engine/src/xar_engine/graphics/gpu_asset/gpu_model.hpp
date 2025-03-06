#pragma once

#include <xar_engine/asset/model.hpp>

#include <xar_engine/graphics/api/buffer_resource.hpp>


namespace xar_engine::graphics::gpu_asset
{
    struct GpuMeshBufferStructure
    {
        std::uint32_t first_vertex;
        std::uint32_t first_index;

        std::uint32_t vertex_counts;
        std::uint32_t index_counts;
    };

    struct GpuModelBufferStructure
    {
        std::uint32_t first_vertex;
        std::uint32_t first_index;

        std::uint32_t vertex_counts;
        std::uint32_t index_counts;

        std::vector<GpuMeshBufferStructure> gpu_mesh_buffer_structure_list;
    };

    struct GpuModelListBufferStructure
    {
        std::uint32_t vertex_counts;
        std::uint32_t index_counts;

        std::uint32_t position_list_byte_size;
        std::uint32_t normal_list_byte_size;
        std::uint32_t texture_coord_list_byte_size;
        std::uint32_t index_list_byte_size;

        std::vector<GpuModelBufferStructure> gpu_model_buffer_structure_list;
    };

    struct GpuModelList
    {
        api::BufferReference position_buffer;
        api::BufferReference normal_buffer;
        api::BufferReference texture_coord_buffer;

        api::BufferReference index_buffer;

        GpuModelListBufferStructure buffer_structure;
    };

    GpuModelListBufferStructure make_gpu_model_list_buffer_structure(const std::vector<asset::Model>& model_list);
}