#include <xar_engine/renderer/renderer.hpp>

#include <xar_engine/graphics/backend/graphics_backend.hpp>

#include <xar_engine/renderer/renderer_impl.hpp>


namespace xar_engine::renderer
{
    IRenderer::~IRenderer() = default;

    IRendererFactory::~IRendererFactory() = default;

    std::unique_ptr<IRenderer> RendererFactory::make(
        std::shared_ptr<graphics::backend::IGraphicsBackend> graphics_backend,
        std::shared_ptr<graphics::context::IWindowSurface> window_surface)
    {
        return std::make_unique<RendererImpl>(
            std::move(graphics_backend),
            std::move(window_surface));
    }
}
