############ Project setup ##################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.5.0)
#############################################################################

include( ${CMAKE_SOURCE_DIR}/base/config/tools.cmake )
Plato_no_src_build()

# Advertise that we support shared libs (cmake squawks otherwise)
SET_PROPERTY(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)

##################################################################
## Load config.cmake                                            ##
##################################################################
if( DEFINED CONFIG )

  message(STATUS "-DCONFIG specified: " ${CONFIG})
  if( EXISTS ${CMAKE_BINARY_DIR}/${CONFIG} )
   include( ${CMAKE_BINARY_DIR}/${CONFIG} )
  else()
    message(FATAL_ERROR "The configuration file '${CONFIG}' specified with -DCONFIG doesn't exist")
  endif()

else()

  if( EXISTS ${CMAKE_BINARY_DIR}/config.cmake )
   include( ${CMAKE_BINARY_DIR}/config.cmake )
  else()
    message(STATUS "A configuration file named 'config.cmake' was not provided in the build directory.")
    message(STATUS "Using command line arguments only to configure build.")
    include( ${CMAKE_SOURCE_DIR}/base/config/defaults.cmake )
  endif()

endif()

if( DEFINED PLATO_SYSTEM_CONFIG )
  if( EXISTS ${PLATO_SYSTEM_CONFIG} )
   include( ${PLATO_SYSTEM_CONFIG} )
  else()
    message(STATUS "Specified system configuration file ($PLATO_SYSTEM_CONFIG} not found")
    message(FATAL_ERROR " ")
  endif()
endif()
###############################################################################
###############################################################################


############################  Trilinos Setup  #################################
###############################################################################
IF (DEFINED TRILINOS_INSTALL_DIR)
  SET(CMAKE_PREFIX_PATH ${TRILINOS_INSTALL_DIR} ${CMAKE_PREFIX_PATH})
ENDIF()
FIND_PACKAGE(Trilinos REQUIRED)

MESSAGE("\nFound Trilinos!  Here are the details: ")
MESSAGE("   Trilinos_DIR = ${Trilinos_DIR}")
MESSAGE("   Trilinos_VERSION = ${Trilinos_VERSION}")
MESSAGE("End of Trilinos details\n")


if( ENABLE_ROL )
  add_definitions(-DENABLE_ROL)
endif()

include_directories(SYSTEM ${Trilinos_INCLUDE_DIRS} ${Trilinos_TPL_INCLUDE_DIRS})
list(REMOVE_DUPLICATES Trilinos_TPL_LIBRARIES)

###########################  Compiler setup ###################################
######################## (pre 'project()' command) ############################
## Compilers cannot be changed after the 'project()' command below:
message("Trilinos_CXX_COMPILER     ${Trilinos_CXX_COMPILER}")
message("Trilinos_C_COMPILER       ${Trilinos_C_COMPILER}")
message("Trilinos_Fortran_COMPILER ${Trilinos_Fortran_COMPILER}")
set(CMAKE_CXX_COMPILER     ${Trilinos_CXX_COMPILER})
set(CMAKE_C_COMPILER       ${Trilinos_C_COMPILER})
set(CMAKE_Fortran_COMPILER ${Trilinos_Fortran_COMPILER})

###############################################################################
project(PlatoEngine)
###############################################################################


if( DAKOTADRIVER )
  find_package(Dakota REQUIRED)
endif()
find_package(Boost REQUIRED COMPONENTS serialization filesystem system program_options regex)

set(PLATO_INCLUDES ${CMAKE_SOURCE_DIR}/apps)

