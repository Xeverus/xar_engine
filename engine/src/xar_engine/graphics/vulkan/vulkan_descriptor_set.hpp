#pragma once

#include <cstdint>
#include <vector>

#include <volk.h>


namespace xar_engine::graphics::vulkan
{
    class VulkanDescriptorSet
    {
    public:
        struct Parameters;

    public:
        explicit VulkanDescriptorSet(const Parameters& parameters);
        ~VulkanDescriptorSet();

        void write(const std::vector<VkWriteDescriptorSet>& data);

        [[nodiscard]]
        const std::vector<VkDescriptorSet>& get_native() const;

    private:
        VkDevice _vk_device;
        std::vector<VkDescriptorSet> _vk_descriptor_sets;
    };

    struct VulkanDescriptorSet::Parameters
    {
        VkDevice vk_device;
        VkDescriptorPool vk_descriptor_pool;
        std::vector<VkDescriptorSetLayout> vk_layouts;
    };
}