#include <xar_engine/graphics/backend/unit/vulkan/vulkan_command_buffer_unit.hpp>


namespace xar_engine::graphics::backend::unit::vulkan
{
    std::vector<api::CommandBufferReference> IVulkanCommandBufferUnit::make_command_buffer_list(const MakeCommandBufferParameters& parameters)
    {
        auto vk_command_buffer_list = get_state()._vulkan_command_buffer_pool.make_buffer_list(parameters.buffer_counts);

        auto _vulkan_command_buffer_list = std::vector<api::CommandBufferReference>{};
        _vulkan_command_buffer_list.reserve(parameters.buffer_counts);
        for (auto& vk_command_buffer: vk_command_buffer_list)
        {
            _vulkan_command_buffer_list.push_back(get_state()._vulkan_resource_storage.add(std::move(vk_command_buffer)));
        }

        return _vulkan_command_buffer_list;
    }

    void IVulkanCommandBufferUnit::begin_command_buffer(const BeginCommandBufferParameters& parameters)
    {
        get_state()._vulkan_resource_storage.get(parameters.command_buffer).begin(
            parameters.command_buffer_type == api::ECommandBufferType::ONE_TIME);
    }

    void IVulkanCommandBufferUnit::end_command_buffer(const EndCommandBufferParameters& parameters)
    {
        get_state()._vulkan_resource_storage.get(parameters.command_buffer).end();
    }

    void IVulkanCommandBufferUnit::submit_command_buffer(const SubmitCommandBufferParameters& parameters)
    {
        get_state()._vulkan_graphics_queue.submit(get_state()._vulkan_resource_storage.get(parameters.command_buffer));
    }
}