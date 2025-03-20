#pragma once

#include <memory>

#include <xar_engine/graphics/backend/graphics_backend.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::vulkan
{
    class VulkanGraphicsBackend
        : public IGraphicsBackend
          , public SharedVulkanGraphicsBackendState
    {
    public:
        VulkanGraphicsBackend(
            std::shared_ptr<VulkanGraphicsBackendState> state,
            std::unique_ptr<component::IBufferComponent> buffer_component,
            std::unique_ptr<component::ICommandBufferComponent> command_buffer_component,
            std::unique_ptr<component::IDescriptorComponent> descriptor_component,
            std::unique_ptr<component::IDeviceComponent> device_component,
            std::unique_ptr<component::IGraphicsPipelineComponent> graphics_pipeline_component,
            std::unique_ptr<component::IImageComponent> image_component,
            std::unique_ptr<component::IShaderComponent> shader_component,
            std::unique_ptr<component::ISwapChainComponent> swap_chain_component);

        component::IBufferComponent& buffer_component() override;
        component::ICommandBufferComponent& command_buffer_component() override;
        component::IDescriptorComponent& descriptor_component() override;
        component::IDeviceComponent& device_component() override;
        component::IGraphicsPipelineComponent& graphics_pipeline_component() override;
        component::IImageComponent& image_component() override;
        component::IShaderComponent& shader_component() override;
        component::ISwapChainComponent& swap_chain_component() override;

    private:
        std::unique_ptr<component::IBufferComponent> _buffer_component;
        std::unique_ptr<component::ICommandBufferComponent> _command_buffer_component;
        std::unique_ptr<component::IDescriptorComponent> _descriptor_component;
        std::unique_ptr<component::IDeviceComponent> _device_component;
        std::unique_ptr<component::IGraphicsPipelineComponent> _graphics_pipeline_component;
        std::unique_ptr<component::IImageComponent> _image_component;
        std::unique_ptr<component::IShaderComponent> _shader_component;
        std::unique_ptr<component::ISwapChainComponent> _swap_chain_component;
    };
}