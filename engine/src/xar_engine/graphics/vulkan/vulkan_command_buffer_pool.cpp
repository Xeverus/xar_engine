#include <xar_engine/graphics/vulkan/vulkan_command_buffer_pool.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    VulkanCommandBufferPool::VulkanCommandBufferPool(const VulkanCommandBufferPool::Parameters& parameters)
        : _vk_device(parameters.vk_device)
        , _vk_graphics_queue(parameters.vk_graphics_queue)
        , _vk_command_pool(nullptr)
    {
        auto create_command_pool = VkCommandPoolCreateInfo{};
        create_command_pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        create_command_pool.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        create_command_pool.queueFamilyIndex = static_cast<std::uint32_t>(parameters.vk_graphics_family_index);

        const auto create_command_pool_result = vkCreateCommandPool(
            _vk_device,
            &create_command_pool,
            nullptr,
            &_vk_command_pool);
        XAR_THROW_IF(
            create_command_pool_result != VK_SUCCESS,
            error::XarException,
            "failed to create command pool!");
    }

    VulkanCommandBufferPool::~VulkanCommandBufferPool()
    {
        vkDestroyCommandPool(
            _vk_device,
            _vk_command_pool,
            nullptr);
    }

    VkCommandBuffer VulkanCommandBufferPool::make_one_time_buffer()
    {
        auto allocation = VkCommandBufferAllocateInfo{};
        allocation.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocation.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocation.commandPool = _vk_command_pool;
        allocation.commandBufferCount = 1;

        auto command_buffer = VkCommandBuffer{};
        vkAllocateCommandBuffers(
            _vk_device,
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
            _vk_graphics_queue,
            1,
            &submit,
            VK_NULL_HANDLE);
        vkQueueWaitIdle(_vk_graphics_queue);

        vkFreeCommandBuffers(
            _vk_device,
            _vk_command_pool,
            1,
            &vk_command_buffer);
    }

    std::vector<VkCommandBuffer> VulkanCommandBufferPool::make_buffers(const std::int32_t count)
    {
        auto allocation = VkCommandBufferAllocateInfo{};
        allocation.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocation.commandPool = _vk_command_pool;
        allocation.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocation.commandBufferCount = count;

        auto vk_command_buffers = std::vector<VkCommandBuffer>(count);
        const auto allocation_result = vkAllocateCommandBuffers(
            _vk_device,
            &allocation,
            vk_command_buffers.data());

        XAR_THROW_IF(
            allocation_result != VK_SUCCESS,
            error::XarException,
            "failed to allocate command buffers!");

        return vk_command_buffers;
    }
}