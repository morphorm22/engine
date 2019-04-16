/*
 * Plato_SteihaugTointSolver.hpp
 *
 *  Created on: Oct 21, 2017
 */

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

#ifndef PLATO_STEIHAUGTOINTSOLVER_HPP_
#define PLATO_STEIHAUGTOINTSOLVER_HPP_

#include <cmath>
#include <limits>
#include <cassert>

#include "Plato_Vector.hpp"
#include "Plato_MultiVector.hpp"
#include "Plato_TrustRegionStageMng.hpp"
#include "Plato_TrustRegionAlgorithmDataMng.hpp"

namespace Plato
{

struct krylov_solver
{
    enum stop_t
    {
        NaN_CURVATURE = 1,
        ZERO_CURVATURE = 2,
        SMALL_CURVATURE = 3,
        NEGATIVE_CURVATURE = 3,
        INF_CURVATURE = 4,
        TOLERANCE = 5,
        TRUST_REGION_RADIUS = 6,
        MAX_ITERATIONS = 7,
        NaN_NORM_RESIDUAL = 8,
        INF_NORM_RESIDUAL = 9,
        INEXACTNESS_MEASURE = 10,
        ORTHOGONALITY_MEASURE = 11,
    };
};

template<typename ScalarType, typename OrdinalType = size_t>
class SteihaugTointSolver
{
public:
    SteihaugTointSolver() :
            mMaxNumIterations(200),
            mNumIterationsDone(0),
            mTolerance(1e-8),
            mNormResidual(0),
            mTrustRegionRadius(0),
            mRelativeTolerance(1e-1),
            mRelativeToleranceExponential(0.5),
            mStoppingCriterion(Plato::krylov_solver::stop_t::MAX_ITERATIONS)
    {
    }
    virtual ~SteihaugTointSolver()
    {
    }

