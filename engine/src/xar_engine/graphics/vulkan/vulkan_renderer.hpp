#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/renderer.hpp>
#include <xar_engine/graphics/vulkan/vulkan_buffer.hpp>
#include <xar_engine/graphics/vulkan/vulkan_command_buffer_pool.hpp>
#include <xar_engine/graphics/vulkan/vulkan_descriptor_pool.hpp>
#include <xar_engine/graphics/vulkan/vulkan_descriptor_set_layout.hpp>
#include <xar_engine/graphics/vulkan/vulkan_device.hpp>
#include <xar_engine/graphics/vulkan/vulkan_graphics_pipeline.hpp>
#include <xar_engine/graphics/vulkan/vulkan_image.hpp>
#include <xar_engine/graphics/vulkan/vulkan_image_view.hpp>
#include <xar_engine/graphics/vulkan/vulkan_instance.hpp>
#include <xar_engine/graphics/vulkan/vulkan_physical_device_list.hpp>
#include <xar_engine/graphics/vulkan/vulkan_sampler.hpp>
#include <xar_engine/graphics/vulkan/vulkan_shader.hpp>
#include <xar_engine/graphics/vulkan/vulkan_surface.hpp>
#include <xar_engine/graphics/vulkan/vulkan_swap_chain.hpp>

#include <xar_engine/logging/logger.hpp>

#include <xar_engine/os/window.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanRenderer
        : public IRenderer
    {
    public:
        explicit VulkanRenderer(
            std::shared_ptr<VulkanInstance> vulkan_instance,
            VkSurfaceKHR vk_surface_khr,
            const os::IWindow* os_window);

        ~VulkanRenderer() override;

        void update() override;

    private:
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

        void run_frame_sandbox();

        void cleanup_sandbox();

    private:
        void copyBuffer(
            VkBuffer srcBuffer,
            VkBuffer dstBuffer,
            VkDeviceSize size);

        void updateUniformBuffer(uint32_t currentImage);

        void copyBufferToImage(
            VkBuffer buffer,
            VkImage image,
            uint32_t width,
            uint32_t height);

        VkFormat findDepthFormat();

    private:
        std::unique_ptr<logging::ILogger> _logger;
        const os::IWindow* _os_window;

        std::shared_ptr<VulkanInstance> _vulkan_instance;
        std::unique_ptr<VulkanSurface> _vulkan_surface;
        std::unique_ptr<VulkanPhysicalDeviceList> _vulkan_physical_device_list;
        std::unique_ptr<VulkanDevice> _vulkan_device;

        std::unique_ptr<VulkanSwapChain> _vulkan_swap_chain;
        std::vector<VulkanImageView> _vulkan_swap_chain_image_views;
        std::unique_ptr<VulkanImage> _vulkan_depth_image;
        std::unique_ptr<VulkanImageView> _vulkan_depth_image_view;
        std::unique_ptr<VulkanImage> _vulkan_color_image;
        std::unique_ptr<VulkanImageView> _vulkan_color_image_view;

        std::unique_ptr<VulkanImage> _vulkan_texture_image;
        std::unique_ptr<VulkanImageView> _vulkan_texture_image_view;
        std::unique_ptr<VulkanSampler> _vulkan_sampler;

        std::unique_ptr<VulkanBuffer> _vulkan_vertex_buffer;
        std::unique_ptr<VulkanBuffer> _vulkan_index_buffer;

        std::unique_ptr<VulkanShader> _vulkan_vertex_shader;
        std::unique_ptr<VulkanShader> _vulkan_fragment_shader;

        std::unique_ptr<VulkanGraphicsPipeline> _vulkan_graphics_pipeline;

        std::unique_ptr<VulkanDescriptorSetLayout> _vulkan_descriptor_set_layout;
        std::unique_ptr<VulkanDescriptorPool> _vulkan_descriptor_pool;
        std::vector<VkDescriptorSet> descriptorSets;

        std::vector<std::unique_ptr<VulkanBuffer>> _vulkan_uniform_buffers;
        std::vector<void*> _uniform_buffers_mapped;

        std::vector<VkCommandBuffer> _vk_command_buffers;
        std::unique_ptr<VulkanCommandBufferPool> _vulkan_command_pool;

        uint32_t mipLevels;
        uint32_t frameCounter;
        uint32_t currentFrame;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    };
}