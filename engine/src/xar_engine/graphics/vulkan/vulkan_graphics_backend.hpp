#pragma once

#include <memory>
#include <vector>

#include <xar_engine/graphics/graphics_backend.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_device.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_instance.hpp>
#include <xar_engine/graphics/vulkan/impl/vulkan_physical_device.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanGraphicsBackend : public IGraphicsBackend
    {
    public:
        VulkanGraphicsBackend();


        void make_shader_object(const std::vector<char>& shader_byte_code) override;

    private:
        std::shared_ptr<vulkan::impl::VulkanInstance> _vulkan_instance;
        std::vector<vulkan::impl::VulkanPhysicalDevice> _vulkan_physical_device_list;
        vulkan::impl::VulkanDevice _vulkan_device;

        VkSampleCountFlagBits _vk_sample_count_flag_bits;
    };
}