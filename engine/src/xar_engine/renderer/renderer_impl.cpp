#include <xar_engine/renderer/renderer_impl.hpp>

#include <chrono>
#include <thread>
#include <vector>

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
            attributeDescriptions[0].format = graphics::api::EFormat::R32G32B32_SIGNED_FLOAT;
            attributeDescriptions[0].offset = 0;

            attributeDescriptions[1].binding_index = 1;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = graphics::api::EFormat::R32G32B32_SIGNED_FLOAT;
            attributeDescriptions[1].offset = 0;

            attributeDescriptions[2].binding_index = 2;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = graphics::api::EFormat::R32G32_SIGNED_FLOAT;
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
        get_state().color_image_ref = get_state().graphics_backend->image_unit().make_image(
            {
                graphics::api::EImageType::COLOR_ATTACHMENT,
                {
                    static_cast<std::uint32_t>(get_state().window_surface->get_pixel_size().x),
                    static_cast<std::uint32_t>(get_state().window_surface->get_pixel_size().y),
                    1
                },
                graphics::api::EFormat::R8G8B8A8_SRGB,
                1,
                get_state().graphics_backend->device_unit().get_sample_count()
            });


        get_state().color_image_view_ref = get_state().graphics_backend->image_unit().make_image_view(
            {
                get_state().color_image_ref,
                graphics::api::EImageAspect::COLOR,
                1
            });
    }


    void RendererImpl::init_depth()
    {
        get_state().depth_image_ref = get_state().graphics_backend->image_unit().make_image(
            {
                graphics::api::EImageType::DEPTH_ATTACHMENT,
                {
                    static_cast<std::uint32_t>(get_state().window_surface->get_pixel_size().x),
                    static_cast<std::uint32_t>(get_state().window_surface->get_pixel_size().y),
                    1
                },
                get_state().graphics_backend->device_unit().find_depth_format(),
                1,
                get_state().graphics_backend->device_unit().get_sample_count()
            });

        get_state().depth_image_view_ref = get_state().graphics_backend->image_unit().make_image_view(
            {
                get_state().depth_image_ref,
                graphics::api::EImageAspect::DEPTH,
                1
            });

        auto tmp_command_buffer = get_state().graphics_backend->command_buffer_unit().make_command_buffer_list({1});
        get_state().graphics_backend->command_buffer_unit().begin_command_buffer(
            {
                tmp_command_buffer[0],
                graphics::api::ECommandBufferType::ONE_TIME
            });
        get_state().graphics_backend->image_unit().transit_image_layout(
            {
                tmp_command_buffer[0],
                get_state().depth_image_ref,
                graphics::api::EImageLayout::DEPTH_STENCIL_ATTACHMENT
            });
        get_state().graphics_backend->command_buffer_unit().end_command_buffer({tmp_command_buffer[0]});
        get_state().graphics_backend->command_buffer_unit().submit_command_buffer({tmp_command_buffer[0]});
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

        ubo.model = ubo.model * get_state().redner_item_list[0].gpu_mesh_instance.model_matrix;

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
            get_state().window_surface->get_pixel_size().x / (float) get_state().window_surface->get_pixel_size().y,
            0.1f,
            10.0f);
        ubo.proj.as_column_list[1].y *= -1;

        get_state().graphics_backend->buffer_unit().update_buffer(
            {
                get_state().uniform_buffer_ref_list[currentImageNr],
                {
                    {
                        &ubo,
                        0,
                        sizeof(ubo),
                    }
                }
            });
    }
}


