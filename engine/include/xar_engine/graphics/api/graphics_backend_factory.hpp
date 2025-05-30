#pragma once

#include <memory>

#include <xar_engine/graphics/api/graphics_backend_type.hpp>


namespace xar_engine::graphics::backend
{
    class IGraphicsBackend;
}

namespace xar_engine::graphics::api
{
    class IGraphicsBackendFactory
    {
    public:
        virtual ~IGraphicsBackendFactory();

        [[nodiscard]]
        virtual std::shared_ptr<backend::IGraphicsBackend> make(EGraphicsBackendType graphics_backend_type) const = 0;
    };


    class GraphicsBackendFactory
        : public IGraphicsBackendFactory
    {
    public:
        [[nodiscard]]
        std::shared_ptr<backend::IGraphicsBackend> make(EGraphicsBackendType graphics_backend_type) const override;
    };
}