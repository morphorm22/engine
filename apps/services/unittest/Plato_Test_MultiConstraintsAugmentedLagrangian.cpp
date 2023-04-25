/*
 * Plato_Test_MultiConstraintsUnconstrainedMethodMovingAsymptotes.cpp
 *
 *  Created on: Apr 13, 2023
 */

#include "gtest/gtest.h"

#include <fstream>

#include "Plato_UnitTestUtils.hpp"

#include "Plato_Utils.hpp"
#include "Plato_Types.hpp"
#include "Plato_Console.hpp"
#include "Plato_DataFactory.hpp"
#include "Plato_CommWrapper.hpp"
#include "Plato_MultiVector.hpp"
#include "Plato_LinearAlgebra.hpp"
#include "Plato_CriterionList.hpp"
#include "Plato_MultiVectorList.hpp"
#include "Plato_ReductionOperations.hpp"

#include "Plato_EpetraSerialDenseMultiVector.hpp"
#include "Plato_StructuralTopologyOptimization.hpp"

namespace Morphorm
{

template<typename ScalarType, typename OrdinalType = size_t>
class ProxyAugLagObjective : public Plato::Criterion<ScalarType, OrdinalType>
{
private:
    bool mIsFilterDisabled = false;

    ScalarType mPenaltyMax = 10000.;
    ScalarType mFirstObjFunc = -1.0;
    ScalarType mPenaltyValue =  1.0;
    ScalarType mPenaltyMultiplier = 1.1;
    ScalarType mLagrangeMultiplier = 0.0;
    ScalarType mCurrentConstraint  = 0.0;
    ScalarType mPreviousConstraint = 0.0;

    Epetra_SerialDenseVector mObjGrad;
    Epetra_SerialDenseVector mPenaltyGrad;
    Epetra_SerialDenseVector mConstraintGrad;
    Epetra_SerialDenseVector mFilteredGradient;
    Epetra_SerialDenseVector mUnfilteredGradient;
    
    std::shared_ptr<Plato::StructuralTopologyOptimization> mSolver;

public:
    explicit ProxyAugLagObjective(const std::shared_ptr<Plato::StructuralTopologyOptimization> & aSolver) :
        mObjGrad(Epetra_SerialDenseVector(aSolver->getNumDesignVariables())),
        mPenaltyGrad(Epetra_SerialDenseVector(aSolver->getNumDesignVariables())),
        mConstraintGrad(Epetra_SerialDenseVector(aSolver->getNumDesignVariables())),
        mFilteredGradient(Epetra_SerialDenseVector(aSolver->getNumDesignVariables())),
        mUnfilteredGradient(Epetra_SerialDenseVector(aSolver->getNumDesignVariables())),
        mSolver(aSolver)
    {}
    virtual ~ProxyAugLagObjective()
    {}

    /******************************************************************************//**
     * @brief Disable sensitivity filter
    **********************************************************************************/
    void disableFilter()
    {
        mIsFilterDisabled = true;
    }

    /******************************************************************************//**
     * @brief Set filter radius
     * @param [in] aInput filter radius
    **********************************************************************************/
    void setFilterRadius(const ScalarType& aInput)
    {
        mSolver->setFilterRadius(aInput);
    }

    /******************************************************************************//**
     * @brief Cache displacement solution
    **********************************************************************************/
    void cacheData()
    {
        mSolver->cacheState();
    }

    void updateProblem(const Plato::MultiVector<ScalarType, OrdinalType> & aControl)
    {
        // access control variables
        const OrdinalType tVectorIndex = 0;
        const Plato::Vector<ScalarType, OrdinalType> & tMyControl = aControl[tVectorIndex];
        const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> & tControl =
                dynamic_cast<const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType>&>(tMyControl);
        // Update Lagrange Multiplier 
        mLagrangeMultiplier = mLagrangeMultiplier + mPenaltyValue * mCurrentConstraint;
        // Update penalty parameter
        const ScalarType tGamma = 0.25;
        ScalarType tGammaTimesPrevConstraint = tGamma * mPreviousConstraint;
        mPenaltyValue = mCurrentConstraint > tGammaTimesPrevConstraint ? 
            mPenaltyMultiplier * mPenaltyValue : mPenaltyValue;
        mPreviousConstraint = mCurrentConstraint;
    }

