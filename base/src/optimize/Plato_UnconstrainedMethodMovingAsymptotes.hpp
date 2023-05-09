#pragma once

#include <memory>

#include "Plato_Types.hpp"
#include "Plato_Console.hpp"
#include "Plato_LinearAlgebra.hpp"
#include "Plato_UnconstrainedMethodMovingAsymptotesFileIO.hpp"
#include "Plato_UnconstrainedMethodMovingAsymptotesDataMng.hpp"

namespace Plato
{

/******************************************************************************//**
 * @tparam ScalarType  type for non-integer values
 * @tparam OrdinalType type for integer values
 * @class UnconstrainedMethodMovingAsymptotes
 * 
 * @brief Solve optimization problem using the unconstrained Method of Moving 
 * Asymptotes (MMA) algorihtm. This algorithm is suitable for optimization problems
 * involving a small or large number of constraints. 
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
class UnconstrainedMethodMovingAsymptotes
{
private:
    typedef Plato::UnconstrainedMethodMovingAsymptotesDataMng<ScalarType, OrdinalType> DataMng;

public:
    /******************************************************************************//**
     * @brief Constructor
     * @param [in] aCriterion interface to objective function 
     * @param [in] aDataFactory interface to factory used to build data structure 
    **********************************************************************************/
    UnconstrainedMethodMovingAsymptotes(
     const std::shared_ptr<Plato::Criterion<ScalarType, OrdinalType>>   &aCriterion,
     const std::shared_ptr<Plato::DataFactory<ScalarType, OrdinalType>> &aDataFactory
    ) : 
        mDataMng(std::make_shared<DataMng>(aDataFactory)),
        mObjective(aCriterion)
    {}

    /******************************************************************************//**
     * @brief Destructor
    **********************************************************************************/
    ~UnconstrainedMethodMovingAsymptotes(){}

    /******************************************************************************//**
     * @brief Enables/Disables output diagnostics
     * @param [in] aInput boolean value
    **********************************************************************************/
    void writeDiagnostics(const bool &aInput)
    {
        mWriteDiagnostics = aInput;
    }

    /******************************************************************************//**
     * @brief Set maximum number of outer optimization iterations
     * @param [in] aInput integer value
    **********************************************************************************/
    void setMaxNumOuterIterations(const OrdinalType &aInput)
    {
        mMaxNumOuterIterations = aInput;
    }

    /******************************************************************************//**
     * @brief Set maximum number of subproblem iterations
     * @param [in] aInput integer value
    **********************************************************************************/
    void setMaxNumSubProbIterations(const OrdinalType &aInput)
    {
        mMaxNumSubProblemIterations = aInput;
    }

    /******************************************************************************//**
     * @brief Set move limit for control variables
     * @param [in] aInput scalar value
    **********************************************************************************/
    void setMoveLimit(const ScalarType &aInput)
    {
        mMoveLimit = aInput;
    }

    /******************************************************************************//**
     * @brief Set tolerance for objective change stopping criterion
     * @param [in] aInput scalar value
    **********************************************************************************/
    void setObjectiveChangeTolerance(const ScalarType &aInput)
    {
        mObjectiveChangeTolerance = aInput;
    }

    /******************************************************************************//**
     * @brief Set tolerance for control change stopping criterion
     * @param [in] aInput scalar value
    **********************************************************************************/
    void setControlChangeTolerance(const ScalarType &aInput)
    {
        mControlChangeTolerance = aInput;
    }

    /******************************************************************************//**
     * @brief Set constant used to compute upper and lower asymptotes
     * @param [in] aInput scalar value
    **********************************************************************************/
    void setAsymptotesConstant(const ScalarType &aInput)
    {
        mAsymptotesConstant = aInput;
    }

    /******************************************************************************//**
     * @brief Set asymptotes initial decrement/increment multiplier
     * @param [in] aInput scalar value
    **********************************************************************************/
    void setAsymptotesInitialMultiplier(const ScalarType &aInput)
    {
        mAsymptotesInitialMultiplier = aInput;
    }

    /******************************************************************************//**
     * @brief Set asymptotes increment multiplier/constant
     * @param [in] aInput scalar value
    **********************************************************************************/
    void setAsymptotesIncrementConstant(const ScalarType &aInput)
    {
        mAsymptotesIncrementConstant = aInput; 
    }

