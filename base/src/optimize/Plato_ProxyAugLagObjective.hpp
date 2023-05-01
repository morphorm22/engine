#pragma once

#include "Plato_Criterion.hpp"
#include "Plato_EpetraSerialDenseMultiVector.hpp"
#include "Plato_StructuralTopologyOptimization.hpp"

namespace Plato
{

/******************************************************************************//**
 * @tparam ScalarType  type for non-integer values
 * @tparam OrdinalType type for integer values
 * @class ProxyAugLagObjective
 * 
 * @brief Evaluates augmented Lagrangian objective function terms needed for 
 *        gradient-based optimization. Formulation is based on a structural 
 *        topology optimization formulation with an internal energy objective 
 *        and a volume constraint. 
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
class ProxyAugLagObjective : public Plato::Criterion<ScalarType, OrdinalType>
{
private:
    bool mIsFilterDisabled = false; /*!< disables/enables sensitivity filter */

    ScalarType mPenaltyMax = 10000.; /*!< maximum augmented Lagrangian penalty */
    ScalarType mFirstObjFunc = -1.0; /*!< objective function value at iteration zero */
    ScalarType mPenaltyValue =  1.0; /*!< current augmented Lagrangian penalty */
    ScalarType mPenaltyMultiplier = 1.1;  /*!< increment multiplier for augmented Lagrangian penalty */
    ScalarType mLagrangeMultiplier = 0.0; /*!< current Lagrange multiplier */
    ScalarType mCurrentConstraint  = 0.0; /*!< current constraint value */
    ScalarType mPreviousConstraint = 0.0; /*!< previous constraint value */

    Epetra_SerialDenseVector mObjGrad; /*!< current objective function gradient */
    Epetra_SerialDenseVector mPenaltyGrad; /*!< current penalty term gradient */
    Epetra_SerialDenseVector mConstraintGrad; /*!< current constraint gradient */
    Epetra_SerialDenseVector mFilteredGradient; /*!< current filtered gradient */
    Epetra_SerialDenseVector mUnfilteredGradient; /*!< current unfiltered gradient */
    
    std::shared_ptr<Plato::StructuralTopologyOptimization> mSolver; /*!< 2D linear elasticity FEM solver */

public:
    /******************************************************************************//**
     * @brief Constructor.
     * @param [in] aSolver const shared pointer reference to FEM solver
    **********************************************************************************/
    explicit ProxyAugLagObjective(const std::shared_ptr<Plato::StructuralTopologyOptimization> & aSolver) :
        mObjGrad(Epetra_SerialDenseVector(aSolver->getNumDesignVariables())),
        mPenaltyGrad(Epetra_SerialDenseVector(aSolver->getNumDesignVariables())),
        mConstraintGrad(Epetra_SerialDenseVector(aSolver->getNumDesignVariables())),
        mFilteredGradient(Epetra_SerialDenseVector(aSolver->getNumDesignVariables())),
        mUnfilteredGradient(Epetra_SerialDenseVector(aSolver->getNumDesignVariables())),
        mSolver(aSolver)
    {}

    /******************************************************************************//**
     * @brief Destructor
    **********************************************************************************/
    virtual ~ProxyAugLagObjective(){}

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

    /******************************************************************************//**
     * @brief Updates Lagrange multipliers and penalty parameters
     * @param [in] aControl current vector of control variables
    **********************************************************************************/
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

    /******************************************************************************//**
     * @brief Evaluates augmented Lagrangian objective
     * @param [in] aControl current vector of control variables
     * @return objective function evaluation
    **********************************************************************************/
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
    
    /******************************************************************************//**
     * @brief Computes gradient of augmented Lagrangian objective
     * @param [in]  aControl current vector of control variables
     * @param [out] aOutput  current gradient
    **********************************************************************************/
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

    /******************************************************************************//**
     * @brief Computes application of search direction to the Hessian of the augmented 
     *        Lagrangian objective
     * @param [in]  aControl current vector of control variables
     * @param [in]  aVector  current search direction
     * @param [out] aOutput  current gradient
    **********************************************************************************/
    void hessian(const Plato::MultiVector<ScalarType, OrdinalType> & aControl,
                 const Plato::MultiVector<ScalarType, OrdinalType> & aVector,
                 Plato::MultiVector<ScalarType, OrdinalType> & aOutput)
    {
        THROWERR("Hessian is not implemented in class ProxyAugLagObjective")
    }

private:
    /******************************************************************************//**
     * @brief Applies sensitivity filter
     * @param [in]  aControl current vector of control variables
     * @param [out] aOutput  current filtered gradient
    **********************************************************************************/
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

