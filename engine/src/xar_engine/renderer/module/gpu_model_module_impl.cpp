#include <xar_engine/renderer/module/gpu_model_module_impl.hpp>

#include <xar_engine/asset/model_loader.hpp>

#include <xar_engine/graphics/backend/graphics_backend.hpp>

#include <xar_engine/logging/logger.hpp>


namespace
{
    constexpr auto tag = "GpuModelModuleImpl";
}


namespace xar_engine::renderer::module
{
    std::vector<gpu_asset::GpuModel> GpuModelModuleImpl::make_gpu_model(const std::vector<asset::Model>& model_list)
    {
        auto gpu_model_data_list_buffer_structure = gpu_asset::make_gpu_model_data_list_buffer_structure(model_list);

        {
            for (auto j = 0; j < model_list.size(); ++j)
            {
                const auto& model = model_list[j];

                for (auto i = 0; i < model.mesh_list.size(); ++i)
                {
                    const auto& mesh = model.mesh_list[i];

                    XAR_LOG(
                        logging::LogLevel::DEBUG,
                        tag,
                        "model {}, mesh {} = position: {}, normal: {}, texcoords: {}, indices: {}",
                        reinterpret_cast<std::uint64_t>(&model),
                        reinterpret_cast<std::uint64_t>(&mesh),
                        mesh.position_list.size(),
                        mesh.normal_list.size(),
                        mesh.texture_coord_list.size(),
                        mesh.index_list.size());
                }
            }

            for (auto j = 0; j < model_list.size(); ++j)
            {
                const auto& model = model_list[j];

                for (auto i = 0; i < model.mesh_list.size(); ++i)
                {
                    const auto& mesh = model.mesh_list[i];

                    XAR_LOG(
                        logging::LogLevel::DEBUG,
                        tag,
                        "first idx: {}, index count: {}, first vertex: {}, vertex count: {}",
                        gpu_model_data_list_buffer_structure.gpu_model_buffer_structure_list[j].gpu_mesh_buffer_structure_list[i].first_index,
                        gpu_model_data_list_buffer_structure.gpu_model_buffer_structure_list[j].gpu_mesh_buffer_structure_list[i].index_counts,
                        gpu_model_data_list_buffer_structure.gpu_model_buffer_structure_list[j].gpu_mesh_buffer_structure_list[i].first_vertex,
                        gpu_model_data_list_buffer_structure.gpu_model_buffer_structure_list[j].gpu_mesh_buffer_structure_list[i].vertex_counts);
                }
            }
        }

        auto gpu_model_data_buffer = gpu_asset::GpuModelDataBuffer{};
        gpu_model_data_buffer.position_buffer = get_state()._graphics_backend->buffer_component().make_vertex_buffer({gpu_model_data_list_buffer_structure.position_list_byte_size});
        gpu_model_data_buffer.normal_buffer = get_state()._graphics_backend->buffer_component().make_vertex_buffer({gpu_model_data_list_buffer_structure.normal_list_byte_size});
        gpu_model_data_buffer.texture_coord_buffer = get_state()._graphics_backend->buffer_component().make_vertex_buffer({gpu_model_data_list_buffer_structure.texture_coord_list_byte_size});
        gpu_model_data_buffer.index_buffer = get_state()._graphics_backend->buffer_component().make_index_buffer({gpu_model_data_list_buffer_structure.index_list_byte_size});
        gpu_model_data_buffer.structure = std::move(gpu_model_data_list_buffer_structure);

        auto staging_buffer = get_state()._graphics_backend->buffer_component().make_staging_buffer({gpu_model_data_buffer.structure.position_list_byte_size});
        {
            auto buffer_update_list = std::vector<graphics::api::BufferUpdate>{};
            auto byte_size_offset = std::uint32_t{0};
            for (auto i = 0; i < model_list.size(); ++i)
            {
                const auto& model = model_list[i];

                for (auto j = 0; j < model.mesh_list.size(); ++j)
                {
                    const auto& mesh = model.mesh_list[j];

                    const auto attribute_byte_size = mesh.position_list.size() * sizeof(mesh.position_list[0]);

                    buffer_update_list.emplace_back(
                        mesh.position_list.data(),
                        byte_size_offset,
                        attribute_byte_size);

                    byte_size_offset += attribute_byte_size;
                }
            }

            get_state()._graphics_backend->buffer_component().update_buffer({
                                                                                staging_buffer,
                                                                                buffer_update_list});
        }

        auto staging_buffer_1 = get_state()._graphics_backend->buffer_component().make_staging_buffer({gpu_model_data_buffer.structure.normal_list_byte_size});
        {
            auto buffer_update_list = std::vector<graphics::api::BufferUpdate>{};
            auto byte_size_offset = std::uint32_t{0};
            for (auto i = 0; i < model_list.size(); ++i)
            {
                const auto& model = model_list[i];

                for (auto j = 0; j < model.mesh_list.size(); ++j)
                {
                    const auto& mesh = model.mesh_list[j];

                    const auto attribute_byte_size = mesh.normal_list.size() * sizeof(mesh.normal_list[0]);

                    buffer_update_list.emplace_back(
                        mesh.normal_list.data(),
                        byte_size_offset,
                        attribute_byte_size);

                    byte_size_offset += attribute_byte_size;
                }
            }

            get_state()._graphics_backend->buffer_component().update_buffer({
                                                                                staging_buffer_1,
                                                                                buffer_update_list});
        }

        auto staging_buffer_2 = get_state()._graphics_backend->buffer_component().make_staging_buffer({gpu_model_data_buffer.structure.texture_coord_list_byte_size});
        {
            auto buffer_update_list = std::vector<graphics::api::BufferUpdate>{};
            auto byte_size_offset = std::uint32_t{0};
            for (auto i = 0; i < model_list.size(); ++i)
            {
                const auto& model = model_list[i];

                for (auto j = 0; j < model.mesh_list.size(); ++j)
                {
                    const auto& mesh = model.mesh_list[j];

                    const auto attribute_byte_size =
                        mesh.texture_coord_list.size() * sizeof(mesh.texture_coord_list[0]);

                    buffer_update_list.emplace_back(
                        mesh.texture_coord_list.data(),
                        byte_size_offset,
                        attribute_byte_size);

                    byte_size_offset += attribute_byte_size;
                }
            }

            get_state()._graphics_backend->buffer_component().update_buffer({
                                                                                staging_buffer_2,
                                                                                buffer_update_list});
        }

        auto staging_buffer_3 = get_state()._graphics_backend->buffer_component().make_staging_buffer({gpu_model_data_buffer.structure.index_list_byte_size});
        {
            auto buffer_update_list = std::vector<graphics::api::BufferUpdate>{};
            auto byte_size_offset = std::uint32_t{0};
            for (auto i = 0; i < model_list.size(); ++i)
            {
                const auto& model = model_list[i];

                for (auto j = 0; j < model.mesh_list.size(); ++j)
                {
                    const auto& mesh = model.mesh_list[j];

                    const auto attribute_byte_size = mesh.index_list.size() * sizeof(mesh.index_list[0]);

                    buffer_update_list.emplace_back(
                        mesh.index_list.data(),
                        byte_size_offset,
                        attribute_byte_size);

                    byte_size_offset += attribute_byte_size;
                }
            }

            get_state()._graphics_backend->buffer_component().update_buffer({
                                                                                staging_buffer_3,
                                                                                buffer_update_list});
        }

        const auto command_buffer = get_state()._graphics_backend->command_buffer_component().make_command_buffer_list({1});
        get_state()._graphics_backend->command_buffer_component().begin_command_buffer({
                                                                                           command_buffer[0],
                                                                                           graphics::api::ECommandBufferType::ONE_TIME});

        get_state()._graphics_backend->buffer_component().copy_buffer({
                                                                          command_buffer[0],
                                                                          staging_buffer,
                                                                          gpu_model_data_buffer.position_buffer});
        get_state()._graphics_backend->buffer_component().copy_buffer({
                                                                          command_buffer[0],
                                                                          staging_buffer_1,
                                                                          gpu_model_data_buffer.normal_buffer});
        get_state()._graphics_backend->buffer_component().copy_buffer({
                                                                          command_buffer[0],
                                                                          staging_buffer_2,
                                                                          gpu_model_data_buffer.texture_coord_buffer});
        get_state()._graphics_backend->buffer_component().copy_buffer({
                                                                          command_buffer[0],
                                                                          staging_buffer_3,
                                                                          gpu_model_data_buffer.index_buffer});

        get_state()._graphics_backend->command_buffer_component().end_command_buffer({command_buffer[0]});
        get_state()._graphics_backend->command_buffer_component().submit_command_buffer({command_buffer[0]});

        auto gpu_model_data_buffer_reference = get_state()._gpu_model_data_buffer_map.add(std::move(gpu_model_data_buffer));

        auto gpu_model_data_list = std::vector<gpu_asset::GpuModel>{};
        {
            for (auto model_index = std::uint32_t{0}; model_index < model_list.size(); ++model_index)
            {
                const auto& model = model_list[model_index];

                auto& gpu_model_data = gpu_model_data_list.emplace_back(
                    get_state()._gpu_model_data_map.add(
                        {
                            model_index,
                            gpu_model_data_buffer_reference
                        }));

                for (auto mesh_index = std::uint32_t{0}; mesh_index < model.mesh_list.size(); ++mesh_index)
                {
                    gpu_model_data.gpu_mesh.push_back(
                        get_state()._gpu_mesh_data_map.add(
                            {
                                mesh_index,
                                gpu_model_data.gpu_model,
                            }));
                }
            }
        }

        return gpu_model_data_list;
    }
}
