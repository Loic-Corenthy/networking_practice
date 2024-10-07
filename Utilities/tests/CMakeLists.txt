add_executable(testTSQueue)

target_sources(testTSQueue
    PRIVATE
        "TestTSQueue.cpp"
)

add_test(NAME "Test queue1" COMMAND "$<TARGET_FILE:testTSQueue>" "[queue]")

####################################
# Setup common to all test targets #
####################################
set(ALL_TEST_TARGETS testTSQueue)

foreach(TEST_TARGET IN LISTS ALL_TEST_TARGETS)
    target_link_libraries(${TEST_TARGET} PRIVATE Utilities Catch2::Catch2WithMain)

    target_compile_features(${TEST_TARGET} PUBLIC cxx_std_20)

    set_target_properties(${TEST_TARGET} PROPERTIES LINKER_LANGUAGE CXX)
    set_target_properties(${TEST_TARGET} PROPERTIES CXX_EXTENSIONS OFF)

    target_compile_options(${TEST_TARGET}
        PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
endforeach()