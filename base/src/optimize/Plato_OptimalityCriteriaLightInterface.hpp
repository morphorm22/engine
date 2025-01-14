/*
//@HEADER
// *************************************************************************
//   Plato Engine v.1.0: Copyright 2018, National Technology & Engineering
//                    Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Sandia Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact the Plato team (plato3D-help@sandia.gov)
//
// *************************************************************************
//@HEADER
*/

/*
 * Plato_OptimalityCriteriaLightInterface.hpp
 *
 *  Created on: Sep 30, 2018
 */

#pragma once

#include "Plato_DataFactory.hpp"
#include "Plato_OptimalityCriteria.hpp"
#include "Plato_OptimalityCriteriaDataMng.hpp"
#include "Plato_OptimalityCriteriaStageMng.hpp"
#include "Plato_NonlinearProgrammingSubProblemOC.hpp"

namespace Plato
{

/******************************************************************************//**
 * @brief Output data structure for the Optimality Criteria (OC) algorithm
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
struct AlgorithmOutputsOC
{
    OrdinalType mNumOuterIter; /*!< number of outer iterations */
    OrdinalType mNumObjFuncEval; /*!< number of objective function evaluations */
    OrdinalType mNumObjGradEval; /*!< number of objective gradient evaluations */

    ScalarType mObjFuncValue; /*!< objective function value */
    ScalarType mNormObjFuncGrad; /*!< norm of the objective function gradient */
    ScalarType mControlStagnationMeasure; /*!< norm of the difference between two subsequent control fields */
    ScalarType mObjectiveStagnationMeasure; /*!< measures stagnation in two subsequent objective function evaluations */

    std::string mStopCriterion; /*!< stopping criterion */

    std::shared_ptr<Plato::Vector<ScalarType,OrdinalType>> mConstraints; /*!< constraint values */
    std::shared_ptr<Plato::MultiVector<ScalarType,OrdinalType>> mSolution; /*!< optimal solution */
};
// struct AlgorithmOutputsOC

/******************************************************************************//**
 * @brief Input data structure for the Optimality Criteria (OC) algorithm
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
struct AlgorithmInputsOC
{
    /******************************************************************************//**
     * @brief Default constructor
    **********************************************************************************/
    AlgorithmInputsOC() :
            mPrintDiagnostics(false),
            mMaxNumIter(500),
            mMoveLimit(0.2),
            mScaleFactor(0.01),
            mDampingPower(0.5),
            mDualLowerBound(0),
            mDualUpperBound(1e7),
            mBisectionTolerance(1e-4),
            mFeasibilityTolerance(1e-5),
            mObjectiveGradientTolerance(1e-8),
            mControlStagnationTolerance(1e-2),
            mObjectiveStagnationTolerance(1e-5),
            mCommWrapper(),
            mMemorySpace(Plato::MemorySpace::HOST),
            mDual(nullptr),
            mLowerBounds(nullptr),
            mUpperBounds(nullptr),
            mInitialGuess(nullptr),
            mReductionOperations(std::make_shared<Plato::StandardVectorReductionOperations<ScalarType, OrdinalType>>())
    {
        mCommWrapper.useDefaultComm();
    }

    /******************************************************************************//**
     * @brief Default destructor
    **********************************************************************************/
    virtual ~AlgorithmInputsOC()
    {
    }

    bool mPrintDiagnostics; /*!< flag to enable problem statistics output (default=false) */

    OrdinalType mMaxNumIter; /*!< maximum number of outer iterations */

    ScalarType mMoveLimit; /*!< control move limit */
    ScalarType mScaleFactor; /*!< control update scale factor */
    ScalarType mDampingPower; /*!< damping exponent */
    ScalarType mDualLowerBound; /*!< Lagrange multiplier lower bounds */
    ScalarType mDualUpperBound; /*!< Lagrange multiplier upper bounds */

    ScalarType mBisectionTolerance; /*!< bisection tolerance */
    ScalarType mFeasibilityTolerance; /*!< feasibility tolerance */
    ScalarType mObjectiveGradientTolerance; /*!< gradient norm tolerance */
    ScalarType mControlStagnationTolerance; /*!< control stagnation tolerance */
    ScalarType mObjectiveStagnationTolerance; /*!< objective function stagnation tolerance */

    Plato::CommWrapper mCommWrapper; /*!< distributed memory communication wrapper */
    Plato::MemorySpace::type_t mMemorySpace; /*!< memory space: HOST (default) OR DEVICE */

    std::shared_ptr<Plato::MultiVector<ScalarType,OrdinalType>> mDual; /*!< Lagrange multipliers */
    std::shared_ptr<Plato::MultiVector<ScalarType,OrdinalType>> mLowerBounds; /*!< lower bounds */
    std::shared_ptr<Plato::MultiVector<ScalarType,OrdinalType>> mUpperBounds; /*!< upper bounds */
    std::shared_ptr<Plato::MultiVector<ScalarType,OrdinalType>> mInitialGuess; /*!< initial guess */
    /*!< operations which require communication across processors, e.g. max, min, global sum */
    std::shared_ptr<Plato::ReductionOperations<ScalarType,OrdinalType>> mReductionOperations;
};
// struct AlgorithmInputsOC

