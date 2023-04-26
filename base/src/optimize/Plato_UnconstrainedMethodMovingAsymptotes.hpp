#pragma once

#include <memory>

#include "Plato_Types.hpp"
#include "Plato_Console.hpp"
#include "Plato_LinearAlgebra.hpp"
#include "Plato_UnconstrainedMethodMovingAsymptotesFileIO.hpp"
#include "Plato_UnconstrainedMethodMovingAsymptotesDataMng.hpp"

namespace Plato
{

template<typename ScalarType, typename OrdinalType = size_t>
class UnconstrainedMethodMovingAsymptotes
{
private:
    typedef Plato::UnconstrainedMethodMovingAsymptotesDataMng<ScalarType, OrdinalType> DataMng;

public:
    UnconstrainedMethodMovingAsymptotes(
     const std::shared_ptr<Plato::Criterion<ScalarType, OrdinalType>>   &aCriterion,
     const std::shared_ptr<Plato::DataFactory<ScalarType, OrdinalType>> &aDataFactory
    ) : 
        mDataMng(std::make_shared<DataMng>(aDataFactory)),
        mObjective(aCriterion)
    {}
    ~UnconstrainedMethodMovingAsymptotes(){}

    void writeDiagnostics(const bool &aInput)
    {
        mWriteDiagnostics = aInput;
    }

    void setMaxNumOuterIterations(const OrdinalType &aInput)
    {
        mMaxNumOuterIterations = aInput;
    }

    void setMaxNumSubProbIterations(const OrdinalType &aInput)
    {
        mMaxNumSubProblemIterations = aInput;
    }

    void setMoveLimit(const ScalarType &aInput)
    {
        mMoveLimit = aInput;
    }

    void setObjectiveChangeTolerance(const ScalarType &aInput)
    {
        mObjectiveChangeTolerance = aInput;
    }

    void setControlChangeTolerance(const ScalarType &aInput)
    {
        mControlChangeTolerance = aInput;
    }

    void setAsymptotesConstant(const ScalarType &aInput)
    {
        mAsymptotesConstant = aInput;
    }

    void setAsymptotesInitialMultiplier(const ScalarType &aInput)
    {
        mAsymptotesInitialMultiplier = aInput;
    }

    void setAsymptotesIncrementConstant(const ScalarType &aInput)
    {
        mAsymptotesIncrementConstant = aInput; 
    }
    
    void setAsymptotesDecrementConstant(const ScalarType &aInput)
    {
        mAsymptotesDecrementConstant = aInput;
    }

    void setLowerBounds(const Plato::MultiVector<ScalarType,OrdinalType> &aInput)
    {
        Plato::update(1.0,aInput,0.0,mDataMng->mControlLowerBounds.operator*());
    }

    void setUpperBounds(const Plato::MultiVector<ScalarType,OrdinalType> &aInput)
    {
        Plato::update(1.0,aInput,0.0,mDataMng->mControlUpperBounds.operator*());
    }

    void setInitialGuess(const Plato::MultiVector<ScalarType,OrdinalType> &aInput)
    {
        Plato::update(1.0,aInput,0.0,mDataMng->mCurrentControls.operator*());
    }

    OrdinalType getNumObjFuncEvals() const
    {
        return mNumObjFuncEvals;
    }

    OrdinalType getNumObjGradEvals() const
    {
        return mNumObjGradEvals;
    }

    OrdinalType getCurrentOuterIteration() const
    {
        return mCurrentOuterIteration;
    }

    ScalarType getCurrentObjFuncValue() const
    {
        return mDataMng->mCurrentObjFuncValue;
    }

    ScalarType getControlChange() const
    {
        return mOutputData.mControlChange;
    }

    ScalarType getObjectiveChange() const
    {
        return mOutputData.mObjFuncChange;
    }

    ScalarType getNormObjFuncGrad() const
    {
        return mOutputData.mNormObjFuncGrad;
    }

    Plato::algorithm::stop_t getStoppingCriterion() const
    {
        return mStoppingCriterion;
    }

