#pragma once

#include <xar_engine/graphics/api/buffer_reference.hpp>
#include <xar_engine/graphics/api/command_buffer_reference.hpp>
#include <xar_engine/graphics/api/image_reference.hpp>


namespace xar_engine::graphics::backend::unit
{
    class IBufferUnit
    {
    public:
        struct MakeBufferParameters;
        struct UpdateBufferParameters;
        struct CopyBufferParameters;
        struct CopyBufferToImageParameters;

    public:
        virtual ~IBufferUnit();


        virtual api::BufferReference make_staging_buffer(const MakeBufferParameters& parameters) = 0;
        virtual api::BufferReference make_vertex_buffer(const MakeBufferParameters& parameters) = 0;
        virtual api::BufferReference make_index_buffer(const MakeBufferParameters& parameters) = 0;
        virtual api::BufferReference make_uniform_buffer(const MakeBufferParameters& parameters) = 0;

        virtual void update_buffer(const UpdateBufferParameters& parameters) = 0;
        virtual void copy_buffer(const CopyBufferParameters& parameters) = 0;
        virtual void copy_buffer_to_image(const CopyBufferToImageParameters& parameters) = 0;
    };


    struct IBufferUnit::MakeBufferParameters
    {
        std::uint32_t byte_size;
    };

    struct IBufferUnit::UpdateBufferParameters
    {
        api::BufferReference buffer;
        std::vector<api::BufferUpdate> data;
    };

    struct IBufferUnit::CopyBufferParameters
    {
        api::CommandBufferReference command_buffer;
        api::BufferReference source_buffer;
        api::BufferReference destination_buffer;
    };

    struct IBufferUnit::CopyBufferToImageParameters
    {
        api::CommandBufferReference command_buffer;
        api::BufferReference source_buffer;
        api::ImageReference target_image;
    };
}