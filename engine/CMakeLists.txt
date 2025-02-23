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
        include/xar_engine/asset/image.hpp
        include/xar_engine/asset/image_loader.hpp
        include/xar_engine/asset/model.hpp
        include/xar_engine/asset/model_loader.hpp
        include/xar_engine/error/exception.hpp
        include/xar_engine/graphics/renderer.hpp
        include/xar_engine/graphics/renderer_type.hpp
        include/xar_engine/input/button.hpp
        include/xar_engine/input/input_event.hpp
        include/xar_engine/math/epsilon.hpp
        include/xar_engine/math/vector.hpp
        include/xar_engine/meta/enum.hpp
        include/xar_engine/meta/overloaded.hpp
        include/xar_engine/logging/console_logger.hpp
        include/xar_engine/logging/default_log_formatter.hpp
        include/xar_engine/logging/file_logger.hpp
        include/xar_engine/logging/log_formatter.hpp
        include/xar_engine/logging/log_level.hpp
        include/xar_engine/logging/logger.hpp
        include/xar_engine/logging/logger_chain.hpp
        include/xar_engine/logging/stream_logger.hpp
        include/xar_engine/os/application.hpp
        include/xar_engine/os/window.hpp
        include/xar_engine/version/version.hpp)

set(XAR_ENGINE_PRIVATE_FILES
        src/xar_engine/asset/assimp_model_loader.cpp
        src/xar_engine/asset/assimp_model_loader.hpp
        src/xar_engine/asset/image.cpp
        src/xar_engine/asset/image_loader.cpp
        src/xar_engine/asset/model_loader.cpp
        src/xar_engine/asset/stb_image_loader.cpp
        src/xar_engine/asset/stb_image_loader.hpp
        src/xar_engine/error/exception.cpp
        src/xar_engine/error/exception_utils.hpp
        src/xar_engine/file/file.cpp
        src/xar_engine/file/file.hpp
        src/xar_engine/graphics/renderer_type.cpp
        src/xar_engine/graphics/renderer.cpp
        src/xar_engine/graphics/vulkan/vulkan_buffer.cpp
        src/xar_engine/graphics/vulkan/vulkan_buffer.hpp
        src/xar_engine/graphics/vulkan/vulkan_command_buffer_pool.cpp
        src/xar_engine/graphics/vulkan/vulkan_command_buffer_pool.hpp
        src/xar_engine/graphics/vulkan/vulkan_descriptor_pool.cpp
        src/xar_engine/graphics/vulkan/vulkan_descriptor_pool.hpp
        src/xar_engine/graphics/vulkan/vulkan_descriptor_set_layout.cpp
        src/xar_engine/graphics/vulkan/vulkan_descriptor_set_layout.hpp
        src/xar_engine/graphics/vulkan/vulkan_device.cpp
        src/xar_engine/graphics/vulkan/vulkan_device.hpp
        src/xar_engine/graphics/vulkan/vulkan_graphics_pipeline.cpp
        src/xar_engine/graphics/vulkan/vulkan_graphics_pipeline.hpp
        src/xar_engine/graphics/vulkan/vulkan_image.cpp
        src/xar_engine/graphics/vulkan/vulkan_image.hpp
        src/xar_engine/graphics/vulkan/vulkan_image_view.cpp
        src/xar_engine/graphics/vulkan/vulkan_image_view.hpp
        src/xar_engine/graphics/vulkan/vulkan_instance.cpp
        src/xar_engine/graphics/vulkan/vulkan_instance.hpp
        src/xar_engine/graphics/vulkan/vulkan_physical_device_list.cpp
        src/xar_engine/graphics/vulkan/vulkan_physical_device_list.hpp
        src/xar_engine/graphics/vulkan/vulkan_renderer.cpp
        src/xar_engine/graphics/vulkan/vulkan_renderer.hpp
        src/xar_engine/graphics/vulkan/vulkan_sampler.cpp
        src/xar_engine/graphics/vulkan/vulkan_sampler.hpp
        src/xar_engine/graphics/vulkan/vulkan_shader.cpp
        src/xar_engine/graphics/vulkan/vulkan_shader.hpp
        src/xar_engine/graphics/vulkan/vulkan_surface.cpp
        src/xar_engine/graphics/vulkan/vulkan_surface.hpp
        src/xar_engine/graphics/vulkan/vulkan_swap_chain.cpp
        src/xar_engine/graphics/vulkan/vulkan_swap_chain.hpp
        src/xar_engine/input/button.cpp
        src/xar_engine/input/input_event.cpp
        src/xar_engine/logging/console_logger.cpp
        src/xar_engine/logging/default_log_formatter.cpp
        src/xar_engine/logging/file_logger.cpp
        src/xar_engine/logging/log_formatter.cpp
        src/xar_engine/logging/log_level.cpp
        src/xar_engine/logging/logger.cpp
        src/xar_engine/logging/logger_chain.cpp
        src/xar_engine/logging/stream_logger.cpp
        src/xar_engine/math/epsilon.cpp
        src/xar_engine/meta/enum_impl.hpp
        src/xar_engine/meta/for_each.hpp
        src/xar_engine/meta/singleton.hpp
        src/xar_engine/os/application.cpp
        src/xar_engine/os/glfw_application.cpp
        src/xar_engine/os/glfw_application.hpp
        src/xar_engine/os/glfw_window.cpp
        src/xar_engine/os/glfw_window.hpp
        src/xar_engine/os/window.cpp
        src/xar_engine/time/time.cpp
        src/xar_engine/time/time.hpp
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