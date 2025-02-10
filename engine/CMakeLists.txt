find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)

add_library(xar_engine_impl STATIC)

target_sources(xar_engine_impl
        PUBLIC
            src/xar_engine/error/exception.cpp
            src/xar_engine/error/exception_utils.hpp
            src/xar_engine/os/application.cpp
            src/xar_engine/os/glfw_application.cpp
            src/xar_engine/os/glfw_application.hpp
            src/xar_engine/os/glfw_window.cpp
            src/xar_engine/os/glfw_window.hpp
            src/xar_engine/os/input_event.cpp
            src/xar_engine/os/window.cpp
            src/xar_engine/version/version.cpp)

target_include_directories(xar_engine_impl
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(xar_engine_impl
        PUBLIC
            fmt::fmt
            glfw)


add_library(xar_engine SHARED)

target_sources(xar_engine
        PUBLIC
            include/xar_engine/error/exception.hpp
            include/xar_engine/meta/overloaded.hpp
            include/xar_engine/os/application.hpp
            include/xar_engine/os/button.hpp
            include/xar_engine/os/input_event.hpp
            include/xar_engine/os/window.hpp
            include/xar_engine/version/version.hpp)

target_include_directories(xar_engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(xar_engine
    PRIVATE
        xar_engine_impl)

add_subdirectory(test)