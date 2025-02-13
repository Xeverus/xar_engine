#include <xar_engine/graphics/vulkan_test.hpp>

#include <xar_engine/graphics/vulkan/vulkan_instance.hpp>

namespace xar_engine::graphics
{
    void run_vulkan_test()
    {
        // intentional memory leak
        auto* vulkan_instance = new vulkan::VulkanInstance();
    }
}