    ScalarType value(const Plato::MultiVector<ScalarType, OrdinalType> & aControl)
    {
        // access control variables
        const OrdinalType tVectorIndex = 0;
        const Plato::Vector<ScalarType, OrdinalType> & tMyControl = aControl[tVectorIndex];
        const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> & tControl =
                dynamic_cast<const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType>&>(tMyControl);
        assert(tControl.size() == static_cast<OrdinalType>(mSolver->getNumDesignVariables()));
        // Solve linear system of equations
        mSolver->solve(tControl.vector());
        // Compute objective function term
        ScalarType tObjFuncTerm = this->objectiveValue(tControl);
        // Compute penalty function term
        ScalarType tPenaltyTerm = this->penaltyFunction(tControl);
        // Compute augmented Lagrangian objective
        auto tOutput = tObjFuncTerm + tPenaltyTerm;
        return (tOutput);
    }
    
    void gradient(const Plato::MultiVector<ScalarType, OrdinalType> & aControl,
                  Plato::MultiVector<ScalarType, OrdinalType> & aOutput)
    {
        // access control variables
        const OrdinalType tVectorIndex = 0;
        const Plato::Vector<ScalarType, OrdinalType> & tControl = aControl[tVectorIndex];
        const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> & tEpetraControl =
                dynamic_cast<const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType>&>(tControl);
        assert(tEpetraControl.size() == tOutput.size());
        assert(tOutput.size() == static_cast<OrdinalType>(mFilteredGradient.Length()));
        assert(tOutput.size() == static_cast<OrdinalType>(mUnfilteredGradient.Length()));
        // Compute unfiltered gradient
        this->objectiveGradient(tEpetraControl);
        this->penaltyGradient(tEpetraControl);
        mUnfilteredGradient.SCAL(mUnfilteredGradient.Length(),0.0,mUnfilteredGradient.A());
        mUnfilteredGradient.AXPY(mUnfilteredGradient.Length(),1.0,mObjGrad.A(),mUnfilteredGradient.A());
        mUnfilteredGradient.AXPY(mUnfilteredGradient.Length(),1.0,mPenaltyGrad.A(),mUnfilteredGradient.A());
        // Apply filter to gradient
        Plato::Vector<ScalarType, OrdinalType> & tOutput = aOutput[tVectorIndex];
        const OrdinalType tLength = mFilteredGradient.Length();
        Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> & tEpetraOutput =
            dynamic_cast<Plato::EpetraSerialDenseVector<ScalarType, OrdinalType>&>(tOutput);
        this->applySensitivityFilter(tEpetraControl,tEpetraOutput);
    }

    void hessian(const Plato::MultiVector<ScalarType, OrdinalType> & aControl,
                 const Plato::MultiVector<ScalarType, OrdinalType> & aVector,
                 Plato::MultiVector<ScalarType, OrdinalType> & aOutput)
    {
        THROWERR("Hessian is not implemented in class ProxyAugLagObjective")
    }

private:
    void applySensitivityFilter
    (const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aControl,
     Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aOutput)
    {
        const OrdinalType tLength = mFilteredGradient.Length();
        if(mIsFilterDisabled == false)
        {
            mSolver->applySensitivityFilter(aControl.vector(), mUnfilteredGradient, mFilteredGradient);
            mFilteredGradient.COPY(tLength,mFilteredGradient.A(),aOutput.vector().A());
        }
        else
        {
            mUnfilteredGradient.COPY(tLength, mUnfilteredGradient.A(), aOutput.vector().A());
        }
    }
    double objectiveValue(const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aControl)
    {
        ScalarType tObjFunc = mSolver->computeCompliance(aControl.vector());
        if(mFirstObjFunc < 0.)
        { mFirstObjFunc = tObjFunc; }
        tObjFunc = tObjFunc / mFirstObjFunc;
        return tObjFunc;
    }

