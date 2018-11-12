/*
 * Plato_Test_Hessian.cpp
 *
 *  Created on: Nov 11, 2018
 */

#include <cmath>
#include <cassert>

namespace Plato
{

template<typename ScalarType, typename OrdinalType = size_t>
class HessianLBFGS : public Plato::LinearOperator<ScalarType, OrdinalType>
{
public:
    explicit HessianLBFGS(const Plato::DataFactory<ScalarType, OrdinalType> & aDataFactory, OrdinalType aStorage = 8) :
            mMaxStorage(aStorage),
            mMemoryLength(0),
            mLowerBoundCurvature(1e-7),
            mUpperBoundCurvature(1e7),
            mNewDeltaControl(aDataFactory.control().create()),
            mNewDeltaGradient(aDataFactory.control().create()),
            mOldHessTimesVector(aDataFactory.control().create()),
            mNewHessTimesVector(aDataFactory.control().create()),
            mDeltaControl(),
            mDeltaGradient(),
            mOldHessTimesDeltaControl(),
            mNewHessTimesDeltaControl()
    {
        this->initialize();
    }

    virtual ~HessianLBFGS()
    {
    }

    void setMaxStorage(const OrdinalType & aInput)
    {
        mMaxStorage = aInput;
        this->reset();
        this->initialize();
    }

    void setCurvatureBounds(const ScalarType & aLower, const ScalarType & aUpper)
    {
        mLowerBoundCurvature = aLower;
        mUpperBoundCurvature = aUpper;
    }

    void update(const Plato::StateData<ScalarType, OrdinalType> & aStateData)
    {
        const ScalarType tCurrentCriterionValue = aStateData.getCurrentCriterionValue();
        const ScalarType tNormCurrentControl = Plato::norm(aStateData.getCurrentControl());
        const ScalarType tNormCurrentCriterionGrad = Plato::norm(aStateData.getCurrentCriterionGradient());
        if(std::isfinite(tCurrentCriterionValue) && std::isfinite(tNormCurrentControl) && std::isfinite(tNormCurrentCriterionGrad))
        {
            this->computeNewSecantInformation(aStateData);
            this->updateMemory();
        }
    }

    void apply(const Plato::MultiVector<ScalarType, OrdinalType> & aControl,
               const Plato::MultiVector<ScalarType, OrdinalType> & aVector,
               Plato::MultiVector<ScalarType, OrdinalType> & aOutput)
    {
        if(mMemoryLength == static_cast<OrdinalType>(0))
        {
            Plato::update(static_cast<ScalarType>(1), aVector, static_cast<ScalarType>(0), aOutput);
            return;
        }

        this->computeInitialApproximation(aVector);

        for(OrdinalType tIndex_I = 1; tIndex_I < mMemoryLength; tIndex_I++)
        {
            Plato::fill(static_cast<ScalarType>(0), *mNewHessTimesDeltaControl);
            for(OrdinalType tIndex_J = tIndex_I; tIndex_J < mMemoryLength; tIndex_J++)
            {
                this->bfgs(tIndex_I, (*mDeltaControl)[tIndex_J], (*mOldHessTimesDeltaControl)[tIndex_J], (*mNewHessTimesDeltaControl)[tIndex_J]);
            }
            this->bfgs(tIndex_I, aVector, *mOldHessTimesVector, *mNewHessTimesVector);
            this->advance(tIndex_I);
        }
    }

private:
    void reset()
    {
        mDeltaControl.reset();
        mDeltaGradient.reset();
        mOldHessTimesDeltaControl.reset();
        mNewHessTimesDeltaControl.reset();
    }

