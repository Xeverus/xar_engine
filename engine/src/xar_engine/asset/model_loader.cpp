#include <xar_engine/asset/model_loader.hpp>

#include <xar_engine/asset/assimp_model_loader.hpp>


namespace xar_engine::asset
{
    IModelLoader::~IModelLoader() = default;


    IModelLoaderFactory::~IModelLoaderFactory() = default;


    std::unique_ptr<IModelLoader> ModelLoaderFactory::make() const
    {
        return std::make_unique<AssimpModelLoader>();
    }
}