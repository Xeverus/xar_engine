#pragma once

#include <memory>
#include <string>
#include <vector>

#include <volk.h>
#include <glfw/glfw3.h>

#include <xar_engine/logging/logger.hpp>

#include <xar_engine/graphics/vulkan/vulkan_instance.hpp>

#include <xar_engine/os/glfw_window.hpp>


namespace xar_engine::graphics::vulkan
{
    class Vulkan
    {
    public:
        const int MAX_FRAMES_IN_FLIGHT = 2;

    public:
        explicit Vulkan(os::GlfwWindow* window);

        void init_surface();
        void init_device();
        void destroy_swapchain();
        void init_swapchain();
        void init_shaders();
        void init_descriptor_set_layout();
        void init_graphics_pipeline();
        void init_vertex_data();
        void init_index_data();
        void init_model();
        void init_ubo_data();
        void init_descriptors();
        void init_cmd_buffers();
        void init_color_msaa();
        void init_depth();
        void init_texture();
        void init_texture_view();
        void init_sampler();
        void init_sync_objects();

        void run_frame_sandbox();

        void wait();

        void cleanup_sandbox();

    private:
        std::uint32_t findMemoryType(
            uint32_t typeFilter,
            VkMemoryPropertyFlags properties);

        void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);

        void copyBuffer(
            VkBuffer srcBuffer,
            VkBuffer dstBuffer,
            VkDeviceSize size);

        void updateUniformBuffer(uint32_t currentImage);

        void createImage(
            uint32_t width,
            uint32_t height,
            VkFormat format,
            VkImageTiling tiling,
            VkImageUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkImage& image,
            VkDeviceMemory& imageMemory,
            uint32_t mipLevels,
            VkSampleCountFlagBits numSamples);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

        void transitionImageLayout(
            VkImage image,
            VkFormat format,
            VkImageLayout oldLayout,
            VkImageLayout newLayout,
            uint32_t mipLevels);

        void copyBufferToImage(
            VkBuffer buffer,
            VkImage image,
            uint32_t width,
            uint32_t height);

        VkImageView createImageView(
            VkImage image,
            VkFormat format,
            VkImageAspectFlags aspectFlags,
            uint32_t mipLevels);

        VkFormat findSupportedFormat(
            const std::vector<VkFormat>& candidates,
            VkImageTiling tiling,
            VkFormatFeatureFlags features);

        VkFormat findDepthFormat();

        bool hasStencilComponent(VkFormat format);

        void generateMipmaps(
            VkImage image,
            VkFormat imageFormat,
            int32_t texWidth,
            int32_t texHeight,
            uint32_t mipLevels);

        VkSampleCountFlagBits getMaxUsableSampleCount();

    private:
        os::GlfwWindow* _glfw_window;
        std::unique_ptr<logging::ILogger> _logger;

        VulkanInstance vk_instance;
        VkSurfaceKHR vk_surface;
        VkPhysicalDevice vk_physical_device;
        VkDevice vk_device;
        VkQueue vk_queue;
        uint32_t graphics_queue_family;
        VkSurfaceFormatKHR format_to_use;
        VkSwapchainKHR vk_swapchain;
        VkExtent2D swapchainExtent;
        std::vector<VkImage> swapchain_images;
        std::vector<VkImageView> swapchain_image_views;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;

        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;

        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBuffersMapped;

        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffer;

        uint32_t mipLevels;
        VkImage textureImage;
        VkImageView textureImageView;
        VkDeviceMemory textureImageMemory;
        VkSampler textureSampler;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;

        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> inFlightFence;

        uint32_t frameCounter;
        uint32_t currentFrame;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    };
}