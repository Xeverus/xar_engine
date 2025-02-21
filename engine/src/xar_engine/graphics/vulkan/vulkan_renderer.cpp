#include <xar_engine/graphics/vulkan/vulkan_renderer.hpp>

#include <chrono>
#include <thread>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <xar_engine/asset/image_loader.hpp>
#include <xar_engine/asset/model_loader.hpp>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/file/file.hpp>

#include <xar_engine/logging/console_logger.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        constexpr auto tag = "Vulkan Sandbox";

        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec2 textureCoords;

            static VkVertexInputBindingDescription getBindingDescription()
            {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return bindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
            {
                std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(Vertex,
                                                           position);

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(Vertex,
                                                           color);

                attributeDescriptions[2].binding = 0;
                attributeDescriptions[2].location = 2;
                attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[2].offset = offsetof(Vertex,
                                                           textureCoords);

                return attributeDescriptions;
            }
        };

        struct UniformBufferObject
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };

        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;
    }

    void VulkanRenderer::init_device()
    {
        _vulkan_physical_device_list = std::make_unique<VulkanPhysicalDeviceList>(
            VulkanPhysicalDeviceList::Parameters{
                _vk_instance
            });
        _vulkan_device = std::make_unique<VulkanDevice>(
            VulkanDevice::Parameters{
                _vulkan_physical_device_list->get_native(0)
            });

        msaaSamples = getMaxUsableSampleCount();
    }

    void VulkanRenderer::destroy_swapchain()
    {
        _vulkan_color_image_view.reset();
        _vulkan_color_image.reset();

        _swap_chain_image_views.clear();

        _vulkan_depth_image_view.reset();
        _vulkan_depth_image.reset();

        vkDestroySwapchainKHR(
            _vulkan_device->get_native(),
            vk_swapchain,
            nullptr);
    }

    void VulkanRenderer::init_swapchain()
    {
        const auto formats = _vulkan_physical_device_list->get_surface_formats(
            0,
            _vk_surface_khr);
        const auto present_modes = _vulkan_physical_device_list->get_present_modes(
            0,
            _vk_surface_khr);
        const auto capabilities = _vulkan_physical_device_list->get_surface_capabilities(
            0,
            _vk_surface_khr);

        for (const auto& format: formats)
        {
            if (format.format == VK_FORMAT_R8G8B8A8_SRGB &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                format_to_use = format;
                break;
            }
        }

        VkPresentModeKHR present_mode_to_use;
        for (const auto& present_mode: present_modes)
        {
            if (present_mode == VK_PRESENT_MODE_FIFO_KHR)
            {
                present_mode_to_use = present_mode;
                break;
            }
        }

        const auto fb_size = _os_window->get_surface_pixel_size();
        swapchainExtent = {
            static_cast<std::uint32_t>(fb_size.x),
            static_cast<std::uint32_t>(fb_size.y),
        };
        swapchainExtent.width = std::clamp(
            swapchainExtent.width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);
        swapchainExtent.height = std::clamp(
            swapchainExtent.height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);
        std::uint32_t image_count = capabilities.minImageCount + 1;
        image_count = capabilities.maxImageCount == 0 ? image_count : std::min(
            image_count,
            capabilities.maxImageCount);

        VkSwapchainCreateInfoKHR swapchain_info{};
        swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.surface = _vk_surface_khr;
        swapchain_info.minImageCount = image_count;
        swapchain_info.imageFormat = format_to_use.format;
        swapchain_info.imageColorSpace = format_to_use.colorSpace;
        swapchain_info.imageExtent = swapchainExtent;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.preTransform = capabilities.currentTransform;
        swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_info.presentMode = present_mode_to_use;
        swapchain_info.clipped = VK_TRUE;
        swapchain_info.oldSwapchain = VK_NULL_HANDLE;

        const auto swapchain_result = vkCreateSwapchainKHR(
            _vulkan_device->get_native(),
            &swapchain_info,
            nullptr,
            &vk_swapchain);
        XAR_THROW_IF(
            swapchain_result != VK_SUCCESS,
            error::XarException,
            "Vulkan swapchain creation failed");

        // get swapchain images p 84
        std::uint32_t swapchain_images_count = 0;
        vkGetSwapchainImagesKHR(
            _vulkan_device->get_native(),
            vk_swapchain,
            &swapchain_images_count,
            nullptr);
        swapchain_images.resize(swapchain_images_count);
        vkGetSwapchainImagesKHR(
            _vulkan_device->get_native(),
            vk_swapchain,
            &swapchain_images_count,
            swapchain_images.data());

        _swap_chain_image_views.reserve(swapchain_images.size());
        for (auto & swap_chain_image : swapchain_images)
        {
            _swap_chain_image_views.emplace_back(VulkanImageView::Parameters{
                _vulkan_device->get_native(),
                swap_chain_image,
                format_to_use.format,
                VK_IMAGE_ASPECT_COLOR_BIT,
                1,
            });
        }
    }

    void VulkanRenderer::init_shaders()
    {
        _vulkan_vertex_shader = std::make_unique<VulkanShader>(
            VulkanShader::Parameters{
                _vulkan_device->get_native(),
                xar_engine::file::read_binary_file("assets/triangle.vert.spv")
            });

        _vulkan_fragment_shader = std::make_unique<VulkanShader>(
            VulkanShader::Parameters{
                _vulkan_device->get_native(),
                xar_engine::file::read_binary_file("assets/triangle.frag.spv")
            });
    }

    void VulkanRenderer::init_descriptor_set_layout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = 1;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<std::uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(
            _vulkan_device->get_native(),
            &layoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void VulkanRenderer::init_graphics_pipeline()
    {
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = _vulkan_vertex_shader->get_native();
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = _vulkan_fragment_shader->get_native();
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        const auto bindingDescription = Vertex::getBindingDescription();
        const auto attributeDescriptions = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<std::uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        const std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = dynamicStates.size();
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_TRUE;
        multisampling.rasterizationSamples = msaaSamples;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                              VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                              VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; // Do fragment shadera
        pushConstantRange.offset = 0;  // Start od początku
        pushConstantRange.size = sizeof(float);  // 4 bajty na float

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; // Optional
        if (vkCreatePipelineLayout(
            _vulkan_device->get_native(),
            &pipelineLayoutInfo,
            nullptr,
            &pipelineLayout) != VK_SUCCESS)
        {
            XAR_THROW(error::XarException,
                      "Cannot create pipeline layout");
        }

        VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo{};
        depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateInfo.stencilTestEnable = VK_FALSE;
        depthStencilStateInfo.depthTestEnable = VK_TRUE;
        depthStencilStateInfo.depthWriteEnable = VK_TRUE;
        depthStencilStateInfo.depthCompareOp = VK_COMPARE_OP_LESS;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencilStateInfo;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;

        pipelineInfo.layout = pipelineLayout;

        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        VkPipelineRenderingCreateInfoKHR pipeline_create{};
        pipeline_create.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
        pipeline_create.pNext = VK_NULL_HANDLE;
        pipeline_create.colorAttachmentCount = 1;
        pipeline_create.pColorAttachmentFormats = &format_to_use.format;
        pipeline_create.depthAttachmentFormat = findDepthFormat();

        pipelineInfo.pNext = &pipeline_create;

        if (vkCreateGraphicsPipelines(
            _vulkan_device->get_native(),
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &graphicsPipeline) != VK_SUCCESS)
        {
            XAR_THROW(error::XarException,
                      "failed to create graphics pipeline!");
        }
    }

    void VulkanRenderer::init_vertex_data()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        auto staging_buffer = VulkanBuffer{{
            _vulkan_device->get_native(),
            _vulkan_physical_device_list->get_native(0),
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        }};

        void* const data = staging_buffer.map();
        memcpy(
            data,
            vertices.data(),
            (size_t) bufferSize);
        staging_buffer.unmap();

        _vulkan_vertex_buffer = std::make_unique<VulkanBuffer>(VulkanBuffer::Parameters{
            _vulkan_device->get_native(),
            _vulkan_physical_device_list->get_native(0),
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        });

        copyBuffer(
            staging_buffer.get_native(),
            _vulkan_vertex_buffer->get_native(),
            bufferSize);
    }

    void VulkanRenderer::init_index_data()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        auto staging_buffer = VulkanBuffer{{
           _vulkan_device->get_native(),
           _vulkan_physical_device_list->get_native(0),
           bufferSize,
           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
       }};

        void* const data = staging_buffer.map();
        memcpy(
            data,
            indices.data(),
            (size_t) bufferSize);
        staging_buffer.unmap();

        _vulkan_index_buffer = std::make_unique<VulkanBuffer>(VulkanBuffer::Parameters{
            _vulkan_device->get_native(),
            _vulkan_physical_device_list->get_native(0),
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        });

        copyBuffer(
            staging_buffer.get_native(),
            _vulkan_index_buffer->get_native(),
            bufferSize);
    }

    void VulkanRenderer::init_model()
    {
        const auto model = xar_engine::asset::ModelLoaderFactory().make()->load_model_from_file("assets/viking_room.obj");

        vertices.resize(model.meshes.back().positions.size());
        for (auto i = 0; i < model.meshes.back().positions.size(); ++i)
        {
            vertices[i].position.x = model.meshes.back().positions[i].x;
            vertices[i].position.y = model.meshes.back().positions[i].y;
            vertices[i].position.z = model.meshes.back().positions[i].z;

            const auto channels = model.meshes.back().texture_coords[0].channel_count;
            vertices[i].textureCoords.x = model.meshes.back().texture_coords[0].coords[i * channels + 0];
            vertices[i].textureCoords.y = model.meshes.back().texture_coords[0].coords[i * channels + 1];
        }

        indices.resize(model.meshes.back().indices.size());
        memcpy(
            indices.data(),
            model.meshes.back().indices.data(),
            sizeof(indices[0]) * indices.size());
    }

    void VulkanRenderer::init_ubo_data()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        _vulkan_uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
        _uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            _vulkan_uniform_buffers[i] = std::make_unique<VulkanBuffer>(VulkanBuffer::Parameters{
                _vulkan_device->get_native(),
                _vulkan_physical_device_list->get_native(0),
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            });
            _uniform_buffers_mapped[i] = _vulkan_uniform_buffers[i]->map();
        }
    }

    void VulkanRenderer::init_descriptors()
    {
        // pool
        std::array<VkDescriptorPoolSize, 2> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<std::int32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(
            _vulkan_device->get_native(),
            &poolInfo,
            nullptr,
            &descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor pool!");
        }

        // sets
        std::vector<VkDescriptorSetLayout> layouts(
            MAX_FRAMES_IN_FLIGHT,
            descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(
            _vulkan_device->get_native(),
            &allocInfo,
            descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = _vulkan_uniform_buffers[i]->get_native();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = _vulkan_texture_image_view->get_native();
            imageInfo.sampler = textureSampler;

            std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            descriptorWrites[0].pImageInfo = nullptr; // Optional
            descriptorWrites[0].pTexelBufferView = nullptr; // Optional

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = nullptr;
            descriptorWrites[1].pImageInfo = &imageInfo; // Optional
            descriptorWrites[1].pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(
                _vulkan_device->get_native(),
                static_cast<std::uint32_t>(descriptorWrites.size()),
                descriptorWrites.data(),
                0,
                nullptr);
        }
    }

    void VulkanRenderer::init_cmd_buffers()
    {
        _vulkan_command_pool = std::make_unique<VulkanCommandBufferPool>(VulkanCommandBufferPool::Parameters{
            _vulkan_device->get_native(),
            _vulkan_device->get_graphics_queue(),
            _vulkan_device->get_graphics_family_index(),
        });
        _vk_command_buffers = _vulkan_command_pool->make_buffers(MAX_FRAMES_IN_FLIGHT);
    }

    void VulkanRenderer::init_color_msaa()
    {
        _vulkan_color_image = std::make_unique<VulkanImage>(VulkanImage::Parameters{
            _vulkan_device->get_native(),
            _vulkan_physical_device_list->get_native(0),
            {static_cast<std::int32_t>(swapchainExtent.width), static_cast<std::int32_t>(swapchainExtent.height), 1},
            format_to_use.format,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            1,
            msaaSamples,
        });

        _vulkan_color_image_view = std::make_unique<VulkanImageView>(VulkanImageView::Parameters{
            _vulkan_device->get_native(),
            _vulkan_color_image->get_native(),
            format_to_use.format,
            VK_IMAGE_ASPECT_COLOR_BIT,
            1,
        });
    }

    void VulkanRenderer::init_depth()
    {
        VkFormat depthFormat = findDepthFormat();

        _vulkan_depth_image = std::make_unique<VulkanImage>(VulkanImage::Parameters{
            _vulkan_device->get_native(),
            _vulkan_physical_device_list->get_native(0),
            {static_cast<std::int32_t>(swapchainExtent.width), static_cast<std::int32_t>(swapchainExtent.height), 1},
            findDepthFormat(),
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            1,
            msaaSamples,
        });

        _vulkan_depth_image_view = std::make_unique<VulkanImageView>(VulkanImageView::Parameters{
            _vulkan_device->get_native(),
            _vulkan_depth_image->get_native(),
            findDepthFormat(),
            VK_IMAGE_ASPECT_DEPTH_BIT,
            1,
        });

        transitionImageLayout(
            _vulkan_depth_image->get_native(),
            depthFormat,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            1);
    }

    void VulkanRenderer::init_texture()
    {
        const auto image = asset::ImageLoaderFactory().make()->load_image_from_file("assets/viking_room.png");
        mipLevels = static_cast<uint32_t>(std::floor(
            std::log2(
                std::max(
                    image.pixel_width,
                    image.pixel_height)))) + 1;

        VkDeviceSize imageSize = asset::image::get_byte_size(image);


        auto staging_buffer = VulkanBuffer{{
           _vulkan_device->get_native(),
           _vulkan_physical_device_list->get_native(0),
           imageSize,
           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
       }};

        void* const data = staging_buffer.map();
        memcpy(
            data,
            image.bytes.data(),
            static_cast<size_t>(imageSize));
        staging_buffer.unmap();

        _vulkan_texture_image = std::make_unique<VulkanImage>(VulkanImage::Parameters{
            _vulkan_device->get_native(),
            _vulkan_physical_device_list->get_native(0),
            {image.pixel_width, image.pixel_height, 1},
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mipLevels,
            VK_SAMPLE_COUNT_1_BIT,
        });

        transitionImageLayout(
            _vulkan_texture_image->get_native(),
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            mipLevels);

        copyBufferToImage(
            staging_buffer.get_native(),
            _vulkan_texture_image->get_native(),
            image.pixel_width,
            image.pixel_height);

        generateMipmaps(
            _vulkan_texture_image->get_native(),
            VK_FORMAT_R8G8B8A8_SRGB,
            image.pixel_width,
            image.pixel_height,
            mipLevels);
    }

    void VulkanRenderer::init_texture_view()
    {
        _vulkan_texture_image_view = std::make_unique<VulkanImageView>(VulkanImageView::Parameters{
            _vulkan_device->get_native(),
            _vulkan_texture_image->get_native(),
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_ASPECT_COLOR_BIT,
            mipLevels,
        });
    }

    void VulkanRenderer::init_sampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = _vulkan_physical_device_list->get_device_properties(0).limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0;
        samplerInfo.maxLod = static_cast<float>(mipLevels);

        if (vkCreateSampler(
            _vulkan_device->get_native(),
            &samplerInfo,
            nullptr,
            &textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void VulkanRenderer::init_sync_objects()
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        imageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFence.resize(MAX_FRAMES_IN_FLIGHT);
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            if (vkCreateSemaphore(
                _vulkan_device->get_native(),
                &semaphoreInfo,
                nullptr,
                &imageAvailableSemaphore[i]) != VK_SUCCESS ||
                vkCreateSemaphore(
                    _vulkan_device->get_native(),
                    &semaphoreInfo,
                    nullptr,
                    &renderFinishedSemaphore[i]) != VK_SUCCESS ||
                vkCreateFence(
                    _vulkan_device->get_native(),
                    &fenceInfo,
                    nullptr,
                    &inFlightFence[i]) != VK_SUCCESS)
            {
                XAR_THROW(error::XarException,
                          "failed to create semaphore nr {}!",
                          i);
            }
        }
    }

    void VulkanRenderer::wait()
    {
        _vulkan_device->wait_idle();
    }

    void VulkanRenderer::run_frame_sandbox()
    {
        vkWaitForFences(
            _vulkan_device->get_native(),
            1,
            &inFlightFence[currentFrame],
            VK_TRUE,
            UINT64_MAX);
        vkResetFences(
            _vulkan_device->get_native(),
            1,
            &inFlightFence[currentFrame]);

        uint32_t imageIndex;
        const auto acquire_img_result = vkAcquireNextImageKHR(
            _vulkan_device->get_native(),
            vk_swapchain,
            UINT64_MAX,
            imageAvailableSemaphore[currentFrame],
            VK_NULL_HANDLE,
            &imageIndex);
        if (acquire_img_result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                *_logger,
                tag,
                "Acquire failed because Swapchain is out of date");
            wait();
            destroy_swapchain();
            init_swapchain();
            init_color_msaa();
            init_depth();
            XAR_LOG(
                logging::LogLevel::DEBUG,
                *_logger,
                tag,
                "Acquire failed because Swapchain is out of date but swapchain was recreated");
            return;
        }
        else if (acquire_img_result != VK_SUCCESS && acquire_img_result != VK_SUBOPTIMAL_KHR)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                *_logger,
                tag,
                "Acquire failed because Swapchain");
            return;
        }

        // Only reset the fence if we are submitting work
        vkResetFences(
            _vulkan_device->get_native(),
            1,
            &inFlightFence[currentFrame]);

        // Record CMD buffer
        {
            vkResetCommandBuffer(
                _vk_command_buffers[currentFrame],
                0);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(
                _vk_command_buffers[currentFrame],
                &beginInfo) != VK_SUCCESS)
            {
                XAR_THROW(error::XarException,
                          "failed to begin recording command buffer!");
            }

            struct Constants
            {
                float time;
            };

            const VkImageMemoryBarrier image_memory_barrier_start{
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .image = swapchain_images[imageIndex],
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            vkCmdPipelineBarrier(
                _vk_command_buffers[currentFrame],
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,  // srcStageMask
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // dstStageMask
                0,
                0,
                nullptr,
                0,
                nullptr,
                1, // imageMemoryBarrierCount
                &image_memory_barrier_start // pImageMemoryBarriers
            );

            VkClearValue clearColorColor{};
            clearColorColor.color = {0.0f, 0.0f, 0.0f, 1.0f};

            VkRenderingAttachmentInfoKHR vkRenderingAttachmentInfoColor{};
            vkRenderingAttachmentInfoColor.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
            vkRenderingAttachmentInfoColor.clearValue = clearColorColor;
            vkRenderingAttachmentInfoColor.imageView = _vulkan_color_image_view->get_native();
            vkRenderingAttachmentInfoColor.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            vkRenderingAttachmentInfoColor.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            vkRenderingAttachmentInfoColor.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            vkRenderingAttachmentInfoColor.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
            vkRenderingAttachmentInfoColor.resolveImageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
            vkRenderingAttachmentInfoColor.resolveImageView = _swap_chain_image_views[imageIndex].get_native();

            VkClearValue clearDepthColor{};
            clearDepthColor.depthStencil = {1.0f, 0};

            VkRenderingAttachmentInfoKHR vkRenderingAttachmentInfoDepth{};
            vkRenderingAttachmentInfoDepth.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
            vkRenderingAttachmentInfoDepth.clearValue = clearDepthColor;
            vkRenderingAttachmentInfoDepth.imageView = _vulkan_depth_image_view->get_native();
            vkRenderingAttachmentInfoDepth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            vkRenderingAttachmentInfoDepth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            vkRenderingAttachmentInfoDepth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            VkRenderingInfoKHR renderingInfo{};
            renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
            renderingInfo.colorAttachmentCount = 1;
            renderingInfo.pColorAttachments = &vkRenderingAttachmentInfoColor;
            renderingInfo.pDepthAttachment = &vkRenderingAttachmentInfoDepth;
            renderingInfo.renderArea = VkRect2D{VkOffset2D{}, swapchainExtent};
            renderingInfo.layerCount = 1;

            vkCmdBeginRenderingKHR(
                _vk_command_buffers[currentFrame],
                &renderingInfo);

            vkCmdBindPipeline(
                _vk_command_buffers[currentFrame],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                graphicsPipeline);

            const Constants pc = {static_cast<float>(frameCounter)};
            vkCmdPushConstants(
                _vk_command_buffers[currentFrame],
                pipelineLayout,
                VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(Constants),
                &pc);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float) swapchainExtent.width;
            viewport.height = (float) swapchainExtent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(
                _vk_command_buffers[currentFrame],
                0,
                1,
                &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = swapchainExtent;
            vkCmdSetScissor(
                _vk_command_buffers[currentFrame],
                0,
                1,
                &scissor);

            VkBuffer vertexBuffers[] = {_vulkan_vertex_buffer->get_native()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(
                _vk_command_buffers[currentFrame],
                0,
                1,
                vertexBuffers,
                offsets);
            vkCmdBindIndexBuffer(
                _vk_command_buffers[currentFrame],
                _vulkan_index_buffer->get_native(),
                0,
                VK_INDEX_TYPE_UINT32);

            vkCmdBindDescriptorSets(
                _vk_command_buffers[currentFrame],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                0,
                1,
                &descriptorSets[currentFrame],
                0,
                nullptr);

            vkCmdDrawIndexed(
                _vk_command_buffers[currentFrame],
                static_cast<uint32_t>(indices.size()),
                1,
                0,
                0,
                0);

            vkCmdEndRenderingKHR(_vk_command_buffers[currentFrame]);

            const VkImageMemoryBarrier image_memory_barrier_end{
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                .image = swapchain_images[imageIndex],
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                }
            };

            vkCmdPipelineBarrier(
                _vk_command_buffers[currentFrame],
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,  // srcStageMask
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, // dstStageMask
                0,
                0,
                nullptr,
                0,
                nullptr,
                1, // imageMemoryBarrierCount
                &image_memory_barrier_end // pImageMemoryBarriers
            );

            if (vkEndCommandBuffer(_vk_command_buffers[currentFrame]) != VK_SUCCESS)
            {
                XAR_THROW(error::XarException,
                          "failed to record command buffer!");
            }

            updateUniformBuffer(currentFrame);
        }

        // Submit buffer
        {
            VkSemaphore waitSemaphores[] = {imageAvailableSemaphore[currentFrame]};
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            VkSemaphore signalSemaphores[] = {renderFinishedSemaphore[currentFrame]};

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &_vk_command_buffers[currentFrame];
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            if (vkQueueSubmit(
                _vulkan_device->get_graphics_queue(),
                1,
                &submitInfo,
                inFlightFence[currentFrame]) != VK_SUCCESS)
            {
                XAR_THROW(error::XarException,
                          "failed to submit draw command buffer!");
            }

            VkSwapchainKHR swapChains[] = {vk_swapchain};
            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr; // Optional
            const auto present_result = vkQueuePresentKHR(
                _vulkan_device->get_graphics_queue(),
                &presentInfo);

            if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR)
            {
                XAR_LOG(
                    logging::LogLevel::ERROR,
                    *_logger,
                    tag,
                    "Present failed because Swapchain is out of date");
                wait();
                destroy_swapchain();
                init_swapchain();
                init_color_msaa();
                init_depth();
                XAR_LOG(
                    logging::LogLevel::DEBUG,
                    *_logger,
                    tag,
                    "Present failed because Swapchain is out of date but swapchain was recreated");
            }
            else if (present_result != VK_SUCCESS)
            {
                XAR_LOG(
                    logging::LogLevel::ERROR,
                    *_logger,
                    tag,
                    "Acquire failed because Swapchain");
                return;
            }
        }

        XAR_LOG(
            xar_engine::logging::LogLevel::DEBUG,
            *_logger,
            tag,
            "Frame buffer nr {}, frames in total {}",
            currentFrame,
            frameCounter);

        // change frame index
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        ++frameCounter;

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(16ms);
    }

    void VulkanRenderer::cleanup_sandbox()
    {
        _vulkan_device->wait_idle();

        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            vkDestroyFence(
                _vulkan_device->get_native(),
                inFlightFence[i],
                nullptr);
            vkDestroySemaphore(
                _vulkan_device->get_native(),
                renderFinishedSemaphore[i],
                nullptr);
            vkDestroySemaphore(
                _vulkan_device->get_native(),
                imageAvailableSemaphore[i],
                nullptr);
        }

        _vulkan_command_pool.reset();

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            _vulkan_uniform_buffers[i].reset();
        }

        vkDestroyDescriptorPool(
            _vulkan_device->get_native(),
            descriptorPool,
            nullptr);

        // here because we may want to reuse it
        vkDestroyDescriptorSetLayout(
            _vulkan_device->get_native(),
            descriptorSetLayout,
            nullptr);

        vkDestroyPipeline(
            _vulkan_device->get_native(),
            graphicsPipeline,
            nullptr);

        vkDestroyPipelineLayout(
            _vulkan_device->get_native(),
            pipelineLayout,
            nullptr);

        _vulkan_fragment_shader.reset();
        _vulkan_vertex_shader.reset();

        _vulkan_index_buffer.reset();
        _vulkan_vertex_buffer.reset();

        vkDestroySampler(
            _vulkan_device->get_native(),
            textureSampler,
            nullptr);
        _vulkan_texture_image_view.reset();
        _vulkan_texture_image.reset();

        destroy_swapchain();

        vkDestroyDevice(
            _vulkan_device->get_native(),
            nullptr);
        vkDestroySurfaceKHR(
            _vk_instance,
            _vk_surface_khr,
            nullptr);
        vkDestroyInstance(
            _vk_instance,
            nullptr);
