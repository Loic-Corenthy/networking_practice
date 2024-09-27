add_executable(NetClient)

target_sources(NetClient
  PRIVATE
    src/SimpleClient.cpp
)

target_include_directories(NetClient
  PRIVATE
    ${CMAKE_SOURCE_DIR}/build/${LOCAL_BUILD_TYPE_LOWER}/_deps/asio-src/asio/include
)

target_link_libraries(NetClient
  PRIVATE
    NetCommon)

target_compile_features(NetClient
PRIVATE
    cxx_std_20
)

set_target_properties(NetClient
  PROPERTIES
    CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
)
target_compile_options(NetClient
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)