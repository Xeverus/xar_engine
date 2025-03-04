#pragma once

#include <xar_engine/graphics/api/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class CommandBufferTag;
    using CommandBufferReference = TResourceReference<CommandBufferTag>;


    enum ECommandBufferType
    {
        REUSABLE,
        ONE_TIME,
    };
}