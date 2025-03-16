#include <xar_engine/graphics/graphics_backend_factory.hpp>

#include <xar_engine/graphics/vulkan/vulkan_graphics_backend.hpp>


namespace xar_engine::graphics
{
    IGraphicsBackendFactory::~IGraphicsBackendFactory() = default;

    std::shared_ptr<IGraphicsBackend> GraphicsBackendFactory::make(EGraphicsBackendType graphics_backend_type) const
    {
        switch (graphics_backend_type)
        {
            case EGraphicsBackendType::VULKAN:
            {
                return std::make_unique<vulkan::VulkanGraphicsBackend>();
            }
        }

        XAR_THROW(
            error::XarException,
            "Invalid graphics backend type {}",
            static_cast<std::uint32_t>(graphics_backend_type));
    }
}