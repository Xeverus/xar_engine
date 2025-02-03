add_library(xar_engine SHARED
        src/library.cpp)

target_include_directories(xar_engine
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include)