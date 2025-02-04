add_library(xar_engine SHARED)

find_package(glfw3 REQUIRED)

target_sources(xar_engine
        PRIVATE
            include/xar_engine/os/application.hpp
            include/xar_engine/os/window.hpp
            include/xar_engine/version/version.hpp

            src/xar_engine/os/application.cpp
            src/xar_engine/os/glfw_application.cpp
            src/xar_engine/os/glfw_application.hpp
            src/xar_engine/os/window.cpp
            src/xar_engine/version/version.cpp)

target_include_directories(xar_engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(xar_engine
        PRIVATE
            glfw)

add_subdirectory(test)