    /******************************************************************************//**
     * @brief Set asymptotes decrement multiplier/constant
     * @param [in] aInput scalar value
    **********************************************************************************/
    void setAsymptotesDecrementConstant(const ScalarType &aInput)
    {
        mAsymptotesDecrementConstant = aInput;
    }

    /******************************************************************************//**
     * @brief Set lower bounds for control variables.
     * @param [in] aInput const reference to multivector container
    **********************************************************************************/
    void setLowerBounds(const Plato::MultiVector<ScalarType,OrdinalType> &aInput)
    {
        Plato::update(1.0,aInput,0.0,mDataMng->mControlLowerBounds.operator*());
    }

    /******************************************************************************//**
     * @brief Set upper bounds for control variables.
     * @param [in] aInput const reference to multivector container
    **********************************************************************************/
    void setUpperBounds(const Plato::MultiVector<ScalarType,OrdinalType> &aInput)
    {
        Plato::update(1.0,aInput,0.0,mDataMng->mControlUpperBounds.operator*());
    }

    /******************************************************************************//**
     * @brief Set initial guess for control variables.
     * @param [in] aInput const reference to multivector container
    **********************************************************************************/
    void setInitialGuess(const Plato::MultiVector<ScalarType,OrdinalType> &aInput)
    {
        Plato::update(1.0,aInput,0.0,mDataMng->mCurrentControls.operator*());
    }

    /******************************************************************************//**
     * @brief Get number of objective function evaluations
     * \return integer value
    **********************************************************************************/
    OrdinalType getNumObjFuncEvals() const
    {
        return mNumObjFuncEvals;
    }

    /******************************************************************************//**
     * @brief Get number of objective gradient evaluations performed
     * \return integer value
    **********************************************************************************/
    OrdinalType getNumObjGradEvals() const
    {
        return mNumObjGradEvals;
    }

    /******************************************************************************//**
     * @brief Get number of outer iterations performed
     * \return integer value
    **********************************************************************************/
    OrdinalType getCurrentOuterIteration() const
    {
        return mCurrentOuterIteration;
    }

    /******************************************************************************//**
     * @brief Get current objective function value
     * \return scalar value
    **********************************************************************************/
    ScalarType getCurrentObjFuncValue() const
    {
        return mDataMng->mCurrentObjFuncValue;
    }

    /******************************************************************************//**
     * @brief Get change between two subsequent control solutions 
     * \return scalar value
    **********************************************************************************/
    ScalarType getControlChange() const
    {
        return mOutputData.mControlChange;
    }

    /******************************************************************************//**
     * @brief Get change in objective function
     * \return scalar value
    **********************************************************************************/
    ScalarType getObjectiveChange() const
    {
        return mOutputData.mObjFuncChange;
    }

    /******************************************************************************//**
     * @brief Get norm of objective function gradient 
     * \return scalar value
    **********************************************************************************/
    ScalarType getNormObjFuncGrad() const
    {
        return mOutputData.mNormObjFuncGrad;
    }

    /******************************************************************************//**
     * @brief Get stopping criterion
     * \return stopping criterion
    **********************************************************************************/
    Plato::algorithm::stop_t getStoppingCriterion() const
    {
        return mStoppingCriterion;
    }

    /******************************************************************************//**
     * @brief Get solution to optimization problem, i.e.; optimal control variables
     * \param [out] aInput reference to multivector container 
    **********************************************************************************/
    void getSolution(Plato::MultiVector<ScalarType,OrdinalType> &aInput) const
    {
        Plato::update(1.0,mDataMng->mCurrentControls.operator*(),0.0,aInput);
    }

    /******************************************************************************//**
     * @brief Get solution to optimization problem, i.e.; optimal control variables
     * \return const reference to multivector container
    **********************************************************************************/
    const Plato::MultiVector<ScalarType,OrdinalType>& getSolution() const
    {
        return mDataMng->mCurrentControls.operator*();
    }
    
