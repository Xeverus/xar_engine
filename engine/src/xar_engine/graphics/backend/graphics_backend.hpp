#pragma once

#include <xar_engine/graphics/backend/component/buffer_component.hpp>
#include <xar_engine/graphics/backend/component/command_buffer_component.hpp>
#include <xar_engine/graphics/backend/component/descriptor_component.hpp>
#include <xar_engine/graphics/backend/component/device_component.hpp>
#include <xar_engine/graphics/backend/component/graphics_pipeline_component.hpp>
#include <xar_engine/graphics/backend/component/image_component.hpp>
#include <xar_engine/graphics/backend/component/shader_component.hpp>
#include <xar_engine/graphics/backend/component/swap_chain_component.hpp>


namespace xar_engine::graphics::backend
{
    class IGraphicsBackend
    {
    public:
        virtual ~IGraphicsBackend();

        virtual component::IBufferComponent& buffer_component() = 0;
        virtual component::ICommandBufferComponent& command_buffer_component() = 0;
        virtual component::IDescriptorComponent& descriptor_component() = 0;
        virtual component::IDeviceComponent& device_component() = 0;
        virtual component::IGraphicsPipelineComponent& graphics_pipeline_component() = 0;
        virtual component::IImageComponent& image_component() = 0;
        virtual component::IShaderComponent& shader_component() = 0;
        virtual component::ISwapChainComponent& swap_chain_component() = 0;
    };
}