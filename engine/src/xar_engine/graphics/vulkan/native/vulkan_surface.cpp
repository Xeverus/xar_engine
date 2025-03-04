#include <xar_engine/graphics/vulkan/native/vulkan_surface.hpp>


namespace xar_engine::graphics::vulkan::native
{

    struct VulkanSurface::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        std::shared_ptr<VulkanInstance> vulkan_instance;

        VkSurfaceKHR vk_surface_khr;
    };

    VulkanSurface::State::State(const Parameters& parameters)
        : vulkan_instance(parameters.vulkan_instance)
        , vk_surface_khr(parameters.vk_surface_khr)
    {
    }

    VulkanSurface::State::~State()
    {
        vkDestroySurfaceKHR(
            vulkan_instance->get_native(),
            vk_surface_khr,
            nullptr);
    }


    VulkanSurface::VulkanSurface()
        : _state(nullptr)
    {
    }

    VulkanSurface::VulkanSurface(const VulkanSurface::Parameters& parameters)
        : _state(std::make_shared<State>(parameters))
    {
    }

    VulkanSurface::~VulkanSurface() = default;

    VkSurfaceKHR VulkanSurface::get_native() const
    {
        return _state->vk_surface_khr;
    }
}