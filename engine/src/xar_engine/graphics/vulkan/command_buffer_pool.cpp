#include <xar_engine/graphics/vulkan/command_buffer_pool.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct VulkanCommandBufferPool::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        Device device;

        VkCommandPool vk_command_pool;
    };

    VulkanCommandBufferPool::State::State(const VulkanCommandBufferPool::Parameters& parameters)
        : device(parameters.device)
        , vk_command_pool(nullptr)
    {
        auto create_command_pool = VkCommandPoolCreateInfo{};
        create_command_pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        create_command_pool.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        create_command_pool.queueFamilyIndex = static_cast<std::uint32_t>(parameters.device.get_graphics_family_index());

        const auto create_command_pool_result = vkCreateCommandPool(
            device.get_native(),
            &create_command_pool,
            nullptr,
            &vk_command_pool);
        XAR_THROW_IF(
            create_command_pool_result != VK_SUCCESS,
            error::XarException,
            "failed to create command pool!");
    }

    VulkanCommandBufferPool::State::~State()
    {
        vkDestroyCommandPool(
            device.get_native(),
            vk_command_pool,
            nullptr);
    }


    VulkanCommandBufferPool::VulkanCommandBufferPool()
        : _state(nullptr)
    {
    }

    VulkanCommandBufferPool::VulkanCommandBufferPool(const VulkanCommandBufferPool::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanCommandBufferPool::~VulkanCommandBufferPool() = default;

    VkCommandBuffer VulkanCommandBufferPool::make_one_time_buffer()
    {
        auto allocation = VkCommandBufferAllocateInfo{};
        allocation.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocation.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocation.commandPool = _state->vk_command_pool;
        allocation.commandBufferCount = 1;

        auto command_buffer = VkCommandBuffer{};
        vkAllocateCommandBuffers(
            _state->device.get_native(),
            &allocation,
            &command_buffer);

        auto begin = VkCommandBufferBeginInfo{};
        begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(
            command_buffer,
            &begin);

        return command_buffer;
    }

    void VulkanCommandBufferPool::submit_one_time_buffer(VkCommandBuffer vk_command_buffer)
    {
        vkEndCommandBuffer(vk_command_buffer);

        auto submit = VkSubmitInfo{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &vk_command_buffer;

        vkQueueSubmit(
            _state->device.get_graphics_queue(),
            1,
            &submit,
            VK_NULL_HANDLE);
        vkQueueWaitIdle(_state->device.get_graphics_queue());

        vkFreeCommandBuffers(
            _state->device.get_native(),
            _state->vk_command_pool,
            1,
            &vk_command_buffer);
    }

    std::vector<VkCommandBuffer> VulkanCommandBufferPool::make_buffers(const std::int32_t count)
    {
        auto allocation = VkCommandBufferAllocateInfo{};
        allocation.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocation.commandPool = _state->vk_command_pool;
        allocation.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocation.commandBufferCount = count;

        auto vk_command_buffers = std::vector<VkCommandBuffer>(count);
        const auto allocation_result = vkAllocateCommandBuffers(
            _state->device.get_native(),
            &allocation,
            vk_command_buffers.data());

        XAR_THROW_IF(
            allocation_result != VK_SUCCESS,
            error::XarException,
            "failed to allocate command buffers!");

        return vk_command_buffers;
    }
}