#include <xar_engine/graphics/backend/vulkan_type_converters.hpp>

#include <xar_engine/error/exception_utils.hpp>


namespace xar_engine::graphics::backend
{
    VkFormat to_vk_format(const api::EFormat image_format)
    {
        switch (image_format)
        {
            case api::EFormat::D32_SFLOAT:
            {
                return VK_FORMAT_D32_SFLOAT;
            }
            case api::EFormat::R32G32_SFLOAT:
            {
                return VK_FORMAT_R32G32_SFLOAT;
            }
            case api::EFormat::R32G32B32_SFLOAT:
            {
                return VK_FORMAT_R32G32B32_SFLOAT;
            }
            case api::EFormat::R8G8B8A8_SRGB:
            {
                return VK_FORMAT_R8G8B8A8_SRGB;
            }
        }

        XAR_THROW(
            error::XarException,
            "api::EFormat value {} is not supported",
            static_cast<std::uint32_t>(image_format));
    }

    VkImageAspectFlagBits to_vk_image_aspec(const api::EImageAspect image_aspect)
    {
        switch (image_aspect)
        {
            case api::EImageAspect::COLOR:
            {
                return VK_IMAGE_ASPECT_COLOR_BIT;
            }
            case api::EImageAspect::DEPTH:
            {
                return VK_IMAGE_ASPECT_DEPTH_BIT;
            }
        }

        XAR_THROW(
            error::XarException,
            "EImageAspect value {} is not supported",
            static_cast<std::uint32_t>(image_aspect));
    }

    std::vector<VkVertexInputBindingDescription> to_vk_vertex_input_binding_description(const std::vector<api::VertexInputBinding>& vertex_input_binding_list)
    {
        auto vk_vertex_input_binding_list = std::vector<VkVertexInputBindingDescription>(vertex_input_binding_list.size());
        for (auto i = 0; i < vertex_input_binding_list.size(); ++i)
        {
            vk_vertex_input_binding_list[i].binding = vertex_input_binding_list[i].binding_index;
            vk_vertex_input_binding_list[i].stride = vertex_input_binding_list[i].stride;
            vk_vertex_input_binding_list[i].inputRate =
                vertex_input_binding_list[i].input_rate == api::VertexInputBindingRate::PER_VERTEX
                ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
        }

        return vk_vertex_input_binding_list;
    }

    std::vector<VkVertexInputAttributeDescription> to_vk_vertex_input_attribute_description(const std::vector<api::VertexInputAttribute>& vertex_input_attribute_list)
    {
        auto vk_vertex_input_attribute_list = std::vector<VkVertexInputAttributeDescription>(vertex_input_attribute_list.size());
        for (auto i = 0; i < vertex_input_attribute_list.size(); ++i)
        {
            vk_vertex_input_attribute_list[i].binding = vertex_input_attribute_list[i].binding_index;
            vk_vertex_input_attribute_list[i].location = vertex_input_attribute_list[i].location;
            vk_vertex_input_attribute_list[i].format = to_vk_format(vertex_input_attribute_list[i].format);
            vk_vertex_input_attribute_list[i].offset = vertex_input_attribute_list[i].offset;
        }

        return vk_vertex_input_attribute_list;
    }

    VkShaderStageFlagBits to_vk_shader_stage(const api::EShaderType shader_type)
    {
        switch (shader_type)
        {
            case api::EShaderType::FRAGMENT:
            {
                return VK_SHADER_STAGE_FRAGMENT_BIT;
            }
            case api::EShaderType::VERTEX:
            {
                return VK_SHADER_STAGE_VERTEX_BIT;
            }
        }

        XAR_THROW(
            error::XarException,
            "EShaderType value {} is not supported",
            static_cast<std::uint32_t>(shader_type));
    }

    api::ESwapChainResult to_swap_chain_result(const VkResult vk_result)
    {
        switch (vk_result)
        {
            case VK_SUCCESS:
            {
                return api::ESwapChainResult::OK;
            }
            case VK_ERROR_OUT_OF_DATE_KHR:
            case VK_SUBOPTIMAL_KHR:
            {
                return api::ESwapChainResult::RECREATION_REQUIRED;
            }
            default:
            {
                return api::ESwapChainResult::ERROR;
            }
        }
    }
}