add_library(NetCommon INTERFACE)

target_sources(NetCommon
  INTERFACE
    include/NetCommon.hpp
    include/NetMessage.hpp
)

target_include_directories(NetCommon
  INTERFACE
    include/
    ${CMAKE_SOURCE_DIR}/build/${LOCAL_BUILD_TYPE_LOWER}/_deps/asio-src/asio/include
)

target_compile_features(NetCommon
  INTERFACE
    cxx_std_20
)

set_target_properties(NetCommon
  PROPERTIES
    CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
)
target_compile_options(NetCommon
  INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)