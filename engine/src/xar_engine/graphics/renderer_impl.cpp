#include <xar_engine/graphics/renderer_impl.hpp>

#include <chrono>
#include <thread>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <xar_engine/asset/image_loader.hpp>
#include <xar_engine/asset/model_loader.hpp>

#include <xar_engine/error/exception_utils.hpp>

#include <xar_engine/file/file.hpp>

#include <xar_engine/graphics/api/graphics_backend.hpp>

#include <xar_engine/logging/logger.hpp>


namespace xar_engine::graphics
{
    namespace
    {
        constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        constexpr auto tag = "Vulkan Sandbox";

        gpu_asset::GpuModelList allocate_gpu_model_list(
            api::IGraphicsBackend& graphics_backend,
            const std::vector<asset::Model>& model_list)
        {
            auto gpu_model_list_buffer_structure_local = gpu_asset::make_gpu_model_list_buffer_structure(model_list);

            auto gpu_model_list = gpu_asset::GpuModelList{
                .position_buffer = graphics_backend.resource().make_vertex_buffer(gpu_model_list_buffer_structure_local.position_list_byte_size),
                .normal_buffer = graphics_backend.resource().make_vertex_buffer(gpu_model_list_buffer_structure_local.normal_list_byte_size),
                .texture_coord_buffer = graphics_backend.resource().make_vertex_buffer(gpu_model_list_buffer_structure_local.texture_coord_list_byte_size),
                .index_buffer = graphics_backend.resource().make_index_buffer(gpu_model_list_buffer_structure_local.index_list_byte_size),
            };

            gpu_model_list.buffer_structure = std::move(gpu_model_list_buffer_structure_local);

            return gpu_model_list;
        }

        std::vector<api::VertexInputBinding> getBindingDescription()
        {
            return {
                api::VertexInputBinding{
                    .binding_index = 0,
                    .stride = sizeof(math::Vector3f),
                    .input_rate = api::VertexInputBindingRate::PER_VERTEX,
                },
                api::VertexInputBinding{
                    .binding_index = 1,
                    .stride = sizeof(math::Vector3f),
                    .input_rate = api::VertexInputBindingRate::PER_VERTEX,
                },
                api::VertexInputBinding{
                    .binding_index = 2,
                    .stride = sizeof(math::Vector2f),
                    .input_rate = api::VertexInputBindingRate::PER_VERTEX,
                },
            };
        }

        std::vector<api::VertexInputAttribute> getAttributeDescriptions()
        {
            std::vector<api::VertexInputAttribute> attributeDescriptions(3);

            attributeDescriptions[0].binding_index = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = api::EFormat::R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = 0;

            attributeDescriptions[1].binding_index = 1;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = api::EFormat::R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = 0;

            attributeDescriptions[2].binding_index = 2;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = api::EFormat::R32G32_SFLOAT;
            attributeDescriptions[2].offset = 0;

            return attributeDescriptions;
        }

