#pragma once

#include <xar_engine/asset/model.hpp>

#include <xar_engine/graphics/api/buffer_reference.hpp>


namespace xar_engine::renderer::gpu_asset
{
    struct GpuMeshDataBufferStructure
    {
        std::uint32_t first_vertex;
        std::uint32_t first_index;

        std::uint32_t vertex_counts;
        std::uint32_t index_counts;
    };

    struct GpuModelDataBufferStructure
    {
        std::uint32_t first_vertex;
        std::uint32_t first_index;

        std::uint32_t vertex_counts;
        std::uint32_t index_counts;

        std::vector<GpuMeshDataBufferStructure> gpu_mesh_buffer_structure_list;
    };

    struct GpuModelDataListBufferStructure
    {
        std::uint32_t vertex_counts;
        std::uint32_t index_counts;

        std::uint32_t position_list_byte_size;
        std::uint32_t normal_list_byte_size;
        std::uint32_t texture_coord_list_byte_size;
        std::uint32_t index_list_byte_size;

        std::vector<GpuModelDataBufferStructure> gpu_model_buffer_structure_list;
    };

    GpuModelDataListBufferStructure make_gpu_model_data_list_buffer_structure(const std::vector<asset::Model>& model_list);


    struct GpuModelDataBuffer
    {
        graphics::api::BufferReference position_buffer;
        graphics::api::BufferReference normal_buffer;
        graphics::api::BufferReference texture_coord_buffer;
        graphics::api::BufferReference index_buffer;

        GpuModelDataListBufferStructure structure;
    };

    enum class GpuModelDataBufferTag;
    using GpuModelDataBufferReference = meta::TResourceReference<GpuModelDataBufferTag>;
}