#pragma once

#include <memory>
#include <vector>

#include <xar_engine/asset/model.hpp>

#include <xar_engine/renderer/gpu_asset/gpu_mesh_instance.hpp>
#include <xar_engine/renderer/gpu_asset/gpu_model.hpp>


namespace xar_engine::graphics
{
    class IWindowSurface;
}

namespace xar_engine::graphics::api
{
    class IGraphicsBackend;
}

namespace xar_engine::renderer
{
    class IRenderer
    {
    public:
        virtual ~IRenderer();


        virtual std::vector<gpu_asset::GpuModel> make_gpu_model(const std::vector<asset::Model>& model_list) = 0;

        virtual void add_gpu_mesh_instance_to_render(const gpu_asset::GpuMeshInstance& gpu_mesh_instance) = 0;

        virtual void update() = 0;
    };


    class IRendererFactory
    {
    public:
        virtual ~IRendererFactory();

        virtual std::unique_ptr<IRenderer> make(
            std::shared_ptr<graphics::api::IGraphicsBackend> graphics_backend,
            std::shared_ptr<graphics::IWindowSurface> window_surface) = 0;
    };


    class RendererFactory : public IRendererFactory
    {
    public:
        std::unique_ptr<IRenderer> make(
            std::shared_ptr<graphics::api::IGraphicsBackend> graphics_backend,
            std::shared_ptr<graphics::IWindowSurface> window_surface) override;
    };
}