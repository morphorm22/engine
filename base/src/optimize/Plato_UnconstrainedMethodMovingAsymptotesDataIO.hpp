#pragma once

#include "Plato_Criterion.hpp"
#include "Plato_UnconstrainedMethodMovingAsymptotes.hpp"

namespace Plato
{

/******************************************************************************//**
 * @brief Output for the Unconstrained Method of Moving Asymptotes (UMMA) algorithm.
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
struct AlgorithmOutputsUMMA
{
    std::string mStopCriterion;      /*!< stopping criterion */

    OrdinalType mNumObjFuncEvals;    /*!< number of objective function evaluations */
    OrdinalType mNumObjGradEvals;    /*!< number of objective gradient evaluations */
    OrdinalType mNumOuterIterations; /*!< number of outer iterations */

    ScalarType mObjFuncValue;        /*!< objective function value */
    ScalarType mControlChange;       /*!< stagnation in two subsequent control fields */
    ScalarType mObjectiveChange;     /*!< stagnation in two subsequent objective function evaluations */
    ScalarType mNormObjFuncGrad;     /*!< Euclidean norm of the objective function gradient */

    std::shared_ptr<Plato::MultiVector<ScalarType,OrdinalType>> mSolution; /*!< optimized controls */
};
// struct AlgorithmOutputsUMMA

/******************************************************************************//**
 * @brief Inputs for the Unconstrained Method of MOving Asymptotes (UMMA) algorithm.
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
struct AlgorithmInputsUMMA
{
    /******************************************************************************//**
     * @brief Constructor
    **********************************************************************************/
    AlgorithmInputsUMMA() :
        mComm(),
        mLowerBounds(nullptr),
        mUpperBounds(nullptr),
        mInitialGuess(nullptr),
        mReductionOperations(std::make_shared<Plato::StandardVectorReductionOperations<ScalarType, OrdinalType>>())
    {
        mComm.useDefaultComm();
    }

    /******************************************************************************//**
     * @brief Destructor
    **********************************************************************************/
    virtual ~AlgorithmInputsUMMA()
    {}

    bool mWriteDiagnostics = false; /*!< write diagnostics to file */

    OrdinalType mNumControlVectors = 1;          /*!< number of control vectors */
    OrdinalType mMaxNumOuterIterations = 150;    /*!< maximum number of outer iterations */
    OrdinalType mMaxNumSubProblemIterations = 5; /*!< maximum number of subproblem iterations */

    ScalarType mMoveLimit                   = 0.15;  /*!< move limit */
    ScalarType mAsymptotesConstant          = 0.2;   /*!< constant asymptotes multiplier */
    ScalarType mControlChangeTolerance      = 1e-3;  /*!< control change tolerance, stopping criterion */ 
    ScalarType mObjectiveChangeTolerance    = 1e-10; /*!< objective change tolerance, stopping criterion */
    ScalarType mAsymptotesInitialMultiplier = 0.2;   /*!< initial asymptotes increment/decrement multiplier */
    ScalarType mAsymptotesIncrementConstant = 1.2;   /*!< asymptotes increment multiplier */
    ScalarType mAsymptotesDecrementConstant = 0.7;   /*!< asymptotes decrement multiplier */

    Plato::CommWrapper mComm; /*!< interface to MPI communicator */
    Plato::MemorySpace::type_t mMemorySpace = Plato::MemorySpace::HOST; /*!< memory space: HOST OR DEVICE */

    std::shared_ptr<Plato::MultiVector<ScalarType,OrdinalType>> mLowerBounds;  /*!< lower bounds */
    std::shared_ptr<Plato::MultiVector<ScalarType,OrdinalType>> mUpperBounds;  /*!< upper bounds */
    std::shared_ptr<Plato::MultiVector<ScalarType,OrdinalType>> mInitialGuess; /*!< initial guess */
    
    /*!< operations which require communication across processors, e.g. max, min, global sum */
    std::shared_ptr<Plato::ReductionOperations<ScalarType,OrdinalType>> mReductionOperations;
};
// struct AlgorithmInputsUMMA

