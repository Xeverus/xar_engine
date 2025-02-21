#include <xar_engine/graphics/vulkan/vulkan_sampler.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanSampler::VulkanSampler(const VulkanSampler::Parameters& parameters)
        : _vk_device(parameters.vk_device)
        , _vk_sampler(nullptr)
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = parameters.max_anisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0;
        samplerInfo.maxLod = static_cast<float>(parameters.max_lod);

        const auto create_sampler_result = vkCreateSampler(
            _vk_device,
            &samplerInfo,
            nullptr,
            &_vk_sampler);
        XAR_THROW_IF(
            create_sampler_result != VK_SUCCESS,
            error::XarException,
            "Failed to create texture sampler!");
    }

    VulkanSampler::~VulkanSampler()
    {
        vkDestroySampler(
            _vk_device,
            _vk_sampler,
            nullptr);
    }

    VkSampler VulkanSampler::get_native() const
    {
        return _vk_sampler;
    }
}