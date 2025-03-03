#pragma once

#include <memory>


namespace xar_engine::graphics
{
    class IWindowSurface;
    class IGraphicsBackend;

    class IRenderer
    {
    public:
        virtual ~IRenderer();

        virtual void update() = 0;
    };


    class IRendererFactory
    {
    public:
        virtual ~IRendererFactory();

        virtual std::unique_ptr<IRenderer> make(
            std::shared_ptr<IGraphicsBackend> graphics_backend,
            std::shared_ptr<IWindowSurface> window_surface) = 0;
    };


    class RendererFactory : public IRendererFactory
    {
    public:
        std::unique_ptr<IRenderer> make(
            std::shared_ptr<IGraphicsBackend> graphics_backend,
            std::shared_ptr<IWindowSurface> window_surface) override;
    };
}