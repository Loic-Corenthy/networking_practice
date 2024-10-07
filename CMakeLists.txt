cmake_minimum_required(VERSION 3.25.0)

project(learning_asio
  VERSION
    0.1.0
  LANGUAGES
    CXX
)

Include(FetchContent)

FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG        asio-1-31-0
)

FetchContent_MakeAvailable(asio)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/_deps/asio-src")

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.0
)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Generate a lowercase variable for build type
string(TOLOWER ${CMAKE_BUILD_TYPE} LOCAL_BUILD_TYPE_LOWER)

# Make CTest available for targets
include(CTest)

add_subdirectory(Example1)
add_subdirectory(NetCommon)
add_subdirectory(NetClient)
add_subdirectory(NetServer)
add_subdirectory(Utilities)
