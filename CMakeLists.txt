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

set(ASIO_CMAKE_ASIO_TAG asio-1-31-0)
FetchContent_MakeAvailable(asio)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/_deps/asio-src")

string(TOLOWER ${CMAKE_BUILD_TYPE} LOCAL_BUILD_TYPE_LOWER)

add_subdirectory(Example1)
add_subdirectory(NetCommon)
add_subdirectory(NetClient)
