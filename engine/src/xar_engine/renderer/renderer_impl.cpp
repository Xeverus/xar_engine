#include <xar_engine/renderer/renderer_impl.hpp>

#include <chrono>
#include <thread>
#include <vector>

#include <xar_engine/asset/image_loader.hpp>
#include <xar_engine/asset/model_loader.hpp>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/file/file.hpp>

#include <xar_engine/graphics/backend/graphics_backend.hpp>

#include <xar_engine/logging/logger.hpp>

#include <xar_engine/math/matrix.hpp>


namespace xar_engine::renderer
{
    namespace
    {
        constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        constexpr auto tag = "Vulkan Sandbox";

        std::vector<graphics::api::VertexInputBinding> getBindingDescription()
        {
            return {
                graphics::api::VertexInputBinding{
                    .binding_index = 0,
                    .stride = sizeof(math::Vector3f),
                    .input_rate = graphics::api::VertexInputBindingRate::PER_VERTEX,
                },
                graphics::api::VertexInputBinding{
                    .binding_index = 1,
                    .stride = sizeof(math::Vector3f),
                    .input_rate = graphics::api::VertexInputBindingRate::PER_VERTEX,
                },
                graphics::api::VertexInputBinding{
                    .binding_index = 2,
                    .stride = sizeof(math::Vector2f),
                    .input_rate = graphics::api::VertexInputBindingRate::PER_VERTEX,
                },
            };
        }

        std::vector<graphics::api::VertexInputAttribute> getAttributeDescriptions()
        {
            std::vector<graphics::api::VertexInputAttribute> attributeDescriptions(3);

            attributeDescriptions[0].binding_index = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = graphics::api::EFormat::R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = 0;

            attributeDescriptions[1].binding_index = 1;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = graphics::api::EFormat::R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = 0;

            attributeDescriptions[2].binding_index = 2;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = graphics::api::EFormat::R32G32_SFLOAT;
            attributeDescriptions[2].offset = 0;

            return attributeDescriptions;
        }

        struct UniformBufferObject
        {
            alignas(16) math::Matrix4x4f model;
            alignas(16) math::Matrix4x4f view;
            alignas(16) math::Matrix4x4f proj;
        };

        static_assert(sizeof(UniformBufferObject) == sizeof(math::Matrix4x4f) * 3);
    }

    void RendererImpl::init_color_msaa()
    {
        _color_image_ref = _graphics_backend->resource().make_image(
            graphics::api::EImageType::COLOR_ATTACHMENT,
            {
                static_cast<std::int32_t>(_window_surface->get_pixel_size().x),
                static_cast<std::int32_t>(_window_surface->get_pixel_size().y),
                1
            },
            graphics::api::EFormat::R8G8B8A8_SRGB,
            1,
            _graphics_backend->host().get_sample_count());


        _color_image_view_ref = _graphics_backend->resource().make_image_view(
            _color_image_ref,
            graphics::api::EImageAspect::COLOR,
            1);
    }


    void RendererImpl::init_depth()
    {
        _depth_image_ref = _graphics_backend->resource().make_image(
            graphics::api::EImageType::DEPTH_ATTACHMENT,
            {
                static_cast<std::int32_t>(_window_surface->get_pixel_size().x),
                static_cast<std::int32_t>(_window_surface->get_pixel_size().y),
                1
            },
            _graphics_backend->host().find_depth_format(),
            1,
            _graphics_backend->host().get_sample_count());

        _depth_image_view_ref = _graphics_backend->resource().make_image_view(
            _depth_image_ref,
            graphics::api::EImageAspect::DEPTH,
            1);

        auto tmp_command_buffer = _graphics_backend->resource().make_command_buffer_list(1);
        _graphics_backend->command().begin_command_buffer(
            tmp_command_buffer[0],
            graphics::api::ECommandBufferType::ONE_TIME);
        _graphics_backend->command().transit_image_layout(
            tmp_command_buffer[0],
            _depth_image_ref,
            graphics::api::EImageLayout::DEPTH_STENCIL_ATTACHMENT);
        _graphics_backend->command().end_command_buffer(tmp_command_buffer[0]);
        _graphics_backend->command().submit_command_buffer(tmp_command_buffer[0]);
    }