    /******************************************************************************//**
     * @brief Main routine: Solve optimization problem
    **********************************************************************************/
    void solve()
    {
        openWriteFile();
        while (true)
        {
            mCurrentOuterIteration += 1;
            for (decltype(mMaxNumSubProblemIterations) tSubProbIter = 0; 
                 tSubProbIter < mMaxNumSubProblemIterations; 
                 tSubProbIter++)
            {
                solveSubProblem();
            }
            auto tConverged = computeStoppingCriteria();
            if( tConverged )
            {
                finalize();
                break;
            }
            updateProblem();
        }
    }

// private functions
private:
    /******************************************************************************//**
     * @brief Solve unconstrained MMA subproblem and update control variables
    **********************************************************************************/
    void solveSubProblem()
    {
        cacheState();
        evaluateObjective();
        writeDiagnostics();
        solveUnconstrainedMethodMovingAsymptotesProblem();
    }

    /******************************************************************************//**
     * @brief Perform final operations before exiting the optimization algorithm
    **********************************************************************************/
    void finalize()
    {
        cacheState();
        evaluateObjective();
        writeDiagnostics();
        writeStoppingCriterion();
        closeWriteFile();
    }

    /******************************************************************************//**
     * @brief Call update problem, enables parameter updates of external application
     * within the optimization problem, i.e.; physics-related parameters.  
    **********************************************************************************/
    void updateProblem()
    {
        mObjective->updateProblem(mDataMng->mCurrentControls.operator*());
    }

    /******************************************************************************//**
     * @brief Open write file; i.e., diagnostics file
    **********************************************************************************/
    void openWriteFile()
    {
        if (mWriteDiagnostics == false)
        {
            return;
        }

        if (mDataMng->mComm->myProcID() == 0)
        {
            mOutputStream.open("plato_umma_algorithm_diagnostics.txt");
            Plato::is_diagnostic_file_close(mDataMng->mComm.operator*(),mOutputStream);
            Plato::write_umma_diagnostics_header(mDataMng->mComm.operator*(),mOutputData, mOutputStream);
        }
    }

    /******************************************************************************//**
     * @brief Close write file; i.e., diagnostics file
    **********************************************************************************/
    void closeWriteFile()
    {
        if (mWriteDiagnostics == false)
        {
            return;
        }

        if (mDataMng->mComm->myProcID() == 0)
        {
            mOutputStream.close();
        }
    }

    /******************************************************************************//**
     * @brief Write stopping criterion to file, why did algorithm stop
    **********************************************************************************/
    void writeStoppingCriterion()
    {
        if(mWriteDiagnostics == false)
        {
            return;
        }
        
        if(mDataMng->mComm->myProcID() == 0)
        {
            mOutputStream << Plato::get_umma_stop_criterion_description(mStoppingCriterion);
        }
    }

    /******************************************************************************//**
     * @brief Write diagnostics for unconstrained MMA algorithm to file
    **********************************************************************************/
    void writeDiagnostics()
    {
        if(mWriteDiagnostics == false)
        { return; }

        if(mDataMng->mComm->myProcID() == 0)
        {
            writeDiagnosticsToFile();
            writeDiagnosticsToConsole();
        }
    }

    /******************************************************************************//**
     * @brief Write algorithm's diagnostics to file 
    **********************************************************************************/
    void writeDiagnosticsToFile()
    {
        mOutputData.mObjFuncEvals = mNumObjFuncEvals;
        mOutputData.mObjGradEvals = mNumObjGradEvals;
        mOutputData.mNumOuterIter = mCurrentOuterIteration;
        mOutputData.mControlChange = computeDeltaControl();
        mOutputData.mObjFuncChange = computeDeltaObjFunc();
        mOutputData.mNormObjFuncGrad = computeObjFuncGradNorm();
        mOutputData.mCurrentObjFuncValue = mDataMng->mCurrentObjFuncValue;
        Plato::is_diagnostic_file_close(mDataMng->mComm.operator*(),mOutputStream);
        Plato::write_umma_diagnostics(mDataMng->mComm.operator*(),mOutputData,mOutputStream);
    }

    /******************************************************************************//**
     * @brief Write algorithm's diagnostics to console. 
    **********************************************************************************/
    void writeDiagnosticsToConsole() const
    {
        std::stringstream tConsoleStream;
        Plato::write_umma_diagnostics_header(mDataMng->mComm.operator*(),mOutputData,tConsoleStream);
        Plato::write_umma_diagnostics(mDataMng->mComm.operator*(),mOutputData, tConsoleStream);
        Plato::Console::Alert(tConsoleStream.str());
    }

