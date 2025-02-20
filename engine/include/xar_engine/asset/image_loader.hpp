#pragma once

#include <filesystem>

#include <xar_engine/asset/image.hpp>


namespace xar_engine::asset
{
    class IImageLoader
    {
    public:
        virtual ~IImageLoader();

        [[nodiscard]]
        virtual Image load_image_from_file(const std::filesystem::path& path) const = 0;
    };

    class IImageLoaderFactory
    {
    public:
        virtual ~IImageLoaderFactory();

        [[nodiscard]]
        virtual std::unique_ptr<IImageLoader> make() const = 0;
    };

    class ImageLoaderFactory : public IImageLoaderFactory
    {
    public:
        [[nodiscard]]
        std::unique_ptr<IImageLoader> make() const override;
    };
}