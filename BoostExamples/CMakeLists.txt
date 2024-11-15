foreach(TARGET_NAME boost_example1 boost_example2)
  add_executable(${TARGET_NAME})

  string(SUBSTRING ${TARGET_NAME} 6 8 FILE_NAME)

  target_sources(${TARGET_NAME}
    PRIVATE
      src/${FILE_NAME}.cpp
  )

  default_configuration(${TARGET_NAME})
endforeach()

