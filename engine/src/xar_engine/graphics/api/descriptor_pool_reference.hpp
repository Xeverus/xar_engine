#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class DescriptorPoolTag;
    using DescriptorPoolReference = meta::TResourceReference<DescriptorPoolTag>;

    enum class EDescriptorPoolType
    {
        UNIFORM_BUFFER,
        SAMPLED_IMAGE,
    };
}