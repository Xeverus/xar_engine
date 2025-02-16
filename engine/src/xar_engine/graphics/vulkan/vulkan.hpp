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
        void init_graphics_pipeline();
        void init_vertex_data();
        void init_index_data();
        void init_cmd_buffers();
        void init_sync_objects();

        void run_frame_sandbox();

        void wait();

        void cleanup_sandbox();

    private:
        void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer& buffer,
            VkDeviceMemory& bufferMemory);

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

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
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        VkCommandPool commandPool;
        std::vector<VkCommandBuffer> commandBuffer;

        std::vector<VkSemaphore> imageAvailableSemaphore;
        std::vector<VkSemaphore> renderFinishedSemaphore;
        std::vector<VkFence> inFlightFence;

        uint32_t frameCounter;
        uint32_t currentFrame;
    };
}