namespace xar_engine::renderer
{
    RendererImpl::RendererImpl(
        std::shared_ptr<RendererState> state,
        std::unique_ptr<unit::IGpuMaterialUnit> gpu_material_unit,
        std::unique_ptr<unit::IGpuModelUnit> gpu_model_unit)
        : SharedRendererState(std::move(state))
        , _gpu_material_unit(std::move(gpu_material_unit))
        , _gpu_model_unit(std::move(gpu_model_unit))
    {
        get_state().command_buffer_list = get_state().graphics_backend->command_buffer_unit().make_command_buffer_list({MAX_FRAMES_IN_FLIGHT});

        get_state().swap_chain_ref = get_state().graphics_backend->swap_chain_unit().make_swap_chain(
            {
                get_state().window_surface,
                MAX_FRAMES_IN_FLIGHT
            });

        get_state().vertex_shader_ref = get_state().graphics_backend->shader_unit().make_shader({xar_engine::file::read_binary_file("assets/triangle.vert.spv")});
        get_state().fragment_shader_ref = get_state().graphics_backend->shader_unit().make_shader({xar_engine::file::read_binary_file("assets/triangle.frag.spv")});

        get_state().ubo_descriptor_set_layout_ref = get_state().graphics_backend->descriptor_unit().make_descriptor_set_layout({{graphics::api::EDescriptorType::UNIFORM_BUFFER}});
        get_state().image_descriptor_set_layout_ref = get_state().graphics_backend->descriptor_unit().make_descriptor_set_layout({{graphics::api::EDescriptorType::SAMPLED_IMAGE}});

        get_state().graphics_pipeline_ref = get_state().graphics_backend->graphics_pipeline_unit().make_graphics_pipeline(
            {
                {get_state().ubo_descriptor_set_layout_ref, get_state().image_descriptor_set_layout_ref},
                get_state().vertex_shader_ref,
                get_state().fragment_shader_ref,
                getAttributeDescriptions(),
                getBindingDescription(),
                graphics::api::EFormat::R8G8B8A8_SRGB,
                get_state().graphics_backend->device_unit().find_depth_format(),
                get_state().graphics_backend->device_unit().get_sample_count()
            });

        init_color_msaa();
        init_depth();

        get_state().uniform_buffer_ref_list.reserve(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            get_state().uniform_buffer_ref_list.push_back(get_state().graphics_backend->buffer_unit().make_uniform_buffer({sizeof(UniformBufferObject)}));
        }

        get_state().ubo_descriptor_pool_ref = get_state().graphics_backend->descriptor_unit().make_descriptor_pool({{graphics::api::EDescriptorType::UNIFORM_BUFFER}});
        get_state().image_descriptor_pool_ref = get_state().graphics_backend->descriptor_unit().make_descriptor_pool({{graphics::api::EDescriptorType::SAMPLED_IMAGE}});

        get_state().ubo_descriptor_set_list_ref = get_state().graphics_backend->descriptor_unit().make_descriptor_set_list(
            {
                get_state().ubo_descriptor_pool_ref,
                get_state().ubo_descriptor_set_layout_ref,
                MAX_FRAMES_IN_FLIGHT
            });
        get_state().graphics_backend->descriptor_unit().write_descriptor_set(
            {
                get_state().ubo_descriptor_set_list_ref[0],
                0,
                {get_state().uniform_buffer_ref_list[0]},
                0,
                {},
                {}
            });
        get_state().graphics_backend->descriptor_unit().write_descriptor_set(
            {
                get_state().ubo_descriptor_set_list_ref[1],
                0,
                {get_state().uniform_buffer_ref_list[1]},
                0,
                {},
                {}});

        get_state().image_descriptor_set_ref = get_state().graphics_backend->descriptor_unit().make_descriptor_set_list(
            {
                get_state().image_descriptor_pool_ref,
                get_state().image_descriptor_set_layout_ref,
                1
            })[0];
    }

    RendererImpl::~RendererImpl()
    {
        get_state().graphics_backend->device_unit().wait_idle();
    }

    unit::IGpuMaterialUnit& RendererImpl::gpu_material_unit()
    {
        return *_gpu_material_unit;
    }

    unit::IGpuModelUnit& RendererImpl::gpu_model_unit()
    {
        return *_gpu_model_unit;
    }

    void RendererImpl::add_gpu_mesh_instance_to_render(
        const gpu_asset::GpuMeshInstance& gpu_mesh_instance,
        const gpu_asset::GpuMaterialReference& gpu_material)
    {
        get_state().redner_item_list.push_back({gpu_mesh_instance, gpu_material});
    }

    void RendererImpl::clear_gpu_mesh_instance_to_render()
    {
        get_state().redner_item_list.clear();
    }