    graphics::api::ImageReference RendererImpl::init_texture(const asset::Image& image)
    {
        const auto imageSize = asset::image::get_byte_size(image);

        auto staging_buffer = _graphics_backend->resource().make_staging_buffer(imageSize);
        _graphics_backend->host().update_buffer(
            staging_buffer,
            {{
                 image.bytes.data(),
                 0,
                 static_cast<std::uint32_t>(imageSize),
             }});

        auto texture_image_ref = _graphics_backend->resource().make_image(
            graphics::api::EImageType::TEXTURE,
            {image.pixel_width, image.pixel_height, 1},
            graphics::api::EFormat::R8G8B8A8_SRGB,
            image.mip_level_count,
            1);

        auto tmp_command_buffer = _graphics_backend->resource().make_command_buffer_list(1);
        _graphics_backend->command().begin_command_buffer(
            tmp_command_buffer[0],
            graphics::api::ECommandBufferType::ONE_TIME);
        _graphics_backend->command().transit_image_layout(
            tmp_command_buffer[0],
            texture_image_ref,
            graphics::api::EImageLayout::TRANSFER_DESTINATION);
        _graphics_backend->command().copy_buffer_to_image(
            tmp_command_buffer[0],
            staging_buffer,
            texture_image_ref);
        _graphics_backend->command().generate_image_mip_maps(
            tmp_command_buffer[0],
            texture_image_ref);
        _graphics_backend->command().end_command_buffer(tmp_command_buffer[0]);
        _graphics_backend->command().submit_command_buffer(tmp_command_buffer[0]);

        return texture_image_ref;
    }


    void RendererImpl::updateUniformBuffer(uint32_t currentImageNr)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};

        ubo.model = math::make_identity_matrix();

        ubo.model = math::rotate_matrix(
            ubo.model,
            time * 90.0f,
            math::Vector3f(
                0.0f,
                0.0f,
                1.0f));

        ubo.model = ubo.model * _redner_item_list[0].gpu_mesh_instance.model_matrix;

        ubo.view = math::make_view_matrix(
            math::Vector3f(
                2.0f,
                2.0f,
                2.0f),
            math::Vector3f(
                0.0f,
                0.0f,
                0.0f),
            math::Vector3f(
                0.0f,
                0.0f,
                1.0f));

        ubo.proj = math::make_projection_matrix(
            45.0f,
            _window_surface->get_pixel_size().x / (float) _window_surface->get_pixel_size().y,
            0.1f,
            10.0f);
        ubo.proj.as_column_list[1].y *= -1;

        _graphics_backend->host().update_buffer(
            _uniform_buffer_ref_list[currentImageNr],
            {{
                 &ubo,
                 0,
                 sizeof(ubo),
             }});
    }
}


