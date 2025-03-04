#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/native/vulkan_device.hpp>


namespace xar_engine::graphics::vulkan::native
{
    class VulkanGraphicsPipeline
    {
    public:
        struct ShaderParameters;
        struct Parameters;

    public:
        VulkanGraphicsPipeline();
        explicit VulkanGraphicsPipeline(const Parameters& parameters);

        ~VulkanGraphicsPipeline();

        [[nodiscard]]
        VkPipelineLayout get_native_pipeline_layout() const;

        [[nodiscard]]
        VkPipeline get_native_pipeline() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct VulkanGraphicsPipeline::ShaderParameters
    {
        VkShaderModule vk_shader_module;
        VkShaderStageFlagBits vk_shader_stage_flag_bits;
        std::string entry_point;
    };

    struct VulkanGraphicsPipeline::Parameters
    {
        VulkanDevice vulkan_device;

        std::vector<ShaderParameters> vk_shader_module_list;
        VkDescriptorSetLayout vk_descriptor_set_layout;
        std::vector<VkVertexInputBindingDescription> vk_vertex_input_binding_descriptor_list;
        std::vector<VkVertexInputAttributeDescription> vk_vertex_input_attribute_descriptor_list;
        std::vector<VkPushConstantRange> vk_push_constant_range_list;

        VkSampleCountFlagBits vk_sample_count_flag_bits;
        VkFormat vk_color_attachment_format;
        VkFormat vk_depth_attachment_format;
    };
}