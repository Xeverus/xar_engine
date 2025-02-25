#pragma once

#include <memory>

#include <volk.h>

#include <xar_engine/graphics/vulkan/physical_device.hpp>


namespace xar_engine::graphics::vulkan
{
    class Instance
    {
    public:
        Instance();

        ~Instance();

        [[nodiscard]]
        std::vector<PhysicalDevice> get_physical_device_list() const;

        [[nodiscard]]
        VkInstance get_native() const;

    private:
        struct State;

    private:
        std::shared_ptr<State> _state;
    };
}
