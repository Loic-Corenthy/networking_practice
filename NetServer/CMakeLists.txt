add_executable(SimpleServer)

target_sources(SimpleServer
  PRIVATE
    src/SimpleServer.cpp
)

target_include_directories(SimpleServer
  PRIVATE
    ${CMAKE_SOURCE_DIR}/build/${LOCAL_BUILD_TYPE_LOWER}/_deps/asio-src/asio/include
)

target_link_libraries(SimpleServer
  PRIVATE
    NetCommon)

target_compile_features(SimpleServer
PRIVATE
    cxx_std_20
)

set_target_properties(SimpleServer
  PROPERTIES
    CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
)
target_compile_options(SimpleServer
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)