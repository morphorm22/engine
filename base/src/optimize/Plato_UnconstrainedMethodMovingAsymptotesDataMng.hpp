#pragma once

#include "Plato_DataFactory.hpp"
#include "Plato_MultiVectorList.hpp"
#include "Plato_ReductionOperations.hpp"

namespace Plato
{

/******************************************************************************//**
 * @tparam ScalarType  type for non-integer values
 * @tparam OrdinalType type for integer values
 * @class UnconstrainedMethodMovingAsymptotesDataMng
 * 
 * @brief Data structures for unconstrained method of moving asymptotes algorithm.
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
struct UnconstrainedMethodMovingAsymptotesDataMng
{
private:
    typedef Plato::MultiVector<ScalarType, OrdinalType> MultiVec;
    typedef Plato::MultiVectorList<ScalarType, OrdinalType> MultiVecList;
    typedef Plato::ReductionOperations<ScalarType, OrdinalType> ReductionOps;

public:
    /******************************************************************************//**
     * @brief Constructor
     * @param [in] aDataFactory const shared pointer reference to data factory
    **********************************************************************************/
    UnconstrainedMethodMovingAsymptotesDataMng
    (const std::shared_ptr<Plato::DataFactory<ScalarType, OrdinalType>> & aDataFactory) : 
        mNumObjFuncs(aDataFactory->getNumObjFuncs()),
        mMoveLimits(aDataFactory->control().create()),
        mDeltaControl(aDataFactory->control().create()),
        mLowerAsymptotes(aDataFactory->control().create()),
        mUpperAsymptotes(aDataFactory->control().create()),
        mCurrentControls(aDataFactory->control().create()),
        mPreviousControls(aDataFactory->control().create()),
        mThirdLastControls(aDataFactory->control().create()),
        mSubProbBetaBounds(aDataFactory->control().create()),
        mSubProbAlphaBounds(aDataFactory->control().create()),
        mControlLowerBounds(aDataFactory->control().create()),
        mControlUpperBounds(aDataFactory->control().create()),
        mApproximationFunctionP(aDataFactory->control().create()),
        mApproximationFunctionQ(aDataFactory->control().create()),
        mDeltaDynamicControlBounds(aDataFactory->control().create()),
        mDynamicControlLowerBounds(aDataFactory->control().create()),
        mDynamicControlUpperBounds(aDataFactory->control().create()),
        mCurrentObjectiveGradient(aDataFactory->control().create()),
        mPreviousObjectiveGradient(aDataFactory->control().create()),
        mComm(aDataFactory->getCommWrapper().create()),
        mReductionOps(aDataFactory->getControlReductionOperations().create())
    {}

    /******************************************************************************//**
     * @brief Destructor
    **********************************************************************************/
    ~UnconstrainedMethodMovingAsymptotesDataMng(){}

public:
    OrdinalType mNumObjFuncs = 1; /*!< total number of objective functions */

    ScalarType mCurrentObjFuncValue   = -1.0; /*!< current objective values */
    ScalarType mPreviousObjFuncValue  = -2.0; /*!< previous objective values */

    std::shared_ptr<MultiVec> mMoveLimits; /*!< vector of move limits */
    std::shared_ptr<MultiVec> mDeltaControl; /*!< delta control, misfit between two subsequent control solutions */
    std::shared_ptr<MultiVec> mLowerAsymptotes; /*!< lower asymptotes */
    std::shared_ptr<MultiVec> mUpperAsymptotes; /*!< upper asymptotes */
    std::shared_ptr<MultiVec> mCurrentControls; /*!< current control variables */
    std::shared_ptr<MultiVec> mPreviousControls; /*!< previous control variables */
    std::shared_ptr<MultiVec> mThirdLastControls; /*!< third to last control variables */
    std::shared_ptr<MultiVec> mSubProbBetaBounds; /*!< subproblem control upper bounds */
    std::shared_ptr<MultiVec> mSubProbAlphaBounds; /*!< subproblem control lower bounds */
    std::shared_ptr<MultiVec> mControlLowerBounds; /*!< control lower bounds */
    std::shared_ptr<MultiVec> mControlUpperBounds; /*!< control upper bounds */
    std::shared_ptr<MultiVec> mApproximationFunctionP; /*!< approximation functions P for mma subproblem */
    std::shared_ptr<MultiVec> mApproximationFunctionQ; /*!< approximation functions Q for mma subproblem */
    std::shared_ptr<MultiVec> mDeltaDynamicControlBounds; /*!< dynamic delta control bounds */
    std::shared_ptr<MultiVec> mDynamicControlUpperBounds; /*!< dynamic control upper bounds */
    std::shared_ptr<MultiVec> mDynamicControlLowerBounds; /*!< dynamic control lower bounds */

    /*!< current and previous objective gradients with respect to the design variables */
    std::shared_ptr<MultiVec> mCurrentObjectiveGradient;    
    std::shared_ptr<MultiVec> mPreviousObjectiveGradient;

    std::shared_ptr<Plato::CommWrapper> mComm;   /*!< interface to MPI communicator */
    std::shared_ptr<ReductionOps> mReductionOps; /*!< interface to math reduction operations */

private:
    UnconstrainedMethodMovingAsymptotesDataMng(
        const Plato::UnconstrainedMethodMovingAsymptotesDataMng<ScalarType, OrdinalType> & aRhs);
    Plato::UnconstrainedMethodMovingAsymptotesDataMng<ScalarType, OrdinalType> 
        & operator=(const Plato::UnconstrainedMethodMovingAsymptotesDataMng<ScalarType, OrdinalType> & aRhs);
};
// class UnconstrainedMethodMovingAsymptotesDataMng

} 
// namespace Plato
