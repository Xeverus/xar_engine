#include <xar_engine/graphics/vulkan/vulkan_graphics_backend.hpp>

#include <xar_engine/meta/ref_counting_singleton.hpp>

#include <xar_engine/graphics/vulkan/impl/vulkan_shader.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanGraphicsBackend::VulkanGraphicsBackend()
        : _vk_sample_count_flag_bits{VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT}
    {
        _vulkan_instance = meta::RefCountedSingleton::get_instance<vulkan::impl::VulkanInstance>();
        _vulkan_physical_device_list = _vulkan_instance->get_physical_device_list();
        _vulkan_device = vulkan::impl::VulkanDevice{{_vulkan_physical_device_list[0]}};

        _vk_sample_count_flag_bits = _vulkan_physical_device_list[0].get_vk_sample_count_flag_bits();
    }

    void VulkanGraphicsBackend::make_shader_object(const std::vector<char>& shader_byte_code)
    {
        vulkan::impl::VulkanShader{
            {
                _vulkan_device,
                shader_byte_code,
            }};
    }
}