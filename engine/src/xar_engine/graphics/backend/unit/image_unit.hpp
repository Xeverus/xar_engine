#pragma once

#include <xar_engine/graphics/api/command_buffer_reference.hpp>
#include <xar_engine/graphics/api/format.hpp>
#include <xar_engine/graphics/api/image_reference.hpp>
#include <xar_engine/graphics/api/image_view_reference.hpp>
#include <xar_engine/graphics/api/sampler_reference.hpp>

#include <xar_engine/math/vector.hpp>


namespace xar_engine::graphics::backend::unit
{
    class IImageUnit
    {
    public:
        struct MakeImageParameters;
        struct MakeImageViewParameters;
        struct MakeSamplerParameters;
        struct GenerateImageMipMapsParameters;
        struct TransitImageLayoutParameters;

    public:
        virtual ~IImageUnit();


        virtual api::ImageReference make_image(const MakeImageParameters& parameters) = 0;
        virtual api::ImageViewReference make_image_view(const MakeImageViewParameters& parameters) = 0;
        virtual api::SamplerReference make_sampler(const MakeSamplerParameters& parameters) = 0;

        virtual void generate_image_mip_maps(const GenerateImageMipMapsParameters& parameters) = 0;
        virtual void transit_image_layout(const TransitImageLayoutParameters& parameters) = 0;
    };


    struct IImageUnit::MakeImageParameters
    {
        api::EImageType image_type;
        math::Vector3u32 dimension;
        api::EFormat image_format;
        std::uint32_t mip_levels;
        std::uint32_t sample_count;
    };

    struct IImageUnit::MakeImageViewParameters
    {
        api::ImageReference& image;
        api::EImageAspect image_aspect;
        std::uint32_t mip_levels;
    };

    struct IImageUnit::MakeSamplerParameters
    {
        float mip_levels;
    };

    struct IImageUnit::GenerateImageMipMapsParameters
    {
        api::CommandBufferReference command_buffer;
        api::ImageReference image;
    };

    struct IImageUnit::TransitImageLayoutParameters
    {
        api::CommandBufferReference& command_buffer;
        api::ImageReference& image;
        api::EImageLayout new_image_layout;
    };
}