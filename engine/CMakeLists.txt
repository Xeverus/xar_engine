find_package(assimp REQUIRED)
find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(stb REQUIRED)
find_package(volk REQUIRED)


##############################
# Common config values
##############################
set(XAR_ENGINE_PUBLIC_FILES
        # asset
        include/xar_engine/asset/image.hpp
        include/xar_engine/asset/image_loader.hpp
        include/xar_engine/asset/model.hpp
        include/xar_engine/asset/model_loader.hpp

        # error
        include/xar_engine/error/exception.hpp

        # graphics api
        include/xar_engine/graphics/api/graphics_backend_factory.hpp
        include/xar_engine/graphics/api/graphics_backend_type.hpp

        # input
        include/xar_engine/input/button.hpp
        include/xar_engine/input/input_event.hpp

        # math
        include/xar_engine/math/epsilon.hpp
        include/xar_engine/math/matrix.hpp
        include/xar_engine/math/vector.hpp

        # meta
        include/xar_engine/meta/enum.hpp
        include/xar_engine/meta/overloaded.hpp
        include/xar_engine/meta/resource_reference.hpp

        # logging
        include/xar_engine/logging/console_logger.hpp
        include/xar_engine/logging/default_log_formatter.hpp
        include/xar_engine/logging/file_logger.hpp
        include/xar_engine/logging/log_formatter.hpp
        include/xar_engine/logging/log_level.hpp
        include/xar_engine/logging/logger.hpp
        include/xar_engine/logging/logger_chain.hpp
        include/xar_engine/logging/stream_logger.hpp

        # os
        include/xar_engine/os/application.hpp
        include/xar_engine/os/service_register.hpp
        include/xar_engine/os/window.hpp

        # renderer
        include/xar_engine/renderer/gpu_asset/gpu_mesh_instance.hpp
        include/xar_engine/renderer/gpu_asset/gpu_model.hpp
        include/xar_engine/renderer/renderer.hpp

        # version
        include/xar_engine/version/version.hpp)


