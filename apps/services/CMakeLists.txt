###############################################################################
# Sources:
###############################################################################
SET(PlatoMain_SRCS Plato_Main.cpp PlatoApp.cpp)

SET(PlatoMain_HDRS PlatoApp.hpp)

include_directories(${PLATO_INCLUDES})

add_compile_definitions(${Dakota_DEFINES})

# actual target:
set(PLATOMAIN_LIBS  ${PLATO_LIBRARIES} ${PLATO_LIBRARIES}  
     ${Trilinos_LIBRARIES} ${Trilinos_TPL_LIBRARIES}
     ${Plato_EXTRA_LINK_FLAGS} Boost::serialization)
add_executable(PlatoMain ${PlatoMain_SRCS})
target_link_libraries(PlatoMain ${PLATOMAIN_LIBS})

set(PLATOMAIN_BINARY ${CMAKE_BINARY_DIR}/apps/services/PlatoMain PARENT_SCOPE)

if( CMAKE_INSTALL_PREFIX )
  install( TARGETS PlatoMain DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
endif()

if( ENABLE_PLATO_SERVICES )
    SET(PlatoEngineServices_SRCS Plato_Services.cpp PlatoApp.cpp)
    add_executable(PlatoEngineServices ${PlatoEngineServices_SRCS})
  target_link_libraries(PlatoEngineServices ${PLATOMAIN_LIBS})
  set(SERVICES_BINARY ${CMAKE_BINARY_DIR}/apps/services/PlatoEngineServices PARENT_SCOPE)
  if( CMAKE_INSTALL_PREFIX )
      install( TARGETS PlatoEngineServices DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
  endif()
endif()

if (PLATO_ENABLE_SERVICES_PYTHON)
    find_package(Python3 COMPONENTS Development Interpreter)
    add_library( PlatoServices SHARED PlatoApp.cpp ServicesPythonModule.cpp )
    target_include_directories(PlatoServices SYSTEM PRIVATE ${Python3_INCLUDE_DIRS} )
    target_link_libraries( PlatoServices ${PLATO_LIBRARIES} ${PLATO_LIBRARIES} ${Trilinos_LIBRARIES} 
                           ${Python3_LIBRARIES} Boost::serialization)
    set_target_properties( PlatoServices PROPERTIES PREFIX "" )
    target_compile_options( PlatoServices PRIVATE "-lmpi" )

    if( CMAKE_INSTALL_PREFIX )
      install( TARGETS PlatoServices DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
    endif()

endif()


###############################################################################
###############################################################################

add_library(PlatoApp PlatoApp.cpp)

if( UNIT_TESTING )
  message( "-- Enabling unit testing " )
add_subdirectory(./unittest)
endif()