/******************************************************************************//**
 * @brief Set Optimality Criteria (OC) algorithm inputs
 * @param [in] aInputs Optimality Criteria algorithm inputs
 * @param [in,out] aStepMng Optimality Criteria subproblem interface
 * @param [in,out] aAlgorithm Optimality Criteria algorithm interface
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
inline void set_optimality_criteria_algorithm_inputs(const Plato::AlgorithmInputsOC<ScalarType, OrdinalType> & aInputs,
                                                     Plato::NonlinearProgrammingSubProblemOC<ScalarType, OrdinalType> & aStepMng,
                                                     Plato::OptimalityCriteria<ScalarType, OrdinalType> & aAlgorithm)
{
    if(aInputs.mPrintDiagnostics == true)
    {
        aAlgorithm.enableDiagnostics();
    }

    aAlgorithm.setMaxNumIterations(aInputs.mMaxNumIter);
    aAlgorithm.setFeasibilityTolerance(aInputs.mFeasibilityTolerance);
    aAlgorithm.setControlStagnationTolerance(aInputs.mControlStagnationTolerance);
    aAlgorithm.setObjectiveGradientTolerance(aInputs.mObjectiveGradientTolerance);
    aAlgorithm.setObjectiveStagnationTolerance(aInputs.mObjectiveStagnationTolerance);

    aStepMng.setMoveLimit(aInputs.mMoveLimit);
    aStepMng.setScaleFactor(aInputs.mScaleFactor);
    aStepMng.setDampingPower(aInputs.mDampingPower);
    aStepMng.setDualLowerBound(aInputs.mDualLowerBound);
    aStepMng.setDualUpperBound(aInputs.mDualUpperBound);
    aStepMng.setBisectionTolerance(aInputs.mBisectionTolerance);
}
// function set_optimality_criteria_algorithm_inputs

/******************************************************************************//**
 * @brief Set Optimality Criteria (OC) algorithm outputs
 * @param [in] aAlgorithm Optimality Criteria algorithm interface
 * @param [in,out] aOutputs Optimality Criteria algorithm outputs
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
inline void set_optimality_criteria_algorithm_outputs(const Plato::OptimalityCriteria<ScalarType, OrdinalType> & aAlgorithm,
                                                      Plato::AlgorithmOutputsOC<ScalarType, OrdinalType> & aOutputs)
{
    aOutputs.mNumOuterIter = aAlgorithm.getNumIterationsDone();
    aOutputs.mNumObjFuncEval = aAlgorithm.getDataMng().getNumObjectiveFunctionEvaluations();
    aOutputs.mNumObjGradEval = aAlgorithm.getDataMng().getNumObjectiveGradientEvaluations();

    aOutputs.mObjFuncValue = aAlgorithm.getDataMng().getCurrentObjectiveValue();
    aOutputs.mNormObjFuncGrad = aAlgorithm.getDataMng().getNormObjectiveGradient();
    aOutputs.mControlStagnationMeasure = aAlgorithm.getDataMng().getControlStagnationMeasure();
    aOutputs.mObjectiveStagnationMeasure = aAlgorithm.getDataMng().getObjectiveStagnationMeasure();

    Plato::get_stop_criterion(aAlgorithm.getDataMng().getStopCriterion(), aOutputs.mStopCriterion);

    const Plato::MultiVector<ScalarType, OrdinalType> & tSolution = aAlgorithm.getDataMng().getCurrentControl();
    aOutputs.mSolution = tSolution.create();
    Plato::update(static_cast<ScalarType>(1), tSolution, static_cast<ScalarType>(0), *aOutputs.mSolution);

    const Plato::Vector<ScalarType, OrdinalType> & tConstraintValues = aAlgorithm.getDataMng().getCurrentConstraintValues();
    aOutputs.mConstraints = tConstraintValues.create();
    aOutputs.mConstraints->update(static_cast<ScalarType>(1), tConstraintValues, static_cast<ScalarType>(0));
}
// function set_optimality_criteria_algorithm_outputs

/******************************************************************************//**
 * @brief Optimality Criteria (OC) algorithm interface
 * @param [in] aObjective user-defined objective function
 * @param [in] aConstraints user-defined list of constraints
 * @param [in] aInputs Optimality Criteria algorithm inputs
 * @param [in,out] aOutputs Optimality Criteria algorithm outputs
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
inline void solve_optimality_criteria(const std::shared_ptr<Plato::Criterion<ScalarType, OrdinalType>> & aObjective,
                                      const std::shared_ptr<Plato::CriterionList<ScalarType, OrdinalType>> & aConstraints,
                                      const Plato::AlgorithmInputsOC<ScalarType, OrdinalType> & aInputs,
                                      Plato::AlgorithmOutputsOC<ScalarType, OrdinalType> & aOutputs)
{
    // ********* ALLOCATE DATA STRUCTURES *********
    std::shared_ptr<Plato::DataFactory<ScalarType, OrdinalType>> tDataFactory;
    tDataFactory = std::make_shared<Plato::DataFactory<ScalarType, OrdinalType>>();
    tDataFactory->setCommWrapper(aInputs.mCommWrapper);
    tDataFactory->allocateDual(*aInputs.mDual);
    tDataFactory->allocateControl(*aInputs.mInitialGuess);
    tDataFactory->allocateControlReductionOperations(*aInputs.mReductionOperations);

    // ********* ALLOCATE OPTIMALITY CRITERIA ALGORITHM DATA MANAGER *********
    std::shared_ptr<Plato::OptimalityCriteriaDataMng<ScalarType, OrdinalType>> tDataMng;
    tDataMng = std::make_shared<Plato::OptimalityCriteriaDataMng<ScalarType, OrdinalType>>(tDataFactory);
    tDataMng->setInitialGuess(*aInputs.mInitialGuess);
    tDataMng->setControlLowerBounds(*aInputs.mLowerBounds);
    tDataMng->setControlUpperBounds(*aInputs.mUpperBounds);

    // ********* ALLOCATE STAGE MANAGER MANAGER *********
    std::shared_ptr<Plato::OptimalityCriteriaStageMng<ScalarType, OrdinalType>> tStageMng;
    tStageMng = std::make_shared<Plato::OptimalityCriteriaStageMng<ScalarType, OrdinalType>>(tDataFactory, aObjective, aConstraints);

    // ********* ALLOCATE OPTIMALITY CRITERIA ALGORITHM AND SOLVE OPTIMIZATION PROBLEM *********
    std::shared_ptr<Plato::NonlinearProgrammingSubProblemOC<ScalarType, OrdinalType>> tSubProblem;
    tSubProblem = std::make_shared<Plato::NonlinearProgrammingSubProblemOC<ScalarType, OrdinalType>>(tDataFactory);
    Plato::OptimalityCriteria<ScalarType, OrdinalType> tAlgorithm(tDataMng, tStageMng, tSubProblem);
    Plato::set_optimality_criteria_algorithm_inputs(aInputs, *tSubProblem, tAlgorithm);
    tAlgorithm.solve();
    Plato::set_optimality_criteria_algorithm_outputs(tAlgorithm, aOutputs);
}
// function solve_optimality_criteria

} // namespace Plato