    double constraintValue(const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aControl)
    {
        ScalarType tCandidateConstraint = mSolver->computeVolume(aControl.vector());
        ScalarType tLambdaOverPenalty = -1.0 * (mLagrangeMultiplier / mPenaltyValue);
        ScalarType tConstraint = std::max(tCandidateConstraint,tLambdaOverPenalty);
        return tConstraint;
    }

    double penaltyFunction(const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aControl)
    {
        // P(z) = \frac{1}{N_g}\sum_{j=1}^{N_g}\left( \lambda_j * h_j(z) + \frac{\mu_j}{2}h_j(z)^2 \right)
        mCurrentConstraint = this->constraintValue(aControl);
        ScalarType tTermOneInPenaltyFunc = mLagrangeMultiplier * mCurrentConstraint;
        ScalarType tTermTwoInPenaltyFunc = (mPenaltyValue / 2.0) * mCurrentConstraint * mCurrentConstraint;
        ScalarType tOutput = tTermOneInPenaltyFunc + tTermTwoInPenaltyFunc;
        return (tOutput);
    }

    void objectiveGradient(const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aEpetraControl)
    {
        ScalarType tValue = 0;
        mObjGrad.SCAL(mObjGrad.Length(), tValue, mObjGrad.A());
        mSolver->computeComplianceGradient(aEpetraControl.vector(), mObjGrad);
        tValue = static_cast<ScalarType>(1) / mFirstObjFunc;
        mObjGrad.SCAL(mObjGrad.Length(), tValue, mObjGrad.A());
    }

    void penaltyGradient(const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aEpetraControl)
    {
        // volume only depends on control variables, there is no dependence on state variables
        ScalarType tValue = 0;
        mConstraintGrad.SCAL(mConstraintGrad.Length(), tValue, mConstraintGrad.A());
        mSolver->computeNormalizedVolumeGradient(aEpetraControl.vector(), mConstraintGrad);
        // add \lambda*\frac{\partial{h}}{\partial{z}}
        mPenaltyGrad.AXPY(mPenaltyGrad.Length(),mLagrangeMultiplier,mConstraintGrad.A(),mPenaltyGrad.A());
        // add \mu*h*\frac{\partial{h}}{\partial{z}}
        ScalarType tConstraintValue = this->constraintValue(aEpetraControl);
        ScalarType tScalar = mPenaltyValue * tConstraintValue;
        mPenaltyGrad.AXPY(mPenaltyGrad.Length(),tScalar,mConstraintGrad.A(),mPenaltyGrad.A());
    }

private:
    ProxyAugLagObjective(const Morphorm::ProxyAugLagObjective<ScalarType, OrdinalType>&);
    Morphorm::ProxyAugLagObjective<ScalarType, OrdinalType> 
        &operator=(const Morphorm::ProxyAugLagObjective<ScalarType, OrdinalType>&);
};

/*****************************************************************************************//**
 * @brief Diagnostic data for the Unconstrained Method of Moving Asymptotes (MMA) algorithm
*********************************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
struct OutputDataUMMA
{
    OrdinalType mNumOuterIter;   /*!< number of outer iterations */
    OrdinalType mObjFuncEvals;   /*!< number of objective function evaluations */
    OrdinalType mObjGradEvals;   /*!< number of objective function gradient evaluations */

    ScalarType mFeasibility;     /*!< change between two subsequent control solutions */
    ScalarType mCurrentObjFuncValue;    /*!< objective function value */
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
void is_diagnostic_file_close(const Plato::CommWrapper &aComm,OutputType &aOutputStream)
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
    aOutputStream << std::scientific << std::setprecision(6) << std::right << "Iter" << std::setw(10) << "F-count"
            << std::setw(12) << "G-count"<< std::setw(12) << "F(X)" << std::setw(16) << "Norm(F')" << std::setw(15) 
            << "abs(dX)" << std::setw(15) << "abs(dF)" << "\n" << std::flush;
}
// function write_umma_diagnostics_header

