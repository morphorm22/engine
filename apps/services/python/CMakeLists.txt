###################################################################
## Python setup                                                  ##
###################################################################
find_package(Python3 COMPONENTS Development Interpreter)

###################################################################
## Exodus setup                                                  ##
###################################################################
Plato_find_lib( EXODUS_LIB_PATH ON exodus ${SEACAS_PATH}/lib )


###################################################################
## Expy setup                                                    ##
###################################################################
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

# Sources:
SET(library_SRCS expy/expymodule.c)

set_source_files_properties( ${library_SRCS} PROPERTIES COMPILE_FLAGS "-Wall -fPIC" )

if( CMAKE_BUILD_TYPE MATCHES Release )
  set_source_files_properties( ${library_SRCS} 
                               PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -O3" )
endif()

if( CMAKE_BUILD_TYPE MATCHES Debug )
  set_source_files_properties( ${library_SRCS} 
                               PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}" )
endif()

# actual target:
add_library( expy SHARED ${library_SRCS} )
set_target_properties( expy PROPERTIES PREFIX "" )

#add libraries:
target_link_libraries( expy ${Python3_LIBRARIES} ${EXODUS_LIB_PATH} )
target_include_directories(expy PRIVATE ${Python3_INCLUDE_DIRS} )

if( CMAKE_INSTALL_PREFIX )
  install( TARGETS expy DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
  install( FILES exodus.py DESTINATION ${CMAKE_INSTALL_PREFIX}/etc )
endif()

