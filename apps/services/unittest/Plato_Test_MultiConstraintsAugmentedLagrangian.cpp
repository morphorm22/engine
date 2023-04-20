/*
 * Plato_Test_MultiConstraintsUnconstrainedMethodMovingAsymptotes.cpp
 *
 *  Created on: Apr 13, 2023
 */

#include "gtest/gtest.h"

#include <fstream>

#include "Plato_Types.hpp"
#include "Plato_DataFactory.hpp"
#include "Plato_CommWrapper.hpp"
#include "Plato_MultiVector.hpp"
#include "Plato_LinearAlgebra.hpp"
#include "Plato_CriterionList.hpp"
#include "Plato_MultiVectorList.hpp"
#include "Plato_ReductionOperations.hpp"

namespace Morphorm
{

/*****************************************************************************************//**
 * @brief Diagnostic data for the Unconstrained Method of Moving Asymptotes (MMA) algorithm
*********************************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
struct OutputDataUMMA
{
    OrdinalType mNumOuterIter;   /*!< number of outer iterations */
    OrdinalType mObjFuncEvals;   /*!< number of objective function evaluations */

    ScalarType mFeasibility;     /*!< change between two subsequent control solutions */
    ScalarType mObjFuncValue;    /*!< objective function value */
    ScalarType mControlChange;   /*!< change between two subsequent control solutions */
    ScalarType mObjFuncChange;   /*!< change between two subsequent objective function values */
    ScalarType mNormObjFuncGrad; /*!< norm of the objective function gradient */
};
// struct OutputDataUMMA

/******************************************************************************//**
 * @brief Check is diagnostic file is closed
 * @param [in] aOutputStream output file
 **********************************************************************************/
template<typename OutputType>
void is_diagnostic_file_close(const OutputType &aOutputStream, const Plato::CommWrapper &aComm)
{
    if(aComm.myProcID() == 0)
    {
        if(aOutputStream.is_open() == false)
        {
            std::string tErrorMsg("Diagnostics requested but write file is closed!");
            std::cout << std::string("\nFILE: ") + __FILE__ \
                + std::string("\nFUNCTION: ") + __PRETTY_FUNCTION__ \
                + std::string("\nLINE:") + std::to_string(__LINE__) \
                + std::string("\nMESSAGE: ") + tErrorMsg << std::endl << std::flush;
            aComm.abort();
        }
        aComm.barrier();
    }
}
// function is_diagnostic_file_close

/******************************************************************************//**
 * @brief Write header for U-MMA diagnostics file
 * @param [in]  aComm         interface to MPI communicator
 * @param [in]  aData         diagnostic data for U-MMA algorithm
 * @param [out] aOutputStream output stream to be written to file
**********************************************************************************/
template<typename ScalarType, typename OrdinalType, typename OutputType>
void write_umma_diagnostics_header(
 const Plato::CommWrapper &aComm,
 const Morphorm::OutputDataUMMA<ScalarType, OrdinalType> &aData,
 OutputType &aOutputStream
)
{
    Morphorm::is_diagnostic_file_close(aOutputStream, aComm);
    aOutputStream << std::scientific << std::setprecision(6) << std::right << "Iter" << std::setw(10) << "F-count"
            << std::setw(14) << "F(X)" << std::setw(16) << "Norm(F')" << std::setw(15) << "abs(dX)" 
            << std::setw(15) << "abs(dF)" << "\n" << std::flush;
}
// function print_umma_diagnostics_header

/******************************************************************************//**
 * @brief Output a brief sentence describing reason for convergence.
 * @param [in] aStopCriterion stopping criterion flag
 * @param [in,out] aOutput string with brief description
**********************************************************************************/
inline std::string get_umma_stop_criterion_description(const Plato::algorithm::stop_t &aStopCriterion)
{
    std::string tOutput;
    switch(aStopCriterion)
    {
         case Plato::algorithm::stop_t::CONTROL_STAGNATION:
        {
            tOutput = "\n\n****** Optimization stopping due to control (i.e. design variable) stagnation. ******\n\n";
            break;
        }
        case Plato::algorithm::stop_t::MAX_NUMBER_ITERATIONS:
        {
            tOutput = "\n\n****** Optimization stopping due to exceeding maximum number of iterations. ******\n\n";
            break;
        }
        case Plato::algorithm::stop_t::NOT_CONVERGED:
        {
            tOutput = "\n\n****** Optimization algorithm did not converge. ******\n\n";
            break;
        }
        default:
        {
            tOutput = "\n\n****** ERROR: Optimization algorithm stopping due to undefined behavior. ******\n\n";
            break;
        }
    }
    return tOutput;
}
// function get_umma_stop_criterion_description

