#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/impl/vulkan_device.hpp>


namespace xar_engine::graphics::vulkan::impl
{
    class VulkanDescriptorSetLayout
    {
    public:
        struct Parameters;

    public:
        VulkanDescriptorSetLayout();
        explicit VulkanDescriptorSetLayout(const Parameters& parameters);

        ~VulkanDescriptorSetLayout();

        [[nodiscard]]
        VkDescriptorSetLayout get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanDescriptorSetLayout::Parameters
    {
        VulkanDevice vulkan_device;
        std::vector<VkDescriptorSetLayoutBinding> vk_descriptor_set_layout_binding_list;
    };
}