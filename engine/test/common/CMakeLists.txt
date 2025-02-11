add_library(xar_engine_test_common)

target_sources(xar_engine_test_common
        PUBLIC
            include/xar_engine/logging/log_formatter_mock.hpp
            include/xar_engine/logging/logger_mock.hpp
        PUBLIC
            src/dummy.cpp)

target_include_directories(xar_engine_test_common
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(xar_engine_test_common
        PUBLIC
            GTest::gmock
            GTest::gtest
            xar_engine_impl)
