#include <xar_engine/graphics/vulkan/impl/vulkan_command_buffer_pool.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan::impl
{
    struct VulkanCommandBufferPool::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;

        VkCommandPool vk_command_pool;
    };

    VulkanCommandBufferPool::State::State(const VulkanCommandBufferPool::Parameters& parameters)
        : vulkan_device(parameters.vulkan_device)
        , vk_command_pool(nullptr)
    {
        auto vk_command_pool_create_info = VkCommandPoolCreateInfo{};
        vk_command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        vk_command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        vk_command_pool_create_info.queueFamilyIndex = parameters.vulkan_device.get_graphics_family_index();

        const auto vk_create_command_pool_result = vkCreateCommandPool(
            vulkan_device.get_native(),
            &vk_command_pool_create_info,
            nullptr,
            &vk_command_pool);
        XAR_THROW_IF(
            vk_create_command_pool_result != VK_SUCCESS,
            error::XarException,
            "Create command pool failed");
    }

    VulkanCommandBufferPool::State::~State()
    {
        vkDestroyCommandPool(
            vulkan_device.get_native(),
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
        auto vk_command_buffer_allocate_info = VkCommandBufferAllocateInfo{};
        vk_command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        vk_command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        vk_command_buffer_allocate_info.commandPool = _state->vk_command_pool;
        vk_command_buffer_allocate_info.commandBufferCount = 1;

        auto vk_command_buffer = VkCommandBuffer{};
        vkAllocateCommandBuffers(
            _state->vulkan_device.get_native(),
            &vk_command_buffer_allocate_info,
            &vk_command_buffer);

        auto vk_command_buffer_begin_info = VkCommandBufferBeginInfo{};
        vk_command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vk_command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(
            vk_command_buffer,
            &vk_command_buffer_begin_info);

        return vk_command_buffer;
    }

    void VulkanCommandBufferPool::submit_one_time_buffer(VkCommandBuffer vk_command_buffer)
    {
        vkEndCommandBuffer(vk_command_buffer);

        auto vk_submit_info = VkSubmitInfo{};
        vk_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vk_submit_info.commandBufferCount = 1;
        vk_submit_info.pCommandBuffers = &vk_command_buffer;

        vkQueueSubmit(
            _state->vulkan_device.get_graphics_queue(),
            1,
            &vk_submit_info,
            VK_NULL_HANDLE);
        vkQueueWaitIdle(_state->vulkan_device.get_graphics_queue());

        vkFreeCommandBuffers(
            _state->vulkan_device.get_native(),
            _state->vk_command_pool,
            1,
            &vk_command_buffer);
    }

    std::vector<VkCommandBuffer> VulkanCommandBufferPool::make_buffers(const std::int32_t count)
    {
        auto vk_command_buffer_allocate_info = VkCommandBufferAllocateInfo{};
        vk_command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        vk_command_buffer_allocate_info.commandPool = _state->vk_command_pool;
        vk_command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        vk_command_buffer_allocate_info.commandBufferCount = count;

        auto vk_command_buffer_list = std::vector<VkCommandBuffer>(count);
        const auto vk_allocate_command_buffer_result = vkAllocateCommandBuffers(
            _state->vulkan_device.get_native(),
            &vk_command_buffer_allocate_info,
            vk_command_buffer_list.data());

        XAR_THROW_IF(
            vk_allocate_command_buffer_result != VK_SUCCESS,
            error::XarException,
            "failed to allocate command buffers!");

        return vk_command_buffer_list;
    }
}