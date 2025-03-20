#include <xar_engine/graphics/backend/component/descriptor_component.hpp>
#include "vulkan_descriptor_component.hpp"


namespace xar_engine::graphics::backend::component::vulkan
{
    namespace
    {
        std::uint32_t get_uniform_buffer_count(const native::vulkan::VulkanDevice& vulkan_device)
        {
            return std::min(
                std::uint32_t{32},
                vulkan_device.get_native_physical_device().get_vk_device_properties().limits.maxDescriptorSetUniformBuffers);
        }

        std::uint32_t get_combined_image_sampler_count(const native::vulkan::VulkanDevice& vulkan_device)
        {
            return std::min(
                std::uint32_t{1024},
                vulkan_device.get_native_physical_device().get_vk_device_properties().limits.maxDescriptorSetSampledImages);
        }
    }

    api::DescriptorPoolReference IVulkanDescriptorComponent::make_descriptor_pool(const MakeDescriptorPoolParameters& parameters)
    {
        const auto uniform_buffer_count =
            parameters.descriptor_pool_type_list.count(api::EDescriptorType::UNIFORM_BUFFER) == 0 ?
            0 : get_uniform_buffer_count(get_state()._vulkan_device);

        const auto combined_image_sampler_count =
            parameters.descriptor_pool_type_list.count(api::EDescriptorType::SAMPLED_IMAGE) == 0 ?
            0 : get_combined_image_sampler_count(get_state()._vulkan_device);

        const auto max_descriptor_set_count = std::uint32_t{16};

        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanDescriptorPool{
                {
                    get_state()._vulkan_device,
                    uniform_buffer_count,
                    combined_image_sampler_count,
                    max_descriptor_set_count,
                }});
    }

    api::DescriptorSetLayoutReference IVulkanDescriptorComponent::make_descriptor_set_layout(const MakeDescriptorSetLayoutParameters& parameters)
    {
        std::vector<VkDescriptorSetLayoutBinding> vk_descriptor_set_layout_binding_list;
        if (parameters.descriptor_pool_type_list.count(api::EDescriptorType::UNIFORM_BUFFER) != 0)
        {
            VkDescriptorSetLayoutBinding uboLayoutBinding{};
            uboLayoutBinding.binding = 0;
            uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            uboLayoutBinding.descriptorCount = get_uniform_buffer_count(get_state()._vulkan_device);
            uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uboLayoutBinding.pImmutableSamplers = nullptr;

            vk_descriptor_set_layout_binding_list.push_back(uboLayoutBinding);
        }

        if (parameters.descriptor_pool_type_list.count(api::EDescriptorType::SAMPLED_IMAGE) != 0)
        {
            VkDescriptorSetLayoutBinding samplerLayoutBinding{};
            samplerLayoutBinding.binding = 1;
            samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            samplerLayoutBinding.descriptorCount = get_combined_image_sampler_count(get_state()._vulkan_device);
            samplerLayoutBinding.pImmutableSamplers = nullptr;
            samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            vk_descriptor_set_layout_binding_list.push_back(samplerLayoutBinding);
        }

        return get_state()._vulkan_resource_storage.add(
            native::vulkan::VulkanDescriptorSetLayout{
                {
                    get_state()._vulkan_device,
                    vk_descriptor_set_layout_binding_list,
                }});
    }

    std::vector<api::DescriptorSetReference> IVulkanDescriptorComponent::make_descriptor_set_list(const MakeDescriptorSetParameters& parameters)
    {
        auto layouts = std::vector<native::vulkan::VulkanDescriptorSetLayout>(
            parameters.descriptor_counts,
            get_state()._vulkan_resource_storage.get(parameters.descriptor_set_layout));

        auto vulkan_descriptor_set_ref_list = std::vector<api::DescriptorSetReference>();
        auto vulkan_descriptor_set_list = get_state()._vulkan_resource_storage.get(parameters.descriptor_pool).make_descriptor_set_list(layouts);
        for (auto& vulkan_descriptor_set: vulkan_descriptor_set_list)
        {
            vulkan_descriptor_set_ref_list.push_back(get_state()._vulkan_resource_storage.add(std::move(vulkan_descriptor_set)));
        }

        return vulkan_descriptor_set_ref_list;
    }

    void IVulkanDescriptorComponent::write_descriptor_set(const WriteDescriptorSetParameters& parameters)
    {

        XAR_THROW_IF(
            parameters.texture_image_view_list.size() != parameters.sampler_list.size(),
            error::XarException,
            "Texture counts {} differs from sampler counts {}",
            parameters.texture_image_view_list.size(),
            parameters.sampler_list.size());

        std::vector<VkWriteDescriptorSet> descriptorWrites{};

        std::vector<VkDescriptorBufferInfo> vk_descriptor_buffer_info_list{};
        if (!parameters.uniform_buffer_list.empty())
        {
            const auto uniform_buffer_counts =
                get_uniform_buffer_count(get_state()._vulkan_device) - parameters.uniform_buffer_first_index;
            for (auto i = 0; i < uniform_buffer_counts; ++i)
            {
                const auto object_index = i < parameters.uniform_buffer_list.size() ? i : 0;
                const auto& vulkan_uniform_buffer = get_state()._vulkan_resource_storage.get(parameters.uniform_buffer_list[object_index]);

                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = vulkan_uniform_buffer.get_native();
                bufferInfo.offset = 0;
                bufferInfo.range = vulkan_uniform_buffer.get_buffer_byte_size();

                vk_descriptor_buffer_info_list.push_back(bufferInfo);
            }

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = get_state()._vulkan_resource_storage.get(parameters.descriptor_set).get_native();
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = parameters.uniform_buffer_first_index;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = vk_descriptor_buffer_info_list.size();
            descriptorWrite.pBufferInfo = vk_descriptor_buffer_info_list.data();
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            descriptorWrites.push_back(descriptorWrite);
        }

        auto imageInfoList = std::vector<VkDescriptorImageInfo>{};
        if (!parameters.texture_image_view_list.empty())
        {
            const auto combined_image_sampler_count =
                get_combined_image_sampler_count(get_state()._vulkan_device) - parameters.texture_image_first_index;
            for (auto texture_index = 0; texture_index < combined_image_sampler_count; ++texture_index)
            {
                const auto object_index = texture_index < parameters.texture_image_view_list.size() ? texture_index : 0;

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = get_state()._vulkan_resource_storage.get(parameters.texture_image_view_list[object_index]).get_native();
                imageInfo.sampler = get_state()._vulkan_resource_storage.get(parameters.sampler_list[object_index]).get_native();

                imageInfoList.push_back(imageInfo);
            }

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = get_state()._vulkan_resource_storage.get(parameters.descriptor_set).get_native();
            descriptorWrite.dstBinding = 1;
            descriptorWrite.dstArrayElement = parameters.texture_image_first_index;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrite.descriptorCount = imageInfoList.size();
            descriptorWrite.pBufferInfo = nullptr;
            descriptorWrite.pImageInfo = imageInfoList.data();
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            descriptorWrites.push_back(descriptorWrite);
        }

        get_state()._vulkan_resource_storage.get(parameters.descriptor_set).write(descriptorWrites);
    }
}