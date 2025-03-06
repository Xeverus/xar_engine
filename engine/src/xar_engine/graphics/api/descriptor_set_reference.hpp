#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class DescriptorSetTag;
    using DescriptorSetReference = meta::TResourceReference<DescriptorSetTag>;
}