/******************************************************************************//**
 * @brief Print diagnostics for MMA algorithm
 * @param [in] aData diagnostic data for mma algorithm
 * @param [in,out] aOutputFile output file
**********************************************************************************/
template<typename ScalarType, typename OrdinalType, typename OutputType>
void print_umma_diagnostics(
 const Morphorm::OutputDataUMMA<ScalarType, OrdinalType> &aData,
 OutputType &aOutputFile
)
{
    Morphorm::is_diagnostic_file_close(aOutputFile);
    aOutputFile << std::scientific << std::setprecision(6) << std::right << aData.mNumOuterIter << std::setw(10)
            << aData.mObjFuncEvals << std::setw(20) << aData.mObjFuncValue << std::setw(15) << aData.mNormObjFuncGrad
            << std::setw(15) << aData.mControlChange << std::setw(15) << aData.mObjFuncChange << "\n" << std::flush;
}
// function print_umma_diagnostics

template<typename ScalarType, typename OrdinalType = size_t>
struct UnconstrainedMethodMovingAsymptotesDataMng
{
private:
    typedef Plato::Vector<ScalarType, OrdinalType> Vec;
    typedef Plato::MultiVector<ScalarType, OrdinalType> MultiVec;
    typedef Plato::MultiVectorList<ScalarType, OrdinalType> MultiVecList;
    typedef Plato::ReductionOperations<ScalarType, OrdinalType> ReductionOps;

public:
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
        mCurrentObjectiveGradients(std::make_shared<MultiVecList>(mNumObjFuncs, aDataFactory->control())),
        mPreviousObjectiveGradients(std::make_shared<MultiVecList>(mNumObjFuncs, aDataFactory->control())),
        mComm(aDataFactory->getCommWrapper().create()),
        mReductionOps(aDataFactory->getControlReductionOperations().create())
    {}
    ~UnconstrainedMethodMovingAsymptotesDataMng(){}

public:
    OrdinalType mNumObjFuncs = 1;

    std::shared_ptr<Vec> mCurrentObjectiveValues;  /*!< current objective values */
    std::shared_ptr<Vec> mPreviousObjectiveValues; /*!< previous objective values */

    std::shared_ptr<MultiVec> mMoveLimits;
    std::shared_ptr<MultiVec> mDeltaControl;
    std::shared_ptr<MultiVec> mLowerAsymptotes;
    std::shared_ptr<MultiVec> mUpperAsymptotes;
    std::shared_ptr<MultiVec> mCurrentControls;
    std::shared_ptr<MultiVec> mPreviousControls;
    std::shared_ptr<MultiVec> mThirdLastControls; 
    std::shared_ptr<MultiVec> mSubProbBetaBounds;
    std::shared_ptr<MultiVec> mSubProbAlphaBounds;
    std::shared_ptr<MultiVec> mControlLowerBounds;
    std::shared_ptr<MultiVec> mControlUpperBounds; 
    std::shared_ptr<MultiVec> mApproximationFunctionP;
    std::shared_ptr<MultiVec> mApproximationFunctionQ;
    std::shared_ptr<MultiVec> mDeltaDynamicControlBounds;
    std::shared_ptr<MultiVec> mDynamicControlUpperBounds;
    std::shared_ptr<MultiVec> mDynamicControlLowerBounds;

    /*!< current and previous objective gradients with respect to the design variables */
    std::shared_ptr<MultiVecList> mCurrentObjectiveGradients;    
    std::shared_ptr<MultiVecList> mPreviousObjectiveGradients;

    std::shared_ptr<Plato::CommWrapper> mComm;   /*!< interface to MPI communicator */
    std::shared_ptr<ReductionOps> mReductionOps; /*!< interface to math reduction operations */
};

template<typename ScalarType, typename OrdinalType = size_t>
class UnconstrainedMethodMovingAsymptotes
{
private:
    typedef Morphorm::UnconstrainedMethodMovingAsymptotesDataMng<ScalarType, OrdinalType> DataMng;

public:
    UnconstrainedMethodMovingAsymptotes(){}
    UnconstrainedMethodMovingAsymptotes(
     const std::shared_ptr<Plato::Criterion<ScalarType, OrdinalType>> &aObjective,
     const std::shared_ptr<Plato::DataFactory<ScalarType, OrdinalType>> &aDataFactory
    ) : 
        mDataMng(std::make_shared<DataMng>(aDataFactory))
    {
        mObjectives->add(aObjective);
    }
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

    void setControlTolerance(const ScalarType &aInput)
    {
        mControlTolerance = aInput;
    }

    void setAsymptotesConstant(const ScalarType &aInput)
    {
        mAsymptotesConstant = aInput;
    }

