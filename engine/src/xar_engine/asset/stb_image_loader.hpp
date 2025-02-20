#pragma once

#include <xar_engine/asset/image_loader.hpp>


namespace xar_engine::asset
{
    class StbImageLoader : public IImageLoader
    {
    public:
        [[nodiscard]]
        Image load_image_from_file(const std::filesystem::path& path) const override;
    };
}