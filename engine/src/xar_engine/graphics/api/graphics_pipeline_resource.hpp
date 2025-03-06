#pragma once

#include <xar_engine/meta/resource_reference.hpp>

#include <xar_engine/graphics/api/format.hpp>


namespace xar_engine::graphics::api
{
    enum class GrahicsPipelineTag;
    using GraphicsPipelineReference = meta::TResourceReference<GrahicsPipelineTag>;

    enum class VertexInputBindingRate
    {
        PER_VERTEX,
        PER_INSTANCE,
    };

    struct VertexInputAttribute
    {
        std::uint32_t binding_index;
        std::uint32_t location;
        std::uint32_t offset;
        EFormat format;
    };

    struct VertexInputBinding
    {
        std::uint32_t binding_index;
        std::uint32_t stride;
        VertexInputBindingRate input_rate;
    };
}