    void initialize()
    {
        const OrdinalType tVECTOR_INDEX = 0;
        assert(mMaxStorage > static_cast<OrdinalType>(0));
        assert(mNewHessTimesVector->getNumVectors() > static_cast<OrdinalType>(0));
        assert((*mNewHessTimesVector)[tVECTOR_INDEX].size() > static_cast<OrdinalType>(0));

        mDeltaControl = std::make_shared<Plato::MultiVectorList<ScalarType, OrdinalType>>(mMaxStorage, *mNewHessTimesVector);
        mDeltaGradient = std::make_shared<Plato::MultiVectorList<ScalarType, OrdinalType>>(mMaxStorage, *mNewHessTimesVector);
        mOldHessTimesDeltaControl = std::make_shared<Plato::MultiVectorList<ScalarType, OrdinalType>>(mMaxStorage, *mNewHessTimesVector);
        mNewHessTimesDeltaControl = std::make_shared<Plato::MultiVectorList<ScalarType, OrdinalType>>(mMaxStorage, *mNewHessTimesVector);
    }

    void advance(const OrdinalType & aOuterIndex)
    {
        Plato::update(static_cast<ScalarType>(1), *mNewHessTimesVector, static_cast<ScalarType>(0), *mOldHessTimesVector);
        for(OrdinalType tIndex = aOuterIndex; tIndex < mMemoryLength; tIndex++)
        {
            Plato::update(static_cast<ScalarType>(1), (*mNewHessTimesDeltaControl)[tIndex], static_cast<ScalarType>(0), (*mOldHessTimesDeltaControl)[tIndex]);
        }
    }

    void computeInitialApproximation(const Plato::MultiVector<ScalarType, OrdinalType> & aVector)
    {
        const ScalarType tDeltaControlDotDeltaGrad =
                Plato::dot( (*mDeltaControl)[mMemoryLength], (*mDeltaGradient)[mMemoryLength] );
        const ScalarType tDeltaGradDotDeltaGrad =
                Plato::dot( (*mDeltaGradient)[mMemoryLength], (*mDeltaGradient)[mMemoryLength] );
        const ScalarType tGamma0 = tDeltaGradDotDeltaGrad / tDeltaControlDotDeltaGrad;

        Plato::fill(static_cast<ScalarType>(0), *mOldHessTimesDeltaControl);
        for(OrdinalType tIndex = 0; tIndex < mMemoryLength; tIndex++)
        {
            Plato::update(tGamma0, (*mDeltaControl)[tIndex], static_cast<ScalarType>(0), (*mOldHessTimesDeltaControl)[tIndex]);
        }
        Plato::update(tGamma0, aVector, static_cast<ScalarType>(0), *mOldHessTimesVector);
    }

    void bfgs(const OrdinalType & aOuterIndex,
              const Plato::MultiVector<ScalarType, OrdinalType> & aVector,
              const Plato::MultiVector<ScalarType, OrdinalType> & aHessTimesVector,
              Plato::MultiVector<ScalarType, OrdinalType> & aOutput)
    {
        const Plato::MultiVector<ScalarType, OrdinalType> & tMyDeltaGrad = (*mDeltaGradient)[aOuterIndex];
        const Plato::MultiVector<ScalarType, OrdinalType> & tMyDeltaControl = (*mDeltaControl)[aOuterIndex];
        const Plato::MultiVector<ScalarType, OrdinalType> & tMyOldHessTimesDeltaControl = (*mOldHessTimesDeltaControl)[aOuterIndex];

        Plato::update(static_cast<ScalarType>(1), aHessTimesVector, static_cast<ScalarType>(0), aOutput);

        const ScalarType tHessTimesDeltaControlTimesVector = Plato::dot(tMyOldHessTimesDeltaControl, aVector);
        const ScalarType tDeltaControlTimesHessTimesDeltaControl = Plato::dot(tMyDeltaControl, tMyOldHessTimesDeltaControl);
        const ScalarType tAlpha = tHessTimesDeltaControlTimesVector / tDeltaControlTimesHessTimesDeltaControl;
        Plato::update(-tAlpha, tMyOldHessTimesDeltaControl, static_cast<ScalarType>(1), aOutput);

        const ScalarType tDeltaGradDotVector = Plato::dot(tMyDeltaGrad, aVector);
        const ScalarType tDeltaGradDotDeltaControl = Plato::dot(tMyDeltaGrad, tMyDeltaControl);
        const ScalarType tBeta = tDeltaGradDotVector / tDeltaGradDotDeltaControl;
        Plato::update(tBeta, tMyDeltaGrad, static_cast<ScalarType>(1), aOutput);
    }

