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

#include <xar_engine/graphics/graphics_backend.hpp>

#include <xar_engine/logging/logger.hpp>


namespace xar_engine::graphics
{
    namespace
    {
        constexpr int MAX_FRAMES_IN_FLIGHT = 2;
        constexpr auto tag = "Vulkan Sandbox";

        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec2 textureCoords;

            static std::vector<VertexInputBinding> getBindingDescription()
            {
                VertexInputBinding binding{};
                binding.binding_index = 0;
                binding.stride = sizeof(Vertex);
                binding.input_rate = VertexInputBindingRate::PER_VERTEX;

                return {binding};
            }

            static std::vector<VertexInputAttribute> getAttributeDescriptions()
            {
                std::vector<VertexInputAttribute> attributeDescriptions(3);

                attributeDescriptions[0].binding_index = 0;
                attributeDescriptions[0].location = 0;
                attributeDescriptions[0].format = EFormat::R32G32B32_SFLOAT;
                attributeDescriptions[0].offset = offsetof(
                    Vertex,
                    position);

                attributeDescriptions[1].binding_index = 0;
                attributeDescriptions[1].location = 1;
                attributeDescriptions[1].format = EFormat::R32G32B32_SFLOAT;
                attributeDescriptions[1].offset = offsetof(
                    Vertex,
                    color);

                attributeDescriptions[2].binding_index = 0;
                attributeDescriptions[2].location = 2;
                attributeDescriptions[2].format = EFormat::R32G32_SFLOAT;
                attributeDescriptions[2].offset = offsetof(
                    Vertex,
                    textureCoords);

                return attributeDescriptions;
            }
        };

        struct UniformBufferObject
        {
            alignas(16) glm::mat4 model;
            alignas(16) glm::mat4 view;
            alignas(16) glm::mat4 proj;
        };

        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;
    }


    void RendererImpl::init_vertex_data()
    {
        const auto bufferSize = sizeof(vertices[0]) * vertices.size();

        auto staging_buffer = _graphics_backend->resource().make_staging_buffer(bufferSize);
        _graphics_backend->host_command().update_buffer(
            staging_buffer,
            vertices.data(),
            bufferSize);

        _vertex_buffer_ref = _graphics_backend->resource().make_vertex_buffer(bufferSize);

        auto tmp_command_buffer = _graphics_backend->resource().make_command_buffer_list(1);
        _graphics_backend->device_command().begin_command_buffer(tmp_command_buffer[0], ECommandBufferType::ONE_TIME);
        _graphics_backend->device_command().copy_buffer(
            tmp_command_buffer[0],
            staging_buffer,
            _vertex_buffer_ref);
        _graphics_backend->device_command().end_command_buffer(tmp_command_buffer[0]);
        _graphics_backend->device_command().submit_command_buffer(tmp_command_buffer[0]);
    }


    void RendererImpl::init_index_data()
    {
        const auto bufferSize = sizeof(indices[0]) * indices.size();

        auto staging_buffer = _graphics_backend->resource().make_staging_buffer(bufferSize);
        _graphics_backend->host_command().update_buffer(
            staging_buffer,
            indices.data(),
            bufferSize);

        _index_buffer_ref = _graphics_backend->resource().make_index_buffer(bufferSize);

        auto tmp_command_buffer = _graphics_backend->resource().make_command_buffer_list(1);
        _graphics_backend->device_command().begin_command_buffer(tmp_command_buffer[0], ECommandBufferType::ONE_TIME);
        _graphics_backend->device_command().copy_buffer(
            tmp_command_buffer[0],
            staging_buffer,
            _index_buffer_ref);
        _graphics_backend->device_command().end_command_buffer(tmp_command_buffer[0]);
        _graphics_backend->device_command().submit_command_buffer(tmp_command_buffer[0]);
    }


    void RendererImpl::init_model()
    {
        const auto model = xar_engine::asset::ModelLoaderFactory().make()->load_model_from_file("assets/viking_room.obj");

        vertices.resize(model.meshes.back().positions.size());
        for (auto i = 0; i < model.meshes.back().positions.size(); ++i)
        {
            vertices[i].position.x = model.meshes.back().positions[i].x;
            vertices[i].position.y = model.meshes.back().positions[i].y;
            vertices[i].position.z = model.meshes.back().positions[i].z;

            const auto channels = model.meshes.back().texture_coords[0].channel_count;
            vertices[i].textureCoords.x = model.meshes.back().texture_coords[0].coords[i * channels + 0];
            vertices[i].textureCoords.y = model.meshes.back().texture_coords[0].coords[i * channels + 1];
        }


        indices.resize(model.meshes.back().indices.size());
        memcpy(
            indices.data(),
            model.meshes.back().indices.data(),
            sizeof(indices[0]) * indices.size());
    }


    void RendererImpl::init_color_msaa()
    {
        _color_image_ref = _graphics_backend->resource().make_image(
            EImageType::COLOR_ATTACHMENT,
            {
                static_cast<std::int32_t>(_window_surface->get_pixel_size().x),
                static_cast<std::int32_t>(_window_surface->get_pixel_size().y),
                1
            },
            EFormat::R8G8B8A8_SRGB,
            1,
            _graphics_backend->host_command().get_sample_count());


        _color_image_view_ref = _graphics_backend->resource().make_image_view(
            _color_image_ref,
            EImageAspect::COLOR,
            1);
    }


    void RendererImpl::init_depth()
    {
        _depth_image_ref = _graphics_backend->resource().make_image(
            EImageType::DEPTH_ATTACHMENT,
            {
                static_cast<std::int32_t>(_window_surface->get_pixel_size().x),
                static_cast<std::int32_t>(_window_surface->get_pixel_size().y),
                1
            },
            _graphics_backend->host_command().find_depth_format(),
            1,
            _graphics_backend->host_command().get_sample_count());

        _depth_image_view_ref = _graphics_backend->resource().make_image_view(
            _depth_image_ref,
            EImageAspect::DEPTH,
            1);

        auto tmp_command_buffer = _graphics_backend->resource().make_command_buffer_list(1);
        _graphics_backend->device_command().begin_command_buffer(tmp_command_buffer[0], ECommandBufferType::ONE_TIME);
        _graphics_backend->device_command().transit_image_layout(
            tmp_command_buffer[0],
            _depth_image_ref,
            EImageLayout::DEPTH_STENCIL_ATTACHMENT);
        _graphics_backend->device_command().end_command_buffer(tmp_command_buffer[0]);
        _graphics_backend->device_command().submit_command_buffer(tmp_command_buffer[0]);
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
        _graphics_backend->host_command().update_buffer(
            staging_buffer,
            image.bytes.data(),
            static_cast<size_t>(imageSize));

        _texture_image_ref = _graphics_backend->resource().make_image(
            EImageType::TEXTURE,
            {image.pixel_width, image.pixel_height, 1},
            EFormat::R8G8B8A8_SRGB,
            mipLevels,
            1);

        auto tmp_command_buffer = _graphics_backend->resource().make_command_buffer_list(1);
        _graphics_backend->device_command().begin_command_buffer(tmp_command_buffer[0], ECommandBufferType::ONE_TIME);
        _graphics_backend->device_command().transit_image_layout(
            tmp_command_buffer[0],
            _texture_image_ref,
            EImageLayout::TRANSFER_DESTINATION);
        _graphics_backend->device_command().copy_buffer_to_image(
            tmp_command_buffer[0],
            staging_buffer,
            _texture_image_ref);
        _graphics_backend->device_command().generate_image_mip_maps(
            tmp_command_buffer[0],
            _texture_image_ref);
        _graphics_backend->device_command().end_command_buffer(tmp_command_buffer[0]);
        _graphics_backend->device_command().submit_command_buffer(tmp_command_buffer[0]);
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

        _graphics_backend->host_command().update_buffer(
            _uniform_buffer_ref_list[currentImageNr],
            &ubo,
            sizeof(ubo));
    }
}