namespace xar_engine::renderer
{
    RendererImpl::RendererImpl(
        std::shared_ptr<graphics::backend::IGraphicsBackend> graphics_backend,
        std::shared_ptr<graphics::context::IWindowSurface> window_surface)
        : _graphics_backend(std::move(graphics_backend))
        , _window_surface(std::move(window_surface))
        , frameCounter(0)
    {
        _command_buffer_list = _graphics_backend->resource().make_command_buffer_list(MAX_FRAMES_IN_FLIGHT);

        _swap_chain_ref = _graphics_backend->resource().make_swap_chain(
            _window_surface,
            MAX_FRAMES_IN_FLIGHT);

        _vertex_shader_ref = _graphics_backend->resource().make_shader(xar_engine::file::read_binary_file("assets/triangle.vert.spv"));
        _fragment_shader_ref = _graphics_backend->resource().make_shader(xar_engine::file::read_binary_file("assets/triangle.frag.spv"));

        _ubo_descriptor_set_layout_ref = _graphics_backend->resource().make_descriptor_set_layout({graphics::api::EDescriptorPoolType::UNIFORM_BUFFER});
        _image_descriptor_set_layout_ref = _graphics_backend->resource().make_descriptor_set_layout({graphics::api::EDescriptorPoolType::SAMPLED_IMAGE});

        _graphics_pipeline_ref = _graphics_backend->resource().make_graphics_pipeline(
            {_ubo_descriptor_set_layout_ref, _image_descriptor_set_layout_ref},
            _vertex_shader_ref,
            _fragment_shader_ref,
            getAttributeDescriptions(),
            getBindingDescription(),
            graphics::api::EFormat::R8G8B8A8_SRGB,
            _graphics_backend->host().find_depth_format(),
            _graphics_backend->host().get_sample_count());

        init_color_msaa();
        init_depth();

        _uniform_buffer_ref_list.reserve(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            _uniform_buffer_ref_list.push_back(_graphics_backend->resource().make_uniform_buffer(sizeof(UniformBufferObject)));
        }

        _ubo_descriptor_pool_ref = _graphics_backend->resource().make_descriptor_pool({graphics::api::EDescriptorPoolType::UNIFORM_BUFFER});
        _image_descriptor_pool_ref = _graphics_backend->resource().make_descriptor_pool({graphics::api::EDescriptorPoolType::SAMPLED_IMAGE});

        _ubo_descriptor_set_list_ref = _graphics_backend->resource().make_descriptor_set_list(
            _ubo_descriptor_pool_ref,
            _ubo_descriptor_set_layout_ref,
            MAX_FRAMES_IN_FLIGHT);
        _graphics_backend->resource().write_descriptor_set(
            _ubo_descriptor_set_list_ref[0],
            0,
            {_uniform_buffer_ref_list[0]},
            0,
            {},
            {});
        _graphics_backend->resource().write_descriptor_set(
            _ubo_descriptor_set_list_ref[1],
            0,
            {_uniform_buffer_ref_list[1]},
            0,
            {},
            {});

        _image_descriptor_set_ref = _graphics_backend->resource().make_descriptor_set_list(
            _image_descriptor_pool_ref,
            _image_descriptor_set_layout_ref,
            1)[0];
    }

    RendererImpl::~RendererImpl()
    {
        _graphics_backend->command().wait_idle();
    }

    std::vector<gpu_asset::GpuModel> RendererImpl::make_gpu_model(const std::vector<asset::Model>& model_list)
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
        gpu_model_data_buffer.position_buffer = _graphics_backend->resource().make_vertex_buffer(gpu_model_data_list_buffer_structure.position_list_byte_size);
        gpu_model_data_buffer.normal_buffer = _graphics_backend->resource().make_vertex_buffer(gpu_model_data_list_buffer_structure.normal_list_byte_size);
        gpu_model_data_buffer.texture_coord_buffer = _graphics_backend->resource().make_vertex_buffer(gpu_model_data_list_buffer_structure.texture_coord_list_byte_size);
        gpu_model_data_buffer.index_buffer = _graphics_backend->resource().make_index_buffer(gpu_model_data_list_buffer_structure.index_list_byte_size);
        gpu_model_data_buffer.structure = std::move(gpu_model_data_list_buffer_structure);

        auto staging_buffer = _graphics_backend->resource().make_staging_buffer(gpu_model_data_buffer.structure.position_list_byte_size);
        {
            auto buffer_update_list = std::vector<graphics::backend::BufferUpdate>{};
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

            _graphics_backend->host().update_buffer(
                staging_buffer,
                buffer_update_list);
        }

        auto staging_buffer_1 = _graphics_backend->resource().make_staging_buffer(gpu_model_data_buffer.structure.normal_list_byte_size);
        {
            auto buffer_update_list = std::vector<graphics::backend::BufferUpdate>{};
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

            _graphics_backend->host().update_buffer(
                staging_buffer_1,
                buffer_update_list);
        }

        auto staging_buffer_2 = _graphics_backend->resource().make_staging_buffer(gpu_model_data_buffer.structure.texture_coord_list_byte_size);
        {
            auto buffer_update_list = std::vector<graphics::backend::BufferUpdate>{};
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

            _graphics_backend->host().update_buffer(
                staging_buffer_2,
                buffer_update_list);
        }

