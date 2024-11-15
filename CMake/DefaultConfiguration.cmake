function(default_configuration TARGET_NAME)
    target_include_directories(${TARGET_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/build/${LOCAL_BUILD_TYPE_LOWER}/_deps/asio-src/asio/include
    )

    target_compile_features(${TARGET_NAME}
    PRIVATE
        cxx_std_20
    )

    set_target_properties(${TARGET_NAME}
    PROPERTIES
        CXX_EXTENSIONS OFF
        LINKER_LANGUAGE CXX
    )

    target_compile_options(${TARGET_NAME}
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
endfunction()
