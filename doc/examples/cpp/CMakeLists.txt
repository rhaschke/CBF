message(STATUS "In subdirectory doc/examples/cpp:")

if(CBF_HAVE_XSD)
  include_directories(${XSD_INC})
endif()

if(CBF_HAVE_MEMORY)
  include_directories(${MEMORY_INCLUDE_DIRS})
endif()

if(CBF_HAVE_KDL)
  include_directories(AFTER ${KDL_INCLUDE_DIRS})
endif()

if(CBF_HAVE_SPACEMOUSE)
  include_directories(${SPACEMOUSE_INCLUDES})
endif()

set(exe cbf_simple_test)
message(STATUS "  adding executable ${exe}")
add_executable(${exe} ${exe}.cc)
target_link_libraries(${exe} ${CBF_LIBRARY_NAME})
add_dependencies(${exe} ${CBF_LIBRARY_NAME})

set(exe cbf_simple_test_memory)
if(CBF_HAVE_MEMORY AND CBF_HAVE_XSD)
  message(STATUS "  adding executable ${exe}")
  add_executable(${exe} ${exe}.cc)
  target_link_libraries(${exe} ${CBF_LIBRARY_NAME} ${MEMORY_LDFLAGS})
  add_dependencies(${exe} ${CBF_LIBRARY_NAME})
else()
  message(STATUS "  not adding executable ${exe}")
  message(STATUS "  because Memory was not found")
endif()

set(exe_list 
  cbf_test_kdl_transforms 
  cbf_test_axis_potential
  cbf_test_axis_potential2)
if(CBF_HAVE_KDL)
  foreach(exe ${exe_list})
	 message(STATUS "  adding executable ${exe}")
	 add_executable(${exe} ${exe}.cc)
	 target_link_libraries(${exe} ${CBF_LIBRARY_NAME})
	 add_dependencies(${exe} ${CBF_LIBRARY_NAME})
  endforeach(exe)
else()
  message(STATUS "  not adding executables:")
  foreach(exe ${exe_list})
	 message(STATUS "  -${exe}")
  endforeach(exe)
  message(STATUS "  because KDL or Eigen2 was not found")
endif()

set(exe cbf_test_kdl_transforms)
if(CBF_HAVE_KDL AND CBF_HAVE_QKDLVIEW)
  target_link_libraries(${exe}
	 ${CBF_LIBRARY_NAME}
	 ${QKDLVIEW_LIB}
	 ${QT_LIBS}
	 )
  include_directories(${QKDLVIEW_INCLUDES} ${QT_INCLUDES})
endif()