set(XAR_ENGINE_PRIVATE_FILES
        # asset
        src/xar_engine/asset/assimp_model_loader.cpp
        src/xar_engine/asset/assimp_model_loader.hpp
        src/xar_engine/asset/image.cpp
        src/xar_engine/asset/image_loader.cpp
        src/xar_engine/asset/model_loader.cpp
        src/xar_engine/asset/stb_image_loader.cpp
        src/xar_engine/asset/stb_image_loader.hpp

        # error
        src/xar_engine/error/exception.cpp
        src/xar_engine/error/exception_utils.hpp

        # file
        src/xar_engine/file/file.cpp
        src/xar_engine/file/file.hpp

        # graphics api
        src/xar_engine/graphics/api/buffer_reference.hpp
        src/xar_engine/graphics/api/command_buffer_reference.hpp
        src/xar_engine/graphics/api/descriptor_pool_reference.hpp
        src/xar_engine/graphics/api/descriptor_set_reference.hpp
        src/xar_engine/graphics/api/descriptor_set_layout_reference.hpp
        src/xar_engine/graphics/api/format.hpp
        src/xar_engine/graphics/api/graphics_backend_factory.cpp
        src/xar_engine/graphics/api/graphics_backend_type.cpp
        src/xar_engine/graphics/api/graphics_pipeline_reference.hpp
        src/xar_engine/graphics/api/image_reference.hpp
        src/xar_engine/graphics/api/image_view_reference.hpp
        src/xar_engine/graphics/api/queue_reference.hpp
        src/xar_engine/graphics/api/sampler_reference.hpp
        src/xar_engine/graphics/api/shader_reference.hpp
        src/xar_engine/graphics/api/surface_reference.hpp
        src/xar_engine/graphics/api/swap_chain_reference.hpp

        # graphics backend
        src/xar_engine/graphics/backend/graphics_backend.cpp
        src/xar_engine/graphics/backend/graphics_backend.hpp
        
        # graphics backend vulkan
        src/xar_engine/graphics/backend/vulkan/vulkan_graphics_backend.cpp
        src/xar_engine/graphics/backend/vulkan/vulkan_graphics_backend.hpp
        src/xar_engine/graphics/backend/vulkan/vulkan_resource_storage.cpp
        src/xar_engine/graphics/backend/vulkan/vulkan_resource_storage.hpp
        src/xar_engine/graphics/backend/vulkan/vulkan_type_converters.cpp
        src/xar_engine/graphics/backend/vulkan/vulkan_type_converters.hpp

        # graphics context
        src/xar_engine/graphics/context/window_surface.cpp
        src/xar_engine/graphics/context/window_surface.hpp

        # graphics context vulkan
        src/xar_engine/graphics/context/vulkan/vulkan_window_surface.cpp
        src/xar_engine/graphics/context/vulkan/vulkan_window_surface.hpp

        # graphics native vulkan
        src/xar_engine/graphics/native/vulkan/vulkan_buffer.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_buffer.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_command_buffer.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_command_buffer.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_command_buffer_pool.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_command_buffer_pool.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_descriptor_pool.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_descriptor_pool.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_descriptor_set.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_descriptor_set.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_descriptor_set_layout.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_descriptor_set_layout.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_device.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_device.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_graphics_pipeline.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_graphics_pipeline.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_image.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_image.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_image_view.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_image_view.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_instance.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_instance.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_physical_device.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_physical_device.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_queue.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_queue.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_sampler.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_sampler.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_shader.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_shader.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_surface.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_surface.hpp
        src/xar_engine/graphics/native/vulkan/vulkan_swap_chain.cpp
        src/xar_engine/graphics/native/vulkan/vulkan_swap_chain.hpp

        # input
        src/xar_engine/input/button.cpp
        src/xar_engine/input/input_event.cpp

        # logging
        src/xar_engine/logging/console_logger.cpp
        src/xar_engine/logging/default_log_formatter.cpp
        src/xar_engine/logging/file_logger.cpp
        src/xar_engine/logging/log_formatter.cpp
        src/xar_engine/logging/log_level.cpp
        src/xar_engine/logging/logger.cpp
        src/xar_engine/logging/logger_chain.cpp
        src/xar_engine/logging/stream_logger.cpp

        # math
        src/xar_engine/math/epsilon.cpp
        src/xar_engine/math/matrix.cpp

        # meta
        src/xar_engine/meta/enum_impl.hpp
        src/xar_engine/meta/for_each.hpp
        src/xar_engine/meta/ref_counting_singleton.cpp
        src/xar_engine/meta/ref_counting_singleton.hpp
        src/xar_engine/meta/resource_map.hpp
        src/xar_engine/meta/singleton.hpp

        # os
        src/xar_engine/os/application.cpp
        src/xar_engine/os/glfw_application.cpp
        src/xar_engine/os/glfw_application.hpp
        src/xar_engine/os/glfw_window.cpp
        src/xar_engine/os/glfw_window.hpp
        src/xar_engine/os/service_register.cpp
        src/xar_engine/os/service_register_impl.cpp
        src/xar_engine/os/service_register_impl.hpp
        src/xar_engine/os/window.cpp

        # renderer gpu_asset
        src/xar_engine/renderer/gpu_asset/gpu_model_data.cpp
        src/xar_engine/renderer/gpu_asset/gpu_model_data.hpp
        src/xar_engine/renderer/gpu_asset/gpu_model_data_buffer.cpp
        src/xar_engine/renderer/gpu_asset/gpu_model_data_buffer.hpp

        # renderer
        src/xar_engine/renderer/renderer.cpp
        src/xar_engine/renderer/renderer_impl.cpp
        src/xar_engine/renderer/renderer_impl.hpp

        # time
        src/xar_engine/time/time.cpp
        src/xar_engine/time/time.hpp

        # version
        src/xar_engine/version/version.cpp)


##############################
# Interface
##############################
add_library(xar_engine_interface INTERFACE)

target_include_directories(xar_engine_interface
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/src)


##############################
# Implementation
##############################
add_library(xar_engine SHARED)

target_sources(xar_engine
        PRIVATE
            ${XAR_ENGINE_PUBLIC_FILES}
            ${XAR_ENGINE_PRIVATE_FILES})

target_include_directories(xar_engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(xar_engine
        PUBLIC
            fmt::fmt
        PRIVATE
            assimp::assimp
            glfw
            glm::glm
            stb::stb
            volk::volk)


##############################
# Tests
##############################
add_subdirectory(test)