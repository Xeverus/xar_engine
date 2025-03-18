#pragma once

#include <xar_engine/meta/enum.hpp>
#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class DescriptorSetTag;
    using DescriptorSetReference = meta::TResourceReference<DescriptorSetTag>;

    enum class EDescriptorType
    {
        UNIFORM_BUFFER,
        SAMPLED_IMAGE,
    };
}

ENUM_TO_STRING(xar_engine::graphics::api::EDescriptorType);