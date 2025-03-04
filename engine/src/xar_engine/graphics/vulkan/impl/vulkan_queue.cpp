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
        VkSemaphore vk_semaphore;
    };

    VulkanQueue::State::State(const VulkanQueue::Parameters& parameters)
        : vulkan_device(parameters.vulkan_device)
        , vk_queue(nullptr)
        , vk_semaphore(nullptr)
    {
        vkGetDeviceQueue(
            vulkan_device.get_native(),
            parameters.queue_family_index,
            0,
            &vk_queue);

        auto vk_semaphore_create_info = VkSemaphoreCreateInfo{};
        vk_semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        const auto vk_create_semaphore_result = vkCreateSemaphore(
            vulkan_device.get_native(),
            &vk_semaphore_create_info,
            nullptr,
            &vk_semaphore);

        XAR_THROW_IF(
            vk_create_semaphore_result != VK_SUCCESS,
            error::XarException,
            "vkCreateSemaphore failed");
    }

    VulkanQueue::State::~State()
    {
        vkQueueWaitIdle(vk_queue);

        vkDestroySemaphore(
            vulkan_device.get_native(),
            vk_semaphore,
            nullptr);
    }


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

#if 0
        vkQueueWaitIdle(vk_queue);
#elif 0
        vk_submit_info.waitSemaphoreCount = 1;
        vk_submit_info.pWaitSemaphores = &_state->vk_semaphore;
        vk_submit_info.signalSemaphoreCount = 1;
        vk_submit_info.pSignalSemaphores = &_state->vk_semaphore;
#endif

        vkQueueSubmit(
            _state->vk_queue,
            1,
            &vk_submit_info,
            VK_NULL_HANDLE);
    }

    VkQueue VulkanQueue::get_native() const
    {
        return _state->vk_queue;
    }
}