    /******************************************************************************//**
     * @brief Compute change in objective function, difference between two subsequent 
     * objective function evaluations. 
    **********************************************************************************/
    ScalarType computeDeltaObjFunc() const
    {
        auto tDelta = std::abs(mDataMng->mCurrentObjFuncValue - mDataMng->mPreviousObjFuncValue);
        return tDelta;
    }

    /******************************************************************************//**
     * @brief Compute average change in control variables
    **********************************************************************************/
    ScalarType computeDeltaControl() const
    {
        Plato::update( 1.0, mDataMng->mCurrentControls.operator*() , 0.0, mDataMng->mDeltaControl.operator*());
        Plato::update(-1.0, mDataMng->mPreviousControls.operator*(), 1.0, mDataMng->mDeltaControl.operator*());
        Plato::modulus(mDataMng->mDeltaControl.operator*());
        ScalarType  tSum  = Plato::sum(mDataMng->mDeltaControl.operator*(), mDataMng->mReductionOps.operator*());
        OrdinalType tSize = Plato::size(mDataMng->mDeltaControl.operator*()); 

        ScalarType tDeltaControl = tSum / static_cast<ScalarType>(tSize);
        return tDeltaControl;
    }

    /******************************************************************************//**
     * @brief Compute norm of objective function gradient
    **********************************************************************************/
    ScalarType computeObjFuncGradNorm() const
    {
        ScalarType tNorm = Plato::norm(mDataMng->mCurrentObjectiveGradient.operator*());
        return (std::sqrt(tNorm));
    }

    /******************************************************************************//**
     * @brief Compute stopping criteria
    **********************************************************************************/
    bool computeStoppingCriteria()
    {
        bool tStop = false;
        auto tDeltaControl = computeDeltaControl();
        auto tDeltaObjFunc = computeDeltaObjFunc();
        if( tDeltaControl < mControlChangeTolerance )
        {
            mStoppingCriterion = Plato::algorithm::CONTROL_STAGNATION;
            tStop = true;
        }
        else if( mCurrentOuterIteration >= mMaxNumOuterIterations )
        {
            mStoppingCriterion = Plato::algorithm::MAX_NUMBER_ITERATIONS;
            tStop = true;
        }
        else if( tDeltaObjFunc < mObjectiveChangeTolerance )
        {
            mStoppingCriterion = Plato::algorithm::OBJECTIVE_STAGNATION;
            tStop = true;
        }
        return tStop;
    }

    /******************************************************************************//**
     * @brief Solve uncontrained MMA subproblem
    **********************************************************************************/
    void solveUnconstrainedMethodMovingAsymptotesProblem()
    {
        updateMoveLimits();
        updateDynamicControlBounds();
        updateAsymptotes();
        updateSubProbControlBounds();
        updateSolution();
    }

    /******************************************************************************//**
     * @brief Compute move limits for optimization problem
    **********************************************************************************/
    void updateMoveLimits()
    {
        Plato::update( 1.0, mDataMng->mControlUpperBounds.operator*(), 0.0, mDataMng->mMoveLimits.operator*());
        Plato::update(-1.0, mDataMng->mControlLowerBounds.operator*(), 1.0, mDataMng->mMoveLimits.operator*());
        Plato::scale(mMoveLimit, mDataMng->mMoveLimits.operator*());
    }

    /***********************Plato**************************************************//**
     * @brief Update dynamic upper and lower bounds needed to compute subproblem bounds 
    **********************************************************************************/
    void updateDynamicControlBounds()
    {
        std::vector<ScalarType> tCriteria(2/* length */);
        const OrdinalType tNumVectors = mDataMng->mCurrentControls->getNumVectors();
        for (OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            const OrdinalType tNumControls = (mDataMng->mCurrentControls.operator*())[tVectorIndex].size();
            for (OrdinalType tControlIndex = 0; tControlIndex < tNumControls; tControlIndex++)
            {
                tCriteria[0] = (mDataMng->mControlLowerBounds.operator*())(tVectorIndex, tControlIndex);
                tCriteria[1] = (mDataMng->mCurrentControls.operator*())(tVectorIndex, tControlIndex) 
                    - (mDataMng->mMoveLimits.operator*())(tVectorIndex, tControlIndex);
                (mDataMng->mDynamicControlLowerBounds.operator*())(tVectorIndex, tControlIndex) = 
                    *std::max_element(tCriteria.begin(), tCriteria.end());

                tCriteria[0] = (mDataMng->mControlUpperBounds.operator*())(tVectorIndex, tControlIndex);
                tCriteria[1] = (mDataMng->mCurrentControls.operator*())(tVectorIndex, tControlIndex) 
                    + (mDataMng->mMoveLimits.operator*())(tVectorIndex, tControlIndex);
                (mDataMng->mDynamicControlUpperBounds.operator*())(tVectorIndex, tControlIndex) = 
                    *std::min_element(tCriteria.begin(), tCriteria.end());

                if((mDataMng->mDynamicControlLowerBounds.operator*())(tVectorIndex, tControlIndex) > 
                    (mDataMng->mDynamicControlUpperBounds.operator*())(tVectorIndex, tControlIndex))
                {
                    (mDataMng->mDynamicControlLowerBounds.operator*())(tVectorIndex, tControlIndex) = 
                        (mDataMng->mControlLowerBounds.operator*())(tVectorIndex, tControlIndex);
                    (mDataMng->mDynamicControlUpperBounds.operator*())(tVectorIndex, tControlIndex) = 
                        (mDataMng->mControlUpperBounds.operator*())(tVectorIndex, tControlIndex);
                }
            }
        }
    }

