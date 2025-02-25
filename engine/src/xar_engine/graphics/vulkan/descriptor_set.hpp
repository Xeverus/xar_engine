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

        void write(const std::vector<VkWriteDescriptorSet>& data);

        [[nodiscard]]
        const std::vector<VkDescriptorSet>& get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanDescriptorSet::Parameters
    {
        Device device;
        VulkanDescriptorPool descriptor_pool;
        std::vector<VkDescriptorSetLayout> vk_layouts;
    };
}