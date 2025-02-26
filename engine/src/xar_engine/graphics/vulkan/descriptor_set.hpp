#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/descriptor_pool.hpp>
#include <xar_engine/graphics/vulkan/device.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanDescriptorSet
    {
    public:
        struct Parameters;

    public:
        VulkanDescriptorSet();
        explicit VulkanDescriptorSet(const Parameters& parameters);

        ~VulkanDescriptorSet();

        void write(const std::vector<VkWriteDescriptorSet>& vk_write_descriptor_set_list);

        [[nodiscard]]
        const std::vector<VkDescriptorSet>& get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanDescriptorSet::Parameters
    {
        VulkanDevice vulkan_device;
        VulkanDescriptorPool vulkan_descriptor_pool;
        std::vector<VkDescriptorSetLayout> vk_descriptor_set_layout_list;
    };
}