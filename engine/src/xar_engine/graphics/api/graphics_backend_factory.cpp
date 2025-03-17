#include <xar_engine/graphics/api/graphics_backend_factory.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend.hpp>


namespace xar_engine::graphics::api
{
    IGraphicsBackendFactory::~IGraphicsBackendFactory() = default;

    std::shared_ptr<backend::IGraphicsBackend> GraphicsBackendFactory::make(EGraphicsBackendType graphics_backend_type) const
    {
        switch (graphics_backend_type)
        {
            case EGraphicsBackendType::VULKAN:
            {
                return std::make_unique<backend::vulkan::VulkanGraphicsBackend>();
            }
        }

        XAR_THROW(
            error::XarException,
            "Invalid graphics backend type {}",
            static_cast<std::uint32_t>(graphics_backend_type));
    }
}