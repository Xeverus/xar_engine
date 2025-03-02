#include <xar_engine/graphics/graphics_backend.hpp>


namespace xar_engine::graphics
{
    IGraphicsBackendResource::~IGraphicsBackendResource() = default;

    IGraphicsBackendHostCommand::~IGraphicsBackendHostCommand() = default;

    IGraphicsBackendDeviceCommand::~IGraphicsBackendDeviceCommand() = default;

    IGraphicsBackend::~IGraphicsBackend() = default;
}