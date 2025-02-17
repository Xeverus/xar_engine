#include <xar_engine/asset/stb_image_loader.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::asset
{
    Image StbImageLoader::load_image_from_file(const std::filesystem::path& path) const
    {
        auto width = std::int32_t{0};
        auto height = std::int32_t{0};
        auto image_channel_count = std::int32_t{0};
        constexpr auto data_channel_count = std::int32_t{STBI_rgb_alpha};

        stbi_uc* const bytes = stbi_load(
            path.string().c_str(),
            &width,
            &height,
            &image_channel_count,
            data_channel_count);
        XAR_THROW_IF(
            bytes == nullptr,
            error::XarException,
            "Failed to load image form file '{}'",
            path.string());

        Image image;
        image.pixel_width = width;
        image.pixel_width = height;
        image.channel_count = data_channel_count;

        image.bytes.resize(image::get_byte_size(image));
        memcpy(
            image.bytes.data(),
            bytes,
            image.bytes.size());

        stbi_image_free(bytes);

        return image;
    }
}
