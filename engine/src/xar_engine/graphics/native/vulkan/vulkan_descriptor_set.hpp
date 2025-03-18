#pragma once

#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/native/vulkan/vulkan_descriptor_pool.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_descriptor_set_layout.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_device.hpp>


namespace xar_engine::graphics::native::vulkan
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
        VulkanDescriptorSetLayout vulkan_descriptor_layout_set;
        VkDescriptorSet vk_descriptor_set;
    };
}