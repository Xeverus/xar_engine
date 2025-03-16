#include <xar_engine/graphics/vulkan/native/vulkan_graphics_pipeline.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::vulkan::native
{
    struct VulkanGraphicsPipeline::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        VulkanDevice vulkan_device;

        VkPipelineLayout vk_pipeline_layout;
        VkPipeline vk_pipeline;
    };

    VulkanGraphicsPipeline::State::State(const Parameters& parameters)
        : vulkan_device(parameters.vulkan_device)
        , vk_pipeline_layout(nullptr)
        , vk_pipeline(nullptr)
    {
        auto vk_pipeline_shader_stage_create_info_list = std::vector<VkPipelineShaderStageCreateInfo>();
        for (const auto& [shader, stage, entry_point]: parameters.vk_shader_module_list)
        {
            auto vk_pipeline_shader_stage_create_info = VkPipelineShaderStageCreateInfo{};
            vk_pipeline_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vk_pipeline_shader_stage_create_info.stage = stage;
            vk_pipeline_shader_stage_create_info.module = shader;
            vk_pipeline_shader_stage_create_info.pName = entry_point.c_str();

            vk_pipeline_shader_stage_create_info_list.push_back(vk_pipeline_shader_stage_create_info);
        }

        auto vk_pipeline_vertex_input_state_create_info = VkPipelineVertexInputStateCreateInfo{};
        vk_pipeline_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vk_pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = static_cast<std::uint32_t>(parameters.vk_vertex_input_binding_descriptor_list.size());
        vk_pipeline_vertex_input_state_create_info.pVertexBindingDescriptions = parameters.vk_vertex_input_binding_descriptor_list.data();
        vk_pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(parameters.vk_vertex_input_attribute_descriptor_list.size());
        vk_pipeline_vertex_input_state_create_info.pVertexAttributeDescriptions = parameters.vk_vertex_input_attribute_descriptor_list.data();

        auto vk_pipeliine_input_assemmbly_state_create_info = VkPipelineInputAssemblyStateCreateInfo{};
        vk_pipeliine_input_assemmbly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        vk_pipeliine_input_assemmbly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        vk_pipeliine_input_assemmbly_state_create_info.primitiveRestartEnable = VK_FALSE;

        const auto vk_dynamic_state_list = std::vector<VkDynamicState>
            {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
            };

        auto vk_pipeline_dynamic_state_create_info = VkPipelineDynamicStateCreateInfo{};
        vk_pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        vk_pipeline_dynamic_state_create_info.dynamicStateCount = vk_dynamic_state_list.size();
        vk_pipeline_dynamic_state_create_info.pDynamicStates = vk_dynamic_state_list.data();

        auto vk_pipeline_viewport_state_create_info = VkPipelineViewportStateCreateInfo{};
        vk_pipeline_viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        vk_pipeline_viewport_state_create_info.viewportCount = 1;
        vk_pipeline_viewport_state_create_info.scissorCount = 1;

        auto vk_pipeline_rasterization_state_create_info = VkPipelineRasterizationStateCreateInfo{};
        vk_pipeline_rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        vk_pipeline_rasterization_state_create_info.depthClampEnable = VK_FALSE;
        vk_pipeline_rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
        vk_pipeline_rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
        vk_pipeline_rasterization_state_create_info.lineWidth = 1.0f;
        vk_pipeline_rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
        vk_pipeline_rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        vk_pipeline_rasterization_state_create_info.depthBiasEnable = VK_FALSE;
        vk_pipeline_rasterization_state_create_info.depthBiasConstantFactor = 0.0f; // Optional
        vk_pipeline_rasterization_state_create_info.depthBiasClamp = 0.0f; // Optional
        vk_pipeline_rasterization_state_create_info.depthBiasSlopeFactor = 0.0f; // Optional

        auto vk_pipeline_multisample_state_create_info = VkPipelineMultisampleStateCreateInfo{};
        vk_pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        vk_pipeline_multisample_state_create_info.sampleShadingEnable = VK_TRUE;
        vk_pipeline_multisample_state_create_info.rasterizationSamples = parameters.vk_sample_count_flag_bits;
        vk_pipeline_multisample_state_create_info.minSampleShading = 1.0f; // Optional
        vk_pipeline_multisample_state_create_info.pSampleMask = nullptr; // Optional
        vk_pipeline_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE; // Optional
        vk_pipeline_multisample_state_create_info.alphaToOneEnable = VK_FALSE; // Optional

        auto vk_pipeline_color_blend_attachment_state = VkPipelineColorBlendAttachmentState{};
        vk_pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                                  VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                                                  VK_COLOR_COMPONENT_A_BIT;
        vk_pipeline_color_blend_attachment_state.blendEnable = VK_TRUE;
        vk_pipeline_color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        vk_pipeline_color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        vk_pipeline_color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
        vk_pipeline_color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        vk_pipeline_color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        vk_pipeline_color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

        auto vk_pipeline_color_blend_state_create_info = VkPipelineColorBlendStateCreateInfo{};
        vk_pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        vk_pipeline_color_blend_state_create_info.logicOpEnable = VK_FALSE;
        vk_pipeline_color_blend_state_create_info.logicOp = VK_LOGIC_OP_COPY; // Optional
        vk_pipeline_color_blend_state_create_info.attachmentCount = 1;
        vk_pipeline_color_blend_state_create_info.pAttachments = &vk_pipeline_color_blend_attachment_state;
        vk_pipeline_color_blend_state_create_info.blendConstants[0] = 0.0f; // Optional
        vk_pipeline_color_blend_state_create_info.blendConstants[1] = 0.0f; // Optional
        vk_pipeline_color_blend_state_create_info.blendConstants[2] = 0.0f; // Optional
        vk_pipeline_color_blend_state_create_info.blendConstants[3] = 0.0f; // Optional

        auto vk_pipeline_layout_create_info = VkPipelineLayoutCreateInfo{};
        vk_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        vk_pipeline_layout_create_info.setLayoutCount = parameters.vk_descriptor_set_layout.size();
        vk_pipeline_layout_create_info.pSetLayouts = parameters.vk_descriptor_set_layout.data();
        vk_pipeline_layout_create_info.pushConstantRangeCount = static_cast<std::uint32_t>(parameters.vk_push_constant_range_list.size()); // Optional
        vk_pipeline_layout_create_info.pPushConstantRanges = parameters.vk_push_constant_range_list.data(); // Optional
        const auto vk_create_pipeline_layout_result = vkCreatePipelineLayout(
            vulkan_device.get_native(),
            &vk_pipeline_layout_create_info,
            nullptr,
            &vk_pipeline_layout);
        XAR_THROW_IF(
            vk_create_pipeline_layout_result != VK_SUCCESS,
            error::XarException,
            "vkCreatePipelineLayout failed");

        auto vk_pipeline_depth_stencil_state_create_info = VkPipelineDepthStencilStateCreateInfo{};
        vk_pipeline_depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        vk_pipeline_depth_stencil_state_create_info.stencilTestEnable = VK_FALSE;
        vk_pipeline_depth_stencil_state_create_info.depthTestEnable = VK_TRUE;
        vk_pipeline_depth_stencil_state_create_info.depthWriteEnable = VK_TRUE;
        vk_pipeline_depth_stencil_state_create_info.depthCompareOp = VK_COMPARE_OP_LESS;

        auto vk_pipeline_rendering_create_info = VkPipelineRenderingCreateInfoKHR{};
        vk_pipeline_rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        vk_pipeline_rendering_create_info.pNext = VK_NULL_HANDLE;
        vk_pipeline_rendering_create_info.colorAttachmentCount = 1;
        vk_pipeline_rendering_create_info.pColorAttachmentFormats = &parameters.vk_color_attachment_format;
        vk_pipeline_rendering_create_info.depthAttachmentFormat = parameters.vk_depth_attachment_format;

        auto vk_graphics_pipeline_create_info = VkGraphicsPipelineCreateInfo{};
        vk_graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        vk_graphics_pipeline_create_info.stageCount = 2;
        vk_graphics_pipeline_create_info.pStages = vk_pipeline_shader_stage_create_info_list.data();
        vk_graphics_pipeline_create_info.pVertexInputState = &vk_pipeline_vertex_input_state_create_info;
        vk_graphics_pipeline_create_info.pInputAssemblyState = &vk_pipeliine_input_assemmbly_state_create_info;
        vk_graphics_pipeline_create_info.pViewportState = &vk_pipeline_viewport_state_create_info;
        vk_graphics_pipeline_create_info.pRasterizationState = &vk_pipeline_rasterization_state_create_info;
        vk_graphics_pipeline_create_info.pMultisampleState = &vk_pipeline_multisample_state_create_info;
        vk_graphics_pipeline_create_info.pDepthStencilState = &vk_pipeline_depth_stencil_state_create_info;
        vk_graphics_pipeline_create_info.pColorBlendState = &vk_pipeline_color_blend_state_create_info;
        vk_graphics_pipeline_create_info.pDynamicState = &vk_pipeline_dynamic_state_create_info;
        vk_graphics_pipeline_create_info.layout = vk_pipeline_layout;
        vk_graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
        vk_graphics_pipeline_create_info.basePipelineIndex = -1;
        vk_graphics_pipeline_create_info.pNext = &vk_pipeline_rendering_create_info;

        const auto vk_create_graphics_pipeline_result = vkCreateGraphicsPipelines(
            vulkan_device.get_native(),
            VK_NULL_HANDLE,
            1,
            &vk_graphics_pipeline_create_info,
            nullptr,
            &vk_pipeline);
        XAR_THROW_IF(
            vk_create_graphics_pipeline_result != VK_SUCCESS,
            error::XarException,
            "vkCreateGraphicsPipelines failed");
    }

    VulkanGraphicsPipeline::State::~State()
    {
        vkDestroyPipeline(
            vulkan_device.get_native(),
            vk_pipeline,
            nullptr);

        vkDestroyPipelineLayout(
            vulkan_device.get_native(),
            vk_pipeline_layout,
            nullptr);
    }


    VulkanGraphicsPipeline::VulkanGraphicsPipeline()
        : _state(nullptr)
    {
    }

    VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanGraphicsPipeline::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline() = default;

    VkPipelineLayout VulkanGraphicsPipeline::get_native_pipeline_layout() const
    {
        return _state->vk_pipeline_layout;
    }

    VkPipeline VulkanGraphicsPipeline::get_native_pipeline() const
    {
        return _state->vk_pipeline;
    }
}