/******************************************************************************//**
 * @brief Output a brief sentence describing reason for convergence.
 * @param [in] aStopCriterion stopping criterion flag
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
        case Plato::algorithm::stop_t::OBJECTIVE_STAGNATION:
        {
            tOutput = "\n\n****** Optimization stopping due to objective stagnation. ******\n\n";
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
 * @param [in]  aComm  interface to MPI communicator
 * @param [in]  aData  diagnostic data for mma algorithm
 * @param [out] aOutputFile output file
**********************************************************************************/
template<typename ScalarType, typename OrdinalType, typename OutputType>
void write_umma_diagnostics(
 const Plato::CommWrapper &aComm,
 const Morphorm::OutputDataUMMA<ScalarType, OrdinalType> &aData,
 OutputType &aOutputStream
)
{
    aOutputStream << std::scientific << std::setprecision(6) << std::right << aData.mNumOuterIter 
        << std::setw(10) << aData.mObjFuncEvals  << std::setw(12) << aData.mObjGradEvals << std::setw(18) 
        << aData.mCurrentObjFuncValue << std::setw(15) << aData.mNormObjFuncGrad << std::setw(15) 
        << aData.mControlChange << std::setw(15) << aData.mObjFuncChange << "\n" << std::flush;
}
// function write_umma_diagnostics

template<typename ScalarType, typename OrdinalType = size_t>
struct UnconstrainedMethodMovingAsymptotesDataMng
{
private:
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
        mCurrentObjectiveGradient(aDataFactory->control().create()),
        mPreviousObjectiveGradient(aDataFactory->control().create()),
        mComm(aDataFactory->getCommWrapper().create()),
        mReductionOps(aDataFactory->getControlReductionOperations().create())
    {}
    ~UnconstrainedMethodMovingAsymptotesDataMng(){}

public:
    OrdinalType mNumObjFuncs = 1;

    ScalarType mCurrentObjFuncValue   = -1.0; /*!< current objective values */
    ScalarType mPreviousObjFuncValue  = -2.0; /*!< previous objective values */

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
    std::shared_ptr<MultiVec> mCurrentObjectiveGradient;    
    std::shared_ptr<MultiVec> mPreviousObjectiveGradient;

    std::shared_ptr<Plato::CommWrapper> mComm;   /*!< interface to MPI communicator */
    std::shared_ptr<ReductionOps> mReductionOps; /*!< interface to math reduction operations */
};

