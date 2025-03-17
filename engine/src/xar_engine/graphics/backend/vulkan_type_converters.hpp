#pragma once

#include <vector>

#include <volk.h>

#include <xar_engine/graphics/api/format.hpp>
#include <xar_engine/graphics/api/graphics_pipeline_reference.hpp>
#include <xar_engine/graphics/api/image_reference.hpp>
#include <xar_engine/graphics/api/shader_reference.hpp>
#include <xar_engine/graphics/api/swap_chain_reference.hpp>


namespace xar_engine::graphics::backend
{
    VkFormat to_vk_format(api::EFormat image_format);

    VkImageAspectFlagBits to_vk_image_aspec(api::EImageAspect image_aspect);

    std::vector<VkVertexInputBindingDescription> to_vk_vertex_input_binding_description(const std::vector<api::VertexInputBinding>& vertex_input_binding_list);

    std::vector<VkVertexInputAttributeDescription> to_vk_vertex_input_attribute_description(const std::vector<api::VertexInputAttribute>& vertex_input_attribute_list);

    VkShaderStageFlagBits to_vk_shader_stage(api::EShaderType shader_type);


    api::ESwapChainResult to_swap_chain_result(VkResult vk_result);
}