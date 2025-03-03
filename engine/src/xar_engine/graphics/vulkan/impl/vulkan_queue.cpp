#include <xar_engine/graphics/vulkan/impl/vulkan_queue.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan::impl
{
    struct VulkanQueue::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;
        VkQueue vk_queue;
    };

    VulkanQueue::State::State(const VulkanQueue::Parameters& parameters)
        : vulkan_device(parameters.vulkan_device)
    {
        vkGetDeviceQueue(
            vulkan_device.get_native(),
            parameters.queue_family_index,
            0,
            &vk_queue);
    }

    VulkanQueue::State::~State() = default;


    VulkanQueue::VulkanQueue()
        : _state(nullptr)
    {
    }

    VulkanQueue::VulkanQueue(const VulkanQueue::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanQueue::~VulkanQueue() = default;

    void VulkanQueue::submit(const VulkanCommandBuffer& vulkan_command_buffer)
    {
        auto vk_command_buffer = vulkan_command_buffer.get_native();

        auto vk_submit_info = VkSubmitInfo{};
        vk_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vk_submit_info.commandBufferCount = 1;
        vk_submit_info.pCommandBuffers = &vk_command_buffer;

        vkQueueSubmit(
            _state->vk_queue,
            1,
            &vk_submit_info,
            VK_NULL_HANDLE);

        vkQueueWaitIdle(_state->vk_queue);
    }

    VkQueue VulkanQueue::get_native() const
    {
        return _state->vk_queue;
    }
}