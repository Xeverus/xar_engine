#include <xar_engine/graphics/backend/unit/vulkan/vulkan_command_buffer_unit.hpp>


namespace xar_engine::graphics::backend::unit::vulkan
{
    std::vector<api::CommandBufferReference> IVulkanCommandBufferUnit::make_command_buffer_list(const MakeCommandBufferParameters& parameters)
    {
        auto& state = get_state();
        auto vk_command_buffer_list = state.vulkan_command_buffer_pool.make_buffer_list(parameters.buffer_counts);

        auto _vulkan_command_buffer_list = std::vector<api::CommandBufferReference>{};
        _vulkan_command_buffer_list.reserve(parameters.buffer_counts);
        for (auto& vk_command_buffer: vk_command_buffer_list)
        {
            _vulkan_command_buffer_list.push_back(state.vulkan_resource_storage.add(std::move(vk_command_buffer)));
        }

        return _vulkan_command_buffer_list;
    }

    void IVulkanCommandBufferUnit::begin_command_buffer(const BeginCommandBufferParameters& parameters)
    {
        XAR_THROW_IF(
            parameters.command_buffer_type != api::ECommandBufferType::ONE_TIME &&
            parameters.command_buffer_type != api::ECommandBufferType::REUSABLE,
            error::XarException,
            "Incorrect command buffer type {}",
            static_cast<std::uint32_t>(parameters.command_buffer_type));

        get_state().vulkan_resource_storage.get(parameters.command_buffer).begin(
            parameters.command_buffer_type == api::ECommandBufferType::ONE_TIME);
    }

    void IVulkanCommandBufferUnit::end_command_buffer(const EndCommandBufferParameters& parameters)
    {
        get_state().vulkan_resource_storage.get(parameters.command_buffer).end();
    }

    void IVulkanCommandBufferUnit::submit_command_buffer(const SubmitCommandBufferParameters& parameters)
    {
        auto& state = get_state();
        state.vulkan_graphics_queue.submit(state.vulkan_resource_storage.get(parameters.command_buffer));
    }
}