    void getSolution(Plato::MultiVector<ScalarType,OrdinalType> &aInput) const
    {
        Plato::update(1.0,mDataMng->mCurrentControls.operator*(),0.0,aInput);
    }

    const Plato::MultiVector<ScalarType,OrdinalType>& getSolution() const
    {
        return mDataMng->mCurrentControls.operator*();
    }
    
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
    void solveSubProblem()
    {
        cacheState();
        evaluateObjective();
        writeDiagnostics();
        solveUnconstrainedMethodMovingAsymptotesProblem();
    }

    void finalize()
    {
        cacheState();
        evaluateObjective();
        writeDiagnostics();
        writeStoppingCriterion();
        closeWriteFile();
    }

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
            mOutputStream.open("Plato_umma_algorithm_diagnostics.txt");
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
     * @brief Write diagnostics for U-MMA algorithm to file
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

    void writeDiagnosticsToConsole() const
    {
        std::stringstream tConsoleStream;
        Plato::write_umma_diagnostics_header(mDataMng->mComm.operator*(),mOutputData,tConsoleStream);
        Plato::write_umma_diagnostics(mDataMng->mComm.operator*(),mOutputData, tConsoleStream);
        Plato::Console::Alert(tConsoleStream.str());
    }

    ScalarType computeDeltaObjFunc() const
    {
        auto tDelta = std::abs(mDataMng->mCurrentObjFuncValue - mDataMng->mPreviousObjFuncValue);
        return tDelta;
    }

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

    void solveUnconstrainedMethodMovingAsymptotesProblem()
    {
        updateMoveLimits();
        updateDynamicControlBounds();
        updateAsymptotes();
        updateSubProbControlBounds();
        updateSolution();
    }

    void updateMoveLimits()
    {
        Plato::update( 1.0, mDataMng->mControlUpperBounds.operator*(), 0.0, mDataMng->mMoveLimits.operator*());
        Plato::update(-1.0, mDataMng->mControlLowerBounds.operator*(), 1.0, mDataMng->mMoveLimits.operator*());
        Plato::scale(mMoveLimit, mDataMng->mMoveLimits.operator*());
    }

    /***********************Plato*******************************************************//**
     * @brief Update subproblem upper and lower bounds
     * @param [in] aDataMng MMA data manager interface
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

    void updateSolution()
    {
        updateApproximationFunctionP();
        updateApproximationFunctionQ();
        Plato::update(1.0, mDataMng->mPreviousControls.operator*(), 0.0, mDataMng->mThirdLastControls.operator*());
        Plato::update(1.0, mDataMng->mCurrentControls.operator*() , 0.0, mDataMng->mPreviousControls.operator*());
        updateCurrentControls();
    }

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

    OrdinalType mNumObjGradEvals = 0;
    OrdinalType mNumObjFuncEvals = 0;
    OrdinalType mCurrentOuterIteration = 0;
    OrdinalType mMaxNumOuterIterations = 150;
    OrdinalType mCurrentSubProblemIteration = 0;
    OrdinalType mMaxNumSubProblemIterations = 5;

    ScalarType mMoveLimit                   = 0.15;
    ScalarType mAsymptotesConstant          = 0.2;
    ScalarType mControlChangeTolerance      = 1e-3;
    ScalarType mObjectiveChangeTolerance    = 1e-10;
    ScalarType mAsymptotesInitialMultiplier = 0.2;
    ScalarType mAsymptotesIncrementConstant = 1.2; 
    ScalarType mAsymptotesDecrementConstant = 0.7; 
    
    const ScalarType mCONSTANT_ONE = 1e-3;
    const ScalarType mCONSTANT_EPS = 1e-6;

    Plato::algorithm::stop_t mStoppingCriterion = Plato::algorithm::NOT_CONVERGED; /*!< stopping criterion */

    std::shared_ptr<DataMng> mDataMng; /*!< unconstrained MMA data manager */
    std::shared_ptr<Plato::Criterion<ScalarType, OrdinalType>> mObjective; /*!< objective criteria interface */
};
// class UnconstrainedMethodMovingAsymptotes

}
// namespace Plato