/******************************************************************************//**
 * @brief Set inputs for Unconstrained Method of Moving Asymptotes (UMMA) algorithm.
 * @param [in]  aInputs    UMMA inputs
 * @param [out] aAlgorithm UMMA algorithm interface
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
inline void set_uconstrained_mma_algorithm_inputs
(const Plato::AlgorithmInputsUMMA<ScalarType,OrdinalType>                 &aInputs,
       Plato::UnconstrainedMethodMovingAsymptotes<ScalarType,OrdinalType> &aAlgorithm)
{
    aAlgorithm.writeDiagnostics(aInputs.mWriteDiagnostics);

    aAlgorithm.setMaxNumOuterIterations(aInputs.mMaxNumOuterIterations);
    aAlgorithm.setMaxNumSubProbIterations(aInputs.mMaxNumSubProblemIterations);
 
    aAlgorithm.setMoveLimit(aInputs.mMoveLimit);
    aAlgorithm.setAsymptotesConstant(aInputs.mAsymptotesConstant);
    aAlgorithm.setControlChangeTolerance(aInputs.mControlChangeTolerance);
    aAlgorithm.setObjectiveChangeTolerance(aInputs.mObjectiveChangeTolerance);
    aAlgorithm.setAsymptotesInitialMultiplier(aInputs.mAsymptotesInitialMultiplier);
    aAlgorithm.setAsymptotesIncrementConstant(aInputs.mAsymptotesIncrementConstant);
    aAlgorithm.setAsymptotesDecrementConstant(aInputs.mAsymptotesDecrementConstant);

    aAlgorithm.setLowerBounds(aInputs.mLowerBounds.operator*());
    aAlgorithm.setUpperBounds(aInputs.mUpperBounds.operator*());
    aAlgorithm.setInitialGuess(aInputs.mInitialGuess.operator*());
}
// function set_uconstrained_mma_algorithm_inputs

/******************************************************************************//**
 * @brief Set outputs for Unconstrained Method of Moving Asymptotes (UMMA) algorithm.
 * @param [in]  aAlgorithm UMMA algorithm interface
 * @param [out] aOutputs   UMMA outputs
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
inline void set_unconstrained_mma_algorithm_outputs
(const Plato::UnconstrainedMethodMovingAsymptotes<ScalarType,OrdinalType> &aAlgorithm,
       Plato::AlgorithmOutputsUMMA<ScalarType,OrdinalType>                &aOutputs)
{
    aOutputs.mNumOuterIterations = aAlgorithm.getCurrentOuterIteration();

    aOutputs.mNumObjFuncEvals = aAlgorithm.getNumObjFuncEvals();
    aOutputs.mNumObjGradEvals = aAlgorithm.getNumObjGradEvals();

    aOutputs.mObjFuncValue    = aAlgorithm.getCurrentObjFuncValue();
    aOutputs.mControlChange   = aAlgorithm.getControlChange();
    aOutputs.mNormObjFuncGrad = aAlgorithm.getNormObjFuncGrad();
    aOutputs.mObjectiveChange = aAlgorithm.getObjectiveChange();

    Plato::get_stop_criterion(aAlgorithm.getStoppingCriterion(), aOutputs.mStopCriterion);
    const Plato::MultiVector<ScalarType,OrdinalType>& tSolution = aAlgorithm.getSolution();
    aOutputs.mSolution = tSolution.create();
    Plato::update(1.0, tSolution, 0.0, aOutputs.mSolution.operator*());
}
// function set_optimality_criteria_algorithm_outputs

/******************************************************************************//**
 * @brief Interface for Unconstrasined Method of Moving Asymptotes algorithm 
 * @param [in]  aObjective objective function interface
 * @param [in]  aInputs   UMMA inputs
 * @param [out] aOutputs  UMMA outputs
**********************************************************************************/
template<typename ScalarType, typename OrdinalType>
inline void solve_unconstrained_method_moving_asymptotes(
 const std::shared_ptr<Plato::Criterion<ScalarType,OrdinalType>> &aObjective,
 const Plato::AlgorithmInputsUMMA<ScalarType,OrdinalType>        &aInputs,
       Plato::AlgorithmOutputsUMMA<ScalarType,OrdinalType>       &aOutputs
)
{
    // create data structures 
    auto tDataFactory = std::make_shared<Plato::DataFactory<ScalarType, OrdinalType>>();
    tDataFactory->setCommWrapper(aInputs.mComm);
    tDataFactory->allocateControl(aInputs.mInitialGuess.operator*());
    tDataFactory->allocateObjFuncValues(1/* num objective functions */);
    tDataFactory->allocateControlReductionOperations(aInputs.mReductionOperations.operator*());
    // create algorithm interface
    Plato::UnconstrainedMethodMovingAsymptotes<ScalarType,OrdinalType> tAlgorithm(aObjective,tDataFactory);
    Plato::set_uconstrained_mma_algorithm_inputs(aInputs,tAlgorithm);
    tAlgorithm.solve();
    Plato::set_unconstrained_mma_algorithm_outputs(tAlgorithm,aOutputs);
}
// function solve_unconstrained_method_moving_asymptotes

}
// namespace Plato