    /******************************************************************************//**
     * @brief Update asymptotes required to compute control variables
    **********************************************************************************/
    void updateAsymptotes()
    {      
        if(mCurrentOuterIteration <= 2)
        {
            computeInitialAsymptotes();
        }
        else
        {
            computeCurrentAsymptotes();
        }
    }

    /******************************************************************************//**
     * @brief Compute initial asymptotes 
    **********************************************************************************/
    void computeInitialAsymptotes()
    {
        Plato::update( 1.0, mDataMng->mDynamicControlUpperBounds.operator*(), 
                       0.0, mDataMng->mDeltaDynamicControlBounds.operator*());
        Plato::update(-1.0, mDataMng->mDynamicControlLowerBounds.operator*(), 
                       1.0, mDataMng->mDeltaDynamicControlBounds.operator*());

        Plato::update( 1.0, mDataMng->mCurrentControls.operator*(), 
                       0.0, mDataMng->mLowerAsymptotes.operator*());
        Plato::update(-mAsymptotesInitialMultiplier, mDataMng->mDeltaDynamicControlBounds.operator*(), 
                       1.0, mDataMng->mLowerAsymptotes.operator*());

        Plato::update(1.0, mDataMng->mCurrentControls.operator*(), 
                      0.0, mDataMng->mUpperAsymptotes.operator*());
        Plato::update(mAsymptotesInitialMultiplier, mDataMng->mDeltaDynamicControlBounds.operator*(), 
                      1.0, mDataMng->mUpperAsymptotes.operator*());
    }

    /******************************************************************************//**
     * @brief Update lower and upper asymptotes
    **********************************************************************************/
    void computeCurrentAsymptotes()
    {
        auto tAsymptotesIncrement = 
            static_cast<ScalarType>( std::min(1.0 + mAsymptotesConstant, mAsymptotesIncrementConstant) );
        auto tAsymptotesDecrement = 
            static_cast<ScalarType>( std::max(1.0 - (2.0*mAsymptotesConstant), mAsymptotesDecrementConstant) );

        const OrdinalType tNumVectors = mDataMng->mCurrentControls->getNumVectors();
        for (OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            const OrdinalType tNumControls = (mDataMng->mCurrentControls.operator*())[tVectorIndex].size();
            for (OrdinalType tControlIndex = 0; tControlIndex < tNumControls; tControlIndex++)
            {
                ScalarType tSign = ( (mDataMng->mCurrentControls.operator*())(tVectorIndex, tControlIndex) 
                                 -   (mDataMng->mPreviousControls.operator*())(tVectorIndex, tControlIndex) )
                                 * ( (mDataMng->mPreviousControls.operator*())(tVectorIndex, tControlIndex) 
                                 -   (mDataMng->mThirdLastControls.operator*())(tVectorIndex, tControlIndex) );
                ScalarType tMultiplier = tSign > static_cast<ScalarType>(0) 
                                       ? tAsymptotesIncrement : tAsymptotesDecrement;
                ScalarType tConstant = std::abs(tSign) <= std::numeric_limits<ScalarType>::min() 
                                     ? static_cast<ScalarType>(1) : tMultiplier;

                (mDataMng->mLowerAsymptotes.operator*())(tVectorIndex, tControlIndex) = 
                    (mDataMng->mCurrentControls.operator*())(tVectorIndex, tControlIndex)
                    - ( tConstant * ( (mDataMng->mPreviousControls.operator*())(tVectorIndex, tControlIndex) - 
                                      (mDataMng->mLowerAsymptotes.operator*())(tVectorIndex, tControlIndex) ) );
                (mDataMng->mUpperAsymptotes.operator*())(tVectorIndex, tControlIndex) = 
                    (mDataMng->mCurrentControls.operator*())(tVectorIndex, tControlIndex)
                    + ( tConstant * ( (mDataMng->mUpperAsymptotes.operator*())(tVectorIndex, tControlIndex) - 
                                      (mDataMng->mPreviousControls.operator*())(tVectorIndex, tControlIndex) ) );
            }
        }        
    }

