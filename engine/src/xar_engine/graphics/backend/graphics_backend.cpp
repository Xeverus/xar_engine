#include <xar_engine/graphics/backend/graphics_backend.hpp>

#include <xar_engine/graphics/backend/vulkan_graphics_backend.hpp>


namespace xar_engine::graphics::backend
{
    IGraphicsBackendResource::~IGraphicsBackendResource() = default;

    IGraphicsBackendHost::~IGraphicsBackendHost() = default;

    IGraphicsBackendCommand::~IGraphicsBackendCommand() = default;

    IGraphicsBackend::~IGraphicsBackend() = default;
}