        struct UniformBufferObject
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };
    }

    void RendererImpl::init_color_msaa()
    {
        _color_image_ref = _graphics_backend->resource().make_image(
            api::EImageType::COLOR_ATTACHMENT,
            {
                static_cast<std::int32_t>(_window_surface->get_pixel_size().x),
                static_cast<std::int32_t>(_window_surface->get_pixel_size().y),
                1
            },
            api::EFormat::R8G8B8A8_SRGB,
            1,
            _graphics_backend->host().get_sample_count());


        _color_image_view_ref = _graphics_backend->resource().make_image_view(
            _color_image_ref,
            api::EImageAspect::COLOR,
            1);
    }


    void RendererImpl::init_depth()
    {
        _depth_image_ref = _graphics_backend->resource().make_image(
            api::EImageType::DEPTH_ATTACHMENT,
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
            api::EImageAspect::DEPTH,
            1);

        auto tmp_command_buffer = _graphics_backend->resource().make_command_buffer_list(1);
        _graphics_backend->command().begin_command_buffer(
            tmp_command_buffer[0],
            api::ECommandBufferType::ONE_TIME);
        _graphics_backend->command().transit_image_layout(
            tmp_command_buffer[0],
            _depth_image_ref,
            api::EImageLayout::DEPTH_STENCIL_ATTACHMENT);
        _graphics_backend->command().end_command_buffer(tmp_command_buffer[0]);
        _graphics_backend->command().submit_command_buffer(tmp_command_buffer[0]);
    }


    void RendererImpl::init_texture()
    {
        auto image = asset::ImageLoaderFactory().make()->load_image_from_file("assets/viking_room.png");
        mipLevels = static_cast<uint32_t>(std::floor(
            std::log2(
                std::max(
                    image.pixel_width,
                    image.pixel_height)))) + 1;

        const auto imageSize = asset::image::get_byte_size(image);

        auto staging_buffer = _graphics_backend->resource().make_staging_buffer(imageSize);
        _graphics_backend->host().update_buffer(
            staging_buffer,
            {{
                 image.bytes.data(),
                 0,
                 static_cast<std::uint32_t>(imageSize),
             }});

        _texture_image_ref = _graphics_backend->resource().make_image(
            api::EImageType::TEXTURE,
            {image.pixel_width, image.pixel_height, 1},
            api::EFormat::R8G8B8A8_SRGB,
            mipLevels,
            1);

        auto tmp_command_buffer = _graphics_backend->resource().make_command_buffer_list(1);
        _graphics_backend->command().begin_command_buffer(
            tmp_command_buffer[0],
            api::ECommandBufferType::ONE_TIME);
        _graphics_backend->command().transit_image_layout(
            tmp_command_buffer[0],
            _texture_image_ref,
            api::EImageLayout::TRANSFER_DESTINATION);
        _graphics_backend->command().copy_buffer_to_image(
            tmp_command_buffer[0],
            staging_buffer,
            _texture_image_ref);
        _graphics_backend->command().generate_image_mip_maps(
            tmp_command_buffer[0],
            _texture_image_ref);
        _graphics_backend->command().end_command_buffer(tmp_command_buffer[0]);
        _graphics_backend->command().submit_command_buffer(tmp_command_buffer[0]);
    }


    void RendererImpl::updateUniformBuffer(uint32_t currentImageNr)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};

        ubo.model = glm::rotate(
            glm::mat4(1.0f),
            time * glm::radians(90.0f),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f));

        // ubo.model = glm::scale(ubo.model, glm::vec3{1.0f, 1.0f, 1.0f} / 1000.0f);

        ubo.view = glm::lookAt(
            glm::vec3(
                2.0f,
                2.0f,
                2.0f),
            glm::vec3(
                0.0f,
                0.0f,
                0.0f),
            glm::vec3(
                0.0f,
                0.0f,
                1.0f));

        ubo.proj = glm::perspective(
            glm::radians(45.0f),
            _window_surface->get_pixel_size().x / (float) _window_surface->get_pixel_size().y,
            0.1f,
            10.0f);

        ubo.proj[1][1] *= -1;

        _graphics_backend->host().update_buffer(
            _uniform_buffer_ref_list[currentImageNr],
            {{
                 &ubo,
                 0,
                 sizeof(ubo),
             }});
    }
}


namespace xar_engine::graphics
{
    RendererImpl::RendererImpl(
        std::shared_ptr<api::IGraphicsBackend> graphics_backend,
        std::shared_ptr<IWindowSurface> window_surface)
        : _graphics_backend(std::move(graphics_backend))
        , _window_surface(std::move(window_surface))
        , frameCounter(0)
        , mipLevels(0)
    {
        _command_buffer_list = _graphics_backend->resource().make_command_buffer_list(MAX_FRAMES_IN_FLIGHT);

        _swap_chain_ref = _graphics_backend->resource().make_swap_chain(
            _window_surface,
            MAX_FRAMES_IN_FLIGHT);

        _vertex_shader_ref = _graphics_backend->resource().make_shader(xar_engine::file::read_binary_file("assets/triangle.vert.spv"));
        _fragment_shader_ref = _graphics_backend->resource().make_shader(xar_engine::file::read_binary_file("assets/triangle.frag.spv"));

        _descriptor_set_layout_ref = _graphics_backend->resource().make_descriptor_set_layout();

        _graphics_pipeline_ref = _graphics_backend->resource().make_graphics_pipeline(
            _descriptor_set_layout_ref,
            _vertex_shader_ref,
            _fragment_shader_ref,
            getAttributeDescriptions(),
            getBindingDescription(),
            api::EFormat::R8G8B8A8_SRGB,
            _graphics_backend->host().find_depth_format(),
            _graphics_backend->host().get_sample_count());

        init_color_msaa();
        init_depth();
        init_texture();

        _texture_image_view_ref = _graphics_backend->resource().make_image_view(
            _texture_image_ref,
            api::EImageAspect::COLOR,
            mipLevels);

        _sampler_ref = _graphics_backend->resource().make_sampler(static_cast<float>(mipLevels));

        _uniform_buffer_ref_list.reserve(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            _uniform_buffer_ref_list.push_back(_graphics_backend->resource().make_uniform_buffer(sizeof(UniformBufferObject)));
        }

        _descriptor_pool_ref = _graphics_backend->resource().make_descriptor_pool();
        _descriptor_set_list_ref = _graphics_backend->resource().make_descriptor_set_list(
            _descriptor_pool_ref,
            _descriptor_set_layout_ref,
            _uniform_buffer_ref_list,
            _texture_image_view_ref,
            _sampler_ref);
    }

