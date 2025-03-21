#include <xar_engine/renderer/unit/gpu_material_unit_impl.hpp>

#include <xar_engine/asset/image_loader.hpp>


namespace xar_engine::renderer::unit
{
    gpu_asset::GpuMaterialReference GpuMaterialUnitImpl::make_gpu_material(const MakeGpuMaterialParameters& parameters)
    {
        const auto material_index = static_cast<std::uint32_t>(get_state()._gpu_material_data_map.size());

        auto image = asset::ImageLoaderFactory().make()->load_image_from_file(*parameters.material.color_base_texture);
        auto texture_image_ref = init_texture(image);
        auto texture_image_view_ref = get_state()._graphics_backend->image_unit().make_image_view(
            {
                texture_image_ref,
                graphics::api::EImageAspect::COLOR,
                image.mip_level_count
            });
        auto sampler_ref = get_state()._graphics_backend->image_unit().make_sampler({static_cast<float>(image.mip_level_count)});

        get_state()._graphics_backend->descriptor_unit().write_descriptor_set(
            {
                get_state()._image_descriptor_set_ref,
                0,
                {},
                material_index,
                {texture_image_view_ref},
                {sampler_ref}
            });

        return get_state()._gpu_material_data_map.add(
            {
                texture_image_ref,
                texture_image_view_ref,
                sampler_ref,
                static_cast<std::uint32_t>(material_index),
            });
    }

    graphics::api::ImageReference GpuMaterialUnitImpl::init_texture(const asset::Image& image)
    {
        const auto imageSize = asset::image::get_byte_size(image);

        auto staging_buffer = get_state()._graphics_backend->buffer_unit().make_staging_buffer({imageSize});
        get_state()._graphics_backend->buffer_unit().update_buffer(
            {
                staging_buffer,
                {
                    {
                        image.bytes.data(),
                        0,
                        static_cast<std::uint32_t>(imageSize),
                    }
                }
            });

        auto texture_image_ref = get_state()._graphics_backend->image_unit().make_image(
            {
                graphics::api::EImageType::TEXTURE,
                {
                    image.pixel_width,
                    image.pixel_height,
                    1
                },
                graphics::api::EFormat::R8G8B8A8_SRGB,
                image.mip_level_count,
                1
            });

        auto tmp_command_buffer = get_state()._graphics_backend->command_buffer_unit().make_command_buffer_list({1});
        get_state()._graphics_backend->command_buffer_unit().begin_command_buffer(
            {
                tmp_command_buffer[0],
                graphics::api::ECommandBufferType::ONE_TIME
            });
        get_state()._graphics_backend->image_unit().transit_image_layout(
            {
                tmp_command_buffer[0],
                texture_image_ref,
                graphics::api::EImageLayout::TRANSFER_DESTINATION
            });
        get_state()._graphics_backend->buffer_unit().copy_buffer_to_image(
            {
                tmp_command_buffer[0],
                staging_buffer,
                texture_image_ref
            });
        get_state()._graphics_backend->image_unit().generate_image_mip_maps(
            {
                tmp_command_buffer[0],
                texture_image_ref
            });
        get_state()._graphics_backend->command_buffer_unit().end_command_buffer({tmp_command_buffer[0]});
        get_state()._graphics_backend->command_buffer_unit().submit_command_buffer({tmp_command_buffer[0]});

        return texture_image_ref;
    }
}
