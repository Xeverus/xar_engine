#include <xar_engine/graphics/backend/unit/vulkan/vulkan_device_unit.hpp>


namespace xar_engine::graphics::backend::unit::vulkan
{
    void IVulkanDeviceUnit::wait_idle()
    {
        get_state()._vulkan_device.wait_idle();
    }

    std::uint32_t IVulkanDeviceUnit::get_sample_count() const
    {
        return get_state()._vulkan_device.get_native_physical_device().get_vk_sample_count_flag_bits();
    }

    api::EFormat IVulkanDeviceUnit::find_depth_format() const
    {
        const auto vk_format = get_state()._vulkan_device.get_native_physical_device().find_supported_vk_format(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

        switch (vk_format)
        {
            case VK_FORMAT_D32_SFLOAT:
            {
                break;
            }
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
            {
                XAR_THROW(
                    error::XarException,
                    "VK_FORMAT_D32_SFLOAT_S8_UINT not supported");
                break;
            }
            case VK_FORMAT_D24_UNORM_S8_UINT:
            {
                XAR_THROW(
                    error::XarException,
                    "VK_FORMAT_D24_UNORM_S8_UINT not supported");
                break;
            }
            default:
            {
                XAR_THROW(
                    error::XarException,
                    "VK_FORMAT not supported");
                break;
            }
        }

        return api::EFormat::D32_SIGNED_FLOAT;
    }
}