    /******************************************************************************//**
     * @brief Evaluates current objective function
     * @param [in]  aControl current vector of control variables
     * @return objective function value
    **********************************************************************************/
    double objectiveValue(const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aControl)
    {
        ScalarType tObjFunc = mSolver->computeCompliance(aControl.vector());
        if(mFirstObjFunc < 0.)
        { mFirstObjFunc = tObjFunc; }
        tObjFunc = tObjFunc / mFirstObjFunc;
        return tObjFunc;
    }

    /******************************************************************************//**
     * @brief Evaluates current constraint
     * @param [in]  aControl current vector of control variables
     * @return current constraint value
    **********************************************************************************/
    double constraintValue(const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aControl)
    {
        ScalarType tCandidateConstraint = mSolver->computeVolume(aControl.vector());
        ScalarType tLambdaOverPenalty = -1.0 * (mLagrangeMultiplier / mPenaltyValue);
        ScalarType tConstraint = std::max(tCandidateConstraint,tLambdaOverPenalty);
        return tConstraint;
    }

    /******************************************************************************//**
     * @brief Evaluates current augmented Lagrangian penalty term
     * @param [in]  aControl current vector of control variables
     * @return current penalty term value
    **********************************************************************************/
    double penaltyFunction(const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aControl)
    {
        // P(z) = \frac{1}{N_g}\sum_{j=1}^{N_g}\left( \lambda_j * h_j(z) + \frac{\mu_j}{2}h_j(z)^2 \right)
        mCurrentConstraint = this->constraintValue(aControl);
        ScalarType tTermOneInPenaltyFunc = mLagrangeMultiplier * mCurrentConstraint;
        ScalarType tTermTwoInPenaltyFunc = (mPenaltyValue / 2.0) * mCurrentConstraint * mCurrentConstraint;
        ScalarType tOutput = tTermOneInPenaltyFunc + tTermTwoInPenaltyFunc;
        return (tOutput);
    }

    /******************************************************************************//**
     * @brief Computes current objective gradient
     * @param [in]  aControl current vector of control variables
    **********************************************************************************/
    void objectiveGradient(const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aControl)
    {
        ScalarType tValue = 0;
        mObjGrad.SCAL(mObjGrad.Length(), tValue, mObjGrad.A());
        mSolver->computeComplianceGradient(aControl.vector(), mObjGrad);
        tValue = static_cast<ScalarType>(1) / mFirstObjFunc;
        mObjGrad.SCAL(mObjGrad.Length(), tValue, mObjGrad.A());
    }

    /******************************************************************************//**
     * @brief Computes current penalty term gradient
     * @param [in]  aControl current vector of control variables
    **********************************************************************************/
    void penaltyGradient(const Plato::EpetraSerialDenseVector<ScalarType, OrdinalType> &aControl)
    {
        // volume only depends on control variables, there is no dependence on state variables
        ScalarType tValue = 0;
        mConstraintGrad.SCAL(mConstraintGrad.Length(), tValue, mConstraintGrad.A());
        mSolver->computeNormalizedVolumeGradient(aControl.vector(), mConstraintGrad);
        // add \lambda*\frac{\partial{h}}{\partial{z}}
        mPenaltyGrad.AXPY(mPenaltyGrad.Length(),mLagrangeMultiplier,mConstraintGrad.A(),mPenaltyGrad.A());
        // add \mu*h*\frac{\partial{h}}{\partial{z}}
        ScalarType tConstraintValue = this->constraintValue(aControl);
        ScalarType tScalar = mPenaltyValue * tConstraintValue;
        mPenaltyGrad.AXPY(mPenaltyGrad.Length(),tScalar,mConstraintGrad.A(),mPenaltyGrad.A());
    }

private:
    ProxyAugLagObjective(const Plato::ProxyAugLagObjective<ScalarType, OrdinalType>&);
    Plato::ProxyAugLagObjective<ScalarType, OrdinalType> 
        &operator=(const Plato::ProxyAugLagObjective<ScalarType, OrdinalType>&);
};
// class ProxyAugLagObjective

}
// namespace Plato