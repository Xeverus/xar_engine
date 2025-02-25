#include <xar_engine/graphics/vulkan/buffer.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan
{
    struct Buffer::State
    {
    public:
        explicit State(const Buffer::Parameters& parameters);

        ~State();

    public:
        Device device;

        VkBuffer vk_buffer;
        VkDeviceMemory vk_device_memory;
        std::uint32_t byte_size;
    };

    Buffer::State::State(const Buffer::Parameters& parameters)
        : device{parameters.device}
        , vk_buffer{nullptr}
        , vk_device_memory{nullptr}
        , byte_size{0}
    {
        auto buffer_info = VkBufferCreateInfo{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = parameters.vk_byte_size;
        buffer_info.usage = parameters.vk_buffer_usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        const auto vk_create_buffer_result = vkCreateBuffer(
            parameters.device.get_native(),
            &buffer_info,
            nullptr,
            &vk_buffer);
        XAR_THROW_IF(
            vk_create_buffer_result != VK_SUCCESS,
            error::XarException,
            "Failed to create buffer");

        auto memory_requirements = VkMemoryRequirements{};
        vkGetBufferMemoryRequirements(
            parameters.device.get_native(),
            vk_buffer,
            &memory_requirements);

        auto alloc_info = VkMemoryAllocateInfo{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = memory_requirements.size;
        alloc_info.memoryTypeIndex = parameters.device.get_physical_device().find_memory_type(
            memory_requirements.memoryTypeBits,
            parameters.vk_memory_properties);

        byte_size = memory_requirements.size;

        const auto vk_allocate_memory_result = vkAllocateMemory(
            parameters.device.get_native(),
            &alloc_info,
            nullptr,
            &vk_device_memory);
        XAR_THROW_IF(
            vk_allocate_memory_result != VK_SUCCESS,
            error::XarException,
            "Memory allocation for buffer failed");

        vkBindBufferMemory(
            parameters.device.get_native(),
            vk_buffer,
            vk_device_memory,
            0);
    }

    Buffer::State::~State()
    {
        if (vk_buffer)
        {
            vkDestroyBuffer(
                device.get_native(),
                vk_buffer,
                nullptr);
        }

        if (vk_device_memory)
        {
            vkFreeMemory(
                device.get_native(),
                vk_device_memory,
                nullptr);
        }
    }


    Buffer::Buffer()
        : _state(nullptr)
    {
    }

    Buffer::Buffer(const Buffer::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    Buffer::~Buffer() = default;

    void* Buffer::map()
    {
        void* mapped_data = nullptr;

        vkMapMemory(
            _state->device.get_native(),
            _state->vk_device_memory,
            0,
            _state->byte_size,
            0,
            &mapped_data);

        return mapped_data;
    }

    void Buffer::unmap()
    {
        vkUnmapMemory(
            _state->device.get_native(),
            _state->vk_device_memory);
    }

    VkBuffer Buffer::get_native() const
    {
        return _state->vk_buffer;
    }
}