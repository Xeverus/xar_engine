#pragma once

#include <xar_engine/meta/enum.hpp>
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

    struct VertexInputBinding
    {
        std::uint32_t binding_index;
        std::uint32_t stride;
        VertexInputBindingRate input_rate;
    };

    struct VertexInputAttribute
    {
        std::uint32_t binding_index;
        std::uint32_t location;
        std::uint32_t offset;
        EFormat format;
    };
}

ENUM_TO_STRING(xar_engine::graphics::api::VertexInputBindingRate);