    /******************************************************************************//**
     * @brief Update bounds for unconstrained MMA subproblem
    **********************************************************************************/
    void updateSubProbControlBounds()
    {
        Plato::update(0.9, mDataMng->mLowerAsymptotes.operator*(), 0.0, mDataMng->mSubProbAlphaBounds.operator*());
        Plato::update(0.1, mDataMng->mCurrentControls.operator*(), 1.0, mDataMng->mSubProbAlphaBounds.operator*());

        Plato::update(0.9, mDataMng->mUpperAsymptotes.operator*(), 1.0, mDataMng->mSubProbBetaBounds.operator*());
        Plato::update(0.1, mDataMng->mCurrentControls.operator*(), 1.0, mDataMng->mSubProbBetaBounds.operator*());
        
        const OrdinalType tNumVectors = mDataMng->mCurrentControls->getNumVectors();
        for (OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            const OrdinalType tNumControls = (mDataMng->mCurrentControls.operator*())[tVectorIndex].size();
            for (OrdinalType tControlIndex = 0; tControlIndex < tNumControls; tControlIndex++)
            {
                (mDataMng->mSubProbAlphaBounds.operator*())[tVectorIndex][tControlIndex] = 
                    std::max( (mDataMng->mDynamicControlLowerBounds.operator*())(tVectorIndex, tControlIndex),
                              (mDataMng->mSubProbAlphaBounds.operator*())(tVectorIndex, tControlIndex) );
                
                (mDataMng->mSubProbBetaBounds.operator*())[tVectorIndex][tControlIndex] = 
                    std::min( (mDataMng->mDynamicControlUpperBounds.operator*())(tVectorIndex, tControlIndex),
                              (mDataMng->mSubProbBetaBounds.operator*())(tVectorIndex, tControlIndex) );
            }
        }           
    }

    /******************************************************************************//**
     * @brief Update control variables and data structures required to update control variables. 
    **********************************************************************************/
    void updateSolution()
    {
        updateApproximationFunctionP();
        updateApproximationFunctionQ();
        Plato::update(1.0, mDataMng->mPreviousControls.operator*(), 0.0, mDataMng->mThirdLastControls.operator*());
        Plato::update(1.0, mDataMng->mCurrentControls.operator*() , 0.0, mDataMng->mPreviousControls.operator*());
        updateCurrentControls();
    }