#pragma endregion
    }

    void VulkanRenderer::copyBuffer(
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize size)
    {
        VkCommandBuffer tmpCommandBuffer = _vulkan_command_pool->make_one_time_buffer();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(
            tmpCommandBuffer,
            srcBuffer,
            dstBuffer,
            1,
            &copyRegion);

        _vulkan_command_pool->submit_one_time_buffer(tmpCommandBuffer);
    }

    void VulkanRenderer::updateUniformBuffer(uint32_t currentImageNr)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};

        ubo.model = glm::rotate(
            glm::mat4(1.0f),
            time * glm::radians(90.0f),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f));

        ubo.view = glm::lookAt(
            glm::vec3(
                2.0f,
                2.0f,
                2.0f),
            glm::vec3(
                0.0f,
                0.0f,
                0.0f),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f));

        ubo.proj = glm::perspective(
            glm::radians(45.0f),
            swapchainExtent.width / (float) swapchainExtent.height,
            0.1f,
            10.0f);

        ubo.proj[1][1] *= -1;

        memcpy(
            _uniform_buffers_mapped[currentImageNr],
            &ubo,
            sizeof(ubo));
    }

    void VulkanRenderer::transitionImageLayout(
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout,
        uint32_t mipLevels)
    {
        VkCommandBuffer tempCommandBuffer = _vulkan_command_pool->make_one_time_buffer();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(format))
            {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
                 newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask =
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            tempCommandBuffer,
            sourceStage,
            destinationStage,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);

        _vulkan_command_pool->submit_one_time_buffer(tempCommandBuffer);
    }

    void VulkanRenderer::copyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height)
    {
        VkCommandBuffer tempCommandBuffer = _vulkan_command_pool->make_one_time_buffer();

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(
            tempCommandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        _vulkan_command_pool->submit_one_time_buffer(tempCommandBuffer);
    }

    VkFormat VulkanRenderer::findSupportedFormat(
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features)
    {
        for (VkFormat format: candidates)
        {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(
                _vulkan_physical_device_list->get_native(0),
                format,
                &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }

    VkFormat VulkanRenderer::findDepthFormat()
    {
        return findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

    bool VulkanRenderer::hasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    void VulkanRenderer::generateMipmaps(
        VkImage image,
        VkFormat imageFormat,
        int32_t texWidth,
        int32_t texHeight,
        uint32_t mipLevels)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(
            _vulkan_physical_device_list->get_native(0),
            imageFormat,
            &formatProperties);
        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer tempCommandBuffer = _vulkan_command_pool->make_one_time_buffer();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(
                tempCommandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(
                tempCommandBuffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(
                tempCommandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &barrier);

            if (mipWidth > 1)
            {
                mipWidth /= 2;
            }
            if (mipHeight > 1)
            {
                mipHeight /= 2;
            }
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            tempCommandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier);


        _vulkan_command_pool->submit_one_time_buffer(tempCommandBuffer);
    }

    VkSampleCountFlagBits VulkanRenderer::getMaxUsableSampleCount()
    {
        const auto& physicalDeviceProperties = _vulkan_physical_device_list->get_device_properties(0);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
                                    physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT)
        {
            return VK_SAMPLE_COUNT_64_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_32_BIT)
        {
            return VK_SAMPLE_COUNT_32_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_16_BIT)
        {
            return VK_SAMPLE_COUNT_16_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_8_BIT)
        {
            return VK_SAMPLE_COUNT_8_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_4_BIT)
        {
            return VK_SAMPLE_COUNT_4_BIT;
        }
        if (counts & VK_SAMPLE_COUNT_2_BIT)
        {
            return VK_SAMPLE_COUNT_2_BIT;
        }

        return VK_SAMPLE_COUNT_1_BIT;
    }
}


namespace xar_engine::graphics::vulkan
{
    VulkanRenderer::VulkanRenderer(
        VkInstance vk_instance,
        VkSurfaceKHR _vk_surface_khr,
        const os::IWindow* os_window)
        : _vk_instance(vk_instance)
        , _vk_surface_khr(_vk_surface_khr)
        , _os_window(os_window)
        , _logger(std::make_unique<logging::ConsoleLogger>())
        , currentFrame(0)
        , frameCounter(0)
    {
        init_device();
        init_swapchain();
        init_shaders();
        init_descriptor_set_layout();
        init_graphics_pipeline();
        init_cmd_buffers();
        init_color_msaa();
        init_depth();
        init_texture();
        init_texture_view();
        init_sampler();
        init_model();
        init_vertex_data();
        init_index_data();
        init_ubo_data();
        init_descriptors();
        init_sync_objects();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        cleanup_sandbox();
    }

    void VulkanRenderer::update()
    {
        run_frame_sandbox();
    }
}