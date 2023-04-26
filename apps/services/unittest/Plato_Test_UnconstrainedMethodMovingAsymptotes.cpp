/*
 * Plato_Test_UnconstrainedMethodMovingAsymptotes.cpp
 *
 *  Created on: Apr 13, 2023
 */

#include "gtest/gtest.h"

#include "Plato_UnitTestUtils.hpp"

#include "Plato_Utils.hpp"
#include "Plato_ProxyAugLagObjective.hpp"
#include "Plato_UnconstrainedMethodMovingAsymptotesDataIO.hpp"

namespace PlatoTest
{

inline 
std::vector<double> get_gold_control_umma_test()
{
    std::vector<double> tGold = {1,1,0.001,0.001,0.001,0.001,0.001,0.100522,1,1,1,1,0.00662942,0.001,0.001,
                                 0.001,0.001,0.105438,1,1,1,1,0.631724,0.001,0.001,0.001,0.001,0.0986644,
                                 1,1,1,1,1,0.362865,0.001,0.001,0.001,0.107532,1,1,1,0.584344,0.637947,1,0.001,
                                 0.001,0.001,0.0942639,1,1,1,0.643879,0.001,1,0.830304,0.001,0.001,0.115356,1,
                                 1,1,0.577697,0.001,0.0274311,1,0.679663,0.001,0.0853201,1,1,1,0.6153,0.001,
                                 0.001,0.562993,1,0.257495,0.0539633,1,1,1,0.601996,0.001,0.001,0.001,0.745042,
                                 1,0.138703,1,1,1,0.621391,0.001,0.001,0.001,0.001,1,0.811985,1,1,1,0.621169,
                                 0.001,0.001,0.001,0.001,0.120148,1,1,1,1,0.634895,0.001,0.001,0.001,0.001,
                                 0.001,0.0650353,1,1,1,0.638724,0.001,0.001,0.001,0.001,0.001,0.0619279,1,1,1,
                                 0.651018,0.001,0.001,0.001,0.001,0.001,1,0.727409,0.791083,1,0.658398,0.001,
                                 0.001,0.001,0.001,0.843752,0.905429,0.001,0.837073,1,0.667983,0.001,0.001,0.001,
                                 0.679544,1,0.0434757,0.001,0.830298,1,0.676863,0.001,0.001,0.556089,1,0.254077,
                                 0.001,0.001,0.832268,1,0.683958,0.001,0.309687,1,0.541891,0.001,0.001,0.001,
                                 0.829907,1,0.746255,0.001,1,0.65662,0.001,0.001,0.001,0.001,0.830711,1,0.525641,
                                 0.883763,0.896486,0.001,0.001,0.001,0.001,0.001,0.829215,1,0.507522,0.976829,
                                 0.001,0.001,0.001,0.001,0.001,0.001,0.830652,1,1,0.001,0.001,0.001,0.001,0.001,
                                 0.001,0.001,0.82834,0.512942,1,0.294769,0.001,0.001,0.001,0.001,0.001,0.001,
                                 0.832023,0.001,0.54638,1,0.353247,0.001,0.001,0.001,0.001,0.001,0.827356,0.001,
                                 0.001,0.464756,1,0.443837,0.001,0.001,0.001,0.001,0.836435,0.001,0.001,0.001,
                                 0.398335,1,0.507749,0.001,0.001,0.001,0.822803,0.001,0.001,0.001,0.001,0.30485,
                                 1,0.567573,0.001,0.001,0.849237,0.001,0.001,0.001,0.001,0.001,0.200601,1,
                                 0.628693,0.001,0.809416,0.001,0.001,0.001,0.001,0.001,0.001,0.0894376,1,
                                 0.671411,0.784094,0.001,0.001,0.001,0.001,0.001,0.001,0.001,0.001,1,1};
    return tGold;
}

TEST(PlatoTest, UnconstrainedMethodMovingAsymptotesDataMng)
{
    // create data factory
    auto tDataFactory = std::make_shared<Plato::DataFactory<double>>();
    tDataFactory->allocateObjFuncValues(1/* num objective functions */);
    tDataFactory->allocateControl(10/* num controls */,1/* num vectors */);
    
    Plato::UnconstrainedMethodMovingAsymptotesDataMng<double> tDataMng(tDataFactory);
    
    // test size of vector containers
    const double tTolerance = 1e-6;
    ASSERT_NEAR(-1.0,tDataMng.mCurrentObjFuncValue,tTolerance);
    ASSERT_NEAR(-2.0,tDataMng.mPreviousObjFuncValue,tTolerance);
    
    // test size of multi-vector container
    ASSERT_EQ(1u,tDataMng.mMoveLimits->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mDeltaControl->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mLowerAsymptotes->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mUpperAsymptotes->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mCurrentControls->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mPreviousControls->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mThirdLastControls->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mSubProbBetaBounds->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mSubProbAlphaBounds->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mControlLowerBounds->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mControlUpperBounds->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mApproximationFunctionP->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mApproximationFunctionQ->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mDeltaDynamicControlBounds->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mDynamicControlUpperBounds->getNumVectors());
    ASSERT_EQ(1u,tDataMng.mDynamicControlLowerBounds->getNumVectors());

    // test vector size
    ASSERT_EQ(10u,tDataMng.mMoveLimits->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mDeltaControl->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mLowerAsymptotes->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mUpperAsymptotes->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mCurrentControls->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mPreviousControls->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mThirdLastControls->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mSubProbBetaBounds->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mSubProbAlphaBounds->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mControlLowerBounds->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mControlUpperBounds->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mApproximationFunctionP->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mApproximationFunctionQ->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mDeltaDynamicControlBounds->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mDynamicControlUpperBounds->operator[](0).size());
    ASSERT_EQ(10u,tDataMng.mDynamicControlLowerBounds->operator[](0).size());

    // test size of multi-vector lists
    ASSERT_EQ(1u,tDataMng.mCurrentObjectiveGradient->getNumVectors() /* num vectors */);
    ASSERT_EQ(1u,tDataMng.mPreviousObjectiveGradient->getNumVectors() /* num vectors */);

    ASSERT_EQ(10u,tDataMng.mCurrentObjectiveGradient->operator[](0).size() /* num vectors */);
    ASSERT_EQ(10u,tDataMng.mPreviousObjectiveGradient->operator[](0).size() /* num vectors */);

    // test reduction operation
    Plato::fill(1.0,tDataMng.mDeltaControl.operator*());
    auto tSum = tDataMng.mReductionOps->sum(tDataMng.mDeltaControl->operator[](0));
    ASSERT_NEAR(10.0,tSum,tTolerance);
}

TEST(PlatoTest, is_diagnostic_file_close)
{
    std::ofstream tOutputStream;
    Plato::CommWrapper tComm;
    tComm.useDefaultComm();
    // CAN'T RUN OPTION SINCE IT WILL FORCE CODE TO STOP
    //ASSERT_THROW(Plato::is_diagnostic_file_close(tComm,tOutputStream),std::runtime_error);
    tOutputStream.open("dummy.txt");
    ASSERT_NO_THROW(Plato::is_diagnostic_file_close(tComm,tOutputStream));
    tOutputStream.close();
}

TEST(PlatoTest, write_umma_diagnostics_header_plus_data)
{
    Plato::CommWrapper tComm;
    tComm.useDefaultComm();

    Plato::OutputDataUMMA<double> tDiagnosticsData;
    tDiagnosticsData.mFeasibility = 1.8e-1;
    tDiagnosticsData.mObjFuncEvals = 33;
    tDiagnosticsData.mObjGradEvals = 33;
    tDiagnosticsData.mNumOuterIter = 33;
    tDiagnosticsData.mControlChange = 3.3e-2;
    tDiagnosticsData.mObjFuncChange = 5.2358e-3;
    tDiagnosticsData.mNormObjFuncGrad = 1.5983e-4;
    tDiagnosticsData.mCurrentObjFuncValue = 1.235e-3;

    std::ofstream tOutputStream;
    tOutputStream.open("dummy.txt");
    ASSERT_NO_THROW(Plato::write_umma_diagnostics_header(tComm,tDiagnosticsData,tOutputStream));
    ASSERT_NO_THROW(Plato::write_umma_diagnostics(tComm,tDiagnosticsData,tOutputStream));

    auto tReadData = PlatoTest::read_data_from_file("dummy.txt");
    auto tGold = std::string("IterF-countG-countF(X)Norm(F')abs(dX)abs(dF)") 
               + "3333331.235000e-031.598300e-043.300000e-025.235800e-03";
    ASSERT_STREQ(tReadData.str().c_str(),tGold.c_str());

    auto tTrash = std::system("rm dummy.txt");
    Plato::Utils::ignore_unused(tTrash);

    tOutputStream.close();
}

TEST(PlatoTest, get_umma_stop_criterion_description)
{
    // option 1
    std::string tMsg = Plato::get_umma_stop_criterion_description(Plato::algorithm::stop_t::CONTROL_STAGNATION);
    auto tGold = std::string("\n\n****** Optimization stopping due to control (i.e. design variable) stagnation. ******\n\n");
    EXPECT_STREQ(tMsg.c_str(),tGold.c_str());

    // option 2
    tMsg = Plato::get_umma_stop_criterion_description(Plato::algorithm::stop_t::MAX_NUMBER_ITERATIONS);
    tGold = std::string("\n\n****** Optimization stopping due to exceeding maximum number of iterations. ******\n\n");
    EXPECT_STREQ(tMsg.c_str(),tGold.c_str());

    // option 3
    tMsg = Plato::get_umma_stop_criterion_description(Plato::algorithm::stop_t::NOT_CONVERGED);
    tGold = std::string("\n\n****** Optimization algorithm did not converge. ******\n\n");
    EXPECT_STREQ(tMsg.c_str(),tGold.c_str());

    // option 4
    tMsg = Plato::get_umma_stop_criterion_description(Plato::algorithm::stop_t::STATIONARITY_MEASURE);
    tGold = std::string("\n\n****** ERROR: Optimization algorithm stopping due to undefined behavior. ******\n\n");
    EXPECT_STREQ(tMsg.c_str(),tGold.c_str());
}

TEST(PlatoTest, PERF_UnconstrainedMethodMovingAsymptotes)
{
    // create interface to linear elasticity solver
    const double tPoissonRatio = 0.3;
    const double tElasticModulus = 1;
    const int tNumElementsXdirection = 30;
    const int tNumElementsYdirection = 10;
    auto tLinearElasticitySolver = std::make_shared<Plato::StructuralTopologyOptimization>(
        tPoissonRatio, tElasticModulus, tNumElementsXdirection, tNumElementsYdirection
    ); 
    // * set force vector
    const int tGlobalNumDofs = tLinearElasticitySolver->getGlobalNumDofs();
    Epetra_SerialDenseVector tForce(tGlobalNumDofs);
    const int tDOFsIndex = 1;
    tForce[tDOFsIndex] = -1;
    tLinearElasticitySolver->setForceVector(tForce);
    // * set fixed degrees of freedom (DOFs) vector
    std::vector<double> tDofs = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 681};
    Epetra_SerialDenseVector tFixedDOFs(Epetra_DataAccess::Copy, tDofs.data(), tDofs.size());
    tLinearElasticitySolver->setFixedDOFs(tFixedDOFs);
    tLinearElasticitySolver->setFilterRadius(1.75);
    // * create augmented Lagrangian objective function
    std::shared_ptr<Plato::Criterion<double,size_t>> tDesignCriterion = 
        std::make_shared< Plato::ProxyAugLagObjective<double,size_t> >(tLinearElasticitySolver);
    
