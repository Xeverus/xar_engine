#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class BufferTag;
    using BufferReference = meta::TResourceReference<BufferTag>;
}