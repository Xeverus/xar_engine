#pragma once

#include <filesystem>

#include <xar_engine/asset/model.hpp>


namespace xar_engine::asset
{
    class IModelLoader
    {
    public:
        virtual ~IModelLoader();

        [[nodiscard]]
        virtual Model load_model_from_file(const std::filesystem::path& path) const = 0;
    };

    class IModelLoaderFactory
    {
    public:
        virtual ~IModelLoaderFactory();

        [[nodiscard]]
        virtual std::unique_ptr<IModelLoader> make() const = 0;
    };

    class ModelLoaderFactory
        : public IModelLoaderFactory
    {
    public:
        [[nodiscard]]
        std::unique_ptr<IModelLoader> make() const override;
    };
}