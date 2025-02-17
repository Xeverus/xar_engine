#include <xar_engine/asset/image_loader.hpp>

#include <xar_engine/asset/stb_image_loader.hpp>


namespace xar_engine::asset
{
    IImageLoader::~IImageLoader() = default;

    std::unique_ptr<IImageLoader> ImageLoaderFactory::make_loader()
    {
        return std::make_unique<StbImageLoader>();
    }
}