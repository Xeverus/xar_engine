#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::renderer::gpu_asset
{
    enum class GpuMeshTag;
    using GpuMeshReference = meta::TResourceReference<GpuMeshTag>;

    enum class GpuModelTag;
    using GpuModelReference = meta::TResourceReference<GpuModelTag>;

    struct GpuModel
    {
        GpuModelReference gpu_model;
        std::vector<GpuMeshReference> gpu_mesh;
    };
}