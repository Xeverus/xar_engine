#include <xar_engine/graphics/vulkan/surface.hpp>


namespace xar_engine::graphics::vulkan
{

    struct VulkanSurface::State
    {
    public:
        explicit State(const Parameters& parameters);

        ~State();

    public:
        std::shared_ptr<Instance> instance;

        VkSurfaceKHR vk_surface_khr;
    };

    VulkanSurface::State::State(const Parameters& parameters)
        : instance(parameters.instance)
        , vk_surface_khr(parameters.vk_surface_khr)
    {
    }

    VulkanSurface::State::~State()
    {
        vkDestroySurfaceKHR(
            instance->get_native(),
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