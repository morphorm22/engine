###############################################################################
# Sources:
###############################################################################
SET(PlatoMainUnitTester_SRCS UnitMain.cpp
							 parse_unit_test.cpp
							 Plato_StkMeshUtils.cpp
							 Plato_PlatoProxyApp.cpp
							 Plato_PhysicsProxyApp.cpp
							 Plato_StructuralTopologyOptimizationProxyGoldResults.cpp
							 Plato_Test_GradFreeCriteria.cpp
							 Plato_Test_ParticleSwarm.cpp
							 Plato_Test_Proxy.cpp
							 Plato_Test_Hessian.cpp
							 Plato_Test_SimpleRocket.cpp
							 Plato_Test_KokkosAlgebra.cpp
							 Plato_Test_InputData.cpp
							 Plato_Test_LinearAlgebra.cpp
							 Plato_Test_OptimizersIO.cpp
							 Plato_Test_OptimalityCriteria.cpp
							 Plato_Test_DataFactory.cpp
							 Plato_Test_KelleySachsBounds.cpp
							 Plato_Test_AugmentedLagrangian.cpp
							 Plato_Test_UnconstrainedMethodMovingAsymptotes.cpp
							 Plato_Test_Bounds.cpp
							 Plato_Test_SimpleTopOpt.cpp
							 Plato_Test_Statistics.cpp
							 Plato_Test_OptimizationTestFunctions.cpp
							 Plato_Test_Vector3DVariations.cpp
							 Plato_Test_UniqueCounter.cpp
							 Plato_Test_SimpleRocketOptimization.cpp
							 Plato_Test_UncertainLoadGeneratorXML.cpp
                                                         Plato_Test_UncertainMaterial.cpp
                                                         Plato_Test_Srom.cpp
							 Plato_Test_LocalStatisticsOperations.cpp
							 Plato_Test_MethodMovingAsymptotes.cpp
							 Plato_Test_WriteParameterStudyData.cpp
                                                         Plato_Test_FreeFunctions.cpp
							 PSL_Test_Triangle.cpp  
							 PSL_Test_OverhangFilter.cpp  
							 PSL_Test_KernelThenHeavisideFilter.cpp  
							 PSL_Test_Standardization.cpp
							 PSL_Test_Random.cpp
							 PSL_Test_ParameterData.cpp
							 PSL_Test_ParallelExchanger.cpp
							 PSL_Test_NearestNeighborSearches.cpp
							 PSL_Test_MpiWrapperInterface.cpp
							 PSL_Test_MpiWrapperImplementation.cpp
							 PSL_Test_MeshScaleAgent.cpp
							 PSL_Test_Mesh.cpp
							 PSL_Test_KernelFilter.cpp
							 PSL_Test_GradientCheck.cpp
							 PSL_Test_GlobalUtilities.cpp
							 PSL_Test_FreeHelpers.cpp
							 PSL_Test_FixedRadiusNearestNeighborsSearches.cpp
							 PSL_Test_DenseMatrixImplementation.cpp
							 PSL_Test_DenseMatrixBuilder.cpp
							 PSL_Test_ConjugateGradient.cpp
							 PSL_Test_CompressedRowSparseMatrixImplementation.cpp
							 PSL_Test_CompressedRowSparseMatrixBuilder.cpp
							 PSL_Test_BoundingBoxSearches.cpp
							 PSL_Test_BoundingBoxMortonHierarchy.cpp
							 PSL_Test_BoundingBox.cpp
							 PSL_Test_BasicDiscreteObjective.cpp
							 PSL_Test_BasicDenseVectorOperations.cpp
							 PSL_Test_AbstractAuthority.cpp
							 PSL_Test_Point.cpp
							 PSL_Test_Vector.cpp
							 Plato_Test_TimersTree.cpp
                                                         PSL_Test_OrthogonalGridUtilities.cpp
                                                         PSL_Test_RegularHex8.cpp
							 )

