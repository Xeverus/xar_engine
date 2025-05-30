#include <xar_engine/graphics/native/vulkan/vulkan_buffer.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::native::vulkan
{
    struct VulkanBuffer::State
    {
    public:
        explicit State(const VulkanBuffer::Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;

        VkBuffer vk_buffer;
        VkDeviceMemory vk_device_memory;
        VkDeviceSize vk_byte_size;

    private:
        void cleanup();
    };

    VulkanBuffer::State::State(const VulkanBuffer::Parameters& parameters)
        : vulkan_device{parameters.vulkan_device}
        , vk_buffer{nullptr}
        , vk_device_memory{nullptr}
        , vk_byte_size{parameters.vk_byte_size}
    {
        try
        {
            auto vk_buffer_create_info = VkBufferCreateInfo{};
            vk_buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            vk_buffer_create_info.size = parameters.vk_byte_size;
            vk_buffer_create_info.usage = parameters.vk_buffer_usage_flags;
            vk_buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            const auto vk_create_buffer_result = vkCreateBuffer(
                parameters.vulkan_device.get_native(),
                &vk_buffer_create_info,
                nullptr,
                &vk_buffer);
            XAR_THROW_IF(
                vk_create_buffer_result != VK_SUCCESS,
                error::XarException,
                "Buffer creation failed");

            auto vk_memory_requirements = VkMemoryRequirements{};
            vkGetBufferMemoryRequirements(
                parameters.vulkan_device.get_native(),
                vk_buffer,
                &vk_memory_requirements);

            auto vk_memory_allocate_info = VkMemoryAllocateInfo{};
            vk_memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            vk_memory_allocate_info.allocationSize = vk_memory_requirements.size;
            vk_memory_allocate_info.memoryTypeIndex = parameters.vulkan_device.get_native_physical_device().find_memory_type(
                vk_memory_requirements.memoryTypeBits,
                parameters.vk_memory_property_flags);

            const auto vk_memory_allocate_result = vkAllocateMemory(
                parameters.vulkan_device.get_native(),
                &vk_memory_allocate_info,
                nullptr,
                &vk_device_memory);
            XAR_THROW_IF(
                vk_memory_allocate_result != VK_SUCCESS,
                error::XarException,
                "Memory allocation for buffer failed");

            vkBindBufferMemory(
                parameters.vulkan_device.get_native(),
                vk_buffer,
                vk_device_memory,
                0);
        }
        catch (...)
        {
            cleanup();
            throw;
        }
    }

    VulkanBuffer::State::~State()
    {
        cleanup();
    }

    void VulkanBuffer::State::cleanup()
    {
        if (vk_buffer)
        {
            vkDestroyBuffer(
                vulkan_device.get_native(),
                vk_buffer,
                nullptr);
            vk_buffer = nullptr;
        }

        if (vk_device_memory)
        {
            vkFreeMemory(
                vulkan_device.get_native(),
                vk_device_memory,
                nullptr);
            vk_device_memory = nullptr;
        }
    }


    VulkanBuffer::VulkanBuffer()
        : _state(nullptr)
    {
    }

    VulkanBuffer::VulkanBuffer(const VulkanBuffer::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanBuffer::~VulkanBuffer() = default;

    void* VulkanBuffer::map()
    {
        void* data_pointer = nullptr;

        vkMapMemory(
            _state->vulkan_device.get_native(),
            _state->vk_device_memory,
            0,
            _state->vk_byte_size,
            0,
            &data_pointer);

        return data_pointer;
    }

    void VulkanBuffer::unmap()
    {
        vkUnmapMemory(
            _state->vulkan_device.get_native(),
            _state->vk_device_memory);
    }

    VkBuffer VulkanBuffer::get_native() const
    {
        return _state->vk_buffer;
    }

    std::uint32_t VulkanBuffer::get_buffer_byte_size() const
    {
        return _state->vk_byte_size;
    }
}