template<typename ScalarType, typename OrdinalType = size_t>
class UnconstrainedMethodMovingAsymptotes
{
private:
    typedef Morphorm::UnconstrainedMethodMovingAsymptotesDataMng<ScalarType, OrdinalType> DataMng;

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
            mOutputStream.open("morphorm_umma_algorithm_diagnostics.txt");
            Morphorm::is_diagnostic_file_close(mDataMng->mComm.operator*(),mOutputStream);
            Morphorm::write_umma_diagnostics_header(mDataMng->mComm.operator*(),mOutputData, mOutputStream);
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
        Morphorm::is_diagnostic_file_close(mDataMng->mComm.operator*(),mOutputStream);
        Morphorm::write_umma_diagnostics(mDataMng->mComm.operator*(),mOutputData,mOutputStream);
    }

    void writeDiagnosticsToConsole() const
    {
        std::stringstream tConsoleStream;
        Morphorm::write_umma_diagnostics_header(mDataMng->mComm.operator*(),mOutputData,tConsoleStream);
        Morphorm::write_umma_diagnostics(mDataMng->mComm.operator*(),mOutputData, tConsoleStream);
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
    Morphorm::OutputDataUMMA<ScalarType,OrdinalType> mOutputData; /*!< output data written to diagnostic file */

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
     * @brief Default constructor
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
     * @brief Default destructor
    **********************************************************************************/
    virtual ~AlgorithmInputsUMMA()
    {
    }

    bool mWriteDiagnostics = false; /*!< write diagnostics to file */

    OrdinalType mMaxNumOuterIterations = 150;
    OrdinalType mMaxNumSubProblemIterations = 5;

    ScalarType mMoveLimit                   = 0.15;
    ScalarType mAsymptotesConstant          = 0.2;
    ScalarType mControlChangeTolerance      = 1e-3;
    ScalarType mObjectiveChangeTolerance    = 1e-10;
    ScalarType mAsymptotesInitialMultiplier = 0.2;
    ScalarType mAsymptotesIncrementConstant = 1.2; 
    ScalarType mAsymptotesDecrementConstant = 0.7; 

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
(const Morphorm::AlgorithmInputsUMMA<ScalarType,OrdinalType>                 &aInputs,
       Morphorm::UnconstrainedMethodMovingAsymptotes<ScalarType,OrdinalType> &aAlgorithm)
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
(const Morphorm::UnconstrainedMethodMovingAsymptotes<ScalarType,OrdinalType> &aAlgorithm,
       Morphorm::AlgorithmOutputsUMMA<ScalarType,OrdinalType>                &aOutputs)
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
template<typename ScalarType, typename OrdinalType = size_t>
inline void solve_unconstrained_method_moving_asymptotes
(const std::shared_ptr<Plato::Criterion<ScalarType,OrdinalType>> &aObjective,
 const Morphorm::AlgorithmInputsUMMA<ScalarType,OrdinalType>     &aInputs,
       Morphorm::AlgorithmOutputsUMMA<ScalarType,OrdinalType>    &aOutputs)
{
    // create data structures 
    auto tDataFactory = std::make_shared<Plato::DataFactory<ScalarType, OrdinalType>>();
    tDataFactory->setCommWrapper(aInputs.mComm);
    tDataFactory->allocateControl(aInputs.mInitialGuess.operator*());
    tDataFactory->allocateObjFuncValues(1/* num objective functions */);
    tDataFactory->allocateControlReductionOperations(aInputs.mReductionOperations.operator*());
    // create algorithm interface
    Morphorm::UnconstrainedMethodMovingAsymptotes<ScalarType,OrdinalType> tAlgorithm(aObjective,tDataFactory);
    Morphorm::set_uconstrained_mma_algorithm_inputs(aInputs,tAlgorithm);
    tAlgorithm.solve();
    Morphorm::set_unconstrained_mma_algorithm_outputs(tAlgorithm,aOutputs);
}
// function solve_unconstrained_method_moving_asymptotes

}

namespace MorphormTest
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