###########################  Compiler setup ###################################
####################### (post 'project()' command) ############################
message("Trilinos_CXX_COMPILER_FLAGS: ${Trilinos_CXX_COMPILER_FLAGS}")
# set(CMAKE_C_FLAGS "${Trilinos_C_COMPILER_FLAGS} -fPIC -fopenmp")
# set(CMAKE_CXX_FLAGS "${Trilinos_CXX_COMPILER_FLAGS} -fPIC -fopenmp")
set(CMAKE_C_FLAGS "${Trilinos_C_COMPILER_FLAGS} -fPIC -Werror")
set(CMAKE_CXX_FLAGS "${Trilinos_CXX_COMPILER_FLAGS} -fPIC -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  # Clang specific flags
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  # gnu specific flags
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lgfortran")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  # intel specific flags
else()
  message(WARNING "Unknown compiler type: ${CMAKE_CXX_COMPILER_ID}. Using default flags.")
endif()


############################## gtest Setup ####################################
###############################################################################
if( UNIT_TESTING )
  find_package(GTest REQUIRED)
endif()

if( ESP_ENABLED )
  Plato_find_lib ( ESP_LIB ON egads ${ESP_LIB_DIR} )
  Plato_find_lib ( OCSM_LIB ON ocsm ${ESP_LIB_DIR} )
  Plato_find_path( ESP_INC ON egads.h ${ESP_INC_DIR} )
  include_directories( ${ESP_INC} )
endif()

###############################################################################
###############################################################################
if("${CMAKE_BUILD_TYPE}" STREQUAL Release)
  add_definitions( -DNDEBUG)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL Debug)
endif()

if( GEOMETRY )
  message( "-- Compiling Cogent " )
  add_definitions( -DGEOMETRY )
endif()

if( STK_ENABLED )
  message( "-- Compiling with STK " )
  add_definitions( -DSTK_ENABLED )
endif()

if( ESP_ENABLED )
  message( "-- Compiling with ESP " )
  add_definitions( -DESP_ENABLED )
endif()

if( DAKOTADRIVER )
  message( "-- Compiling Dakota Driver " )
  add_definitions(-DDAKOTADRIVER)
endif()

if( ENABLE_ISO )
  message( "-- Compiling PlatoIso " )
  add_definitions(-DENABLE_ISO)
endif()

if( ENABLE_PRUNE )
  message( "-- Compiling Plato prune and refine " )
  add_definitions(-DENABLE_PRUNE)
endif()



if( REGRESSION OR UNIT_TESTING )
    enable_testing()
endif()

