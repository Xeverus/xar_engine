#pragma once

#include <xar_engine/graphics/backend/unit/buffer_unit.hpp>
#include <xar_engine/graphics/backend/unit/command_buffer_unit.hpp>
#include <xar_engine/graphics/backend/unit/descriptor_unit.hpp>
#include <xar_engine/graphics/backend/unit/device_unit.hpp>
#include <xar_engine/graphics/backend/unit/graphics_pipeline_unit.hpp>
#include <xar_engine/graphics/backend/unit/image_unit.hpp>
#include <xar_engine/graphics/backend/unit/shader_unit.hpp>
#include <xar_engine/graphics/backend/unit/swap_chain_unit.hpp>


namespace xar_engine::graphics::backend
{
    class IGraphicsBackend
    {
    public:
        virtual ~IGraphicsBackend();

        virtual unit::IBufferUnit& buffer_unit() = 0;
        virtual unit::ICommandBufferUnit& command_buffer_unit() = 0;
        virtual unit::IDescriptorUnit& descriptor_unit() = 0;
        virtual unit::IDeviceUnit& device_unit() = 0;
        virtual unit::IGraphicsPipelineUnit& graphics_pipeline_unit() = 0;
        virtual unit::IImageUnit& image_unit() = 0;
        virtual unit::IShaderUnit& shader_unit() = 0;
        virtual unit::ISwapChainUnit& swap_chain_unit() = 0;
    };
}