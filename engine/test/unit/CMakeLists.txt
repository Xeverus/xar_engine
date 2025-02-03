add_executable(xar_engine_unit_tests)

target_sources(xar_engine_unit_tests
        PRIVATE
            xar_engine/version/version_test.cpp)

target_link_libraries(xar_engine_unit_tests
        PRIVATE
            GTest::gtest
            GTest::gtest_main
            xar_engine)

add_custom_command(TARGET xar_engine_unit_tests
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:xar_engine>
            $<TARGET_FILE_DIR:xar_engine_unit_tests>)