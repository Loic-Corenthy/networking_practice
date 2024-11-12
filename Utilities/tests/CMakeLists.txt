add_executable(testTSQueue1)

target_sources(testTSQueue1
    PRIVATE
        src/Helper.hpp
        src/BenchmarkFixture.hpp
        src/TestTSQueue1.cpp
        src/BenchmarkTSQueue1.cpp
)

add_test(NAME "Test queue 1" COMMAND "$<TARGET_FILE:testTSQueue1>" "[test]")
add_test(NAME "Benchmark queue 1" COMMAND "$<TARGET_FILE:testTSQueue1>" "[benchmark]")

add_executable(testTSQueue2)

target_sources(testTSQueue2
    PRIVATE
        src/Helper.hpp
        src/BenchmarkFixture.hpp
        src/TestTSQueue2.cpp
        src/BenchmarkTSQueue2.cpp
)

add_test(NAME "Test queue 2" COMMAND "$<TARGET_FILE:testTSQueue2>" "[test]")
add_test(NAME "Benchmark queue 2" COMMAND "$<TARGET_FILE:testTSQueue2>" "[benchmark]")


add_executable(testTSLookupTable1)

target_sources(testTSLookupTable1
    PRIVATE
        src/Helper.hpp
        src/TestTSLookupTable1.cpp
)

add_test(NAME "Test lookup table 1" COMMAND "$<TARGET_FILE:testTSLookupTable1>" "[test]")


add_executable(testTSList1)

target_sources(testTSList1
    PRIVATE
        src/Helper.hpp
        src/BenchmarkFixture.hpp
        src/TestTSList1.cpp
        src/BenchmarkTSList1.cpp

)

add_test(NAME "Test list 1" COMMAND "$<TARGET_FILE:testTSList1>" "[test]")
add_test(NAME "Benchmark list 1" COMMAND "$<TARGET_FILE:testTSList1>" "[benchmark]")


####################################
# Setup common to all test targets #
####################################
set(ALL_TEST_TARGETS testTSQueue1 testTSQueue2 testTSLookupTable1 testTSList1)

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
