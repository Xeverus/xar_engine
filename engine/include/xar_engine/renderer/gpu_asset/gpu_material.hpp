#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::renderer::gpu_asset
{
    enum class GpuMaterialTag;
    using GpuMaterialReference = meta::TResourceReference<GpuMaterialTag>;
}