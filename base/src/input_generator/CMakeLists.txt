###############################################################################
# Sources:
###############################################################################
SET(XMLGenerator_SRCS XMLGenerator_Main.cpp 
                      XMLGenerator.cpp
                      XMLGeneratorProblem.cpp
                      XMLGeneratorParseBlock.cpp
                      XMLGeneratorParseLoads.cpp
                      XMLGeneratorParseOutput.cpp
                      XMLGeneratorParseScenario.cpp
                      XMLGeneratorParseServices.cpp
                      XMLGeneratorParseMaterial.cpp
                      XMLGeneratorParseAssembly.cpp
                      XMLGeneratorParseCriteria.cpp
                      XMLGeneratorDakotaProblem.cpp
                      XMLGeneratorOutputMetadata.cpp 
                      XMLGeneratorParseObjective.cpp
                      XMLGeneratorOutputUtilities.cpp
                      XMLGeneratorStagesUtilities.cpp
                      XMLGeneratorParseConstraint.cpp
                      XMLGeneratorParserUtilities.cpp
                      XMLGeneratorServiceMetadata.cpp
                      XMLGeneratorProblemUtilities.cpp
                      XMLGeneratorServiceUtilities.cpp
                      XMLGeneratorScenarioMetadata.cpp
                      XMLGeneratorParseUncertainty.cpp
                      XMLGeneratorConstraintMetadata.cpp
                      XMLGeneratorSharedDataUtilities.cpp
                      XMLGeneratorPerformersUtilities.cpp
                      XMLGeneratorUncertaintyMetadata.cpp
                      XMLGeneratorDefinesFileUtilities.cpp
                      XMLGeneratorPlatoAnalyzeUtilities.cpp
                      XMLGeneratorInterfaceFileUtilities.cpp
                      XMLGeneratorGradBasedOptimizerOptions.cpp
                      XMLGeneratorStagesOperationsUtilities.cpp
                      XMLGeneratorParseOptimizationParameters.cpp
                      XMLGeneratorDakotaInterfaceFileUtilities.cpp
                      XMLGeneratorDakotaDriverInputFileUtilities.cpp 
                      XMLGeneratorOptimizationParametersMetadata.cpp
                      XMLGeneratorParseEssentialBoundaryCondition.cpp
                      XMLGeneratorParseMethodInputOptionsUtilities.cpp
                      XMLGeneratorAnalyzePhysicsFunctionInterface.cpp
                      XMLGeneratorMaterialFunctionInterface.cpp
                      XMLGeneratorRandomInterfaceFileUtilities.cpp
                      XMLGeneratorPlatoMainInputFileUtilities.cpp
                      XMLGeneratorPlatoESPInputFileUtilities.cpp
                      XMLGeneratorPlatoESPOperationsFileUtilities.cpp
                      XMLGeneratorPlatoAnalyzeInputFileUtilities.cpp
                      XMLGeneratorPlatoMainOperationFileUtilities.cpp
                      XMLGeneratorPlatoAnalyzeOperationsFileUtilities.cpp
                      XMLGeneratorLaunchUtilities.cpp
                      XMLGeneratorAnalyzeLoadFunctionInterface.cpp
                      XMLGeneratorAnalyzeLoadTagFunctionInterface.cpp
                      XMLGeneratorAnalyzeEssentialBCFunctionInterface.cpp
                      XMLGeneratorAnalyzeEssentialBCTagFunctionInterface.cpp
                      XMLGeneratorAnalyzeAssemblyFunctionInterface.cpp
                      XMLGeneratorAnalyzeMaterialModelFunctionInterface.cpp
                      XMLGeneratorPlatoMainConstraintValueOperationInterface.cpp
                      XMLGeneratorPlatoMainConstraintGradientOperationInterface.cpp
                      XMLGeneratorLaunchScriptUtilities.cpp
                      XMLGeneratorRunMetadata.cpp
                      XMLGeneratorUtilities.cpp
                      XMLGeneratorParseRun.cpp
                      XMLGeneratorPlatoXTKInputFile.cpp
                      XMLGeneratorPlatoXTKOperationFile.cpp
                      )
                      