if( AMFILTER_ENABLED )
    SET(PlatoMainUnitTester_SRCS ${PlatoMainUnitTester_SRCS} PSL_Test_TetMeshUtilities.cpp PSL_Test_AMFilterUtilities.cpp)
endif()

IF( DAKOTADRIVER )
    SET(PlatoMainUnitTester_SRCS ${PlatoMainUnitTester_SRCS} Plato_Test_PlatoDakotaDriver.cpp)
    add_compile_definitions(${Dakota_DEFINES})
ENDIF()

###############################################################################
# Headers:
###############################################################################
SET(PlatoMainUnitTester_HDRS Plato_StructuralTopologyOptimizationProxyGoldResults.hpp
							 ParseUnitTestStrings.hpp
							 Plato_UnitTestUtils.hpp
							 Plato_StkMeshUtils.hpp
							 )

set(PLATOUNIT_INCLUDES ${CMAKE_SOURCE_DIR}/apps/services
                       ${CMAKE_SOURCE_DIR}/apps/services/proxy
                       ${CMAKE_SOURCE_DIR}/apps/services/unittest
                       ${CMAKE_SOURCE_DIR}/base/src/analyze
                       ${CMAKE_SOURCE_DIR}/base/src/interface
                       ${CMAKE_SOURCE_DIR}/base/src/input_generator
                       ${CMAKE_SOURCE_DIR}/base/src/data_layer
                       ${CMAKE_SOURCE_DIR}/base/src/Filters
                       ${CMAKE_SOURCE_DIR}/base/src/tools
                       ${CMAKE_SOURCE_DIR}/base/src/optimize
                       ${CMAKE_SOURCE_DIR}/base/src/operations
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibraryInterface
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/AbstractInterface
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/ActivationFunction
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/Agent
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/BoundedSupportFunction
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/DataLossFunction
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/DataOrganizer
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/DecisionMetric
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/DiscreteGlobalOptimization
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/DistanceVoting
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/Eigen
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/Example
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/Filter
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/Geometry
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/Graph
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/Helper
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/InputOutput
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/InvertibleUnitRange
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/LearningPreprocessor
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/NeighborsSearching
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/NodeSetMatching
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/ParameterData
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/SpatialSearching
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/SupervisedLearning
                       ${CMAKE_SOURCE_DIR}/base/src/PlatoSubproblemLibrary/VectorKernel
                       )

if( DAKOTADRIVER )
    SET(PLATOUNIT_INCLUDES ${PLATOUNIT_INCLUDES} ${Dakota_INCLUDE_DIRS})
endif()

INCLUDE_DIRECTORIES(${PLATOUNIT_INCLUDES})

# actual target:
set(PLATOMAINUNITTESTER_LIBS ${PLATOMAINUNITTESTER_ADDITIONAL_LIBS} ${PLATO_LIBRARIES} ${PLATO_LIBRARIES} 
                             ${NLOPT_LIBRARY} GTest::GTest ${Trilinos_LIBRARIES} ${PYTHON_LIBRARY} 
                             ${PYTHON_LINK_LIBS} ${Plato_EXTRA_LINK_FLAGS})

if( DAKOTADRIVER )
	set(PLATOMAINUNITTESTER_LIBS ${PLATOMAINUNITTESTER_LIBS} ${Dakota_LIBRARIES} ${Boost_LIBRARIES})
endif()

add_executable(PlatoMainUnitTester ${PlatoMainUnitTester_SRCS})
target_link_libraries(PlatoMainUnitTester PlatoApp ${PLATOMAINUNITTESTER_LIBS})
set(PLATOMAINUNITTESTER_BINARY ${CMAKE_BINARY_DIR}/apps/unittest/PlatoMainUnitTester PARENT_SCOPE)
add_test(NAME PlatoMainUnitTester COMMAND ${CMAKE_BINARY_DIR}/apps/services/unittest/PlatoMainUnitTester)
set_property(TEST PlatoMainUnitTester PROPERTY LABELS "large")

if( CMAKE_INSTALL_PREFIX )
  install( TARGETS PlatoMainUnitTester DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
endif()
###############################################################################
###############################################################################
