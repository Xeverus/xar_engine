#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <volk.h>

#include <xar_engine/graphics/renderer.hpp>
#include <xar_engine/graphics/vulkan/buffer.hpp>
#include <xar_engine/graphics/vulkan/command_buffer_pool.hpp>
#include <xar_engine/graphics/vulkan/descriptor_pool.hpp>
#include <xar_engine/graphics/vulkan/descriptor_set.hpp>
#include <xar_engine/graphics/vulkan/descriptor_set_layout.hpp>
#include <xar_engine/graphics/vulkan/device.hpp>
#include <xar_engine/graphics/vulkan/graphics_pipeline.hpp>
#include <xar_engine/graphics/vulkan/image.hpp>
#include <xar_engine/graphics/vulkan/image_view.hpp>
#include <xar_engine/graphics/vulkan/instance.hpp>
#include <xar_engine/graphics/vulkan/physical_device.hpp>
#include <xar_engine/graphics/vulkan/sampler.hpp>
#include <xar_engine/graphics/vulkan/shader.hpp>
#include <xar_engine/graphics/vulkan/surface.hpp>
#include <xar_engine/graphics/vulkan/swap_chain.hpp>

#include <xar_engine/logging/logger.hpp>

#include <xar_engine/os/window.hpp>


namespace xar_engine::graphics::vulkan
{
    class VulkanRenderer
        : public IRenderer
    {
    public:
        explicit VulkanRenderer(
            const std::shared_ptr<Instance>& vulkan_instance,
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

        std::shared_ptr<Instance> _instance;
        VulkanSurface _vulkan_surface;
        std::vector<PhysicalDevice> _physical_device_list;
        Device _device;

        VulkanSwapChain _swap_chain;
        std::vector<VulkanImageView> _swap_chain_image_views;
        VulkanImage _depth_image;
        VulkanImageView _depth_image_view;
        VulkanImage _color_image;
        VulkanImageView _color_image_view;

        VulkanImage _texture_image;
        VulkanImageView _texture_image_view;
        VulkanSampler _vulkan_sampler;

        Buffer _vertex_buffer;
        Buffer _index_buffer;

        VulkanShader _vertex_shader;
        VulkanShader _fragment_shader;

        VulkanGraphicsPipeline _vulkan_graphics_pipeline;

        VulkanDescriptorSetLayout _vulkan_descriptor_set_layout;
        VulkanDescriptorPool _vulkan_descriptor_pool;
        VulkanDescriptorSet _vulkan_descriptor_sets;

        std::vector<Buffer> _uniform_buffers;
        std::vector<void*> _uniform_buffers_mapped;

        std::vector<VkCommandBuffer> _vk_command_buffers;
        VulkanCommandBufferPool _vulkan_command_pool;

        uint32_t mipLevels;
        uint32_t frameCounter;

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    };
}