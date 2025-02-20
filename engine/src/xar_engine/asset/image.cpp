#include <xar_engine/asset/image.hpp>


namespace xar_engine::asset::image
{
    std::int32_t get_byte_size(const Image& image)
    {
        return image.pixel_width * image.pixel_height * image.channel_count;
    }
}