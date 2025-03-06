#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class DescriptorSetLayoutTag;
    using DescriptorSetLayoutReference = meta::TResourceReference<DescriptorSetLayoutTag>;
}