    void RendererImpl::update()
    {
        const auto begin_frame_result = get_state().graphics_backend->swap_chain_unit().begin_frame({get_state().swap_chain_ref});

        if (std::get<0>(begin_frame_result) == graphics::api::ESwapChainResult::RECREATION_REQUIRED)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Acquire failed because Swapchain is out of date");

            get_state().graphics_backend->device_unit().wait_idle();
            get_state().swap_chain_ref = {};

            get_state().swap_chain_ref = get_state().graphics_backend->swap_chain_unit().make_swap_chain(
                {
                    get_state().window_surface,
                    MAX_FRAMES_IN_FLIGHT
                });

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

        get_state().graphics_backend->swap_chain_unit().begin_rendering(
            {
                get_state().command_buffer_list[frame_index],
                get_state().swap_chain_ref,
                current_image_index,
                get_state().color_image_view_ref,
                get_state().depth_image_view_ref
            });

        get_state().graphics_backend->graphics_pipeline_unit().set_pipeline_state(
            {
                get_state().command_buffer_list[frame_index],
                get_state().swap_chain_ref,
                get_state().graphics_pipeline_ref,
                {get_state().ubo_descriptor_set_list_ref[frame_index], get_state().image_descriptor_set_ref}});

        static float pcc = 0.0f;
        struct Constants
        {
            float time = pcc++;
            std::int32_t material_index;
        } pc;

        for (const auto& render_item: get_state().redner_item_list)
        {
            const auto& gpu_mesh_data = get_state().gpu_mesh_data_map.get(render_item.gpu_mesh_instance.gpu_mesh);
            const auto& gpu_model_data = get_state().gpu_model_data_map.get(gpu_mesh_data.gpu_model);
            const auto& gpu_buffer_data = get_state().gpu_model_data_buffer_map.get(gpu_model_data.gpu_model_data_buffer);

            const auto& gpu_material_data = get_state().gpu_material_data_map.get(render_item.gpu_material);

            pc.material_index = gpu_material_data.index;
            get_state().graphics_backend->graphics_pipeline_unit().push_constants(
                {
                    get_state().command_buffer_list[frame_index],
                    get_state().graphics_pipeline_ref,
                    graphics::api::EShaderType::FRAGMENT,
                    0,
                    sizeof(Constants),
                    &pc
                });

            get_state().graphics_backend->graphics_pipeline_unit().set_vertex_buffer_list(
                {
                    get_state().command_buffer_list[frame_index],
                    {
                        gpu_buffer_data.position_buffer,
                        gpu_buffer_data.normal_buffer,
                        gpu_buffer_data.texture_coord_buffer,
                    },
                    {0, 0, 0},
                    0
                });
            get_state().graphics_backend->graphics_pipeline_unit().set_index_buffer(
                {
                    get_state().command_buffer_list[frame_index],
                    gpu_buffer_data.index_buffer,
                    0
                });

            const auto gpu_mesh_buffer_structure = gpu_buffer_data
                .structure
                .gpu_model_buffer_structure_list[gpu_model_data.model_index]
                .gpu_mesh_buffer_structure_list[gpu_mesh_data.mesh_index];

            get_state().graphics_backend->graphics_pipeline_unit().draw_indexed(
                {
                    get_state().command_buffer_list[frame_index],
                    gpu_mesh_buffer_structure.index_counts,
                    1,
                    gpu_mesh_buffer_structure.first_index,
                    gpu_mesh_buffer_structure.first_vertex,
                    0
                });
        }

        get_state().graphics_backend->swap_chain_unit().end_rendering(
            {
                get_state().command_buffer_list[frame_index],
                get_state().swap_chain_ref,
                current_image_index
            });

        if (!get_state().redner_item_list.empty())
        {
            updateUniformBuffer(frame_index);
        }

        const auto end_result = get_state().graphics_backend->swap_chain_unit().end_frame(
            {
                get_state().command_buffer_list[frame_index],
                get_state().swap_chain_ref
            });
        if (end_result == graphics::api::ESwapChainResult::RECREATION_REQUIRED)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Present failed because Swapchain is out of date");

            get_state().graphics_backend->device_unit().wait_idle();
            get_state().swap_chain_ref = {};

            get_state().swap_chain_ref = get_state().graphics_backend->swap_chain_unit().make_swap_chain(
                {
                    get_state().window_surface,
                    MAX_FRAMES_IN_FLIGHT
                });

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
            get_state().frameCounter);

        ++get_state().frameCounter;
    }
}