TEST(MorphormTest, UnconstrainedMethodMovingAsymptotesDataMng)
{
    // create data factory
    auto tDataFactory = std::make_shared<Plato::DataFactory<double>>();
    tDataFactory->allocateObjFuncValues(1/* num objective functions */);
    tDataFactory->allocateControl(10/* num controls */,1/* num vectors */);
    
    Morphorm::UnconstrainedMethodMovingAsymptotesDataMng<double> tDataMng(tDataFactory);
    
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

TEST(MorphormTest, is_diagnostic_file_close)
{
    std::ofstream tOutputStream;
    Plato::CommWrapper tComm;
    tComm.useDefaultComm();
    // CAN'T RUN OPTION SINCE IT WILL FORCE CODE TO STOP
    //ASSERT_THROW(Morphorm::is_diagnostic_file_close(tComm,tOutputStream),std::runtime_error);
    tOutputStream.open("dummy.txt");
    ASSERT_NO_THROW(Morphorm::is_diagnostic_file_close(tComm,tOutputStream));
    tOutputStream.close();
}

TEST(MorphormTest, write_umma_diagnostics_header_plus_data)
{
    Plato::CommWrapper tComm;
    tComm.useDefaultComm();

    Morphorm::OutputDataUMMA<double> tDiagnosticsData;
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
    ASSERT_NO_THROW(Morphorm::write_umma_diagnostics_header(tComm,tDiagnosticsData,tOutputStream));
    ASSERT_NO_THROW(Morphorm::write_umma_diagnostics(tComm,tDiagnosticsData,tOutputStream));

    auto tReadData = PlatoTest::read_data_from_file("dummy.txt");
    auto tGold = std::string("IterF-countG-countF(X)Norm(F')abs(dX)abs(dF)") 
               + "3333331.235000e-031.598300e-043.300000e-025.235800e-03";
    ASSERT_STREQ(tReadData.str().c_str(),tGold.c_str());

    auto tTrash = std::system("rm dummy.txt");
    Plato::Utils::ignore_unused(tTrash);

    tOutputStream.close();
}

TEST(MorphormTest, get_umma_stop_criterion_description)
{
    // option 1
    std::string tMsg = Morphorm::get_umma_stop_criterion_description(Plato::algorithm::stop_t::CONTROL_STAGNATION);
    auto tGold = std::string("\n\n****** Optimization stopping due to control (i.e. design variable) stagnation. ******\n\n");
    EXPECT_STREQ(tMsg.c_str(),tGold.c_str());

    // option 2
    tMsg = Morphorm::get_umma_stop_criterion_description(Plato::algorithm::stop_t::MAX_NUMBER_ITERATIONS);
    tGold = std::string("\n\n****** Optimization stopping due to exceeding maximum number of iterations. ******\n\n");
    EXPECT_STREQ(tMsg.c_str(),tGold.c_str());

    // option 3
    tMsg = Morphorm::get_umma_stop_criterion_description(Plato::algorithm::stop_t::NOT_CONVERGED);
    tGold = std::string("\n\n****** Optimization algorithm did not converge. ******\n\n");
    EXPECT_STREQ(tMsg.c_str(),tGold.c_str());

    // option 4
    tMsg = Morphorm::get_umma_stop_criterion_description(Plato::algorithm::stop_t::STATIONARITY_MEASURE);
    tGold = std::string("\n\n****** ERROR: Optimization algorithm stopping due to undefined behavior. ******\n\n");
    EXPECT_STREQ(tMsg.c_str(),tGold.c_str());
}

TEST(MorphormTest, PERF_UnconstrainedMethodMovingAsymptotes)
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
    std::shared_ptr<Plato::Criterion<double>> tDesignCriterion = 
        std::make_shared<Morphorm::ProxyAugLagObjective<double>>(tLinearElasticitySolver);
    
    // create data factory
    const size_t tNumVectors = 1;
    const size_t tNumControls = tLinearElasticitySolver->getNumDesignVariables();
    Morphorm::AlgorithmInputsUMMA<double> tInputs;
    tInputs.mControlChangeTolerance = 1e-4;
    const double tValue = tLinearElasticitySolver->getVolumeFraction();
    tInputs.mUpperBounds  = std::make_shared<Plato::EpetraSerialDenseMultiVector<double>>(tNumVectors, tNumControls, 1.0 /* base value */);
    tInputs.mLowerBounds  = std::make_shared<Plato::EpetraSerialDenseMultiVector<double>>(tNumVectors, tNumControls, 1e-3 /* base value */);
    tInputs.mInitialGuess = std::make_shared<Plato::EpetraSerialDenseMultiVector<double>>(tNumVectors, tNumControls, tValue);
    
    // create optimization algorithm and solve
    Morphorm::AlgorithmOutputsUMMA<double> tOutputs;
    Morphorm::solve_unconstrained_method_moving_asymptotes(tDesignCriterion,tInputs,tOutputs);

    // test diagnostics
    ASSERT_EQ(25u, tOutputs.mNumOuterIterations);
    ASSERT_EQ(126u,tOutputs.mNumObjFuncEvals);
    ASSERT_EQ(126u,tOutputs.mNumObjGradEvals);

    // test solution
    const double tTolerance = 1e-6;
    auto tGold = MorphormTest::get_gold_control_umma_test();
    for(size_t tIndex = 0; tIndex < tOutputs.mSolution->operator[](0).size(); tIndex++)
    {
        ASSERT_NEAR(tGold[tIndex],tOutputs.mSolution->operator()(0/* vector index */,tIndex), tTolerance);
    }            
}

}
// namespace MorphTest