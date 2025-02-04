add_library(xar_engine SHARED)

find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)

target_sources(xar_engine
        PRIVATE
            include/xar_engine/error/exception.hpp
            include/xar_engine/os/application.hpp
            include/xar_engine/os/window.hpp
            include/xar_engine/version/version.hpp

            src/xar_engine/error/exception.cpp
            src/xar_engine/error/exception_utils.hpp
            src/xar_engine/os/application.cpp
            src/xar_engine/os/glfw_application.cpp
            src/xar_engine/os/glfw_application.hpp
            src/xar_engine/os/window.cpp
            src/xar_engine/version/version.cpp)

target_include_directories(xar_engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC # TODO: change to private
            ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(xar_engine
        PUBLIC # TODO: change to private
            fmt::fmt
            glfw)

add_subdirectory(test)