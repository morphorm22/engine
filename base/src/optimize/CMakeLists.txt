set(LIB_NAMES)

set(LIB_NAME Optimizer)
set(LIB_NAMES ${LIB_NAMES} ${LIB_NAME})
set(${LIB_NAME}_SOURCES Plato_OptimizersIO_Utilities.cpp
                        Plato_OptimizersIO.cpp
                        )
                        
set(${LIB_NAME}_HEADERS Plato_BoundsBase.hpp
                        Plato_GradFreeCircle.hpp
                        Plato_GradFreeRadius.hpp
                        Plato_GradFreeCriterion.hpp
                        Plato_GradFreeRosenbrock.hpp
                        Plato_GradFreeHimmelblau.hpp
                        Plato_ProxyAugLagObjective.hpp
                        Plato_GradFreeRocketObjFunc.hpp
                        Plato_GradFreeGoldsteinPrice.hpp
                        Plato_GradFreeShiftedEllipse.hpp
                        Plato_GradFreeEngineCriterion.hpp
                        Plato_RestartFileUtilities.hpp
                        Plato_ParticleSwarmTypes.hpp
                        Plato_ParticleSwarmParser.hpp
                        Plato_ParticleSwarmDataMng.hpp
                        Plato_ParticleSwarmStageMng.hpp
                        Plato_ParticleSwarmIO_BCPSO.hpp
                        Plato_ParticleSwarmIO_ALPSO.hpp
                        Plato_ParticleSwarmOperations.hpp
                        Plato_ParticleSwarmEngineBCPSO.hpp
                        Plato_ParticleSwarmEngineALPSO.hpp
                        Plato_ParticleSwarmIO_Utilities.hpp
                        Plato_ParticleSwarmStageMngBCPSO.hpp
                        Plato_ParticleSwarmStageMngALPSO.hpp
                        Plato_ParticleSwarmAlgorithmBCPSO.hpp
                        Plato_ParticleSwarmAlgorithmALPSO.hpp
                        Plato_ParticleSwarmInterfaceBCPSO.hpp
                        Plato_ParticleSwarmInterfaceALPSO.hpp
                        Plato_GradFreeCriteriaList.hpp
                        Plato_MethodMovingAsymptotes.hpp
                        Plato_MethodMovingAsymptotesIO.hpp
                        Plato_MethodMovingAsymptotesEngine.hpp
                        Plato_MethodMovingAsymptotesParser.hpp
                        Plato_MethodMovingAsymptotesIO_Data.hpp
                        Plato_MethodMovingAsymptotesDataMng.hpp
                        Plato_MethodMovingAsymptotesCriterion.hpp
                        Plato_MethodMovingAsymptotesInterface.hpp
                        Plato_MethodMovingAsymptotesOperations.hpp
                        Plato_UnconstrainedMethodMovingAsymptotes.hpp
                        Plato_UnconstrainedMethodMovingAsymptotesParser.hpp
                        Plato_UnconstrainedMethodMovingAsymptotesEngine.hpp
                        Plato_UnconstrainedMethodMovingAsymptotesDataIO.hpp
                        Plato_UnconstrainedMethodMovingAsymptotesFileIO.hpp
                        Plato_UnconstrainedMethodMovingAsymptotesDataMng.hpp
                        Plato_NullConstraint.hpp
                        Plato_OptimizerParser.hpp
                        Plato_DriverFactory.hpp
                        Plato_OptimizersIO_Utilities.hpp
                        Plato_AugmentedLagrangian.hpp
                        Plato_TrustRegionUtilities.hpp
                        Plato_CommWrapper.hpp
                        Plato_Types.hpp
                        Plato_Circle.hpp
                        Plato_HostBounds.hpp
                        Plato_Himmelblau.hpp
                        Plato_KokkosTypes.hpp
                        Plato_ErrorChecks.hpp
                        Plato_HessianLBFGS.hpp
                        Plato_DeviceBounds.hpp
                        Plato_OptimizersIO.hpp
                        Plato_GoldsteinPrice.hpp
                        Plato_StateData.hpp
                        Plato_StateManager.hpp
                        Plato_AlgebraFactory.hpp
                        Plato_EngineObjective.hpp
                        Plato_EngineConstraint.hpp
                        Plato_SteihaugTointSolver.hpp
                        Plato_GradientOperator.hpp
                        Plato_GradientOperatorList.hpp
                        Plato_IdentityHessian.hpp
                        Plato_IdentityPreconditioner.hpp
                        Plato_KelleySachsAlgorithm.hpp
                        Plato_TrustRegionStageMng.hpp
                        Plato_TrustRegionStepMng.hpp
                        Plato_TrustRegionAlgorithmDataMng.hpp
                        Plato_KelleySachsBoundConstrained.hpp
                        Plato_ReducedSpaceTrustRegionStageMng.hpp
                        Plato_KelleySachsBoundLightInterface.hpp
                        Plato_KelleySachsBoundConstrainedInterface.hpp
                        Plato_KelleySachsAugmentedLagrangianInterface.hpp
                        Plato_KelleySachsAugmentedLagrangianLightInterface.hpp
                        Plato_KelleySachsStepMng.hpp
                        Plato_LineSearch.hpp
                        Plato_LinearOperator.hpp
                        Plato_LinearOperatorList.hpp
                        Plato_MultiVectorList.hpp
                        Plato_Preconditioner.hpp
                        Plato_ProjectedSteihaugTointPcg.hpp
                        Plato_QuadraticLineSearch.hpp
                        Plato_Radius.hpp
                        Plato_Rosenbrock.hpp
                        Plato_CcsaTestObjective.hpp
                        Plato_CcsaTestInequality.hpp
                        Plato_AnalyticalHessian.hpp
                        Plato_AnalyticalGradient.hpp
                        Plato_AugmentedLagrangianStageMng.hpp
                        Plato_CubicLineSearch.hpp
                        Plato_Criterion.hpp
                        Plato_CriterionList.hpp
                        Plato_DataFactory.hpp
                        Plato_DistributedReductionOperations.hpp
                        Plato_LinearAlgebra.hpp
                        Plato_MultiVector.hpp
                        Plato_OptimalityCriteriaDataMng.hpp
                        Plato_OptimalityCriteria.hpp
                        Plato_OptimalityCriteriaStageMngBase.hpp
                        Plato_OptimalityCriteriaStageMng.hpp
                        Plato_OptimalityCriteriaSubProblem.hpp
                        Plato_OptimalityCriteriaLightInterface.hpp
                        Plato_OptimalityCriteriaTestInequalityOne.hpp
                        Plato_OptimalityCriteriaTestInequalityTwo.hpp
                        Plato_OptimalityCriteriaTestObjectiveOne.hpp
                        Plato_OptimalityCriteriaTestObjectiveTwo.hpp
                        Plato_ReductionOperations.hpp
                        Plato_SingleConstraintSubProblemTypeLP.hpp
                        Plato_StandardMultiVector.hpp
                        Plato_StandardVector.hpp
                        Plato_OptimalityCriteriaUpdate.hpp
                        Plato_HostOptimalityCriteriaUpdate.hpp
                        Plato_StandardVectorReductionOperations.hpp
                        Plato_NonlinearProgrammingSubProblemOC.hpp
                        Plato_Vector.hpp
                        Plato_DriverInterface.hpp
                        Plato_SOParameterStudies.hpp
                        Plato_DistributedVector.hpp
                        Plato_EpetraSerialDenseVector.hpp
                        Plato_SOParameterStudiesInterface.hpp
                        Plato_OptimalityCriteriaInterface.hpp
                        Plato_EpetraSerialDenseMultiVector.hpp
                        Plato_BetaDistribution.hpp
                        Plato_Distribution.hpp
                        Plato_NormalDistribution.hpp
                        Plato_UniformDistribution.hpp
                        Plato_SromSolve.hpp
                        Plato_SromUtilis.hpp
                        Plato_SromHelpers.hpp
                        Plato_SromConstraint.hpp
                        Plato_SromObjective.hpp
                        Plato_SromStatistics.hpp
                        Plato_SromLoadUtils.hpp
                        # Plato_SromXMLGenLoad.hpp
                        # Plato_SromXMLGenMaterial.hpp
                        # Plato_SromXMLGenTools.hpp
                        # Plato_SromXMLGenHelpers.hpp
                        Plato_SromMaterialUtils.hpp
                        Plato_SromMetadata.hpp
                        Plato_SromProbDataStruct.hpp
                        Plato_StatisticsUtils.hpp
                        Plato_RandomLoadMetadata.hpp
                        Plato_RandomMaterialMetadata.hpp
                        Plato_LinearCriterionHessian.hpp
                        Plato_CentralDifferenceHessian.hpp
                        Plato_DakotaFactory.hpp
                        Plato_OptimizerFactory.hpp
                        Plato_OptimizerInterface.hpp
                        Plato_OptimizerUtilities.hpp
                        Plato_KokkosVector.hpp
                        Plato_KokkosReductionOperations.hpp
                        Plato_DeviceOptimalityCriteriaUpdate.hpp
                        Plato_SolveUncertaintyProblem.hpp
                        Plato_DistributionFactory.hpp
                        Plato_DiagnosticsInterface.hpp
                        Plato_Diagnostics.hpp
                        )
                        
INCLUDE_DIRECTORIES(${PLATO_INCLUDES})

FOREACH(PLATO_LIB ${LIB_NAMES})
  add_library(Plato${PLATO_LIB}     ${${PLATO_LIB}_SOURCES}     ${${PLATO_LIB}_HEADERS}     )
  set(ADD_PLATO_LIBRARIES ${ADD_PLATO_LIBRARIES} Plato${PLATO_LIB})
ENDFOREACH()
set(PLATO_LIBRARIES ${PLATO_LIBRARIES} ${ADD_PLATO_LIBRARIES} PARENT_SCOPE)

if( CMAKE_INSTALL_PREFIX )
 FOREACH(PLATO_LIB ${LIB_NAMES})
  install( TARGETS Plato${PLATO_LIB} EXPORT Plato${PLATO_LIB}
           LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
           ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
           PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
  install( FILES ${${PLATO_LIB}_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
 ENDFOREACH()
endif()
