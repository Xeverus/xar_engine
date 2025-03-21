#include <xar_engine/graphics/api/graphics_backend_factory.hpp>

#include <xar_engine/graphics/backend/unit/vulkan/vulkan_buffer_unit.hpp>
#include <xar_engine/graphics/backend/unit/vulkan/vulkan_command_buffer_unit.hpp>
#include <xar_engine/graphics/backend/unit/vulkan/vulkan_descriptor_unit.hpp>
#include <xar_engine/graphics/backend/unit/vulkan/vulkan_device_unit.hpp>
#include <xar_engine/graphics/backend/unit/vulkan/vulkan_graphics_pipeline_unit.hpp>
#include <xar_engine/graphics/backend/unit/vulkan/vulkan_image_unit.hpp>
#include <xar_engine/graphics/backend/unit/vulkan/vulkan_shader_unit.hpp>
#include <xar_engine/graphics/backend/unit/vulkan/vulkan_swap_chain_unit.hpp>

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
                    std::make_unique<backend::unit::vulkan::IVulkanBufferUnit>(state),
                    std::make_unique<backend::unit::vulkan::IVulkanCommandBufferUnit>(state),
                    std::make_unique<backend::unit::vulkan::IVulkanDescriptorUnit>(state),
                    std::make_unique<backend::unit::vulkan::IVulkanDeviceUnit>(state),
                    std::make_unique<backend::unit::vulkan::IVulkanGraphicsPipelineUnit>(state),
                    std::make_unique<backend::unit::vulkan::IVulkanImageUnit>(state),
                    std::make_unique<backend::unit::vulkan::IVulkanShaderUnit>(state),
                    std::make_unique<backend::unit::vulkan::IVulkanSwapChainUnit>(state));
            }
        }

        XAR_THROW(
            error::XarException,
            "Invalid graphics backend type {}",
            static_cast<std::uint32_t>(graphics_backend_type));
    }
}