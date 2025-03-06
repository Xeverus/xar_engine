#pragma once

#include <memory>
#include <vector>

#include <xar_engine/asset/model.hpp>

#include <xar_engine/graphics/gpu_asset/gpu_model_list_reference.hpp>


namespace xar_engine::graphics::api
{
    class IGraphicsBackend;
}

namespace xar_engine::graphics
{
    class IWindowSurface;

    class IRenderer
    {
    public:
        virtual ~IRenderer();


        virtual gpu_asset::GpuModelListReference make_gpu_model_list(const std::vector<asset::Model>& model_list) = 0;

        virtual void update() = 0;
    };


    class IRendererFactory
    {
    public:
        virtual ~IRendererFactory();

        virtual std::unique_ptr<IRenderer> make(
            std::shared_ptr<api::IGraphicsBackend> graphics_backend,
            std::shared_ptr<IWindowSurface> window_surface) = 0;
    };


    class RendererFactory : public IRendererFactory
    {
    public:
        std::unique_ptr<IRenderer> make(
            std::shared_ptr<api::IGraphicsBackend> graphics_backend,
            std::shared_ptr<IWindowSurface> window_surface) override;
    };
}