foreach(TARGET_NAME boost_tutorial1 boost_tutorial2 boost_tutorial3)
  add_executable(${TARGET_NAME})

  string(SUBSTRING ${TARGET_NAME} 6 9 FILE_NAME)

  target_sources(${TARGET_NAME}
    PRIVATE
      src/${FILE_NAME}.cpp
  )

  default_configuration(${TARGET_NAME})
endforeach()

