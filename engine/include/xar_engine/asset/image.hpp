#pragma once

#include <cstdint>
#include <vector>


namespace xar_engine::asset
{
    struct Image
    {
        std::vector<std::uint8_t> bytes;
        std::uint32_t channel_count;
        std::uint32_t pixel_width;
        std::uint32_t pixel_height;
        std::uint32_t mip_level_count;
    };
}

namespace xar_engine::asset::image
{
    std::uint32_t get_byte_size(const Image& image);
}
