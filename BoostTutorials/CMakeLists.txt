file(GLOB
  ALL_SOURCE_FILE_NAMES 
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/src
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

foreach(SOURCE_FILE_NAME ${ALL_SOURCE_FILE_NAMES})
  # TODO: Replace the 9 with length - 4
  string(SUBSTRING ${SOURCE_FILE_NAME} 0 9 SOURCE_FILE_NAME)
  set(TARGET_NAME "boost_${SOURCE_FILE_NAME}")

  add_executable(${TARGET_NAME})

  target_sources(${TARGET_NAME}
    PRIVATE
      src/${SOURCE_FILE_NAME}.cpp
  )

  default_configuration(${TARGET_NAME})
endforeach()

