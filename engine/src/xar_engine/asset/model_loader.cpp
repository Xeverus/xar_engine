#include <xar_engine/asset/model_loader.hpp>

#include <xar_engine/asset/assimp_model_loader.hpp>


namespace xar_engine::asset
{
    IModelLoader::~IModelLoader() = default;

    std::unique_ptr<IModelLoader> ModelLoaderFactory::make_loader()
    {
        return std::make_unique<AssimpModelLoader>();
    }
}