    RendererImpl::~RendererImpl()
    {
        _graphics_backend->command().wait_idle();
    }

    gpu_asset::GpuModelListReference RendererImpl::make_gpu_model_list(const std::vector<asset::Model>& model_list)
    {
        auto gpu_model_list = allocate_gpu_model_list(
            *_graphics_backend,
            model_list);

        auto staging_buffer = _graphics_backend->resource().make_staging_buffer(gpu_model_list.buffer_structure.position_list_byte_size);
        {
            auto buffer_update_list = std::vector<api::BufferUpdate>{};
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

        auto staging_buffer_1 = _graphics_backend->resource().make_staging_buffer(gpu_model_list.buffer_structure.normal_list_byte_size);
        {
            auto buffer_update_list = std::vector<api::BufferUpdate>{};
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

        auto staging_buffer_2 = _graphics_backend->resource().make_staging_buffer(gpu_model_list.buffer_structure.texture_coord_list_byte_size);
        {
            auto buffer_update_list = std::vector<api::BufferUpdate>{};
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

        auto staging_buffer_3 = _graphics_backend->resource().make_staging_buffer(gpu_model_list.buffer_structure.index_list_byte_size);
        {
            auto buffer_update_list = std::vector<api::BufferUpdate>{};
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
            api::ECommandBufferType::ONE_TIME);

        _graphics_backend->command().copy_buffer(
            command_buffer[0],
            staging_buffer,
            gpu_model_list.position_buffer);
        _graphics_backend->command().copy_buffer(
            command_buffer[0],
            staging_buffer_1,
            gpu_model_list.normal_buffer);
        _graphics_backend->command().copy_buffer(
            command_buffer[0],
            staging_buffer_2,
            gpu_model_list.texture_coord_buffer);
        _graphics_backend->command().copy_buffer(
            command_buffer[0],
            staging_buffer_3,
            gpu_model_list.index_buffer);

        _graphics_backend->command().end_command_buffer(command_buffer[0]);
        _graphics_backend->command().submit_command_buffer(command_buffer[0]);

        return _gpu_model_map.add(std::move(gpu_model_list));
    }

    void RendererImpl::add_gpu_model_list_to_render(const gpu_asset::GpuModelListReference& gpu_model_list)
    {
        _gpu_model_list_to_render.push_back(gpu_model_list);
    }

    void RendererImpl::remove_gpu_model_list_from_render(const gpu_asset::GpuModelListReference& gpu_model_list)
    {
        std::erase_if(
            _gpu_model_list_to_render,
            [&](const auto& gpu_model_list_ref)
            {
                return gpu_model_list.get_id() == gpu_model_list_ref.get_id();
            });
    }

    void RendererImpl::update()
    {
        const auto begin_frame_result = _graphics_backend->host().begin_frame(_swap_chain_ref);

        if (std::get<0>(begin_frame_result) == api::ESwapChainResult::RECREATION_REQUIRED)
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
        else if (std::get<0>(begin_frame_result) != api::ESwapChainResult::OK)
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
            _descriptor_set_list_ref[frame_index]);

        static float pcc = 0.0f;
        struct Constants
        {
            float time = pcc++;
        } pc;

        _graphics_backend->command().push_constants(
            _command_buffer_list[frame_index],
            _graphics_pipeline_ref,
            api::EShaderType::FRAGMENT,
            0,
            sizeof(Constants),
            &pc);

        for (const auto& gpu_model_list: _gpu_model_list_to_render)
        {
            const auto& gpu_model = _gpu_model_map.get_object(gpu_model_list);

            _graphics_backend->command().set_vertex_buffer_list(
                _command_buffer_list[frame_index],
                {
                    gpu_model.position_buffer,
                    gpu_model.normal_buffer,
                    gpu_model.texture_coord_buffer,
                },
                {0, 0, 0},
                0);
            _graphics_backend->command().set_index_buffer(
                _command_buffer_list[frame_index],
                gpu_model.index_buffer,
                0);

            _graphics_backend->command().draw_indexed(
                _command_buffer_list[frame_index],
                gpu_model.buffer_structure.index_counts,
                1,
                0,
                0,
                0);
        }

        _graphics_backend->command().end_rendering(
            _command_buffer_list[frame_index],
            _swap_chain_ref,
            current_image_index);

        updateUniformBuffer(frame_index);

        const auto end_result = _graphics_backend->command().end_frame(
            _command_buffer_list[frame_index],
            _swap_chain_ref);
        if (end_result == api::ESwapChainResult::RECREATION_REQUIRED)
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
        else if (end_result != api::ESwapChainResult::OK)
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