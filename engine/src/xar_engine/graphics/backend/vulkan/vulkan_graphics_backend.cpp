#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend.hpp>

#include <xar_engine/graphics/native/vulkan/vulkan_queue.hpp>

#include <xar_engine/meta/ref_counting_singleton.hpp>


namespace xar_engine::graphics::backend::vulkan
{
    VulkanGraphicsBackend::VulkanGraphicsBackend(
        std::shared_ptr<VulkanGraphicsBackendState> state,
        std::unique_ptr<::xar_engine::graphics::backend::component::IBufferComponent> buffer_component,
        std::unique_ptr<::xar_engine::graphics::backend::component::ICommandBufferComponent> command_buffer_component,
        std::unique_ptr<::xar_engine::graphics::backend::component::IDescriptorComponent> descriptor_component,
        std::unique_ptr<::xar_engine::graphics::backend::component::IDeviceComponent> device_component,
        std::unique_ptr<::xar_engine::graphics::backend::component::IGraphicsPipelineComponent> graphics_pipeline_component,
        std::unique_ptr<::xar_engine::graphics::backend::component::IImageComponent> image_component,
        std::unique_ptr<::xar_engine::graphics::backend::component::IShaderComponent> shader_component,
        std::unique_ptr<::xar_engine::graphics::backend::component::ISwapChainComponent> swap_chain_component)
        : SharedVulkanGraphicsBackendState(std::move(state))
        , _buffer_component(std::move(buffer_component))
        , _command_buffer_component(std::move(command_buffer_component))
        , _descriptor_component(std::move(descriptor_component))
        , _device_component(std::move(device_component))
        , _graphics_pipeline_component(std::move(graphics_pipeline_component))
        , _image_component(std::move(image_component))
        , _shader_component(std::move(shader_component))
        , _swap_chain_component(std::move(swap_chain_component))
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

    component::IBufferComponent& VulkanGraphicsBackend::buffer_component()
    {
        return *_buffer_component;
    }

    component::ICommandBufferComponent& VulkanGraphicsBackend::command_buffer_component()
    {
        return *_command_buffer_component;
    }

    component::IDescriptorComponent& VulkanGraphicsBackend::descriptor_component()
    {
        return *_descriptor_component;
    }

    component::IDeviceComponent& VulkanGraphicsBackend::device_component()
    {
        return *_device_component;
    }

    component::IGraphicsPipelineComponent& VulkanGraphicsBackend::graphics_pipeline_component()
    {
        return *_graphics_pipeline_component;
    }

    component::IImageComponent& VulkanGraphicsBackend::image_component()
    {
        return *_image_component;
    }

    component::IShaderComponent& VulkanGraphicsBackend::shader_component()
    {
        return *_shader_component;
    }

    component::ISwapChainComponent& VulkanGraphicsBackend::swap_chain_component()
    {
        return *_swap_chain_component;
    }
}