    /******************************************************************************//**
     * @brief Update current control variables
    **********************************************************************************/
    void updateCurrentControls()
    {
        const OrdinalType tNumVectors = mDataMng->mCurrentControls->getNumVectors();
        for (OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            const OrdinalType tNumControls = (mDataMng->mCurrentControls.operator*())[tVectorIndex].size();
            for (OrdinalType tControlIndex = 0; tControlIndex < tNumControls; tControlIndex++)
            {
                auto tLowerAsymTimesAppxFuncP = 
                    (mDataMng->mLowerAsymptotes.operator*())(tVectorIndex,tControlIndex) * 
                    (mDataMng->mApproximationFunctionP.operator*())(tVectorIndex,tControlIndex);
                auto tUpperAsymTimesAppxFuncQ = 
                    (mDataMng->mUpperAsymptotes.operator*())(tVectorIndex,tControlIndex) * 
                    (mDataMng->mApproximationFunctionQ.operator*())(tVectorIndex,tControlIndex);
                auto tUpperMinusLowerAsym = (mDataMng->mUpperAsymptotes.operator*())(tVectorIndex,tControlIndex)
                                          - (mDataMng->mLowerAsymptotes.operator*())(tVectorIndex,tControlIndex);
                auto tSqrtAppxFuncPTimesAppxFuncQ = 
                    (mDataMng->mApproximationFunctionP.operator*())(tVectorIndex,tControlIndex) * 
                    (mDataMng->mApproximationFunctionQ.operator*())(tVectorIndex,tControlIndex);
                tSqrtAppxFuncPTimesAppxFuncQ = std::sqrt(tSqrtAppxFuncPTimesAppxFuncQ);

                ScalarType tNumerator = tLowerAsymTimesAppxFuncP - tUpperAsymTimesAppxFuncQ 
                                      + ( tUpperMinusLowerAsym * tSqrtAppxFuncPTimesAppxFuncQ );
                ScalarType tDenominator = (mDataMng->mApproximationFunctionP.operator*())(tVectorIndex,tControlIndex) 
                                        - (mDataMng->mApproximationFunctionQ.operator*())(tVectorIndex,tControlIndex);
                auto tControlCandidate = tNumerator / tDenominator;
                tControlCandidate = std::min( (mDataMng->mSubProbBetaBounds.operator*())(tVectorIndex,tControlIndex), 
                                              tControlCandidate);
                (mDataMng->mCurrentControls.operator*())(tVectorIndex,tControlIndex) = 
                    std::max( (mDataMng->mSubProbAlphaBounds.operator*())(tVectorIndex,tControlIndex), 
                              tControlCandidate);
            }
        } 
    }

    /******************************************************************************//**
     * @brief Update approximation function used in asymptotes calculation
    **********************************************************************************/
    void updateApproximationFunctionP()
    {
        const OrdinalType tNumVectors = mDataMng->mCurrentObjectiveGradient->getNumVectors();
        for (OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            auto& tUpperAsymptotes_V = mDataMng->mUpperAsymptotes->operator[](tVectorIndex);
            auto& tLowerAsymptotes_V = mDataMng->mLowerAsymptotes->operator[](tVectorIndex);

            auto& tCurrentControl_V = mDataMng->mCurrentControls->operator[](tVectorIndex);
            auto& tCurrentObjGrad_V = mDataMng->mCurrentObjectiveGradient->operator[](tVectorIndex);
            auto& tApproximationFuncP_V = mDataMng->mApproximationFunctionP->operator[](tVectorIndex);
            
            const OrdinalType tNumControls = tCurrentObjGrad_V.size();
            for (OrdinalType tControlIndex = 0; tControlIndex < tNumControls; tControlIndex++)
            {
                tApproximationFuncP_V[tControlIndex] = 
                      std::max( tCurrentObjGrad_V[tControlIndex], 0.0 ) 
                    + ( mCONSTANT_ONE * std::abs( tCurrentObjGrad_V[tControlIndex] ) )
                    + ( mCONSTANT_EPS / ( tUpperAsymptotes_V[tControlIndex] 
                                        - tLowerAsymptotes_V[tControlIndex] 
                                        + std::numeric_limits<ScalarType>::epsilon() ) );
                ScalarType tUpperAsymMinusControlSquared = 
                    tUpperAsymptotes_V[tControlIndex] - tCurrentControl_V[tControlIndex];
                tUpperAsymMinusControlSquared *= tUpperAsymMinusControlSquared;
                tApproximationFuncP_V[tControlIndex] *= tUpperAsymMinusControlSquared;
            }
        }
    }

