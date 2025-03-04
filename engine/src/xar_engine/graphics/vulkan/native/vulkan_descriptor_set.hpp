#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/native/vulkan_descriptor_pool.hpp>
#include <xar_engine/graphics/vulkan/native/vulkan_device.hpp>


namespace xar_engine::graphics::vulkan::native
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
        VkDescriptorSet get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanDescriptorSet::Parameters
    {
        VulkanDescriptorPool vulkan_descriptor_pool;
        VkDescriptorSet vk_descriptor_set;
    };
}