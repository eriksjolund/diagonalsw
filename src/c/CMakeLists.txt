set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/FindTBB/")
find_package(TBB REQUIRED)
include_directories(${TBB_INCLUDE_DIRS})
link_directories(${TBB_LIBRARY_DIRS})




include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#set(TARGETS 1load_HFsave16th_var2 1load_HFsave16th 1load 2load_HFsave16th 2load_HFsave8th 2load)


#set(BYTE_TARGETS 1load 2load_HFsave8th 2load)
#set(WORD_TARGETS 2load 1load_HFsave4th)
set(BYTE_TARGETS 2load_HFsave8th 2load_HFsave16th 2load 1load)
#set(WORD_TARGETS 1load 2load 1load_HFsave4th)
set(WORD_TARGETS 2load)
#set(TARGETS 2load_HFsave8th )


foreach(i ${BYTE_TARGETS})
 list(APPEND xmlList smith_waterman_sse_byte.${i}  )
endforeach(i)

foreach(i ${WORD_TARGETS})
 list(APPEND xmlList smith_waterman_sse_word.${i} )
endforeach(i)

 set(variants normal H EFH)

foreach(i ${xmlList})
 foreach(variant ${variants})
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${i}${variant}.c COMMAND ${xsltproc_EXECUTABLE} --stringparam variant ${variant} ${CMAKE_CURRENT_SOURCE_DIR}/generate_c.xsl ${CMAKE_CURRENT_SOURCE_DIR}/${i}.xml > ${CMAKE_CURRENT_BINARY_DIR}/${i}${variant}.c DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/generate_c.xsl ${CMAKE_CURRENT_SOURCE_DIR}/${i}.xml )
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/${i}${variant}.c PROPERTIES GENERATED true)
 endforeach(variant)
endforeach(i)


set(PROGRAMS diagonalsw) 

foreach(i ${PROGRAMS} )
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${i}.ggo.cmake ${CMAKE_CURRENT_BINARY_DIR}/${i}.ggo @ONLY)
  add_custom_command(OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/cmdline_${i}.c   ${CMAKE_CURRENT_BINARY_DIR}/cmdline_${i}.h 
                     COMMAND ${gengetopt_EXECUTABLE} -i ${CMAKE_CURRENT_BINARY_DIR}/${i}.ggo --file-name=cmdline_${i} --output-dir=${CMAKE_CURRENT_BINARY_DIR}
                     DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/${i}.ggo  
                     VERBATIM)
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/cmdline_${i}.c PROPERTIES GENERATED true)
  set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/cmdline_${i}.h PROPERTIES GENERATED true)
endforeach(i)


if(CMAKE_C_COMPILER MATCHES "gcc")
 set(COMPFLAGSLIST "-msse4.1  -O3 -g -p" "-msse4.1 -O3") 
 set(BESTCOMPFLAGS "-msse4.1 -O3") 
endif(CMAKE_C_COMPILER MATCHES "gcc")



if(CMAKE_COMPILER_IS_GNUCXX)
 set(COMPFLAGSLIST "-msse4.1  -O3 -g -p" "-msse4.1 -O3") 
 set(BESTCOMPFLAGS "-msse4.1 -O3") 
endif(CMAKE_COMPILER_IS_GNUCXX)

if(CMAKE_C_COMPILER MATCHES "icc")
 set(COMPFLAGSLIST "-O3") 
 set(BESTCOMPFLAGS "-O3") 
endif(CMAKE_C_COMPILER MATCHES "icc")

set(c_files read_matrix.c read_sequence.c read_profile.c  cmdline_diagonalsw.c create_profile.c smith_waterman_reference_impl.c)


# just for testing different flags and implementation variants of smith_waterman_sse_*.xml for benchmarking

if(COMPARE_DIFFERENT_APPROACHES)
 foreach(k ${COMPFLAGSLIST} )
  string(REPLACE " " "_" k2 ${k} )   
  foreach(n ${WORD_TARGETS} ) 
   foreach(j ${BYTE_TARGETS} )
    set(name diagonalsw_BYTE${j}_WORD${n}_${k2})
    set(sw_src_list)
    foreach(variant ${variants} )
     list(APPEND sw_src_list ${CMAKE_CURRENT_BINARY_DIR}/smith_waterman_sse_byte.${j}${variant}.c  ${CMAKE_CURRENT_BINARY_DIR}/smith_waterman_sse_word.${n}${variant}.c)
    endforeach(variant)
    add_executable(${name} ${c_files} ${sw_src_list} diagonalsw.c sse_funcs.c  )
    set_target_properties(${name} PROPERTIES COMPILE_FLAGS "${k} -DTARGET_BYTE_${j} -DTARGET_WORD_${n}")
   endforeach(j)
  endforeach(n)
 endforeach(k)
endif(COMPARE_DIFFERENT_APPROACHES)

# The "official" diagonalsw binary:

set(best_byte_target 2load_HFsave8th)
set(best_word_target 2load)
set(best_compflags "-O3 ")
set(sw_src_list)
foreach(variant ${variants} )
  list(APPEND sw_src_list ${CMAKE_CURRENT_BINARY_DIR}/smith_waterman_sse_byte.${best_byte_target}${variant}.c  ${CMAKE_CURRENT_BINARY_DIR}/smith_waterman_sse_word.${best_word_target}${variant}.c)
endforeach(variant)
foreach(sfile ${c_files} ${sw_src_list}  sse_funcs.c  )
 set_source_files_properties(${sfile} PROPERTIES LANGUAGE CXX) 
 message(STATUS  "${sfile} " )
endforeach(sfile)

  set_source_files_properties(diagonalsw.c PROPERTIES LANGUAGE CXX) 

add_executable(diagonalsw ${c_files} ${sw_src_list} diagonalsw.c sse_funcs.c  )
target_link_libraries(diagonalsw tbbmalloc tbb )


set_target_properties(diagonalsw PROPERTIES COMPILE_FLAGS "${BESTCOMPFLAGS} -DTARGET_BYTE_${best_byte_target} -DTARGET_WORD_${best_word_target}")

install(TARGETS diagonalsw DESTINATION bin)