SET(XMLGenerator_HDRS XMLGenerator.hpp 
                      XMLGeneratorProblem.hpp 
                      XMLGeneratorDataStruct.hpp
                      XMLGeneratorDakotaProblem.hpp
                      XMLGeneratorValidInputKeys.hpp
                      XMLGeneratorRandomMetadata.hpp
                      XMLGeneratorOutputMetadata.hpp 
                      XMLGeneratorStagesUtilities.hpp
                      XMLGeneratorParserUtilities.hpp 
                      XMLGeneratorOutputUtilities.hpp
                      XMLGeneratorServiceMetadata.hpp
                      XMLGeneratorProblemUtilities.hpp
                      XMLGeneratorServiceUtilities.hpp
                      XMLGeneratorScenarioMetadata.hpp
                      XMLGeneratorAssemblyMetadata.hpp
                      XMLGeneratorBoundaryMetadata.hpp
                      XMLGeneratorMaterialMetadata.hpp 
                      XMLGeneratorCriterionMetadata.hpp 
                      XMLGeneratorConstraintMetadata.hpp
                      XMLGeneratorSharedDataUtilities.hpp
                      XMLGeneratorPerformersUtilities.hpp
                      XMLGeneratorUncertaintyMetadata.hpp
                      XMLGeneratorDefinesFileUtilities.hpp 
                      XMLGeneratorPlatoAnalyzeUtilities.hpp 
                      XMLGeneratorInterfaceFileUtilities.hpp
                      XMLGeneratorDakotaInterfaceFileUtilities.hpp 
                      XMLGeneratorDakotaDriverInputFileUtilities.hpp 
                      XMLGeneratorGradBasedOptimizerOptions.hpp
                      XMLGeneratorStagesOperationsUtilities.hpp
                      XMLGeneratorOptimizationParametersMetadata.hpp
                      XMLGeneratorParseMethodInputOptionsUtilities.hpp
                      XMLGeneratorAnalyzePhysicsFunctionInterface.hpp
                      XMLGeneratorMaterialFunctionInterface.hpp
                      XMLGeneratorPlatoMainInputFileUtilities.hpp
                      XMLGeneratorPlatoESPInputFileUtilities.hpp
                      XMLGeneratorPlatoESPOperationsFileUtilities.hpp
                      XMLGeneratorPlatoAnalyzeInputFileUtilities.hpp
                      XMLGeneratorPlatoMainOperationFileUtilities.hpp
                      XMLGeneratorPlatoAnalyzeOperationsFileUtilities.hpp
                      XMLGeneratorLaunchUtilities.hpp
                      XMLGeneratorAnalyzeLoadFunctionInterface.hpp
                      XMLGeneratorAnalyzeLoadTagFunctionInterface.hpp
                      XMLGeneratorAnalyzeEssentialBCFunctionInterface.hpp
                      XMLGeneratorAnalyzeEssentiallBCTagFunctionInterface.hpp
                      XMLGeneratorAnalyzeAssemblyFunctionInterface.hpp
                      XMLGeneratorPlatoMainConstraintValueOperationInterface.hpp
                      XMLGeneratorPlatoMainConstraintGradientOperationInterface.hpp
                      XMLGeneratorLaunchScriptUtilities.hpp
                      XMLGeneratorUtilities.hpp
                      XMLGeneratorParseOutput.hpp 
                      XMLGeneratorParseMaterial.hpp 
                      XMLGeneratorParseAssembly.hpp 
                      XMLGeneratorParseEssentialBoundaryCondition.hpp 
                      XMLGeneratorParseCriteria.hpp 
                      XMLGeneratorParseObjective.hpp 
                      XMLGeneratorParseBlock.hpp
                      XMLGeneratorParseScenario.hpp 
                      XMLGeneratorParseOptimizationParameters.hpp 
                      XMLGeneratorParseServices.hpp 
                      XMLGeneratorParseConstraint.hpp 
                      XMLGeneratorParseUncertainty.hpp
                      XMLGeneratorPlatoXTKInputFile.hpp
                      XMLGeneratorPlatoXTKOperationFile.hpp
                      XMLGeneratorRunMetadata.hpp
                      XMLGeneratorParseRun.hpp
                      )

find_package(MPI)

# try to determine if Open MPI is being used
STRING(FIND ${MPI_C_COMPILER} "openmpi" FIND_POS) 

if( FIND_POS EQUAL -1 )
  execute_process(COMMAND mpirun --version OUTPUT_VARIABLE OUT)
  STRING(FIND ${OUT} "Open MPI" FIND_POS) 
  if( FIND_POS EQUAL -1 )
  else()
    add_definitions(-DUSING_OPEN_MPI)
  endif()
 else()
   add_definitions(-DUSING_OPEN_MPI)
endif()

INCLUDE_DIRECTORIES(${PLATO_INCLUDES})

# actual target:
set(PLATOMAIN_LIBS ${PLATO_LIBRARIES} ${Plato_EXTRA_LINK_FLAGS})
add_executable(XMLGenerator ${XMLGenerator_SRCS})
target_link_libraries(XMLGenerator ${PLATOMAIN_LIBS} ${Trilinos_LIBRARIES} )
#set(INTERFACEXMLCHECKER_BINARY ${CMAKE_BINARY_DIR}/base/src/input_checkers/XMLGenerator PARENT_SCOPE)

if( CMAKE_INSTALL_PREFIX )
  install( TARGETS XMLGenerator DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
endif()
###############################################################################
###############################################################################

if( UNIT_TESTING )
  add_subdirectory(./unittest)
endif()

