#include <xar_engine/graphics/vulkan/vulkan.hpp>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/file/file.hpp>

#include <xar_engine/logging/console_logger.hpp>


namespace xar_engine::graphics::vulkan
{
    constexpr auto tag = "Vulkan Sandbox";

    Vulkan::Vulkan(os::GlfwWindow* window)
        : vk_instance()
        , _glfw_window(window)
        , _logger(std::make_unique<logging::ConsoleLogger>())
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
        glfwGetFramebufferSize(_glfw_window->get_native(), &fb_size_x, &fb_size_y);

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
            VkImageViewCreateInfo view_info = {};
            view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            view_info.image = swapchain_images[i];
            view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            view_info.format = format_to_use.format;
            view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            view_info.subresourceRange.baseMipLevel = 0;
            view_info.subresourceRange.levelCount = 1;
            view_info.subresourceRange.baseArrayLayer = 0;
            view_info.subresourceRange.layerCount = 1;

            const auto view_result = vkCreateImageView(
                vk_device,
                &view_info,
                nullptr,
                &swapchain_image_views[i]);
            XAR_THROW_IF(
                view_result != VK_SUCCESS,
                error::XarException,
                "Vulkan image view creation failed");
        }
    }

    void Vulkan::init_shaders()
    {
        const auto vert_bytes = xar_engine::file::File::read_binary_file("shaders/hardcoded_triangle.vert.spv");
        const auto frag_bytes = xar_engine::file::File::read_binary_file("shaders/hardcoded_triangle.frag.spv");

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

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional

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
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
        if (vkCreatePipelineLayout(
            vk_device,
            &pipelineLayoutInfo,
            nullptr,
            &pipelineLayout) != VK_SUCCESS)
        {
            XAR_THROW(std::runtime_error,
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
            XAR_THROW(std::runtime_error,
                      "failed to create graphics pipeline!");
        }
    }
}