namespace xar_engine::graphics
{
    RendererImpl::RendererImpl(
        std::shared_ptr<IGraphicsBackend> graphics_backend,
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
            Vertex::getAttributeDescriptions(),
            Vertex::getBindingDescription(),
            EFormat::R8G8B8A8_SRGB,
            _graphics_backend->host_command().find_depth_format(),
            _graphics_backend->host_command().get_sample_count());

        init_color_msaa();
        init_depth();
        init_texture();

        _texture_image_view_ref = _graphics_backend->resource().make_image_view(
            _texture_image_ref,
            EImageAspect::COLOR,
            mipLevels);

        _sampler_ref = _graphics_backend->resource().make_sampler(static_cast<float>(mipLevels));

        init_model();
        init_vertex_data();
        init_index_data();

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
        _graphics_backend->device_command().wait_idle();
    }

    void RendererImpl::update()
    {
        const auto begin_frame_result = _graphics_backend->host_command().begin_frame(_swap_chain_ref);

        if (std::get<0>(begin_frame_result) == ESwapChainResult::RECREATION_REQUIRED)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Acquire failed because Swapchain is out of date");

            _graphics_backend->device_command().wait_idle();
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
        else if (std::get<0>(begin_frame_result) != ESwapChainResult::OK)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Acquire failed because Swapchain");
            return;
        }

        const auto current_image_index = std::get<1>(begin_frame_result);
        const auto frame_index = std::get<2>(begin_frame_result);

        _graphics_backend->device_command().TMP_RECORD_FRAME(
            _command_buffer_list[frame_index],
            _swap_chain_ref,
            _graphics_pipeline_ref,
            current_image_index,
            _descriptor_set_list_ref[frame_index],
            _vertex_buffer_ref,
            _index_buffer_ref,
            _color_image_view_ref,
            _depth_image_view_ref,
            indices.size());

        updateUniformBuffer(frame_index);

        const auto end_result = _graphics_backend->device_command().end_frame(
            _command_buffer_list[frame_index],
            _swap_chain_ref);
        if (end_result == ESwapChainResult::RECREATION_REQUIRED)
        {
            XAR_LOG(
                logging::LogLevel::ERROR,
                tag,
                "Present failed because Swapchain is out of date");

            _graphics_backend->device_command().wait_idle();
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
        else if (end_result != ESwapChainResult::OK)
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