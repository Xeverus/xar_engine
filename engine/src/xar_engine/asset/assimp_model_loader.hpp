#pragma once

#include <xar_engine/asset/model_loader.hpp>


namespace xar_engine::asset
{
    class AssimpModelLoader : public IModelLoader
    {
    public:
        [[nodiscard]]
        Model load_model_from_file(const std::filesystem::path& path) const override;
    };
}