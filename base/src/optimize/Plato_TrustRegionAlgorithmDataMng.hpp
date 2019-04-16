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
// *************************************************************************
//@HEADER
*/

/*
 * Plato_TrustRegionAlgorithmDataMng.hpp
 *
 *  Created on: Oct 21, 2017
 */

#ifndef PLATO_TRUSTREGIONALGORITHMDATAMNG_HPP_
#define PLATO_TRUSTREGIONALGORITHMDATAMNG_HPP_

#include <limits>
#include <memory>
#include <cassert>

#include "Plato_Vector.hpp"
#include "Plato_HostBounds.hpp"
#include "Plato_MultiVector.hpp"
#include "Plato_DataFactory.hpp"
#include "Plato_DeviceBounds.hpp"
#include "Plato_LinearAlgebra.hpp"
#include "Plato_ReductionOperations.hpp"

namespace Plato
{

template<typename ScalarType, typename OrdinalType = size_t>
class TrustRegionAlgorithmDataMng
{
public:
    /******************************************************************************//**
     * @brief Constructor
     * @param [in] aFactory enables allocation of core vectors and multi-vectors
    **********************************************************************************/
    TrustRegionAlgorithmDataMng(const std::shared_ptr<Plato::DataFactory<ScalarType, OrdinalType>> & aFactory) :
            mNumObjFuncEval(0),
            mNumObjGradEval(0),
            mNumDualVectors(aFactory->dual().getNumVectors()),
            mNumControlVectors(aFactory->control().getNumVectors()),
            mStationarityMeasure(0),
            mNormProjectedGradient(0),
            mControlStagnationMeasure(0),
            mObjectiveStagnationMeasure(0),
            mGradientInexactnessTolerance(0),
            mObjectiveInexactnessTolerance(0),
            mCurrentObjectiveFunctionValue(std::numeric_limits<ScalarType>::max()),
            mPreviousObjectiveFunctionValue(std::numeric_limits<ScalarType>::max()),
            mIsMeanNormEnabled(false),
            mIsInitialGuessSet(false),
            mGradientInexactnessToleranceExceeded(false),
            mObjectiveInexactnessToleranceExceeded(false),
            mDual(aFactory->dual().create()),
            mTrialStep(aFactory->control().create()),
            mActiveSet(aFactory->control().create()),
            mInactiveSet(aFactory->control().create()),
            mCurrentControl(aFactory->control().create()),
            mPreviousControl(aFactory->control().create()),
            mCurrentGradient(aFactory->control().create()),
            mPreviousGradient(aFactory->control().create()),
            mControlLowerBounds(aFactory->control().create()),
            mControlUpperBounds(aFactory->control().create()),
            mControlWorkVector(),
            mControlWorkMultiVector(aFactory->control().create()),
            mCommWrapper(aFactory->getCommWrapper().create()),
            mBounds(std::make_shared<Plato::HostBounds<ScalarType, OrdinalType>>()),
            mDualReductionOperations(aFactory->getDualReductionOperations().create()),
            mControlReductionOperations(aFactory->getControlReductionOperations().create())
    {
        this->initialize(aFactory.operator*());
    }

    /******************************************************************************//**
     * @brief Destructor
    **********************************************************************************/
    virtual ~TrustRegionAlgorithmDataMng()
    {
    }

    /******************************************************************************//**
     * @brief Return a const reference to the distributed memory communication wrapper
     * @return const reference to the distributed memory communication wrapper
    **********************************************************************************/
    const Plato::CommWrapper& getCommWrapper() const
    {
        return (mCommWrapper.operator*());
    }

    /******************************************************************************//**
     * @brief Return number of control vectors
     * @return number of control vectors
    **********************************************************************************/
    OrdinalType getNumControlVectors() const
    {
        return (mNumControlVectors);
    }

