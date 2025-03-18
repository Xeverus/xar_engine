#pragma once

#include <xar_engine/meta/resource_reference.hpp>


namespace xar_engine::graphics::api
{
    enum class BufferTag;
    using BufferReference = meta::TResourceReference<BufferTag>;

    struct BufferUpdate
    {
        const void* data;
        std::uint32_t byte_offset;
        std::uint32_t byte_size;
    };
}