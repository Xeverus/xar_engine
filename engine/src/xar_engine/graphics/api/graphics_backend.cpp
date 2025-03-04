#include <xar_engine/graphics/api/graphics_backend.hpp>

#include <xar_engine/graphics/vulkan/vulkan_graphics_backend.hpp>


namespace xar_engine::graphics::api
{
    IGraphicsBackendResource::~IGraphicsBackendResource() = default;

    IGraphicsBackendHost::~IGraphicsBackendHost() = default;

    IGraphicsBackendCommand::~IGraphicsBackendCommand() = default;

    IGraphicsBackend::~IGraphicsBackend() = default;
}