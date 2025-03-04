#include <xar_engine/graphics/vulkan/native/vulkan_command_buffer.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan::native
{
    struct VulkanCommandBuffer::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanCommandBufferPool vulkan_command_buffer_pool;
        VkCommandBuffer vk_command_buffer;
    };

    VulkanCommandBuffer::State::State(const VulkanCommandBuffer::Parameters& parameters)
        : vulkan_command_buffer_pool(parameters.vulkan_command_buffer_pool)
        , vk_command_buffer(parameters.vk_command_buffer)
    {
    }

    VulkanCommandBuffer::State::~State()
    {
        vkFreeCommandBuffers(
            vulkan_command_buffer_pool.get_device().get_native(),
            vulkan_command_buffer_pool.get_native(),
            1,
            &vk_command_buffer);
    }


    VulkanCommandBuffer::VulkanCommandBuffer()
        : _state(nullptr)
    {
    }

    VulkanCommandBuffer::VulkanCommandBuffer(const VulkanCommandBuffer::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanCommandBuffer::~VulkanCommandBuffer() = default;

    void VulkanCommandBuffer::begin(const bool one_time)
    {
        vkResetCommandBuffer(
            _state->vk_command_buffer,
            0);

        const auto vk_flags = static_cast<VkCommandBufferUsageFlags>(
            one_time ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0);

        auto vk_command_buffer_begin_info = VkCommandBufferBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = vk_flags,
            .pInheritanceInfo = nullptr,
        };

        const auto vk_begin_command_buffer_result = vkBeginCommandBuffer(
            _state->vk_command_buffer,
            &vk_command_buffer_begin_info);
        XAR_THROW_IF(
            vk_begin_command_buffer_result != VK_SUCCESS,
            error::XarException,
            "Begin command buffer failed");
    }

    void VulkanCommandBuffer::end()
    {
        vkEndCommandBuffer(_state->vk_command_buffer);
    }

    VkCommandBuffer VulkanCommandBuffer::get_native() const
    {
        return _state->vk_command_buffer;
    }
}