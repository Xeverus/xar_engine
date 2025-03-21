#include <xar_engine/renderer/renderer.hpp>

#include <xar_engine/graphics/backend/graphics_backend.hpp>

#include <xar_engine/renderer/renderer_impl.hpp>

#include <xar_engine/renderer/unit/gpu_material_unit_impl.hpp>
#include <xar_engine/renderer/unit/gpu_model_unit_impl.hpp>

namespace xar_engine::renderer
{
    IRenderer::~IRenderer() = default;

    IRendererFactory::~IRendererFactory() = default;


    std::unique_ptr<IRenderer> RendererFactory::make(
        std::shared_ptr<graphics::backend::IGraphicsBackend> graphics_backend,
        std::shared_ptr<graphics::context::IWindowSurface> window_surface)
    {
        auto state = std::make_shared<RendererState>();
        state->graphics_backend = graphics_backend;
        state->window_surface = window_surface;

        return std::make_unique<RendererImpl>(
            state,
            std::make_unique<unit::GpuMaterialUnitImpl>(state),
            std::make_unique<unit::GpuModelUnitImpl>(state));
    }
}
