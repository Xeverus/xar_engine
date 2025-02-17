#include <xar_engine/graphics/vulkan/vulkan.hpp>

#include <chrono>
#include <thread>
#include <vector>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <xar_engine/asset/image_loader.hpp>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/file/file.hpp>

#include <xar_engine/logging/console_logger.hpp>


namespace xar_engine::graphics::vulkan
{
    namespace
    {
        constexpr auto tag = "Vulkan Sandbox";

        struct Vertex
        {
            glm::vec2 position;
            glm::vec3 color;

            static VkVertexInputBindingDescription getBindingDescription()
            {
                VkVertexInputBindingDescription bindingDescription{};
                bindingDescription.binding = 0;
                bindingDescription.stride = sizeof(Vertex);
                bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                return bindingDescription;
            }

            static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
            {
                std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

                attributeDescriptions[0].binding = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(Vertex,
                                                           position);

                attributeDescriptions[1].binding = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(Vertex,
                                                           color);

                return attributeDescriptions;
            }
        };

        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f,  -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f,  0.5f},  {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f},  {1.0f, 1.0f, 1.0f}}
        };

        const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
        };

        struct UniformBufferObject
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };
    }

    Vulkan::Vulkan(os::GlfwWindow* window)
        : vk_instance()
        , _glfw_window(window)
        , _logger(std::make_unique<logging::ConsoleLogger>())
        , currentFrame(0)
        , frameCounter(0)
    {
    }

    void Vulkan::init_surface()
    {
        const auto result = glfwCreateWindowSurface(
            vk_instance.get_native(),
            _glfw_window->get_native(),
            nullptr,
            &vk_surface);
        XAR_THROW_IF(
            result != VK_SUCCESS,
            error::XarException,
            "Failed to create window surface");
    }

    void Vulkan::init_device()
    {
        // physical device
        std::uint32_t physical_devices_count = 0;
        vkEnumeratePhysicalDevices(
            vk_instance.get_native(),
            &physical_devices_count,
            nullptr);
        std::vector<VkPhysicalDevice> physical_devices(physical_devices_count);
        vkEnumeratePhysicalDevices(
            vk_instance.get_native(),
            &physical_devices_count,
            physical_devices.data());

        // log physical device devices
        for (const auto physical_device: physical_devices)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(
                physical_device,
                &properties);

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(
                physical_device,
                &features);
            XAR_THROW_IF(
                !features.samplerAnisotropy,
                error::XarException,
                "Sampler anisotropy is not supported");

            XAR_LOG(
                logging::LogLevel::INFO,
                *_logger,
                tag,
                "{} {} {}",
                properties.deviceName,
                properties.apiVersion,
                properties.driverVersion);

            std::uint32_t queue_families_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(
                physical_device,
                &queue_families_count,
                nullptr);
            std::vector<VkQueueFamilyProperties> queue_families(queue_families_count);
            vkGetPhysicalDeviceQueueFamilyProperties(
                physical_device,
                &queue_families_count,
                queue_families.data());

            std::uint32_t queue_family_index = 0;
            for (const auto& queue_family: queue_families)
            {
                VkBool32 present_support = VK_FALSE;
                vkGetPhysicalDeviceSurfaceSupportKHR(
                    physical_device,
                    queue_family_index,
                    vk_surface,
                    &present_support);
                ++queue_family_index;

                XAR_LOG(
                    logging::LogLevel::INFO,
                    *_logger,
                    tag,
                    "{} {} {}",
                    queue_family.queueCount,
                    (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT),
                    present_support);
            }

            std::uint32_t device_extensions_count = 0;
            vkEnumerateDeviceExtensionProperties(
                physical_device,
                nullptr,
                &device_extensions_count,
                nullptr);
            std::vector<VkExtensionProperties> device_extensions(device_extensions_count);
            vkEnumerateDeviceExtensionProperties(
                physical_device,
                nullptr,
                &device_extensions_count,
                device_extensions.data());
            for (const auto& device_extension: device_extensions)
            {
                XAR_LOG(
                    logging::LogLevel::INFO,
                    *_logger,
                    tag,
                    "{} {}",
                    device_extension.extensionName,
                    device_extension.specVersion);
            }
        }

        graphics_queue_family = 0;  // has VK_QUEUE_GRAPHICS_BIT

        // logical device
        float queue_priority = 1.0f;

        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = graphics_queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;

        VkPhysicalDeviceFeatures device_features{};
        device_features.samplerAnisotropy = VK_TRUE;

        std::vector<const char*> physical_device_extension_names = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
        };

        VkDeviceCreateInfo device_create_info{};
        device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos = &queue_create_info;
        device_create_info.queueCreateInfoCount = 1;
        device_create_info.pEnabledFeatures = &device_features;
        device_create_info.enabledExtensionCount = physical_device_extension_names.size();
        device_create_info.ppEnabledExtensionNames = physical_device_extension_names.data();
        VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_rendering_feature{};
        dynamic_rendering_feature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        dynamic_rendering_feature.dynamicRendering = VK_TRUE;

        device_create_info.pNext = &dynamic_rendering_feature;

        vk_physical_device = physical_devices[0];
        const auto device_create_result = vkCreateDevice(
            physical_devices[0],
            &device_create_info,
            nullptr,
            &vk_device);
        XAR_THROW_IF(
            device_create_result != VK_SUCCESS,
            error::XarException,
            "Vulkan device creation failed");

        vkGetDeviceQueue(
            vk_device,
            graphics_queue_family,
            0,
            &vk_queue);
    }

    void Vulkan::destroy_swapchain()
    {
        for (auto& image: swapchain_image_views)
        {
            vkDestroyImageView(
                vk_device,
                image,
                nullptr);
        }
        swapchain_image_views.clear();

        vkDestroySwapchainKHR(
            vk_device,
            vk_swapchain,
            nullptr);
    }

    void Vulkan::init_swapchain()
    {
        // physical device
        std::uint32_t physical_devices_count = 0;
        vkEnumeratePhysicalDevices(
            vk_instance.get_native(),
            &physical_devices_count,
            nullptr);
        std::vector<VkPhysicalDevice> physical_devices(physical_devices_count);
        vkEnumeratePhysicalDevices(
            vk_instance.get_native(),
            &physical_devices_count,
            physical_devices.data());

        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            physical_devices[0],
            vk_surface,
            &capabilities);

        std::uint32_t formats_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_devices[0],
            vk_surface,
            &formats_count,
            nullptr);
        std::vector<VkSurfaceFormatKHR> formats(formats_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_devices[0],
            vk_surface,
            &formats_count,
            formats.data());

        std::uint32_t present_modes_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_devices[0],
            vk_surface,
            &present_modes_count,
            nullptr);
        std::vector<VkPresentModeKHR> present_modes(present_modes_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            physical_devices[0],
            vk_surface,
            &present_modes_count,
            present_modes.data());

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

        std::int32_t fb_size_x;
        std::int32_t fb_size_y;
        glfwGetFramebufferSize(
            _glfw_window->get_native(),
            &fb_size_x,
            &fb_size_y);

        swapchainExtent = {
            static_cast<std::uint32_t>(fb_size_x),
            static_cast<std::uint32_t>(fb_size_y),
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
        swapchain_info.surface = vk_surface;
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
            vk_device,
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
            vk_device,
            vk_swapchain,
            &swapchain_images_count,
            nullptr);
        swapchain_images.resize(swapchain_images_count);
        vkGetSwapchainImagesKHR(
            vk_device,
            vk_swapchain,
            &swapchain_images_count,
            swapchain_images.data());

        swapchain_image_views.resize(swapchain_images.size());
        for (auto i = 0; i < swapchain_image_views.size(); ++i)
        {
            swapchain_image_views[i] = createImageView(
                swapchain_images[i],
                format_to_use.format);
        }
    }

    void Vulkan::init_shaders()
    {
        const auto vert_bytes = xar_engine::file::File::read_binary_file("assets/triangle.vert.spv");
        const auto frag_bytes = xar_engine::file::File::read_binary_file("assets/triangle.frag.spv");

        VkShaderModuleCreateInfo vertInfo{};
        vertInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertInfo.codeSize = vert_bytes.size();
        vertInfo.pCode = reinterpret_cast<const std::uint32_t*>(vert_bytes.data());

        if (vkCreateShaderModule(
            vk_device,
            &vertInfo,
            nullptr,
            &vertShaderModule) != VK_SUCCESS)
        {
            XAR_THROW(
                error::XarException,
                "vert shader failed");
        }

        VkShaderModuleCreateInfo fragInfo{};
        fragInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragInfo.codeSize = frag_bytes.size();
        fragInfo.pCode = reinterpret_cast<const std::uint32_t*>(frag_bytes.data());

        if (vkCreateShaderModule(
            vk_device,
            &fragInfo,
            nullptr,
            &fragShaderModule) != VK_SUCCESS)
        {
            XAR_THROW(
                error::XarException,
                "frag shader failed");
        }
    }

    void Vulkan::init_descriptor_set_layout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(
            vk_device,
            &layoutInfo,
            nullptr,
            &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void Vulkan::init_graphics_pipeline()
    {
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
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
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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
            vk_device,
            &pipelineLayoutInfo,
            nullptr,
            &pipelineLayout) != VK_SUCCESS)
        {
            XAR_THROW(error::XarException,
                      "Cannot create pipeline layout");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;

        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
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

        pipelineInfo.pNext = &pipeline_create;

        if (vkCreateGraphicsPipelines(
            vk_device,
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

    void Vulkan::init_vertex_data()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(
            vk_device,
            stagingBufferMemory,
            0,
            bufferSize,
            0,
            &data);
        memcpy(
            data,
            vertices.data(),
            (size_t) bufferSize);
        vkUnmapMemory(
            vk_device,
            stagingBufferMemory);

        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer,
            vertexBufferMemory);

        copyBuffer(
            stagingBuffer,
            vertexBuffer,
            bufferSize);

        vkDestroyBuffer(
            vk_device,
            stagingBuffer,
            nullptr);
        vkFreeMemory(
            vk_device,
            stagingBufferMemory,
            nullptr);
    }

    void Vulkan::init_index_data()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(
            vk_device,
            stagingBufferMemory,
            0,
            bufferSize,
            0,
            &data);
        memcpy(
            data,
            indices.data(),
            (size_t) bufferSize);
        vkUnmapMemory(
            vk_device,
            stagingBufferMemory);

        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer,
            indexBufferMemory);

        copyBuffer(
            stagingBuffer,
            indexBuffer,
            bufferSize);

        vkDestroyBuffer(
            vk_device,
            stagingBuffer,
            nullptr);
        vkFreeMemory(
            vk_device,
            stagingBufferMemory,
            nullptr);
    }

    void Vulkan::init_ubo_data()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            createBuffer(
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                uniformBuffers[i],
                uniformBuffersMemory[i]);

            vkMapMemory(
                vk_device,
                uniformBuffersMemory[i],
                0,
                bufferSize,
                0,
                &uniformBuffersMapped[i]);
        }
    }

    void Vulkan::init_descriptors()
    {
        // pool
        VkDescriptorPoolSize poolSize{};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

        if (vkCreateDescriptorPool(
            vk_device,
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
            vk_device,
            &allocInfo,
            descriptorSets.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = descriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr; // Optional
            descriptorWrite.pTexelBufferView = nullptr; // Optional

            vkUpdateDescriptorSets(
                vk_device,
                1,
                &descriptorWrite,
                0,
                nullptr);
        }
    }

    void Vulkan::init_cmd_buffers()
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = graphics_queue_family;
        if (vkCreateCommandPool(
            vk_device,
            &poolInfo,
            nullptr,
            &commandPool) != VK_SUCCESS)
        {
            XAR_THROW(error::XarException,
                      "failed to create command pool!");
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

        commandBuffer.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateCommandBuffers(
            vk_device,
            &allocInfo,
            commandBuffer.data()) != VK_SUCCESS)
        {
            XAR_THROW(error::XarException,
                      "failed to allocate command buffers!");
        }
    }

    void Vulkan::init_texture()
    {
        const auto image = asset::ImageLoaderFactory::make_loader()->load_image_from_file("assets/image.png");

        VkDeviceSize imageSize = asset::image::get_byte_size(image);

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(
            imageSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(
            vk_device,
            stagingBufferMemory,
            0,
            imageSize,
            0,
            &data);
        memcpy(
            data,
            image.bytes.data(),
            static_cast<size_t>(imageSize));
        vkUnmapMemory(
            vk_device,
            stagingBufferMemory);

        createImage(
            image.pixel_width,
            image.pixel_height,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            textureImage,
            textureImageMemory);

        transitionImageLayout(
            textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        copyBufferToImage(
            stagingBuffer,
            textureImage,
            image.pixel_width,
            image.pixel_height);

        transitionImageLayout(
            textureImage,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(
            vk_device,
            stagingBuffer,
            nullptr);
        vkFreeMemory(
            vk_device,
            stagingBufferMemory,
            nullptr);
    }

    void Vulkan::init_texture_view()
    {
        textureImageView = createImageView(
            textureImage,
            VK_FORMAT_R8G8B8A8_SRGB);
    }

    void Vulkan::init_sampler()
    {
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(
            vk_physical_device,
            &properties);

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(
            vk_device,
            &samplerInfo,
            nullptr,
            &textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void Vulkan::init_sync_objects()
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
                vk_device,
                &semaphoreInfo,
                nullptr,
                &imageAvailableSemaphore[i]) != VK_SUCCESS ||
                vkCreateSemaphore(
                    vk_device,
                    &semaphoreInfo,
                    nullptr,
                    &renderFinishedSemaphore[i]) != VK_SUCCESS ||
                vkCreateFence(
                    vk_device,
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

    void Vulkan::wait()
    {
        vkDeviceWaitIdle(vk_device);
    }

    void Vulkan::run_frame_sandbox()
    {
        vkWaitForFences(
            vk_device,
            1,
            &inFlightFence[currentFrame],
            VK_TRUE,
            UINT64_MAX);
        vkResetFences(
            vk_device,
            1,
            &inFlightFence[currentFrame]);

        uint32_t imageIndex;
        const auto acquire_img_result = vkAcquireNextImageKHR(
            vk_device,
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
            vk_device,
            1,
            &inFlightFence[currentFrame]);

        // Record CMD buffer
        {
            vkResetCommandBuffer(
                commandBuffer[currentFrame],
                0);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = 0; // Optional
            beginInfo.pInheritanceInfo = nullptr; // Optional

            if (vkBeginCommandBuffer(
                commandBuffer[currentFrame],
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
                commandBuffer[currentFrame],
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

            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

            VkRenderingAttachmentInfoKHR vkRenderingAttachmentInfo{};
            vkRenderingAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
            vkRenderingAttachmentInfo.clearValue = clearColor;
            vkRenderingAttachmentInfo.imageView = swapchain_image_views[imageIndex];
            vkRenderingAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
            vkRenderingAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            vkRenderingAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

            VkRenderingInfoKHR renderingInfo{};
            renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
            renderingInfo.colorAttachmentCount = 1;
            renderingInfo.pColorAttachments = &vkRenderingAttachmentInfo;
            renderingInfo.renderArea = VkRect2D{VkOffset2D{}, swapchainExtent};
            renderingInfo.layerCount = 1;

            vkCmdBeginRenderingKHR(
                commandBuffer[currentFrame],
                &renderingInfo);

            vkCmdBindPipeline(
                commandBuffer[currentFrame],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                graphicsPipeline);

            const Constants pc = {static_cast<float>(frameCounter)};
            vkCmdPushConstants(
                commandBuffer[currentFrame],
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
                commandBuffer[currentFrame],
                0,
                1,
                &viewport);

            VkRect2D scissor{};
            scissor.offset = {0, 0};
            scissor.extent = swapchainExtent;
            vkCmdSetScissor(
                commandBuffer[currentFrame],
                0,
                1,
                &scissor);

            VkBuffer vertexBuffers[] = {vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(
                commandBuffer[currentFrame],
                0,
                1,
                vertexBuffers,
                offsets);
            vkCmdBindIndexBuffer(
                commandBuffer[currentFrame],
                indexBuffer,
                0,
                VK_INDEX_TYPE_UINT16);

            vkCmdBindDescriptorSets(
                commandBuffer[currentFrame],
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipelineLayout,
                0,
                1,
                &descriptorSets[currentFrame],
                0,
                nullptr);

            vkCmdDrawIndexed(
                commandBuffer[currentFrame],
                static_cast<uint32_t>(indices.size()),
                1,
                0,
                0,
                0);

            vkCmdEndRenderingKHR(commandBuffer[currentFrame]);

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
                commandBuffer[currentFrame],
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

            if (vkEndCommandBuffer(commandBuffer[currentFrame]) != VK_SUCCESS)
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
            submitInfo.pCommandBuffers = &commandBuffer[currentFrame];
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            if (vkQueueSubmit(
                vk_queue,
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
                vk_queue,
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

    void Vulkan::cleanup_sandbox()
    {
        vkDeviceWaitIdle(vk_device);

        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            vkDestroyFence(
                vk_device,
                inFlightFence[i],
                nullptr);
            vkDestroySemaphore(
                vk_device,
                renderFinishedSemaphore[i],
                nullptr);
            vkDestroySemaphore(
                vk_device,
                imageAvailableSemaphore[i],
                nullptr);
        }

        vkDestroyCommandPool(
            vk_device,
            commandPool,
            nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(
                vk_device,
                uniformBuffers[i],
                nullptr);
            vkFreeMemory(
                vk_device,
                uniformBuffersMemory[i],
                nullptr);
        }

        vkDestroyDescriptorPool(
            vk_device,
            descriptorPool,
            nullptr);

        // here because we may want to reuse it
        vkDestroyDescriptorSetLayout(
            vk_device,
            descriptorSetLayout,
            nullptr);

        vkDestroyPipeline(
            vk_device,
            graphicsPipeline,
            nullptr);

        vkDestroyPipelineLayout(
            vk_device,
            pipelineLayout,
            nullptr);

        vkDestroyShaderModule(
            vk_device,
            fragShaderModule,
            nullptr);
        vkDestroyShaderModule(
            vk_device,
            vertShaderModule,
            nullptr);

        vkDestroyBuffer(
            vk_device,
            indexBuffer,
            nullptr);
        vkFreeMemory(
            vk_device,
            indexBufferMemory,
            nullptr);
        vkDestroyBuffer(
            vk_device,
            vertexBuffer,
            nullptr);
        vkFreeMemory(
            vk_device,
            vertexBufferMemory,
            nullptr);

        vkDestroySampler(
            vk_device,
            textureSampler,
            nullptr);
        vkDestroyImageView(
            vk_device,
            textureImageView,
            nullptr);
        vkDestroyImage(
            vk_device,
            textureImage,
            nullptr);
        vkFreeMemory(
            vk_device,
            textureImageMemory,
            nullptr);

        // cleanup
        for (auto view: swapchain_image_views)
        {
            vkDestroyImageView(
                vk_device,
                view,
                nullptr);
        }
        vkDestroySwapchainKHR(
            vk_device,
            vk_swapchain,
            nullptr);

        vkDestroyDevice(
            vk_device,
            nullptr);
        vkDestroySurfaceKHR(
            vk_instance.get_native(),
            vk_surface,
            nullptr);
        vkDestroyInstance(
            vk_instance.get_native(),
            nullptr);
#pragma endregion
    }

    std::uint32_t Vulkan::findMemoryType(
        uint32_t typeFilter,
        VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(
            vk_physical_device,
            &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        XAR_THROW(error::XarException,
                  "Failed to find suitable memory type");
    };

    void Vulkan::createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(
            vk_device,
            &bufferInfo,
            nullptr,
            &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(
            vk_device,
            buffer,
            &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(
            memRequirements.memoryTypeBits,
            properties);

        if (vkAllocateMemory(
            vk_device,
            &allocInfo,
            nullptr,
            &bufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(
            vk_device,
            buffer,
            bufferMemory,
            0);
    }

    void Vulkan::copyBuffer(
        VkBuffer srcBuffer,
        VkBuffer dstBuffer,
        VkDeviceSize size)
    {
        VkCommandBuffer tmpCommandBuffer = beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(
            tmpCommandBuffer,
            srcBuffer,
            dstBuffer,
            1,
            &copyRegion);

        endSingleTimeCommands(tmpCommandBuffer);
    }

    void Vulkan::updateUniformBuffer(uint32_t currentImageNr)
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
            uniformBuffersMapped[currentImageNr],
            &ubo,
            sizeof(ubo));
    }

    void Vulkan::createImage(
        uint32_t width,
        uint32_t height,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImage& image,
        VkDeviceMemory& imageMemory)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(width);
        imageInfo.extent.height = static_cast<uint32_t>(height);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional

        if (vkCreateImage(
            vk_device,
            &imageInfo,
            nullptr,
            &textureImage) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(
            vk_device,
            textureImage,
            &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(
            memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(
            vk_device,
            &allocInfo,
            nullptr,
            &textureImageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(
            vk_device,
            textureImage,
            textureImageMemory,
            0);
    }

    VkCommandBuffer Vulkan::beginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer tmpCommandBuffer;
        vkAllocateCommandBuffers(
            vk_device,
            &allocInfo,
            &tmpCommandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(
            tmpCommandBuffer,
            &beginInfo);

        return tmpCommandBuffer;
    }

    void Vulkan::endSingleTimeCommands(VkCommandBuffer tmpCommandBuffer)
    {
        vkEndCommandBuffer(tmpCommandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &tmpCommandBuffer;

        vkQueueSubmit(
            vk_queue,
            1,
            &submitInfo,
            VK_NULL_HANDLE);
        vkQueueWaitIdle(vk_queue);

        vkFreeCommandBuffers(
            vk_device,
            commandPool,
            1,
            &tmpCommandBuffer);
    }

    void Vulkan::transitionImageLayout(
        VkImage image,
        VkFormat format,
        VkImageLayout oldLayout,
        VkImageLayout newLayout)
    {
        VkCommandBuffer tempCommandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

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

        endSingleTimeCommands(tempCommandBuffer);
    }

    void Vulkan::copyBufferToImage(
        VkBuffer buffer,
        VkImage image,
        uint32_t width,
        uint32_t height)
    {
        VkCommandBuffer tempCommandBuffer = beginSingleTimeCommands();

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

        endSingleTimeCommands(tempCommandBuffer);
    }

    VkImageView Vulkan::createImageView(
        VkImage image,
        VkFormat format)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        if (vkCreateImageView(
            vk_device,
            &viewInfo,
            nullptr,
            &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }

        return imageView;
    }
}