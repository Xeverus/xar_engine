add_executable(xar_engine_test_unit)

target_sources(xar_engine_test_unit
        PRIVATE
            xar_engine/algorithm/interval_container_test.cpp
            xar_engine/asset/image_loader_test.cpp
            xar_engine/asset/model_loader_test.cpp
            xar_engine/error/exception_utils_test.cpp
            xar_engine/logging/file_logger_test.cpp
            xar_engine/logging/logger_chain_test.cpp
            xar_engine/logging/logging_macros_test.cpp
            xar_engine/logging/stream_logger_test.cpp
            xar_engine/math/epsilon_test.cpp
            xar_engine/meta/enum_test.cpp
            xar_engine/meta/ref_counting_singleton_test.cpp
            xar_engine/os/application_lifecycle_test.cpp
            xar_engine/os/window_input_test.cpp
            xar_engine/version/version_test.cpp)

target_link_libraries(xar_engine_test_unit
        PRIVATE
            xar_engine_test_common
            GTest::gtest_main)

add_custom_command(TARGET xar_engine_test_unit
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:xar_engine>
            $<TARGET_FILE_DIR:xar_engine_test_unit>)

add_custom_command(TARGET xar_engine_test_unit
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/resources
            $<TARGET_FILE_DIR:xar_engine_test_unit>/resources)