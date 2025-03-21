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
            std::unique_ptr<unit::IBufferUnit> buffer_unit,
            std::unique_ptr<unit::ICommandBufferUnit> command_buffer_unit,
            std::unique_ptr<unit::IDescriptorUnit> descriptor_unit,
            std::unique_ptr<unit::IDeviceUnit> device_unit,
            std::unique_ptr<unit::IGraphicsPipelineUnit> graphics_pipeline_unit,
            std::unique_ptr<unit::IImageUnit> image_unit,
            std::unique_ptr<unit::IShaderUnit> shader_unit,
            std::unique_ptr<unit::ISwapChainUnit> swap_chain_unit);

        unit::IBufferUnit& buffer_unit() override;
        unit::ICommandBufferUnit& command_buffer_unit() override;
        unit::IDescriptorUnit& descriptor_unit() override;
        unit::IDeviceUnit& device_unit() override;
        unit::IGraphicsPipelineUnit& graphics_pipeline_unit() override;
        unit::IImageUnit& image_unit() override;
        unit::IShaderUnit& shader_unit() override;
        unit::ISwapChainUnit& swap_chain_unit() override;

    private:
        std::unique_ptr<unit::IBufferUnit> _buffer_unit;
        std::unique_ptr<unit::ICommandBufferUnit> _command_buffer_unit;
        std::unique_ptr<unit::IDescriptorUnit> _descriptor_unit;
        std::unique_ptr<unit::IDeviceUnit> _device_unit;
        std::unique_ptr<unit::IGraphicsPipelineUnit> _graphics_pipeline_unit;
        std::unique_ptr<unit::IImageUnit> _image_unit;
        std::unique_ptr<unit::IShaderUnit> _shader_unit;
        std::unique_ptr<unit::ISwapChainUnit> _swap_chain_unit;
    };
}