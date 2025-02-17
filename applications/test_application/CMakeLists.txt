add_executable(xar_engine_test_application)

target_sources(xar_engine_test_application
        PRIVATE
            main.cpp)

target_link_libraries(xar_engine_test_application
        PRIVATE
            xar_engine)

add_custom_command(TARGET xar_engine_test_application
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:xar_engine>
            $<TARGET_FILE_DIR:xar_engine_test_application>)

add_custom_command(TARGET xar_engine_test_application
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:xar_engine_test_application>/assets)
