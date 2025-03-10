#pragma once

#include <xar_engine/renderer/gpu_asset/gpu_model.hpp>
#include <xar_engine/renderer/gpu_asset/gpu_model_data_buffer.hpp>


namespace xar_engine::renderer::gpu_asset
{
    struct GpuModelData
    {
        std::uint32_t model_index;
        GpuModelDataBufferReference gpu_model_data_buffer;
    };

    struct GpuMeshData
    {
        std::uint32_t mesh_index;
        GpuModelReference gpu_model;
    };
}
