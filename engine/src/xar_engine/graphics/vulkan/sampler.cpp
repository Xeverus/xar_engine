#include <xar_engine/graphics/vulkan/sampler.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct VulkanSampler::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;

        VkSampler vk_sampler;
    };

    VulkanSampler::State::State(const Parameters& parameters)
        : vulkan_device(parameters.vulkan_device)
        , vk_sampler(nullptr)
    {
        auto vk_sampler_create_info = VkSamplerCreateInfo{};
        vk_sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        vk_sampler_create_info.magFilter = VK_FILTER_LINEAR;
        vk_sampler_create_info.minFilter = VK_FILTER_LINEAR;
        vk_sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vk_sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vk_sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        vk_sampler_create_info.anisotropyEnable = VK_TRUE;
        vk_sampler_create_info.maxAnisotropy = parameters.max_anisotropy;
        vk_sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        vk_sampler_create_info.unnormalizedCoordinates = VK_FALSE;
        vk_sampler_create_info.compareEnable = VK_FALSE;
        vk_sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
        vk_sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        vk_sampler_create_info.mipLodBias = 0.0f;
        vk_sampler_create_info.minLod = 0;
        vk_sampler_create_info.maxLod = static_cast<float>(parameters.max_lod);

        const auto vk_create_sampler_result = vkCreateSampler(
            vulkan_device.get_native(),
            &vk_sampler_create_info,
            nullptr,
            &vk_sampler);
        XAR_THROW_IF(
            vk_create_sampler_result != VK_SUCCESS,
            error::XarException,
            "vkCreateSampler failed");
    }

    VulkanSampler::State::~State()
    {
        vkDestroySampler(
            vulkan_device.get_native(),
            vk_sampler,
            nullptr);
    }


    VulkanSampler::VulkanSampler()
        : _state(nullptr)
    {
    }

    VulkanSampler::VulkanSampler(const VulkanSampler::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanSampler::~VulkanSampler() = default;

    VkSampler VulkanSampler::get_native() const
    {
        return _state->vk_sampler;
    }
}