    // create data factory
    const size_t tNumVectors = 1;
    const size_t tNumControls = tLinearElasticitySolver->getNumDesignVariables();
    Plato::AlgorithmInputsUMMA<double> tInputs;
    tInputs.mControlChangeTolerance = 1e-4;
    const double tValue = tLinearElasticitySolver->getVolumeFraction();
    tInputs.mUpperBounds  = std::make_shared<Plato::EpetraSerialDenseMultiVector<double>>(tNumVectors, tNumControls, 1.0 /* base value */);
    tInputs.mLowerBounds  = std::make_shared<Plato::EpetraSerialDenseMultiVector<double>>(tNumVectors, tNumControls, 1e-3 /* base value */);
    tInputs.mInitialGuess = std::make_shared<Plato::EpetraSerialDenseMultiVector<double>>(tNumVectors, tNumControls, tValue);
    
    // create optimization algorithm and solve
    Plato::AlgorithmOutputsUMMA<double> tOutputs;
    Plato::solve_unconstrained_method_moving_asymptotes(tDesignCriterion,tInputs,tOutputs);

    // test diagnostics
    ASSERT_EQ(25u, tOutputs.mNumOuterIterations);
    ASSERT_EQ(126u,tOutputs.mNumObjFuncEvals);
    ASSERT_EQ(126u,tOutputs.mNumObjGradEvals);

    // test solution
    const double tTolerance = 1e-6;
    auto tGold = PlatoTest::get_gold_control_umma_test();
    for(size_t tIndex = 0; tIndex < tOutputs.mSolution->operator[](0).size(); tIndex++)
    {
        ASSERT_NEAR(tGold[tIndex],tOutputs.mSolution->operator()(0/* vector index */,tIndex), tTolerance);
    }            
}

}
// namespace PlatoTest