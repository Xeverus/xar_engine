#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend.hpp>

#include <xar_engine/graphics/native/vulkan/vulkan_queue.hpp>

#include <xar_engine/meta/ref_counting_singleton.hpp>


namespace xar_engine::graphics::backend::vulkan
{
    VulkanGraphicsBackend::VulkanGraphicsBackend(
        std::shared_ptr<VulkanGraphicsBackendState> state,
        std::unique_ptr<::xar_engine::graphics::backend::unit::IBufferUnit> buffer_unit,
        std::unique_ptr<::xar_engine::graphics::backend::unit::ICommandBufferUnit> command_buffer_unit,
        std::unique_ptr<::xar_engine::graphics::backend::unit::IDescriptorUnit> descriptor_unit,
        std::unique_ptr<::xar_engine::graphics::backend::unit::IDeviceUnit> device_unit,
        std::unique_ptr<::xar_engine::graphics::backend::unit::IGraphicsPipelineUnit> graphics_pipeline_unit,
        std::unique_ptr<::xar_engine::graphics::backend::unit::IImageUnit> image_unit,
        std::unique_ptr<::xar_engine::graphics::backend::unit::IShaderUnit> shader_unit,
        std::unique_ptr<::xar_engine::graphics::backend::unit::ISwapChainUnit> swap_chain_unit)
        : SharedVulkanGraphicsBackendState(std::move(state))
        , _buffer_unit(std::move(buffer_unit))
        , _command_buffer_unit(std::move(command_buffer_unit))
        , _descriptor_unit(std::move(descriptor_unit))
        , _device_unit(std::move(device_unit))
        , _graphics_pipeline_unit(std::move(graphics_pipeline_unit))
        , _image_unit(std::move(image_unit))
        , _shader_unit(std::move(shader_unit))
        , _swap_chain_unit(std::move(swap_chain_unit))
    {
        get_state()._vulkan_instance = meta::RefCountedSingleton::get_instance_t<native::vulkan::VulkanInstance>();
        get_state()._vulkan_physical_device_list = get_state()._vulkan_instance->get_physical_device_list();
        get_state()._vulkan_device = native::vulkan::VulkanDevice{{get_state()._vulkan_physical_device_list[0]}};
        get_state()._vulkan_graphics_queue = native::vulkan::VulkanQueue{
            {
                get_state()._vulkan_device,
                get_state()._vulkan_device.get_graphics_family_index(),
            }
        };

        get_state()._vulkan_command_buffer_pool = native::vulkan::VulkanCommandBufferPool{
            {
                get_state()._vulkan_device,
            }};
    }

    unit::IBufferUnit& VulkanGraphicsBackend::buffer_unit()
    {
        return *_buffer_unit;
    }

    unit::ICommandBufferUnit& VulkanGraphicsBackend::command_buffer_unit()
    {
        return *_command_buffer_unit;
    }

    unit::IDescriptorUnit& VulkanGraphicsBackend::descriptor_unit()
    {
        return *_descriptor_unit;
    }

    unit::IDeviceUnit& VulkanGraphicsBackend::device_unit()
    {
        return *_device_unit;
    }

    unit::IGraphicsPipelineUnit& VulkanGraphicsBackend::graphics_pipeline_unit()
    {
        return *_graphics_pipeline_unit;
    }

    unit::IImageUnit& VulkanGraphicsBackend::image_unit()
    {
        return *_image_unit;
    }

    unit::IShaderUnit& VulkanGraphicsBackend::shader_unit()
    {
        return *_shader_unit;
    }

    unit::ISwapChainUnit& VulkanGraphicsBackend::swap_chain_unit()
    {
        return *_swap_chain_unit;
    }
}