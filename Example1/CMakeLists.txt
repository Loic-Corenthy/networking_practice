add_executable(example1)

target_sources(example1
  PRIVATE
    src/main.cpp
)

target_include_directories(example1
  PRIVATE
    ${CMAKE_SOURCE_DIR}/build/${LOCAL_BUILD_TYPE_LOWER}/_deps/asio-src/asio/include
)

target_link_libraries(example1
  PRIVATE
  NetCommon
)

target_compile_features(example1
  PRIVATE
    cxx_std_20
)

set_target_properties(example1
  PROPERTIES
    CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
)
target_compile_options(example1
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)