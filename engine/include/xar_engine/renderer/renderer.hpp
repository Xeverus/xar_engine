#pragma once

#include <memory>
#include <vector>

#include <xar_engine/asset/material.hpp>
#include <xar_engine/asset/model.hpp>

#include <xar_engine/renderer/gpu_asset/gpu_material.hpp>
#include <xar_engine/renderer/gpu_asset/gpu_mesh_instance.hpp>
#include <xar_engine/renderer/gpu_asset/gpu_model.hpp>

#include <xar_engine/renderer/unit/gpu_material_unit.hpp>
#include <xar_engine/renderer/unit/gpu_model_unit.hpp>


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


        virtual unit::IGpuMaterialUnit& gpu_material_unit() = 0;
        virtual unit::IGpuModelUnit& gpu_model_unit() = 0;

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


    class RendererFactory
        : public IRendererFactory
    {
    public:
        std::unique_ptr<IRenderer> make(
            std::shared_ptr<graphics::backend::IGraphicsBackend> graphics_backend,
            std::shared_ptr<graphics::context::IWindowSurface> window_surface) override;
    };
}