    /******************************************************************************//**
     * @brief Return number of dual vectors
     * @return number of dual vectors
    **********************************************************************************/
    OrdinalType getNumDualVectors() const
    {
        return (mNumDualVectors);
    }

    /******************************************************************************//**
     * @brief Get number of objective function evaluations
     * @return number of objective function evaluations
    **********************************************************************************/
    OrdinalType getNumObjectiveFunctionEvaluations() const
    {
        return (mNumObjFuncEval);
    }

    /******************************************************************************//**
     * @brief Set number of objective function evaluations
     * @param [in] aInput number of objective function evaluations
    **********************************************************************************/
    void setNumObjectiveFunctionEvaluations(const OrdinalType& aInput)
    {
        mNumObjFuncEval = aInput;
    }

    /******************************************************************************//**
     * @brief Get number of objective gradient evaluations
     * @return number of objective gradient evaluations
    **********************************************************************************/
    OrdinalType getNumObjectiveGradientEvaluations() const
    {
        return (mNumObjGradEval);
    }

    /******************************************************************************//**
     * @brief Set number of objective gradient evaluations
     * @param [in] aInput number of objective gradient evaluations
    **********************************************************************************/
    void setNumObjectiveGradientEvaluations(const OrdinalType& aInput)
    {
        mNumObjGradEval = aInput;
    }

    /******************************************************************************//**
     * @brief Return current objective function value
     * @return objective function value
    **********************************************************************************/
    ScalarType getCurrentObjectiveFunctionValue() const
    {
        return (mCurrentObjectiveFunctionValue);
    }

    /******************************************************************************//**
     * @brief Set current objective function value
     * @param [in] objective function value
    **********************************************************************************/
    void setCurrentObjectiveFunctionValue(const ScalarType & aInput)
    {
        mCurrentObjectiveFunctionValue = aInput;
    }

    /******************************************************************************//**
     * @brief Return previous objective function value
     * @return objective function value
    **********************************************************************************/
    ScalarType getPreviousObjectiveFunctionValue() const
    {
        return (mPreviousObjectiveFunctionValue);
    }

    /******************************************************************************//**
     * @brief Set previous objective function value
     * @param [in] objective function value
    **********************************************************************************/
    void setPreviousObjectiveFunctionValue(const ScalarType & aInput)
    {
        mPreviousObjectiveFunctionValue = aInput;
    }

    /******************************************************************************//**
     * @brief Set initial guess: each element is set to a constant scalar value
     * @param [in] aInput scalar value
    **********************************************************************************/
    void setInitialGuess(const ScalarType & aInput)
    {
        assert(mCurrentControl.get() != nullptr);
        assert(mCurrentControl->getNumVectors() > static_cast<OrdinalType>(0));

        OrdinalType tNumVectors = mCurrentControl->getNumVectors();
        for(OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            mCurrentControl->operator [](tVectorIndex).fill(aInput);
        }
        mIsInitialGuessSet = true;
    }

    /******************************************************************************//**
     * @brief Set initial guess: each element in specified vector is set to a constant scalar value
     * @param [in] aVectorIndex control vector index
     * @param [in] aValue scalar value
    **********************************************************************************/
    void setInitialGuess(const OrdinalType & aVectorIndex, const ScalarType & aValue)
    {
        assert(mCurrentControl.get() != nullptr);
        assert(aVectorIndex < mCurrentControl->getNumVectors());

        mCurrentControl->operator [](aVectorIndex).fill(aValue);
        mIsInitialGuessSet = true;
    }

    /******************************************************************************//**
     * @brief Set initial guess
     * @param [in] aVectorIndex control vector index
     * @param [in] aInput control vector
    **********************************************************************************/
    void setInitialGuess(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mCurrentControl.get() != nullptr);
        assert(aVectorIndex < mCurrentControl->getNumVectors());

        mCurrentControl->operator [](aVectorIndex).update(1., aInput, 0.);
        mIsInitialGuessSet = true;
    }