        auto staging_buffer_3 = _graphics_backend->resource().make_staging_buffer(gpu_model_data_buffer.structure.index_list_byte_size);
        {
            auto buffer_update_list = std::vector<graphics::backend::BufferUpdate>{};
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

            _graphics_backend->host().update_buffer(
                staging_buffer_3,
                buffer_update_list);
        }

        const auto command_buffer = _graphics_backend->resource().make_command_buffer_list(1);
        _graphics_backend->command().begin_command_buffer(
            command_buffer[0],
            graphics::api::ECommandBufferType::ONE_TIME);

        _graphics_backend->command().copy_buffer(
            command_buffer[0],
            staging_buffer,
            gpu_model_data_buffer.position_buffer);
        _graphics_backend->command().copy_buffer(
            command_buffer[0],
            staging_buffer_1,
            gpu_model_data_buffer.normal_buffer);
        _graphics_backend->command().copy_buffer(
            command_buffer[0],
            staging_buffer_2,
            gpu_model_data_buffer.texture_coord_buffer);
        _graphics_backend->command().copy_buffer(
            command_buffer[0],
            staging_buffer_3,
            gpu_model_data_buffer.index_buffer);

        _graphics_backend->command().end_command_buffer(command_buffer[0]);
        _graphics_backend->command().submit_command_buffer(command_buffer[0]);

        auto gpu_model_data_buffer_reference = _gpu_model_data_buffer_map.add(std::move(gpu_model_data_buffer));

        auto gpu_model_data_list = std::vector<gpu_asset::GpuModel>{};
        {
            for (auto model_index = std::uint32_t{0}; model_index < model_list.size(); ++model_index)
            {
                const auto& model = model_list[model_index];

                auto& gpu_model_data = gpu_model_data_list.emplace_back(
                    _gpu_model_data_map.add(
                        {
                            model_index,
                            gpu_model_data_buffer_reference
                        }));

                for (auto mesh_index = std::uint32_t{0}; mesh_index < model.mesh_list.size(); ++mesh_index)
                {
                    gpu_model_data.gpu_mesh.push_back(
                        _gpu_mesh_data_map.add(
                            {
                                mesh_index,
                                gpu_model_data.gpu_model,
                            }));
                }
            }
        }