if( AMFILTER_ENABLED )
    message( "-- Compiling AMFilter, c++14 required " )
    string(REPLACE "-std=c++11" "-std=c++14" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REPLACE "-std=c++11" "-std=c++14" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "-std=c++11" "-std=c++14" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
    add_definitions(-DAMFILTER_ENABLED)
    find_package(ArborX REQUIRED)
    set(PLATO_LIBRARIES ${PLATO_LIBRARIES} ArborX::ArborX)
    set(PLATO_INCLUDES ${PLATO_INCLUDES} ${ARBORX_INCLUDE_DIR} ${ARBORX_INCLUDE_DIR}/details)
endif()

if ( IPOPT_ENABLED )
    message( "-- Compiling with IPOPT to solve MMA subproblem " )
    add_definitions(-DENABLE_IPOPT_FOR_MMA_SUBPROBLEM)
    Plato_find_lib ( IPOPT_LIB ON ipopt ${IPOPT_INSTALL_DIR}/lib )
    set(PLATO_LIBRARIES ${PLATO_LIBRARIES} ${IPOPT_LIB})
    set(PLATO_INCLUDES ${PLATO_INCLUDES} ${IPOPT_INSTALL_DIR}/include)
endif()

add_subdirectory(base/src)
add_subdirectory(base/src/tools)
add_subdirectory(base/src/analyze)
add_subdirectory(base/src/data_layer)
add_subdirectory(base/src/Filters)
add_subdirectory(base/src/interface)
add_subdirectory(base/src/models)
add_subdirectory(base/src/optimize)
add_subdirectory(base/src/proxy)
if(GEOMETRY)
  add_subdirectory(base/src/geometry/MovingLeastSquares)
  add_subdirectory(base/src/geometry/Cogent)
endif()
add_subdirectory(base/src/PlatoSubproblemLibrary/Agent)
add_subdirectory(base/src/PlatoSubproblemLibraryInterface)
add_subdirectory(base/src/PlatoSubproblemLibrary/SpatialSearching)
add_subdirectory(base/src/PlatoSubproblemLibrary/AbstractInterface)
add_subdirectory(base/src/PlatoSubproblemLibrary/BoundedSupportFunction)
add_subdirectory(base/src/PlatoSubproblemLibrary/DiscreteGlobalOptimization)
add_subdirectory(base/src/PlatoSubproblemLibrary/Example)
add_subdirectory(base/src/PlatoSubproblemLibrary/Filter)
add_subdirectory(base/src/PlatoSubproblemLibrary/Geometry)
add_subdirectory(base/src/PlatoSubproblemLibrary/Helper)
add_subdirectory(base/src/PlatoSubproblemLibrary/ParameterData)
add_subdirectory(base/src/input_checkers)
add_subdirectory(base/src/input_generator)
add_subdirectory(base/src/operations)
add_subdirectory(base/src/Su2ToExodus)
add_subdirectory(base/src/MarkFaces)

if( ENABLE_ISO )
  add_subdirectory(base/src/iso)
endif()

if( ENABLE_PRUNE )
  add_subdirectory(base/src/prune/main)
  add_subdirectory(base/src/prune)
endif()

if( ESP_ENABLED )
  add_subdirectory(base/src/geometry/ESP)
  add_subdirectory(apps/esp)
endif()

if( DAKOTADRIVER )
  add_subdirectory(base/src/dakota)
endif()

if( STK_ENABLED )
  add_subdirectory(base/src/support_structure)
  add_subdirectory(base/src/support_structure/main)
  add_subdirectory(base/src/DecomposeMesh)
endif()

if( PLATOMAIN )
  message( "-- Compiling PlatoMain " )
  add_subdirectory(apps/services)
endif()

if(XTK_ENABLED)
    message( "-- Compiling xtk " )
    add_subdirectory(apps/xtk)
endif()

if( PLATOPROXY )
  message( "-- Compiling PlatoProxy " )
  add_subdirectory(apps/proxy)
  add_subdirectory(apps/rocket)
  add_subdirectory(apps/ksal_proxy)
  add_subdirectory(apps/rosenbrock)
  add_subdirectory(apps/alpso_proxy)
  add_subdirectory(apps/bcpso_proxy)
endif()

if( PLATOSTATICS )
  message( "-- Compiling Statics " )
  add_subdirectory(apps/statics)
endif()

if(EXPY)
  message( "-- Compiling Expy " )
  add_subdirectory(apps/services/python)
endif()

if(GEOMETRY)
  add_subdirectory(apps/cogent)
endif()

if( REGRESSION )
Plato_find_exe(NUMDIFF_COMMAND numdiff ${NUMDIFF_PATH}/bin)
  if( PLATOMAIN )
    if(SEACAS)
      Plato_find_exe(SEACAS_EPU     epu      ${SEACAS_PATH}/bin)
      Plato_find_exe(SEACAS_EXODIFF exodiff  ${SEACAS_PATH}/bin)
      Plato_find_exe(SEACAS_DECOMP  decomp   ${SEACAS_PATH}/bin)
    endif()
    add_subdirectory(examples)
    add_subdirectory(regression)
  else()
    message("-- !! -- PlatoMain is required for regression testing.")
    message("-- !! -- Regression has not been enabled.")
  endif()
endif()

if( XTK_ENABLED )
   message( "\n-- Compiling with XTK \n" ) 
    find_package(XTK REQUIRED NAMES)
    find_library(XTK_COM_Libs NAMES libCOM.a HINTS $ENV{XTK_LIBS} PATH_SUFFIXES COM)
    find_library(XTK_MTK_Libs NAMES libMTK.a HINTS $ENV{XTK_LIBS} PATH_SUFFIXES MTK)
    set(XTK_Libs ${XTK_COM_Libs} ${XTK_MTK_Libs} CACHE FILEPATH "List of library paths." )
endif()

add_subdirectory(share)