    /******************************************************************************//**
     * @brief Set initial guess
     * @param [in] aInput control multi-vector
    **********************************************************************************/
    void setInitialGuess(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mCurrentControl->getNumVectors());
        Plato::update(1., aInput, 0., *mCurrentControl);
        mIsInitialGuessSet = true;
    }

    /******************************************************************************//**
     * @brief Enable mean norm, i.e. /f$ \mu = \frac{1}{N} \sum_{i=1}^{N} x_i * x_i /f$
    **********************************************************************************/
    void enableMeanNorm()
    {
        mIsMeanNormEnabled = true;
    }

    // NOTE: DUAL VECTOR
    const Plato::MultiVector<ScalarType, OrdinalType> & getDual() const
    {
        assert(mDual.get() != nullptr);
        return (mDual.operator *());
    }
    const Plato::Vector<ScalarType, OrdinalType> & getDual(const OrdinalType & aVectorIndex) const
    {
        assert(mDual.get() != nullptr);
        assert(aVectorIndex < mDual->getNumVectors());
        return (mDual->operator [](aVectorIndex));
    }
    void setDual(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mDual->getNumVectors());
        Plato::update(1., aInput, 0., *mDual);
    }
    void setDual(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mDual.get() != nullptr);
        assert(aVectorIndex < mDual->getNumVectors());
        mDual->operator [](aVectorIndex).update(1., aInput, 0.);
    }

    // NOTE: TRIAL STEP
    const Plato::MultiVector<ScalarType, OrdinalType> & getTrialStep() const
    {
        assert(mTrialStep.get() != nullptr);

        return (mTrialStep.operator *());
    }
    const Plato::Vector<ScalarType, OrdinalType> & getTrialStep(const OrdinalType & aVectorIndex) const
    {
        assert(mTrialStep.get() != nullptr);
        assert(aVectorIndex < mTrialStep->getNumVectors());

        return (mTrialStep->operator [](aVectorIndex));
    }
    void setTrialStep(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mTrialStep->getNumVectors());
        Plato::update(1., aInput, 0., *mTrialStep);
    }
    void setTrialStep(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mTrialStep.get() != nullptr);
        assert(aVectorIndex < mTrialStep->getNumVectors());

        mTrialStep->operator [](aVectorIndex).update(1., aInput, 0.);
    }

    // NOTE: ACTIVE SET FUNCTIONS
    const Plato::MultiVector<ScalarType, OrdinalType> & getActiveSet() const
    {
        assert(mActiveSet.get() != nullptr);

        return (mActiveSet.operator *());
    }
    const Plato::Vector<ScalarType, OrdinalType> & getActiveSet(const OrdinalType & aVectorIndex) const
    {
        assert(mActiveSet.get() != nullptr);
        assert(aVectorIndex < mActiveSet->getNumVectors());

        return (mActiveSet->operator [](aVectorIndex));
    }
    void setActiveSet(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mActiveSet->getNumVectors());
        Plato::update(1., aInput, 0., *mActiveSet);
    }
    void setActiveSet(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mActiveSet.get() != nullptr);
        assert(aVectorIndex < mActiveSet->getNumVectors());

        mActiveSet->operator [](aVectorIndex).update(1., aInput, 0.);
    }

    // NOTE: INACTIVE SET FUNCTIONS
    const Plato::MultiVector<ScalarType, OrdinalType> & getInactiveSet() const
    {
        assert(mInactiveSet.get() != nullptr);

        return (mInactiveSet.operator *());
    }
    const Plato::Vector<ScalarType, OrdinalType> & getInactiveSet(const OrdinalType & aVectorIndex) const
    {
        assert(mInactiveSet.get() != nullptr);
        assert(aVectorIndex < mInactiveSet->getNumVectors());

        return (mInactiveSet->operator [](aVectorIndex));
    }
    void setInactiveSet(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mInactiveSet->getNumVectors());
        Plato::update(1., aInput, 0., *mInactiveSet);
    }
    void setInactiveSet(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mInactiveSet.get() != nullptr);
        assert(aVectorIndex < mInactiveSet->getNumVectors());

        mInactiveSet->operator [](aVectorIndex).update(1., aInput, 0.);
    }

    // NOTE: CURRENT CONTROL
    const Plato::MultiVector<ScalarType, OrdinalType> & getCurrentControl() const
    {
        assert(mCurrentControl.get() != nullptr);

        return (mCurrentControl.operator *());
    }
    const Plato::Vector<ScalarType, OrdinalType> & getCurrentControl(const OrdinalType & aVectorIndex) const
    {
        assert(mCurrentControl.get() != nullptr);
        assert(aVectorIndex < mCurrentControl->getNumVectors());

        return (mCurrentControl->operator [](aVectorIndex));
    }
    void setCurrentControl(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mCurrentControl->getNumVectors());
        Plato::update(static_cast<ScalarType>(1), aInput, static_cast<ScalarType>(0), *mCurrentControl);
    }
    void setCurrentControl(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mCurrentControl.get() != nullptr);
        assert(aVectorIndex < mCurrentControl->getNumVectors());

        mCurrentControl->operator [](aVectorIndex).update(1., aInput, 0.);
    }

    // NOTE: PREVIOUS CONTROL
    const Plato::MultiVector<ScalarType, OrdinalType> & getPreviousControl() const
    {
        assert(mPreviousControl.get() != nullptr);

        return (mPreviousControl.operator *());
    }
    const Plato::Vector<ScalarType, OrdinalType> & getPreviousControl(const OrdinalType & aVectorIndex) const
    {
        assert(mPreviousControl.get() != nullptr);
        assert(aVectorIndex < mPreviousControl->getNumVectors());

        return (mPreviousControl->operator [](aVectorIndex));
    }
    void setPreviousControl(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mPreviousControl->getNumVectors());
        Plato::update(1., aInput, 0., *mPreviousControl);
    }
    void setPreviousControl(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mPreviousControl.get() != nullptr);
        assert(aVectorIndex < mPreviousControl->getNumVectors());

        mPreviousControl->operator [](aVectorIndex).update(1., aInput, 0.);
    }

    // NOTE: CURRENT GRADIENT
    const Plato::MultiVector<ScalarType, OrdinalType> & getCurrentGradient() const
    {
        assert(mCurrentGradient.get() != nullptr);

        return (mCurrentGradient.operator *());
    }
    const Plato::Vector<ScalarType, OrdinalType> & getCurrentGradient(const OrdinalType & aVectorIndex) const
    {
        assert(mCurrentGradient.get() != nullptr);
        assert(aVectorIndex < mCurrentGradient->getNumVectors());

        return (mCurrentGradient->operator [](aVectorIndex));
    }
    void setCurrentGradient(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mCurrentGradient->getNumVectors());
        Plato::update(1., aInput, 0., *mCurrentGradient);
    }
    void setCurrentGradient(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mCurrentGradient.get() != nullptr);
        assert(aVectorIndex < mCurrentGradient->getNumVectors());

        mCurrentGradient->operator [](aVectorIndex).update(1., aInput, 0.);
    }

    // NOTE: PREVIOUS GRADIENT
    const Plato::MultiVector<ScalarType, OrdinalType> & getPreviousGradient() const
    {
        assert(mPreviousGradient.get() != nullptr);

        return (mPreviousGradient.operator *());
    }
    const Plato::Vector<ScalarType, OrdinalType> & getPreviousGradient(const OrdinalType & aVectorIndex) const
    {
        assert(mPreviousGradient.get() != nullptr);
        assert(aVectorIndex < mPreviousGradient->getNumVectors());

        return (mPreviousGradient->operator [](aVectorIndex));
    }
    void setPreviousGradient(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mPreviousGradient->getNumVectors());
        Plato::update(1., aInput, 0., *mPreviousGradient);
    }
    void setPreviousGradient(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mPreviousGradient.get() != nullptr);
        assert(aVectorIndex < mPreviousGradient->getNumVectors());

        mPreviousGradient->operator [](aVectorIndex).update(1., aInput, 0.);
    }

    // NOTE: SET CONTROL LOWER BOUNDS
    const Plato::MultiVector<ScalarType, OrdinalType> & getControlLowerBounds() const
    {
        assert(mControlLowerBounds.get() != nullptr);

        return (mControlLowerBounds.operator *());
    }
    const Plato::Vector<ScalarType, OrdinalType> & getControlLowerBounds(const OrdinalType & aVectorIndex) const
    {
        assert(mControlLowerBounds.get() != nullptr);
        assert(aVectorIndex < mControlLowerBounds->getNumVectors());

        return (mControlLowerBounds->operator [](aVectorIndex));
    }
    void setControlLowerBounds(const ScalarType & aValue)
    {
        assert(mControlLowerBounds.get() != nullptr);
        assert(mControlLowerBounds->getNumVectors() > static_cast<OrdinalType>(0));

        OrdinalType tNumVectors = mControlLowerBounds->getNumVectors();
        for(OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            mControlLowerBounds->operator [](tVectorIndex).fill(aValue);
        }
    }
    void setControlLowerBounds(const OrdinalType & aVectorIndex, const ScalarType & aValue)
    {
        assert(mControlLowerBounds.get() != nullptr);
        assert(aVectorIndex < mControlLowerBounds->getNumVectors());

        mControlLowerBounds->operator [](aVectorIndex).fill(aValue);
    }
    void setControlLowerBounds(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mControlLowerBounds.get() != nullptr);
        assert(aVectorIndex < mControlLowerBounds->getNumVectors());

        mControlLowerBounds->operator [](aVectorIndex).update(1., aInput, 0.);
    }
    void setControlLowerBounds(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mControlLowerBounds->getNumVectors());
        Plato::update(1., aInput, 0., *mControlLowerBounds);
    }

    // NOTE: SET CONTROL UPPER BOUNDS
    const Plato::MultiVector<ScalarType, OrdinalType> & getControlUpperBounds() const
    {
        assert(mControlUpperBounds.get() != nullptr);

        return (mControlUpperBounds.operator *());
    }
    const Plato::Vector<ScalarType, OrdinalType> & getControlUpperBounds(const OrdinalType & aVectorIndex) const
    {
        assert(mControlUpperBounds.get() != nullptr);
        assert(aVectorIndex < mControlUpperBounds->getNumVectors());

        return (mControlUpperBounds->operator [](aVectorIndex));
    }
    void setControlUpperBounds(const ScalarType & aValue)
    {
        assert(mControlUpperBounds.get() != nullptr);
        assert(mControlUpperBounds->getNumVectors() > static_cast<OrdinalType>(0));

        OrdinalType tNumVectors = mControlUpperBounds->getNumVectors();
        for(OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            mControlUpperBounds->operator [](tVectorIndex).fill(aValue);
        }
    }
    void setControlUpperBounds(const OrdinalType & aVectorIndex, const ScalarType & aValue)
    {
        assert(mControlUpperBounds.get() != nullptr);
        assert(aVectorIndex < mControlUpperBounds->getNumVectors());

        mControlUpperBounds->operator [](aVectorIndex).fill(aValue);
    }
    void setControlUpperBounds(const OrdinalType & aVectorIndex, const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        assert(mControlUpperBounds.get() != nullptr);
        assert(aVectorIndex < mControlUpperBounds->getNumVectors());

        mControlUpperBounds->operator [](aVectorIndex).update(1., aInput, 0.);
    }
    void setControlUpperBounds(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        assert(aInput.getNumVectors() == mControlUpperBounds->getNumVectors());
        Plato::update(1., aInput, 0., *mControlUpperBounds);
    }

    // NOTE: OBJECTIVE AND GRADIENT INEXACTNESS VIOLATION FLAGS
    bool isInitialGuessSet() const
    {
        return (mIsInitialGuessSet);
    }
    void setGradientInexactnessFlag(const bool & aInput)
    {
        mGradientInexactnessToleranceExceeded = aInput;
    }
    bool isGradientInexactnessToleranceExceeded() const
    {
        return (mGradientInexactnessToleranceExceeded);
    }
    void setObjectiveInexactnessFlag(const bool & aInput)
    {
        mObjectiveInexactnessToleranceExceeded = aInput;
    }
    bool isObjectiveInexactnessToleranceExceeded() const
    {
        return (mObjectiveInexactnessToleranceExceeded);
    }

    // NOTE: COMPUTE OBJECTIVE FUNCTION STAGNATION MEASURE
    void computeObjectiveStagnationMeasure()
    {
        mObjectiveStagnationMeasure = std::abs(mPreviousObjectiveFunctionValue - mCurrentObjectiveFunctionValue);
    }
    ScalarType getObjectiveStagnationMeasure() const
    {
        return (mObjectiveStagnationMeasure);
    }

    // NOTE: STAGNATION MEASURE CRITERION
    void computeControlStagnationMeasure()
    {
        OrdinalType tNumVectors = mCurrentControl->getNumVectors();
        std::vector<ScalarType> tStorage(tNumVectors, std::numeric_limits<ScalarType>::min());
        for(OrdinalType tIndex = 0; tIndex < tNumVectors; tIndex++)
        {
            const Plato::Vector<ScalarType, OrdinalType> & tMyCurrentControl = mCurrentControl->operator[](tIndex);
            mControlWorkVector->update(1., tMyCurrentControl, 0.);
            const Plato::Vector<ScalarType, OrdinalType> & tMyPreviousControl = mPreviousControl->operator[](tIndex);
            mControlWorkVector->update(-1., tMyPreviousControl, 1.);
            mControlWorkVector->modulus();
            tStorage[tIndex] = mControlReductionOperations->max(*mControlWorkVector);
        }
        mControlStagnationMeasure = *std::max_element(tStorage.begin(), tStorage.end());
    }
    ScalarType getControlStagnationMeasure() const
    {
        return (mControlStagnationMeasure);
    }

    // NOTE: NORM OF CURRENT PROJECTED GRADIENT
    ScalarType computeProjectedVectorNorm(const Plato::MultiVector<ScalarType, OrdinalType> & aInput)
    {
        Plato::update(1., aInput, 0., *mControlWorkMultiVector);
        Plato::entryWiseProduct(*mInactiveSet, *mControlWorkMultiVector);
        const ScalarType tOutput =
                mIsMeanNormEnabled == true ? Plato::norm_mean(*mControlWorkMultiVector) : Plato::norm(*mControlWorkMultiVector);
        return(tOutput);
    }
    void computeNormProjectedGradient()
    {
        Plato::update(1., *mCurrentGradient, 0., *mControlWorkMultiVector);
        Plato::entryWiseProduct(*mInactiveSet, *mControlWorkMultiVector);
        mNormProjectedGradient =
                mIsMeanNormEnabled == true ? Plato::norm_mean(*mControlWorkMultiVector) : Plato::norm(*mControlWorkMultiVector);
    }

    ScalarType getNormProjectedGradient() const
    {
        return (mNormProjectedGradient);
    }

    void computeActiveAndInactiveSet()
    {
        assert(mCurrentControl.get() != nullptr);
        assert(mCurrentGradient.get() != nullptr);
        assert(mControlLowerBounds.get() != nullptr);
        assert(mControlUpperBounds.get() != nullptr);

        Plato::update(1., *mCurrentControl, 0., *mControlWorkMultiVector);
        Plato::update(-1., *mCurrentGradient, 1., *mControlWorkMultiVector);
        mBounds->computeActiveAndInactiveSets(*mControlWorkMultiVector,
                                              *mControlLowerBounds,
                                              *mControlUpperBounds,
                                              *mActiveSet,
                                              *mInactiveSet);
        this->checkInactiveSet();
    }

    // NOTE: STATIONARITY MEASURE CALCULATION
    void computeStationarityMeasure()
    {
        assert(mCurrentControl.get() != nullptr);
        assert(mCurrentGradient.get() != nullptr);
        assert(mControlLowerBounds.get() != nullptr);
        assert(mControlUpperBounds.get() != nullptr);

        Plato::update(1., *mCurrentControl, 0., *mControlWorkMultiVector);
        Plato::update(-1., *mCurrentGradient, 1., *mControlWorkMultiVector);
        mBounds->project(*mControlLowerBounds, *mControlUpperBounds, *mControlWorkMultiVector);
        Plato::update(1., *mCurrentControl, -1., *mControlWorkMultiVector);
        Plato::entryWiseProduct(*mInactiveSet, *mControlWorkMultiVector);
        mStationarityMeasure =
                mIsMeanNormEnabled == true ? Plato::norm_mean(*mControlWorkMultiVector) : Plato::norm(*mControlWorkMultiVector);
    }

    ScalarType getStationarityMeasure() const
    {
        return (mStationarityMeasure);
    }

    // NOTE: RESET AND STORE STAGE DATA
    void resetCurrentStageDataToPreviousStageData()
    {
        OrdinalType tNumVectors = mCurrentGradient->getNumVectors();
        for(OrdinalType tIndex = 0; tIndex < tNumVectors; tIndex++)
        {
            Plato::Vector<ScalarType, OrdinalType> & tMyCurrentControl = (*mCurrentControl)[tIndex];
            const Plato::Vector<ScalarType, OrdinalType> & tMyPreviousControl = (*mPreviousControl)[tIndex];
            tMyCurrentControl.update(1., tMyPreviousControl, 0.);

            Plato::Vector<ScalarType, OrdinalType> & tMyCurrentGradient = (*mCurrentGradient)[tIndex];
            const Plato::Vector<ScalarType, OrdinalType> & tMyPreviousGradient = (*mPreviousGradient)[tIndex];
            tMyCurrentGradient.update(1., tMyPreviousGradient, 0.);
        }
        mCurrentObjectiveFunctionValue = mPreviousObjectiveFunctionValue;
    }
    void cacheCurrentStageData()
    {
        const ScalarType tCurrentObjectiveValue = this->getCurrentObjectiveFunctionValue();
        this->setPreviousObjectiveFunctionValue(tCurrentObjectiveValue);

        OrdinalType tNumVectors = mCurrentControl->getNumVectors();
        for(OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            const Plato::Vector<ScalarType, OrdinalType> & tMyCurrentControl = this->getCurrentControl(tVectorIndex);
            this->setPreviousControl(tVectorIndex, tMyCurrentControl);

            const Plato::Vector<ScalarType, OrdinalType> & tMyCurrentGradient = this->getCurrentGradient(tVectorIndex);
            this->setPreviousGradient(tVectorIndex, tMyCurrentGradient);
        }
    }

    /******************************************************************************//**
     * @brief Return bound constraints interface
     * @return bound constraints interface
    **********************************************************************************/
    const Plato::BoundsBase<ScalarType, OrdinalType> & bounds() const
    {
        return (*mBounds);
    }

