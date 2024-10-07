add_library(Utilities INTERFACE)

target_sources(Utilities
  INTERFACE
    include/TSQueue1.hpp
    include/TSQueue2.hpp
)

target_include_directories(Utilities
  INTERFACE
    include/
)

target_compile_features(Utilities
  INTERFACE
    cxx_std_20
)

set_target_properties(Utilities
  PROPERTIES
    CXX_EXTENSIONS OFF
    LINKER_LANGUAGE CXX
)
target_compile_options(Utilities
  INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_subdirectory(tests)