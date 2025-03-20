#include <xar_engine/graphics/backend/component/vulkan/vulkan_graphics_pipeline_component.hpp>

#include <xar_engine/graphics/backend/vulkan/vulkan_type_converters.hpp>


namespace xar_engine::graphics::backend::component::vulkan
{
    api::GraphicsPipelineReference IVulkanGraphicsPipelineComponent::make_graphics_pipeline(const MakeGraphicsPipelineParameters& parameters)
    {
        struct Constants
        {
            float time;
            std::int32_t material_index;
        } pc;

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(Constants); // TODO()

        std::vector<VkDescriptorSetLayout> vk_descriptor_set_layout{};
        for (const auto& descriptor_set_layout: parameters.descriptor_set_layout_list)
        {
            vk_descriptor_set_layout.push_back(get_state()._vulkan_resource_storage.get(descriptor_set_layout).get_native());
        }


        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanGraphicsPipeline{
                {
                    get_state()._vulkan_device,
                    {
                        {
                            get_state()._vulkan_resource_storage.get(parameters.vertex_shader).get_native(),
                            VK_SHADER_STAGE_VERTEX_BIT,
                            "main"
                        },
                        {
                            get_state()._vulkan_resource_storage.get(parameters.fragment_shader).get_native(),
                            VK_SHADER_STAGE_FRAGMENT_BIT,
                            "main"
                        }
                    },
                    std::move(vk_descriptor_set_layout),
                    xar_engine::graphics::backend::vulkan::to_vk_vertex_input_binding_description(parameters.vertex_input_binding_list),
                    xar_engine::graphics::backend::vulkan::to_vk_vertex_input_attribute_description(parameters.vertex_input_attribute_list),
                    {pushConstantRange},
                    static_cast<VkSampleCountFlagBits>(parameters.sample_counts),
                    xar_engine::graphics::backend::vulkan::to_vk_format(parameters.color_format),
                    xar_engine::graphics::backend::vulkan::to_vk_format(parameters.depth_format),
                }});
    }

    void IVulkanGraphicsPipelineComponent::set_pipeline_state(const SetPipelineStateParameters& parameters)
    {
        const auto vk_command_buffer = get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native();

        vkCmdBindPipeline(
            vk_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            get_state()._vulkan_resource_storage.get(parameters.graphics_pipeline).get_native());

        const auto swap_chain_vk_extent = get_state()._vulkan_resource_storage.get(parameters.swap_chain).get_vk_extent();

        auto vk_viewport = VkViewport{};
        vk_viewport.x = 0.0f;
        vk_viewport.y = 0.0f;
        vk_viewport.width = static_cast<float>(swap_chain_vk_extent.width);
        vk_viewport.height = static_cast<float>(swap_chain_vk_extent.height);
        vk_viewport.minDepth = 0.0f;
        vk_viewport.maxDepth = 1.0f;
        vkCmdSetViewport(
            vk_command_buffer,
            0,
            1,
            &vk_viewport);

        auto scissor_vk_rect_2d = VkRect2D{};
        scissor_vk_rect_2d.offset = {0, 0};
        scissor_vk_rect_2d.extent = swap_chain_vk_extent;
        vkCmdSetScissor(
            vk_command_buffer,
            0,
            1,
            &scissor_vk_rect_2d);

        std::vector<VkDescriptorSet> vk_descriptor_set_list{};
        for (const auto& descriptor_set: parameters.descriptor_set_list)
        {
            vk_descriptor_set_list.push_back(get_state()._vulkan_resource_storage.get(descriptor_set).get_native());
        }

        vkCmdBindDescriptorSets(
            vk_command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            get_state()._vulkan_resource_storage.get(parameters.graphics_pipeline).get_native_pipeline_layout(),
            0,
            vk_descriptor_set_list.size(),
            vk_descriptor_set_list.data(),
            0,
            nullptr);
    }

    void IVulkanGraphicsPipelineComponent::set_vertex_buffer_list(const SetVertexBufferListParameters& parameters)
    {

        XAR_THROW_IF(
            parameters.vertex_buffer_list.size() != parameters.vertex_buffer_offset_list.size(),
            error::XarException,
            "Number of buffers {} is different than number of offsets {}",
            parameters.vertex_buffer_list.size(),
            parameters.vertex_buffer_offset_list.size());

        auto vk_buffer_list = std::vector<VkBuffer>();
        auto vk_buffer_size_list = std::vector<VkDeviceSize>();
        for (auto i = 0; i < parameters.vertex_buffer_list.size(); ++i)
        {
            vk_buffer_list.push_back(get_state()._vulkan_resource_storage.get(parameters.vertex_buffer_list[i]).get_native());
            vk_buffer_size_list.push_back(static_cast<VkDeviceSize>(parameters.vertex_buffer_offset_list[i]));
        }

        vkCmdBindVertexBuffers(
            get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native(),
            parameters.first_slot,
            parameters.vertex_buffer_list.size(),
            vk_buffer_list.data(),
            vk_buffer_size_list.data());
    }

    void IVulkanGraphicsPipelineComponent::set_index_buffer(const SetIndexBufferParameters& parameters)
    {
        vkCmdBindIndexBuffer(
            get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native(),
            get_state()._vulkan_resource_storage.get(parameters.index_buffer).get_native(),
            parameters.first_index,
            VK_INDEX_TYPE_UINT32);
    }

    void IVulkanGraphicsPipelineComponent::push_constants(const PushConstantsParameters& parameters)
    {
        vkCmdPushConstants(
            get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native(),
            get_state()._vulkan_resource_storage.get(parameters.graphics_pipeline).get_native_pipeline_layout(),
            xar_engine::graphics::backend::vulkan::to_vk_shader_stage(parameters.shader_type),
            parameters.offset,
            parameters.byte_size,
            parameters.data_byte);
    }

    void IVulkanGraphicsPipelineComponent::draw_indexed(const DrawIndexedParameters& parameters)
    {
        vkCmdDrawIndexed(
            get_state()._vulkan_resource_storage.get(parameters.command_buffer).get_native(),
            static_cast<std::uint32_t>(parameters.index_counts),
            static_cast<std::uint32_t>(parameters.instance_counts),
            static_cast<std::uint32_t>(parameters.first_index),
            static_cast<std::int32_t>(parameters.vertex_offset),
            static_cast<std::uint32_t>(parameters.first_instance));
    }
}