    /******************************************************************************//**
     * @brief Update approximation function used in asymptotes calculation
    **********************************************************************************/
    void updateApproximationFunctionQ()
    {
        const OrdinalType tNumVectors = mDataMng->mCurrentObjectiveGradient->getNumVectors();
        for (OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            auto& tUpperAsymptotes_V = mDataMng->mUpperAsymptotes->operator[](tVectorIndex);
            auto& tLowerAsymptotes_V = mDataMng->mLowerAsymptotes->operator[](tVectorIndex);

            auto& tCurrentControl_V = mDataMng->mCurrentControls->operator[](tVectorIndex);
            auto& tCurrentObjGrad_V = mDataMng->mCurrentObjectiveGradient->operator[](tVectorIndex);
            auto& tApproximationFuncQ_V = mDataMng->mApproximationFunctionQ->operator[](tVectorIndex);
            
            const OrdinalType tNumControls = tCurrentObjGrad_V.size();
            for (OrdinalType tControlIndex = 0; tControlIndex < tNumControls; tControlIndex++)
            {
                tApproximationFuncQ_V[tControlIndex] = 
                    -1.0*std::min( tCurrentObjGrad_V[tControlIndex], 0.0 )
                    + ( mCONSTANT_ONE * std::abs( tCurrentObjGrad_V[tControlIndex] ) )
                    + ( mCONSTANT_EPS / ( tUpperAsymptotes_V[tControlIndex] 
                                        - tLowerAsymptotes_V[tControlIndex] 
                                        + std::numeric_limits<ScalarType>::epsilon() ) );
                ScalarType tControlMinusLowerAsymSquared = 
                    tCurrentControl_V[tControlIndex] - tLowerAsymptotes_V[tControlIndex];
                tControlMinusLowerAsymSquared *= tControlMinusLowerAsymSquared;
                tApproximationFuncQ_V[tControlIndex] *= tControlMinusLowerAsymSquared;
            }
        }
    }

    /******************************************************************************//**
     * @brief Cache optimization problem state
    **********************************************************************************/
    void cacheState()
    {
        mDataMng->mPreviousObjFuncValue = mDataMng->mCurrentObjFuncValue;
        Plato::update(1.0,mDataMng->mCurrentObjectiveGradient.operator*(),
                      0.0,mDataMng->mPreviousObjectiveGradient.operator*());
    }

    /******************************************************************************//**
     * @brief Evaluate objective criterion and its gradient.
    **********************************************************************************/
    void evaluateObjective()
    {       
        mDataMng->mCurrentObjFuncValue = mObjective->value(mDataMng->mCurrentControls.operator*());
        mNumObjFuncEvals++; 
        mObjective->cacheData();
        mObjective->gradient(mDataMng->mCurrentControls.operator*(),mDataMng->mCurrentObjectiveGradient.operator*());
        mNumObjGradEvals++;
    }

// private member data
private:
    bool mWriteDiagnostics = false;  /*!< output diagnostics to file on disk (default=false) */
    std::ofstream mOutputStream;     /*!< output string stream with diagnostics */
    Plato::OutputDataUMMA<ScalarType,OrdinalType> mOutputData; /*!< output data written to diagnostic file */

    OrdinalType mNumObjGradEvals = 0; /*!< number of objective function gradient evaluations */
    OrdinalType mNumObjFuncEvals = 0; /*!< number of objective function evaluations */
    OrdinalType mCurrentOuterIteration = 0; /*!< current number of outer iterations */
    OrdinalType mMaxNumOuterIterations = 150; /*!< maximum number of outer iterations */
    OrdinalType mCurrentSubProblemIteration = 0; /*!< current number of subproblem iterations */
    OrdinalType mMaxNumSubProblemIterations = 5; /*!< maximum number of subproblem iterations */

    ScalarType mMoveLimit                   = 0.15; /*!< control variable move limit */
    ScalarType mAsymptotesConstant          = 0.2;  /*!< constant used to compute asymptote increment/decrement */
    ScalarType mControlChangeTolerance      = 1e-3; /*!< tolerance on max change between current & pevious control */
    ScalarType mObjectiveChangeTolerance    = 1e-10; /*!< tolerance on max change between current & pevious objective*/
    ScalarType mAsymptotesInitialMultiplier = 0.2; /*!< increment multiplier on initial asymptote*/
    ScalarType mAsymptotesIncrementConstant = 1.2; /*!< increment multiplier on aymptotes */
    ScalarType mAsymptotesDecrementConstant = 0.7; /*!< decrement multiplier on aymptotes */
    
    const ScalarType mCONSTANT_ONE = 1e-3; /*!< constant use in asymptotes calculation */
    const ScalarType mCONSTANT_EPS = 1e-6; /*!< constant use in asymptotes calculation */

    Plato::algorithm::stop_t mStoppingCriterion = Plato::algorithm::NOT_CONVERGED; /*!< stopping criterion */

    std::shared_ptr<DataMng> mDataMng; /*!< unconstrained MMA data manager */
    std::shared_ptr<Plato::Criterion<ScalarType, OrdinalType>> mObjective; /*!< objective criteria interface */
};
// class UnconstrainedMethodMovingAsymptotes

}
// namespace Plato