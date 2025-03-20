#pragma once

#include <memory>
#include <vector>

#include <xar_engine/asset/material.hpp>
#include <xar_engine/asset/model.hpp>

#include <xar_engine/renderer/gpu_asset/gpu_material.hpp>
#include <xar_engine/renderer/gpu_asset/gpu_mesh_instance.hpp>
#include <xar_engine/renderer/gpu_asset/gpu_model.hpp>

#include <xar_engine/renderer/module/gpu_model_module.hpp>


namespace xar_engine::graphics::backend
{
    class IGraphicsBackend;
}

namespace xar_engine::graphics::context
{
    class IWindowSurface;
}

namespace xar_engine::renderer
{
    class IRenderer
    {
    public:
        virtual ~IRenderer();


        virtual module::IGpuModelModule& gpu_model_module() = 0;

        virtual gpu_asset::GpuMaterialReference make_gpu_material(const asset::Material& material) = 0;

        virtual void add_gpu_mesh_instance_to_render(
            const gpu_asset::GpuMeshInstance& gpu_mesh_instance,
            const gpu_asset::GpuMaterialReference& gpu_material) = 0;
        virtual void clear_gpu_mesh_instance_to_render() = 0;

        virtual void update() = 0;
    };


    class IRendererFactory
    {
    public:
        virtual ~IRendererFactory();

        virtual std::unique_ptr<IRenderer> make(
            std::shared_ptr<graphics::backend::IGraphicsBackend> graphics_backend,
            std::shared_ptr<graphics::context::IWindowSurface> window_surface) = 0;
    };


    class RendererFactory : public IRendererFactory
    {
    public:
        std::unique_ptr<IRenderer> make(
            std::shared_ptr<graphics::backend::IGraphicsBackend> graphics_backend,
            std::shared_ptr<graphics::context::IWindowSurface> window_surface) override;
    };
}