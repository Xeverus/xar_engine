#include <xar_engine/graphics/native/vulkan/vulkan_command_buffer_pool.hpp>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/graphics/native/vulkan/vulkan_command_buffer.hpp>


namespace xar_engine::graphics::native::vulkan
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

    std::vector<VulkanCommandBuffer> VulkanCommandBufferPool::make_buffer_list(const std::uint32_t count)
    {
        auto vk_command_buffer_allocate_info = VkCommandBufferAllocateInfo{};
        vk_command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        vk_command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        vk_command_buffer_allocate_info.commandPool = _state->vk_command_pool;
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

        auto vulkan_command_buffer_list = std::vector<VulkanCommandBuffer>();
        vulkan_command_buffer_list.reserve(count);
        for (auto vk_command_buffer: vk_command_buffer_list)
        {
            vulkan_command_buffer_list.push_back(
                VulkanCommandBuffer{
                    {
                        *this,
                        vk_command_buffer,
                    }});
        }

        return vulkan_command_buffer_list;
    }

    const VulkanDevice& VulkanCommandBufferPool::get_device() const
    {
        return _state->vulkan_device;
    }

    VulkanDevice& VulkanCommandBufferPool::get_device()
    {
        return _state->vulkan_device;
    }

    VkCommandPool VulkanCommandBufferPool::get_native() const
    {
        return _state->vk_command_pool;
    }
}