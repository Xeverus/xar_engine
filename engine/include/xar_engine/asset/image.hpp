#pragma once

#include <cstdint>
#include <vector>


namespace xar_engine::asset
{
    struct Image
    {
        std::vector<std::uint8_t> bytes;
        std::int32_t channel_count;
        std::int32_t pixel_width;
        std::int32_t pixel_height;
    };
}

namespace xar_engine::asset::image
{
    std::int32_t get_byte_size(const Image& image);
}