    void setFeasibilityTolerance(const ScalarType &aInput)
    {
        mFeasibilityTolerance = aInput;
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
                cacheState();
                evaluateObjective();
                writeDiagnostics();
                solveUnconstrainedMethodMovingAsymptotesProblem();
            }
            auto tConverged = computeStoppingCriteria();
            if( tConverged )
            {
                cacheState();
                evaluateObjective();
                writeDiagnostics();
                writeStoppingCriterion();
                closeWriteFile();
                break;
            }
        }
    }

// private functions
private:
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
            mOutputStream.open("morphorm_umma_algorithm_diagnostics.txt");
            Morphorm::write_umma_diagnostics_header(mOutputData, mOutputStream);
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
            mOutputStream << Morphorm::get_umma_stop_criterion_description(mStoppingCriterion);
        }
    }

    /******************************************************************************//**
     * @brief Write diagnostics for U-MMA algorithm to file
    **********************************************************************************/
    void writeDiagnostics()
    {
        if(mWriteDiagnostics == false)
        {
            return;
        }

        if(mDataMng->mComm->myProcID() == 0)
        {
            /*mOutputData.mNumOuterIter = mCurrentOuterIteration;
            mOutputData.mObjFuncEvals = mNumObjFuncEvals;
            mOutputData.mObjFuncValue = mDataMng->getCurrentObjectiveValue();
            mOutputData.mNormObjFuncGrad = mDataMng->getNormObjectiveGradient();
            mOutputData.mControlStagnationMeasure = mDataMng->getControlStagnationMeasure();
            mOutputData.mObjectiveStagnationMeasure = mDataMng->getObjectiveStagnationMeasure();

            const OrdinalType tNumConstraints = mDataMng->getNumConstraints();
            for(OrdinalType tConstraintIndex = 0; tConstraintIndex < tNumConstraints; tConstraintIndex++)
            {
                mOutputData.mConstraints[tConstraintIndex] = mDataMng->getCurrentConstraintValue(tConstraintIndex);
            }

            Plato::print_mma_diagnostics(mOutputData, mOutputStream);

            std::stringstream tConsoleStream;
            Plato::print_mma_diagnostics_header(mOutputData, tConsoleStream);
            Plato::print_mma_diagnostics(mOutputData, tConsoleStream);
            Plato::Console::Alert(tConsoleStream.str());*/
        }
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
    /******************************************************************************//**
     * @brief Compute stopping criteria
    **********************************************************************************/
    void computeStoppingCriteria()
    {
        bool tStop = false;

        auto tDeltaControl = computeDeltaControl();
        if( tDeltaControl < mControlTolerance ){
            tStop = true;
        }
        else if( mCurrentOuterIteration > mMaxNumOuterIterations ){
            tStop = true;
        }
    }

    void solveUnconstrainedMethodMovingAsymptotesProblem()
    {
        updateMoveLimits();
        updateDynamicControlBounds();
        updateAsymptotes();
        updateSubProbControlBounds();
        solveSubProblem();
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

    void solveSubProblem()
    {
        updateApproximationFunctions();
        Plato::update(1.0, mDataMng->mPreviousControls.operator*(), 0.0, mDataMng->mThirdLastControls.operator*());
        Plato::update(1.0, mDataMng->mCurrentControls.operator*() , 0.0, mDataMng->mPreviousControls.operator*());
        updateControls();
    }

    void updateControls()
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

    void updateApproximationFunctions()
    {
        const OrdinalType tNumVectors = mDataMng->mCurrentControls->getNumVectors();
        for (OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            const OrdinalType tNumControls = (mDataMng->mCurrentControls.operator*())[tVectorIndex].size();
            for (OrdinalType tControlIndex = 0; tControlIndex < tNumControls; tControlIndex++)
            {
                updateApproximationFunctionP(tVectorIndex, tControlIndex);
                updateApproximationFunctionQ(tVectorIndex, tControlIndex);
            }
        }
    }

    void updateApproximationFunctionP(const OrdinalType& aVectorIndex, const OrdinalType& aControlIndex)
    {
        (mDataMng->mApproximationFunctionP.operator*())(aVectorIndex,aControlIndex) = 
              std::max( (mDataMng->mCurrentObjectiveGradients.operator*())(aVectorIndex,aControlIndex), 0.0 ) 
            + ( mCONSTANT_ONE 
            *   std::abs( (mDataMng->mCurrentObjectiveGradients.operator*())(aVectorIndex,aControlIndex) ) )
            + ( mCONSTANT_EPS / ( (mDataMng->mUpperAsymptotes.operator*())(aVectorIndex,aControlIndex) 
                                - (mDataMng->mLowerAsymptotes.operator*())(aVectorIndex,aControlIndex) 
                                + std::numeric_limits<ScalarType>::epsilon() ) );
        ScalarType tUpperAsymMinusControlSquared = 
              (mDataMng->mUpperAsymptotes.operator*())(aVectorIndex,aControlIndex) 
            - (mDataMng->mCurrentControls.operator*())(aVectorIndex,aControlIndex);
        tUpperAsymMinusControlSquared *= tUpperAsymMinusControlSquared;
        (mDataMng->mApproximationFunctionP.operator*())(aVectorIndex,aControlIndex) *= 
            tUpperAsymMinusControlSquared;
    }

    void updateApproximationFunctionQ(const OrdinalType& aVectorIndex, const OrdinalType& aControlIndex)
    {
        (mDataMng->mApproximationFunctionQ.operator*())(aVectorIndex,aControlIndex) = 
            -1.0*std::min( (mDataMng->mCurrentObjectiveGradients.operator*())(aVectorIndex,aControlIndex), 0.0 )
            + ( mCONSTANT_ONE 
            *   std::abs( (mDataMng->mCurrentObjectiveGradients.operator*())(aVectorIndex,aControlIndex) ) )
            + ( mCONSTANT_EPS / ( (mDataMng->mUpperAsymptotes.operator*())(aVectorIndex,aControlIndex) 
                                - (mDataMng->mLowerAsymptotes.operator*())(aVectorIndex,aControlIndex) 
                                + std::numeric_limits<ScalarType>::epsilon() ) );
        ScalarType tControlMinusLowerAsymSquared = 
              (mDataMng->mCurrentControls.operator*())(aVectorIndex,aControlIndex) 
            - (mDataMng->mLowerAsymptotes.operator*())(aVectorIndex,aControlIndex);
        tControlMinusLowerAsymSquared *= tControlMinusLowerAsymSquared;
        (mDataMng->mApproximationFunctionQ.operator*())(aVectorIndex,aControlIndex) *= 
            tControlMinusLowerAsymSquared;
    }

    void cacheState()
    {
        mDataMng->mPreviousObjectiveValues->update(1.0, mDataMng->mCurrentObjectiveValues->operator*(), 0.0);
        mDataMng->mPreviousObjectiveGradients->update(1.0, mDataMng->mCurrentObjectiveGradients->operator*(), 0.0);
    }

    /******************************************************************************//**
     * @brief Evaluate objective criterion and its gradient.
    **********************************************************************************/
    void evaluateObjective()
    {       
        const OrdinalType tNumObjectiveFuncs = mObjectives->size();
        for(OrdinalType tIndex = 0; tIndex < tNumObjectiveFuncs; tIndex++)
        {
            mDataMng->mCurrentObjectiveValues->operator[](tIndex) = 
                (*mObjectives)[tIndex].value(mDataMng->mCurrentControls.operator*());
            (*mObjectives)[tIndex].cacheData();

            (*mObjectives)[tIndex].gradient(mDataMng->mCurrentControls.operator*(), 
                                            mDataMng->mCurrentObjectiveGradients->operator[](tIndex));
        }

        mNumObjFuncEvals++; 
        mNumObjGradEvals++;
    }

// private member data
private:
    bool mWriteDiagnostics;      /*!< output diagnostics to file on disk (default=false) */
    std::ofstream mOutputStream; /*!< output string stream with diagnostics */
    Morphorm::OutputDataUMMA<ScalarType,OrdinalType> mOutputData; /*!< output data written to diagnostic file */

    OrdinalType mNumObjGradEvals = 0;
    OrdinalType mNumObjFuncEvals = 0;
    OrdinalType mCurrentOuterIteration = 0;
    OrdinalType mMaxNumOuterIterations = 150;
    OrdinalType mCurrentSubProblemIteration = 0;
    OrdinalType mMaxNumSubProblemIterations = 5;

    ScalarType mMoveLimit            = 0.15;
    ScalarType mControlTolerance     = 2e-3;
    ScalarType mAsymptotesConstant   = 0.2;
    ScalarType mFeasibilityTolerance = 5e-3;
    ScalarType mAsymptotesInitialMultiplier = 0.2;
    ScalarType mAsymptotesIncrementConstant = 1.2; 
    ScalarType mAsymptotesDecrementConstant = 0.7; 
    
    const ScalarType mCONSTANT_ONE = 1e-3;
    const ScalarType mCONSTANT_EPS = 1e-6;

    Plato::algorithm::stop_t mStoppingCriterion; /*!< stopping criterion */

    std::shared_ptr<DataMng> mDataMng; /*!< unconstrained MMA data manager */
    std::shared_ptr<Plato::CriterionList<ScalarType, OrdinalType>> mObjectives; /*!< objective criteria interface */
};

}

namespace MorphormTest
{

TEST(MorphormTest, UnconstrainedMethodMovingAsymptotes)
{
    Morphorm::UnconstrainedMethodMovingAsymptotes<double> tAL;
}

}
// namespace MorphTest