        return gpu_model_data_list;
    }

    gpu_asset::GpuMaterialReference RendererImpl::make_gpu_material(const asset::Material& material)
    {
        const auto material_index = _gpu_material_data_map.size();

        auto image = asset::ImageLoaderFactory().make()->load_image_from_file(*material.color_base_texture);
        auto texture_image_ref = init_texture(image);
        auto texture_image_view_ref = _graphics_backend->resource().make_image_view(
            texture_image_ref,
            graphics::api::EImageAspect::COLOR,
            image.mip_level_count);
        auto sampler_ref = _graphics_backend->resource().make_sampler(static_cast<float>(image.mip_level_count));

        _graphics_backend->resource().write_descriptor_set(
            _image_descriptor_set_ref,
            0,
            {},
            material_index,
            {texture_image_view_ref},
            {sampler_ref});

        return _gpu_material_data_map.add(
            {
                texture_image_ref,
                texture_image_view_ref,
                sampler_ref,
                static_cast<std::uint32_t>(material_index),
            });
    }

    void RendererImpl::add_gpu_mesh_instance_to_render(
        const gpu_asset::GpuMeshInstance& gpu_mesh_instance,
        const gpu_asset::GpuMaterialReference& gpu_material)
    {
        _redner_item_list.push_back({gpu_mesh_instance, gpu_material});
    }

    void RendererImpl::clear_gpu_mesh_instance_to_render()
    {
        _redner_item_list.clear();
    }

    void RendererImpl::update()
    {
        const auto begin_frame_result = _graphics_backend->host().begin_frame(_swap_chain_ref);

        if (std::get<0>(begin_frame_result) == graphics::api::ESwapChainResult::RECREATION_REQUIRED)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Acquire failed because Swapchain is out of date");

            _graphics_backend->command().wait_idle();
            _swap_chain_ref = {};

            _swap_chain_ref = _graphics_backend->resource().make_swap_chain(
                _window_surface,
                MAX_FRAMES_IN_FLIGHT);

            init_color_msaa();
            init_depth();

            XAR_LOG(
                logging::LogLevel::DEBUG,
                tag,
                "Acquire failed because Swapchain is out of date but swapchain was recreated");
            return;
        }
        else if (std::get<0>(begin_frame_result) != graphics::api::ESwapChainResult::OK)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Acquire failed because Swapchain");
            return;
        }

        const auto current_image_index = std::get<1>(begin_frame_result);
        const auto frame_index = std::get<2>(begin_frame_result);

        _graphics_backend->command().begin_rendering(
            _command_buffer_list[frame_index],
            _swap_chain_ref,
            current_image_index,
            _color_image_view_ref,
            _depth_image_view_ref);

        _graphics_backend->command().set_pipeline_state(
            _command_buffer_list[frame_index],
            _swap_chain_ref,
            _graphics_pipeline_ref,
            {_ubo_descriptor_set_list_ref[frame_index], _image_descriptor_set_ref});

        static float pcc = 0.0f;
        struct Constants
        {
            float time = pcc++;
            std::int32_t material_index;
        } pc;

        for (const auto& render_item: _redner_item_list)
        {
            const auto& gpu_mesh_data = _gpu_mesh_data_map.get(render_item.gpu_mesh_instance.gpu_mesh);
            const auto& gpu_model_data = _gpu_model_data_map.get(gpu_mesh_data.gpu_model);
            const auto& gpu_buffer_data = _gpu_model_data_buffer_map.get(gpu_model_data.gpu_model_data_buffer);

            const auto& gpu_material_data = _gpu_material_data_map.get(render_item.gpu_material);

            pc.material_index = gpu_material_data.index;
            _graphics_backend->command().push_constants(
                _command_buffer_list[frame_index],
                _graphics_pipeline_ref,
                graphics::api::EShaderType::FRAGMENT,
                0,
                sizeof(Constants),
                &pc);

            _graphics_backend->command().set_vertex_buffer_list(
                _command_buffer_list[frame_index],
                {
                    gpu_buffer_data.position_buffer,
                    gpu_buffer_data.normal_buffer,
                    gpu_buffer_data.texture_coord_buffer,
                },
                {0, 0, 0},
                0);
            _graphics_backend->command().set_index_buffer(
                _command_buffer_list[frame_index],
                gpu_buffer_data.index_buffer,
                0);

            const auto gpu_mesh_buffer_structure = gpu_buffer_data
                .structure
                .gpu_model_buffer_structure_list[gpu_model_data.model_index]
                .gpu_mesh_buffer_structure_list[gpu_mesh_data.mesh_index];

            _graphics_backend->command().draw_indexed(
                _command_buffer_list[frame_index],
                gpu_mesh_buffer_structure.index_counts,
                1,
                gpu_mesh_buffer_structure.first_index,
                gpu_mesh_buffer_structure.first_vertex,
                0);
        }

        _graphics_backend->command().end_rendering(
            _command_buffer_list[frame_index],
            _swap_chain_ref,
            current_image_index);

        if (!_redner_item_list.empty())
        {
            updateUniformBuffer(frame_index);
        }

        const auto end_result = _graphics_backend->command().end_frame(
            _command_buffer_list[frame_index],
            _swap_chain_ref);
        if (end_result == graphics::api::ESwapChainResult::RECREATION_REQUIRED)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Present failed because Swapchain is out of date");

            _graphics_backend->command().wait_idle();
            _swap_chain_ref = {};

            _swap_chain_ref = _graphics_backend->resource().make_swap_chain(
                _window_surface,
                MAX_FRAMES_IN_FLIGHT);

            init_color_msaa();
            init_depth();

            XAR_LOG(
                logging::LogLevel::DEBUG,
                tag,
                "Present failed because Swapchain is out of date but swapchain was recreated");
        }
        else if (end_result != graphics::api::ESwapChainResult::OK)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Acquire failed because Swapchain");
            return;
        }

        XAR_LOG(
            xar_engine::logging::LogLevel::DEBUG,
            tag,
            "Frame buffer nr {}, frames in total {}",
            frame_index,
            frameCounter);

        ++frameCounter;
    }
}