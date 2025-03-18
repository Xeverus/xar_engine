#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/graphics/native/vulkan/vulkan_descriptor_set_layout.hpp>
#include <xar_engine/graphics/native/vulkan/vulkan_device.hpp>


namespace xar_engine::graphics::native::vulkan
{
    class VulkanDescriptorSet;

    class VulkanDescriptorPool
    {
    public:
        struct Parameters;

    public:
        VulkanDescriptorPool();
        explicit VulkanDescriptorPool(const Parameters& parameters);

        ~VulkanDescriptorPool();


        std::vector<VulkanDescriptorSet> make_descriptor_set_list(const std::vector<VulkanDescriptorSetLayout>& vulkan_descriptor_set_layout_list);


        [[nodiscard]]
        VkDescriptorPool get_native() const;

        [[nodiscard]]
        VulkanDevice& get_device();

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanDescriptorPool::Parameters
    {
        VulkanDevice vulkan_device;

        std::uint32_t uniform_buffer_count;
        std::uint32_t combined_image_sampler_count;
        std::uint32_t max_descriptor_set_count;
    };
}