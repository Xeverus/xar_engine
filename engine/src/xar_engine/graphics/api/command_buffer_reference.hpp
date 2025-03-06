#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class CommandBufferTag;
    using CommandBufferReference = meta::TResourceReference<CommandBufferTag>;


    enum class ECommandBufferType
    {
        REUSABLE,
        ONE_TIME,
    };
}