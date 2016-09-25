cmake_minimum_required(VERSION 2.6)
project(diagonalsw CXX)

SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "9")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
set(PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})


set(CPACK_GENERATOR RPM;DEB)
#set(CPACK_DEBIAN_PACKAGE_DEPENDS "libxml2 (>= 2.6.16 )")

set(CPACK_SOURCE_GENERATOR TGZ;ZIP)
# set(CPACK_RPM_PACKAGE_ARCHITECTURE "i386")
set(CPACK_PACKAGE_CONTACT "erik.sjolund@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An efficient implementation of the Smith-Waterman algorithm that takes advantage of SIMD instruction sets in modern CPUs. The Smith-Waterman algorithm is used for sequence alignment in bioinformatics.")
set(CPACK_PACKAGE_VENDOR "")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${PACKAGE_VERSION}")
include(CPack)


macro(my_find_program progname)
  find_program(${progname}_EXECUTABLE ${progname})
  if(NOT ${progname}_EXECUTABLE)
    message(FATAL_ERROR  "${progname} not found" )
  endif(NOT ${progname}_EXECUTABLE)
endmacro(my_find_program)

my_find_program(xsltproc)
my_find_program(gengetopt)

add_subdirectory(src/c)
add_subdirectory(src/docbook)


