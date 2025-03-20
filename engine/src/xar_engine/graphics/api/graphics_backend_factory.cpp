#include <xar_engine/graphics/api/graphics_backend_factory.hpp>

#include <xar_engine/graphics/backend/component/vulkan/vulkan_buffer_component.hpp>
#include <xar_engine/graphics/backend/component/vulkan/vulkan_command_buffer_component.hpp>
#include <xar_engine/graphics/backend/component/vulkan/vulkan_descriptor_component.hpp>
#include <xar_engine/graphics/backend/component/vulkan/vulkan_device_component.hpp>
#include <xar_engine/graphics/backend/component/vulkan/vulkan_graphics_pipeline_component.hpp>
#include <xar_engine/graphics/backend/component/vulkan/vulkan_image_component.hpp>
#include <xar_engine/graphics/backend/component/vulkan/vulkan_shader_component.hpp>
#include <xar_engine/graphics/backend/component/vulkan/vulkan_swap_chain_component.hpp>

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
                auto state = std::make_shared<backend::vulkan::VulkanGraphicsBackendState>();

                return std::make_unique<backend::vulkan::VulkanGraphicsBackend>(
                    state,
                    std::make_unique<backend::component::vulkan::IVulkanBufferComponent>(state),
                    std::make_unique<backend::component::vulkan::IVulkanCommandBufferComponent>(state),
                    std::make_unique<backend::component::vulkan::IVulkanDescriptorComponent>(state),
                    std::make_unique<backend::component::vulkan::IVulkanDeviceComponent>(state),
                    std::make_unique<backend::component::vulkan::IVulkanGraphicsPipelineComponent>(state),
                    std::make_unique<backend::component::vulkan::IVulkanImageComponent>(state),
                    std::make_unique<backend::component::vulkan::IVulkanShaderComponent>(state),
                    std::make_unique<backend::component::vulkan::IVulkanSwapChainComponent>(state));
            }
        }

        XAR_THROW(
            error::XarException,
            "Invalid graphics backend type {}",
            static_cast<std::uint32_t>(graphics_backend_type));
    }
}