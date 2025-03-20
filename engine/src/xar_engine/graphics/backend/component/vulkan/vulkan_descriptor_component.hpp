#pragma once

#include <xar_engine/graphics/backend/component/descriptor_component.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_graphics_backend_state.hpp>


namespace xar_engine::graphics::backend::component::vulkan
{
    class IVulkanDescriptorComponent
        : public IDescriptorComponent
          , public ::xar_engine::graphics::backend::vulkan::SharedVulkanGraphicsBackendState
    {
    public:
        using SharedVulkanGraphicsBackendState::SharedVulkanGraphicsBackendState;

        api::DescriptorPoolReference make_descriptor_pool(const MakeDescriptorPoolParameters& parameters) override;
        api::DescriptorSetLayoutReference make_descriptor_set_layout(const MakeDescriptorSetLayoutParameters& parameters) override;
        std::vector<api::DescriptorSetReference> make_descriptor_set_list(const MakeDescriptorSetParameters& parameters) override;

        void write_descriptor_set(const WriteDescriptorSetParameters& parameters) override;
    };
}