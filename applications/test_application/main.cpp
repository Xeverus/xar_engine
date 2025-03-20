#include <xar_engine/logging/logger.hpp>

#include <xar_engine/asset/model_loader.hpp>

#include <xar_engine/graphics/api/graphics_backend_factory.hpp>

#include <xar_engine/meta/overloaded.hpp>

#include <xar_engine/os/application.hpp>

#include <xar_engine/renderer/renderer.hpp>


int main()
{
    const auto application = xar_engine::os::ApplicationFactory().make({"Test Application"});
    auto window = application->make_window({"Test Application"});
    auto graphics_backend = xar_engine::graphics::api::GraphicsBackendFactory().make(xar_engine::graphics::api::EGraphicsBackendType::VULKAN);
    auto renderer = xar_engine::renderer::RendererFactory().make(
        graphics_backend,
        window->get_surface());

    std::vector<xar_engine::renderer::gpu_asset::GpuMaterialReference> gpu_material_list;
    std::vector<xar_engine::renderer::gpu_asset::GpuModel> gpu_model_list;
    window->set_on_run(
        [&]()
        {
            gpu_model_list = renderer->gpu_model_module().make_gpu_model(
                std::vector<xar_engine::asset::Model>{
                    xar_engine::asset::ModelLoaderFactory().make()->load_model_from_file("assets/viking_room.obj"),
                    xar_engine::asset::ModelLoaderFactory().make()->load_model_from_file("assets/house.obj"),
                });
            gpu_material_list.push_back(renderer->make_gpu_material({"assets/viking_room.png"}));
            gpu_material_list.push_back(renderer->make_gpu_material({"assets/house.png"}));
        });

    window->set_on_update(
        [&]()
        {
            renderer->update();
        });

    window->set_on_resize_event(
        [](
            const std::int32_t new_width,
            const std::int32_t new_height)
        {
        });

    window->set_on_keyboard_event(
        [&](const xar_engine::input::KeyboardEvent& event)
        {
            std::visit(
                xar_engine::meta::TOverloaded{
                    [&](const xar_engine::input::KeyboardKeyEvent& event)
                    {
                        XAR_LOG(
                            xar_engine::logging::LogLevel::DEBUG,
                            "TestApp",
                            "key code={}, state={}",
                            xar_engine::meta::enum_to_string(event.code),
                            xar_engine::meta::enum_to_string(event.state));

                        if (event.code != xar_engine::input::ButtonCode::_0 &&
                            event.code != xar_engine::input::ButtonCode::_1 &&
                            event.code != xar_engine::input::ButtonCode::_2)
                        {
                            return;
                        }

                        renderer->clear_gpu_mesh_instance_to_render();

                        switch (event.code)
                        {
                            case xar_engine::input::ButtonCode::_0:
                            {
                                break;
                            }
                            case xar_engine::input::ButtonCode::_1:
                            {
                                renderer->add_gpu_mesh_instance_to_render(
                                    {
                                        gpu_model_list[0].gpu_mesh[0],
                                        xar_engine::math::make_identity_matrix(),
                                    },
                                    gpu_material_list[0]);
                                break;
                            }
                            case xar_engine::input::ButtonCode::_2:
                            {
                                auto model_matrix = xar_engine::math::make_identity_matrix();

                                model_matrix = xar_engine::math::scale_matrix(
                                    model_matrix,
                                    {0.0015f, 0.0015f, 0.0015f});

                                model_matrix = xar_engine::math::rotate_matrix(
                                    model_matrix,
                                    90.0f,
                                    {1.0f, 0.0f, 0.0f});
                                model_matrix = xar_engine::math::rotate_matrix(
                                    model_matrix,
                                    45.0f,
                                    {0.0f, 1.0f, 0.0f});

                                for (auto i = 0; i < 7; ++i)
                                {
                                    renderer->add_gpu_mesh_instance_to_render(
                                        {
                                            gpu_model_list[1].gpu_mesh[i],
                                            model_matrix,
                                        },
                                        gpu_material_list[1]);
                                }
                                break;
                            }
                        }
                    }
                },
                event);
        });

    window->set_on_mouse_event(
        [&](const xar_engine::input::MouseEvent& event)
        {
            std::visit(
                xar_engine::meta::TOverloaded{
                    [&](const xar_engine::input::MouseButtonEvent& event)
                    {
                        XAR_LOG(
                            xar_engine::logging::LogLevel::DEBUG,
                            "TestApp",
                            "button code={}, state={}",
                            xar_engine::meta::enum_to_string(event.code),
                            xar_engine::meta::enum_to_string(event.state));
                    },
                    [&](const xar_engine::input::MouseMotionEvent& event)
                    {
                        XAR_LOG(
                            xar_engine::logging::LogLevel::INFO,
                            "TestApp",
                            "motion x={}, y={}",
                            event.position_x,
                            event.position_y);
                    },
                    [&](const xar_engine::input::MouseScrollEvent& event)
                    {
                        XAR_LOG(
                            xar_engine::logging::LogLevel::WARNING,
                            "TestApp",
                            "scroll x={}, y={}",
                            event.delta_x,
                            event.delta_y);
                    }
                },
                event);
        });

    window->set_on_close(
        [&]()
        {
            application->request_close();
        });

    application->run();

    return 0;
}