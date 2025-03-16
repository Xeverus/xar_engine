#pragma once

#include <xar_engine/graphics/api/image_reference.hpp>
#include <xar_engine/graphics/api/image_view_reference.hpp>
#include <xar_engine/graphics/api/sampler_reference.hpp>


namespace xar_engine::renderer::gpu_asset
{
    struct GpuMaterialData
    {
        graphics::api::ImageReference image;
        graphics::api::ImageViewReference image_view;
        graphics::api::SamplerReference sampler;
        std::uint32_t index;
    };
}