    void setMaxNumIterations(const OrdinalType & aInput)
    {
        mMaxNumIterations = aInput;
    }
    OrdinalType getMaxNumIterations() const
    {
        return (mMaxNumIterations);
    }
    void setNumIterationsDone(const OrdinalType & aInput)
    {
        mNumIterationsDone = aInput;
    }
    OrdinalType getNumIterationsDone() const
    {
        return (mNumIterationsDone);
    }
    void setSolverTolerance(const ScalarType & aInput)
    {
        mTolerance = aInput;
    }
    ScalarType getSolverTolerance() const
    {
        return (mTolerance);
    }
    void setTrustRegionRadius(const ScalarType & aInput)
    {
        mTrustRegionRadius = aInput;
    }
    ScalarType getTrustRegionRadius() const
    {
        return (mTrustRegionRadius);
    }
    void setNormResidual(const ScalarType & aInput)
    {
        mNormResidual = aInput;
    }
    ScalarType getNormResidual() const
    {
        return (mNormResidual);
    }
    void setRelativeTolerance(const ScalarType & aInput)
    {
        mRelativeTolerance = aInput;
    }
    ScalarType getRelativeTolerance() const
    {
        return (mRelativeTolerance);
    }
    void setRelativeToleranceExponential(const ScalarType & aInput)
    {
        mRelativeToleranceExponential = aInput;
    }
    ScalarType getRelativeToleranceExponential() const
    {
        return (mRelativeToleranceExponential);
    }
    void setStoppingCriterion(const Plato::krylov_solver::stop_t & aInput)
    {
        mStoppingCriterion = aInput;
    }
    Plato::krylov_solver::stop_t getStoppingCriterion() const
    {
        return (mStoppingCriterion);
    }
    ScalarType computeSteihaugTointStep(const Plato::MultiVector<ScalarType, OrdinalType> & aNewtonStep,
                                        const Plato::MultiVector<ScalarType, OrdinalType> & aConjugateDir,
                                        const Plato::MultiVector<ScalarType, OrdinalType> & aPrecTimesNewtonStep,
                                        const Plato::MultiVector<ScalarType, OrdinalType> & aPrecTimesConjugateDir)
    {
        assert(aNewtonStep.getNumVectors() == aConjugateDir.getNumVectors());
        assert(aNewtonStep.getNumVectors() == aPrecTimesNewtonStep.getNumVectors());
        assert(aNewtonStep.getNumVectors() == aPrecTimesConjugateDir.getNumVectors());

        // Dogleg trust region step
        OrdinalType tNumVectors = aNewtonStep.getNumVectors();
        ScalarType tNewtonStepDotPrecTimesNewtonStep = 0;
        ScalarType tNewtonStepDotPrecTimesConjugateDir = 0;
        ScalarType tConjugateDirDotPrecTimesConjugateDir = 0;
        for(OrdinalType tVectorIndex = 0; tVectorIndex < tNumVectors; tVectorIndex++)
        {
            tNewtonStepDotPrecTimesNewtonStep += aNewtonStep[tVectorIndex].dot(aPrecTimesNewtonStep[tVectorIndex]);
            tNewtonStepDotPrecTimesConjugateDir += aNewtonStep[tVectorIndex].dot(aPrecTimesConjugateDir[tVectorIndex]);
            tConjugateDirDotPrecTimesConjugateDir += aConjugateDir[tVectorIndex].dot(aPrecTimesConjugateDir[tVectorIndex]);
        }

        ScalarType tTrustRegionRadius = this->getTrustRegionRadius();
        ScalarType tAlpha = tNewtonStepDotPrecTimesConjugateDir * tNewtonStepDotPrecTimesConjugateDir;
        ScalarType tBeta = tConjugateDirDotPrecTimesConjugateDir
                * (tTrustRegionRadius * tTrustRegionRadius - tNewtonStepDotPrecTimesNewtonStep);
        ScalarType tAlphaPlusBeta = tAlpha + tBeta;
        ScalarType tNumerator = -tNewtonStepDotPrecTimesConjugateDir + std::sqrt(tAlphaPlusBeta);
        ScalarType tStep = tNumerator / tConjugateDirDotPrecTimesConjugateDir;

        return (tStep);
    }
    bool invalidCurvatureDetected(const ScalarType & aInput)
    {
        bool tInvalidCurvatureDetected = false;

        if(aInput < static_cast<ScalarType>(0.))
        {
            this->setStoppingCriterion(Plato::krylov_solver::stop_t::NEGATIVE_CURVATURE);
            tInvalidCurvatureDetected = true;
        }
        else if(std::abs(aInput) <= std::numeric_limits<ScalarType>::min())
        {
            this->setStoppingCriterion(Plato::krylov_solver::stop_t::ZERO_CURVATURE);
            tInvalidCurvatureDetected = true;
        }
        else if(std::isinf(aInput))
        {
            this->setStoppingCriterion(Plato::krylov_solver::stop_t::INF_CURVATURE);
            tInvalidCurvatureDetected = true;
        }
        else if(std::isnan(aInput))
        {
            this->setStoppingCriterion(Plato::krylov_solver::stop_t::NaN_CURVATURE);
            tInvalidCurvatureDetected = true;
        }
        else if(std::abs(aInput) <= std::numeric_limits<ScalarType>::epsilon())
        {
            this->setStoppingCriterion(Plato::krylov_solver::stop_t::SMALL_CURVATURE);
            tInvalidCurvatureDetected = true;
        }

        return (tInvalidCurvatureDetected);
    }
    bool toleranceSatisfied(const ScalarType & aNormDescentDirection)
    {
        this->setNormResidual(aNormDescentDirection);
        ScalarType tStoppingTolerance = this->getSolverTolerance();

        bool tToleranceCriterionSatisfied = false;
        if(aNormDescentDirection <= tStoppingTolerance)
        {
            this->setStoppingCriterion(Plato::krylov_solver::stop_t::TOLERANCE);
            tToleranceCriterionSatisfied = true;
        }
        else if(std::isinf(aNormDescentDirection))
        {
            this->setStoppingCriterion(Plato::krylov_solver::stop_t::INF_NORM_RESIDUAL);
            tToleranceCriterionSatisfied = true;
        }
        else if(std::isnan(aNormDescentDirection))
        {
            this->setStoppingCriterion(Plato::krylov_solver::stop_t::NaN_NORM_RESIDUAL);
            tToleranceCriterionSatisfied = true;
        }

        return (tToleranceCriterionSatisfied);
    }

    virtual void solve(Plato::TrustRegionStageMng<ScalarType, OrdinalType> & aStageMng,
                       Plato::TrustRegionAlgorithmDataMng<ScalarType, OrdinalType> & aDataMng) = 0;

private:
    OrdinalType mMaxNumIterations;
    OrdinalType mNumIterationsDone;

    ScalarType mTolerance;
    ScalarType mNormResidual;
    ScalarType mTrustRegionRadius;
    ScalarType mRelativeTolerance;
    ScalarType mRelativeToleranceExponential;

    Plato::krylov_solver::stop_t mStoppingCriterion;

private:
    SteihaugTointSolver(const Plato::SteihaugTointSolver<ScalarType, OrdinalType> & aRhs);
    Plato::SteihaugTointSolver<ScalarType, OrdinalType> & operator=(const Plato::SteihaugTointSolver<ScalarType, OrdinalType> & aRhs);
};

}

#endif /* PLATO_STEIHAUGTOINTSOLVER_HPP_ */
