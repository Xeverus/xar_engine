add_library(xar_engine SHARED)

target_sources(xar_engine
        PRIVATE
            src/xar_engine/version/version.cpp)

target_include_directories(xar_engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(test)