private:
    /******************************************************************************//**
     * @brief Initialize defaults for core data structures
     * @param [in] aDataFactory linear algebra factory
    **********************************************************************************/
    void initialize(const Plato::DataFactory<ScalarType, OrdinalType> & aDataFactory)
    {
        assert(aDataFactory.control().getNumVectors() > 0);

        const OrdinalType tVectorIndex = 0;
        mControlWorkVector = aDataFactory.control(tVectorIndex).create();
        Plato::fill(static_cast<ScalarType>(0), *mActiveSet);
        Plato::fill(static_cast<ScalarType>(1), *mInactiveSet);

        ScalarType tScalarValue = std::numeric_limits<ScalarType>::max();
        Plato::fill(tScalarValue, *mControlUpperBounds);
        tScalarValue = -std::numeric_limits<ScalarType>::max();
        Plato::fill(tScalarValue, *mControlLowerBounds);

        Plato::MemorySpace::type_t tMemorySpace = aDataFactory.getMemorySpace();
        switch(tMemorySpace)
        {
            case Plato::MemorySpace::DEVICE:
            {
                mBounds = std::make_shared<Plato::DeviceBounds<ScalarType, OrdinalType>>();
                break;
            }
            default:
            case Plato::MemorySpace::HOST:
            {
                mBounds = std::make_shared<Plato::HostBounds<ScalarType, OrdinalType>>();
                break;
            }
        }
    }

    /******************************************************************************//**
     * @brief Check if inactive set is null
    **********************************************************************************/
    void checkInactiveSet()
    {
        const OrdinalType tNumControlVectors = mInactiveSet->getNumVectors();
        for(OrdinalType tIndex = 0; tIndex < tNumControlVectors; tIndex++)
        {
            ScalarType tNormInactiveSet = mInactiveSet->operator[](tIndex).dot(mInactiveSet->operator[](tIndex));
            tNormInactiveSet = std::sqrt(tNormInactiveSet);
            if(tNormInactiveSet <= static_cast<ScalarType>(0))
            {
                mActiveSet->operator[](tIndex).fill(static_cast<ScalarType>(0));
                mInactiveSet->operator[](tIndex).fill(static_cast<ScalarType>(1));
            }
        }
    }

