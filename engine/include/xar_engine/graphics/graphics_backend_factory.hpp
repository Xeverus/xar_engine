#pragma once

#include <memory>

#include <xar_engine/graphics/graphics_backend_type.hpp>


namespace xar_engine::graphics
{
    class IGraphicsBackend;

    class IGraphicsBackendFactory
    {
    public:
        virtual ~IGraphicsBackendFactory();

        [[nodiscard]]
        virtual std::shared_ptr<IGraphicsBackend> make(EGraphicsBackendType graphics_backend_type) const = 0;
    };


    class GraphicsBackendFactory : public IGraphicsBackendFactory
    {
    public:
        [[nodiscard]]
        std::shared_ptr<IGraphicsBackend> make(EGraphicsBackendType graphics_backend_type) const override;
    };
}