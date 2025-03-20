#pragma once

#include <xar_engine/graphics/api/command_buffer_reference.hpp>


namespace xar_engine::graphics::backend::component
{
    class ICommandBufferComponent
    {
    public:
        struct MakeCommandBufferParameters;
        struct BeginCommandBufferParameters;
        struct EndCommandBufferParameters;
        struct SubmitCommandBufferParameters;

    public:
        virtual ~ICommandBufferComponent();


        virtual std::vector<api::CommandBufferReference> make_command_buffer_list(const MakeCommandBufferParameters& parameters) = 0;

        virtual void begin_command_buffer(const BeginCommandBufferParameters& parameters) = 0;
        virtual void end_command_buffer(const EndCommandBufferParameters& parameters) = 0;
        virtual void submit_command_buffer(const SubmitCommandBufferParameters& parameters) = 0;
    };


    struct ICommandBufferComponent::MakeCommandBufferParameters
    {
        std::uint32_t buffer_counts;
    };

    struct ICommandBufferComponent::BeginCommandBufferParameters
    {
        api::CommandBufferReference command_buffer;
        api::ECommandBufferType command_buffer_type;
    };

    struct ICommandBufferComponent::EndCommandBufferParameters
    {
        api::CommandBufferReference command_buffer;
    };

    struct ICommandBufferComponent::SubmitCommandBufferParameters
    {
        api::CommandBufferReference command_buffer;
    };
}