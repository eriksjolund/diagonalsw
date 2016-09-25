if(BUILD_DOCUMENTATION)
  find_program(XMLTO xmlto)
  include(FindPythonInterp)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/index.xml.cmake ${CMAKE_CURRENT_BINARY_DIR}/index.xml.before-preprocess)
  get_target_property(DIAGONALSW_EXE diagonalsw LOCATION)

  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/index.xml COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/preprocess.py  ${CMAKE_BINARY_DIR}/src/c ${CMAKE_CURRENT_BINARY_DIR}/index.xml.before-preprocess ${CMAKE_CURRENT_BINARY_DIR}/index.xml DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/preprocess.py ${CMAKE_CURRENT_BINARY_DIR}/index.xml.before-preprocess  diagonalsw WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/example_files )

  add_custom_target(docbook-doc ALL ${XMLTO} --skip-validation -m ${CMAKE_CURRENT_SOURCE_DIR}/params.xsl xhtml-nochunks -o ${CMAKE_CURRENT_BINARY_DIR}  ${CMAKE_CURRENT_BINARY_DIR}/index.xml     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/params.xsl ${CMAKE_CURRENT_BINARY_DIR}/index.xml )


  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/style DESTINATION share/doc/diagonalsw PATTERN ".svn" EXCLUDE )
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/example_files DESTINATION share/doc/diagonalsw PATTERN ".svn" EXCLUDE )
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/index.html DESTINATION share/doc/diagonalsw )
endif(BUILD_DOCUMENTATION)