    void updateMemory()
    {
        const ScalarType tCurvatureCondition = Plato::dot(*mNewDeltaControl, *mNewDeltaGradient);
        bool tIsCurvatureConditionAboveLowerBound = tCurvatureCondition > mLowerBoundCurvature ? true : false;
        bool tIsCurvatureConditionBelowUpperBound = tCurvatureCondition < mUpperBoundCurvature ? true : false;
        if(tIsCurvatureConditionAboveLowerBound && tIsCurvatureConditionBelowUpperBound)
        {
            if(mMemoryLength == mMaxStorage)
            {
                const OrdinalType tLength = mMaxStorage - static_cast<OrdinalType>(1);
                for(OrdinalType tBaseIndex = 0; tBaseIndex < tLength; tBaseIndex++)
                {
                    const OrdinalType tNextIndex = tBaseIndex + static_cast<OrdinalType>(1);
                    Plato::update(static_cast<ScalarType>(1), (*mDeltaControl)[tNextIndex], static_cast<ScalarType>(0), (mDeltaControl)[tBaseIndex]);
                    Plato::update(static_cast<ScalarType>(1), (*mDeltaGradient)[tNextIndex], static_cast<ScalarType>(0), (mDeltaGradient)[tBaseIndex]);
                }
            }

            Plato::MultiVector<ScalarType, OrdinalType> & tMyDeltaControl = (*mDeltaControl)[mMemoryLength];
            Plato::update(static_cast<ScalarType>(1), *mNewDeltaControl, static_cast<ScalarType>(0), tMyDeltaControl);
            Plato::MultiVector<ScalarType, OrdinalType> & tMyDeltaGradient = (*mDeltaGradient)[mMemoryLength];
            Plato::update(static_cast<ScalarType>(1), *mNewDeltaGradient, static_cast<ScalarType>(0), tMyDeltaGradient);

            mMemoryLength++;
            mMemoryLength = std::min(mMemoryLength, mMaxStorage);
        }
    }


    void computeNewSecantInformation(const Plato::StateData<ScalarType, OrdinalType> & aStateData)
    {
        Plato::update(static_cast<ScalarType>(1), aStateData.getCurrentControl(), static_cast<ScalarType>(0), *mNewDeltaControl);
        Plato::update(static_cast<ScalarType>(-1), aStateData.getPreviousControl(), static_cast<ScalarType>(1), *mNewDeltaControl);
        Plato::update(static_cast<ScalarType>(1), aStateData.getCurrentCriterionGradient(), static_cast<ScalarType>(0), *mNewDeltaGradient);
        Plato::update(static_cast<ScalarType>(-1), aStateData.getPreviousCriterionGradient(), static_cast<ScalarType>(1), *mNewDeltaGradient);
    }

private:
    OrdinalType mMaxStorage;
    OrdinalType mMemoryLength;

    ScalarType mLowerBoundCurvature;
    ScalarType mUpperBoundCurvature;

    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mNewDeltaControl;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mNewDeltaGradient;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mOldHessTimesVector;
    std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> mNewHessTimesVector;

    std::shared_ptr<Plato::MultiVectorList<ScalarType, OrdinalType>> mDeltaControl;
    std::shared_ptr<Plato::MultiVectorList<ScalarType, OrdinalType>> mDeltaGradient;
    std::shared_ptr<Plato::MultiVectorList<ScalarType, OrdinalType>> mOldHessTimesDeltaControl;
    std::shared_ptr<Plato::MultiVectorList<ScalarType, OrdinalType>> mNewHessTimesDeltaControl;
};
// class HessianLBFGS

}
// namespace Plato
