add_executable(SimpleClient)

target_sources(SimpleClient
  PRIVATE
    src/SimpleClient.cpp
)

target_include_directories(SimpleClient
  PRIVATE
    ${CMAKE_SOURCE_DIR}/build/${LOCAL_BUILD_TYPE_LOWER}/_deps/asio-src/asio/include
)

target_link_libraries(SimpleClient
  PRIVATE
    NetCommon)

target_compile_features(SimpleClient
PRIVATE
    cxx_std_20
)

set_target_properties(SimpleClient
  PROPERTIES
    CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
)
target_compile_options(SimpleClient
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)