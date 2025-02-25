#pragma once

#include <cstdint>
#include <memory>

#include <volk.h>

#include <xar_engine/graphics/vulkan/physical_device.hpp>


namespace xar_engine::graphics::vulkan
{
    class Device
    {
    public:
        struct Parameters;

    public:
        Device();
        explicit Device(const Parameters& parameters);

        ~Device();


        void wait_idle() const;

        [[nodiscard]]
        const PhysicalDevice& get_physical_device() const;

        [[nodiscard]]
        VkDevice get_native() const;

        [[nodiscard]]
        std::uint32_t get_graphics_family_index() const;

        [[nodiscard]]
        VkQueue get_graphics_queue() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };

    struct Device::Parameters
    {
        PhysicalDevice physical_device;
    };
}