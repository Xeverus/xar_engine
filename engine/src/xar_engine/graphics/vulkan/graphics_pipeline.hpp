#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/vulkan/device.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanGraphicsPipeline
    {
    public:
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

    struct VulkanGraphicsPipeline::Parameters
    {
        Device device;

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