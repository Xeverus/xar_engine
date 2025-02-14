#include <xar_engine/graphics/vulkan/vulkan.hpp>

#include <thread>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/file/file.hpp>

#include <xar_engine/logging/console_logger.hpp>


#define USE_DYNAMIC 1

namespace xar_engine::graphics::vulkan
{
    constexpr auto tag = "Vulkan Sandbox";

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

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT; // Do fragment shadera
        pushConstantRange.offset = 0;  // Start od początku
        pushConstantRange.size = sizeof(float);  // 4 bajty na float

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
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
        vkAcquireNextImageKHR(
            vk_device,
            vk_swapchain,
            UINT64_MAX,
            imageAvailableSemaphore[currentFrame],
            VK_NULL_HANDLE,
            &imageIndex);

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

            struct Constants {
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

            const Constants pc = { static_cast<float>(frameCounter) };
            vkCmdPushConstants(commandBuffer[currentFrame],
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

            vkCmdDraw(
                commandBuffer[currentFrame],
                3,
                1,
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
            vkQueuePresentKHR(
                vk_queue,
                &presentInfo);
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
        std::this_thread::sleep_for(33ms);
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
}