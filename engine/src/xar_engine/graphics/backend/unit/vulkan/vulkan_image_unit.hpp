#pragma once

#include <xar_engine/graphics/backend/unit/image_unit.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::unit::vulkan
{
    class IVulkanImageUnit
        : public IImageUnit
          , public backend::vulkan::SharedVulkanGraphicsBackendState
    {
    public:
        using SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState;

        api::ImageReference make_image(const MakeImageParameters& parameters) override;
        api::ImageViewReference make_image_view(const MakeImageViewParameters& parameters) override;
        api::SamplerReference make_sampler(const MakeSamplerParameters& parameters) override;

        void generate_image_mip_maps(const GenerateImageMipMapsParameters& parameters) override;
        void transit_image_layout(const TransitImageLayoutParameters& parameters) override;
    };
}