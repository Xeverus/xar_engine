#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanDescriptorSetLayout
    {
    public:
        struct Parameters;

    public:
        explicit VulkanDescriptorSetLayout(const Parameters& parameters);
        ~VulkanDescriptorSetLayout();

        [[nodiscard]]
        VkDescriptorSetLayout get_native() const;

    private:
        VkDevice _vk_device;
        VkDescriptorSetLayout _vk_descriptor_set_layout;
    };

    struct VulkanDescriptorSetLayout::Parameters
    {
        VkDevice vk_device;
        std::vector<VkDescriptorSetLayoutBinding> vk_bindings;
    };
}