private:
    OrdinalType mNumObjFuncEval;
    OrdinalType mNumObjGradEval;
    OrdinalType mNumDualVectors;
    OrdinalType mNumControlVectors;

    ScalarType mStationarityMeasure;
    ScalarType mNormProjectedGradient;
    ScalarType mControlStagnationMeasure;
    ScalarType mObjectiveStagnationMeasure;
    ScalarType mGradientInexactnessTolerance;
    ScalarType mObjectiveInexactnessTolerance;
    ScalarType mCurrentObjectiveFunctionValue;
    ScalarType mPreviousObjectiveFunctionValue;

    bool mIsMeanNormEnabled;
    bool mIsInitialGuessSet;
    bool mGradientInexactnessToleranceExceeded;
    bool mObjectiveInexactnessToleranceExceeded;

    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mDual;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mTrialStep;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mActiveSet;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mInactiveSet;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mCurrentControl;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mPreviousControl;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mCurrentGradient;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mPreviousGradient;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mControlLowerBounds;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mControlUpperBounds;

    std::shared_ptr<Plato::Vector<ScalarType, OrdinalType>> mControlWorkVector;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mControlWorkMultiVector;

    std::shared_ptr<Plato::CommWrapper> mCommWrapper;
    std::shared_ptr<Plato::BoundsBase<ScalarType, OrdinalType>> mBounds;
    std::shared_ptr<Plato::ReductionOperations<ScalarType, OrdinalType>> mDualReductionOperations;
    std::shared_ptr<Plato::ReductionOperations<ScalarType, OrdinalType>> mControlReductionOperations;

private:
    TrustRegionAlgorithmDataMng(const Plato::TrustRegionAlgorithmDataMng<ScalarType, OrdinalType> & aRhs);
    Plato::TrustRegionAlgorithmDataMng<ScalarType, OrdinalType> & operator=(const Plato::TrustRegionAlgorithmDataMng<ScalarType, OrdinalType> & aRhs);
};

}

#endif /* PLATO_TRUSTREGIONALGORITHMDATAMNG_HPP_ */
