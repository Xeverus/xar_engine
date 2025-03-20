#pragma once

#include <memory>

#include <xar_engine/graphics/backend/component/buffer_component.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::component::vulkan
{
    class IVulkanBufferComponent
        : public IBufferComponent
          , public backend::vulkan::SharedVulkanGraphicsBackendState
    {
    public:
        using SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState;

        api::BufferReference make_staging_buffer(const MakeBufferParameters& parameters) override;
        api::BufferReference make_vertex_buffer(const MakeBufferParameters& parameters) override;
        api::BufferReference make_index_buffer(const MakeBufferParameters& parameters) override;
        api::BufferReference make_uniform_buffer(const MakeBufferParameters& parameters) override;

        void update_buffer(const UpdateBufferParameters& parameters) override;
        void copy_buffer(const CopyBufferParameters& parameters) override;
        void copy_buffer_to_image(const CopyBufferToImageParameters& parameters) override;
    };

}