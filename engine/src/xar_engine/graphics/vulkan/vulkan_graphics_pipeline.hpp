#pragma once

#include <volk.h>

#include <string>
#include <tuple>
#include <vector>


namespace xar_engine::graphics::vulkan
{
    class VulkanGraphicsPipeline
    {
    public:
        struct Parameters;

    public:
        explicit VulkanGraphicsPipeline(const Parameters& parameters);
        ~VulkanGraphicsPipeline();

        [[nodiscard]]
        VkPipelineLayout get_native_pipeline_layout() const;

        [[nodiscard]]
        VkPipeline get_native_pipeline() const;

    private:
        VkDevice _vk_device;
        VkPipelineLayout _vk_pipeline_layout;
        VkPipeline _vk_pipeline;
    };

    struct VulkanGraphicsPipeline::Parameters
    {
        VkDevice vk_device;

        std::vector<std::tuple<VkShaderModule, VkShaderStageFlagBits, std::string>> vk_shader_with_entry_points;
        VkDescriptorSetLayout vk_descriptor_set_layout;
        std::vector<VkVertexInputBindingDescription> vk_vertex_input_binding_descriptors;
        std::vector<VkVertexInputAttributeDescription> vk_vertex_input_attribute_descriptors;
        std::vector<VkPushConstantRange> vk_push_constants;

        VkSampleCountFlagBits vk_msaa_sample_count;
        VkFormat vk_color_attachment_format;
        VkFormat vk_depth_attachment_format;
    };
}