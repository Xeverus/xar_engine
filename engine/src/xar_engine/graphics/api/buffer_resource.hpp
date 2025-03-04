#pragma once

#include <xar_engine/graphics/api/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class BufferTag;
    using BufferReference = TResourceReference<BufferTag>;
}