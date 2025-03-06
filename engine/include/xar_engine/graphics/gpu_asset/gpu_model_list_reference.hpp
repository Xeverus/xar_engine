#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::gpu_asset
{
    enum class GpuModelListTag;
    using GpuModelListReference = meta::TResourceReference<GpuModelListTag>;
}