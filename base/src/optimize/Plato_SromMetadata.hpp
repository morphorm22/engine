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
//
// *************************************************************************
//@HEADER
*/

/*
 * Plato_SromMetadata.hpp
 *
 *  Created on: June 18, 2019
 */

#pragma once

#include <string>
#include <vector>

namespace Plato
{

namespace srom
{

/******************************************************************************//**
 * \struct Statistics
 * \brief Statistics metadata for Stochastic Reduced Order Model (SROM) problem.
**********************************************************************************/
struct Statistics
{
    bool mReadStatsFromFile = false;     /*!< true = read statistics from file, false = solve SROM problem */
    std::string mNumSamples;        /*!< number of samples */
    std::string mDistribution;      /*!< probability distribution */
    std::string mMean;              /*!< probability distribution mean */
    std::string mUpperBound;        /*!< probability distribution upper bound */
    std::string mLowerBound;        /*!< probability distribution lower bound */
    std::string mStandardDeviation; /*!< probability distribution standard deviation */
};
// struct Statistics

/******************************************************************************//**
 * \struct RandomVariable
 * \brief Random variable metadata for Stochastic Reduced Order Model (SROM) problem.
**********************************************************************************/
struct RandomVariable
{
    std::string mTag;                    /*!< label, e.g. angle variation */
    std::string mAttribute;              /*!< random variable attribute, e.g. rotation axis */
    Plato::srom::Statistics mStatistics; /*!< statistics for this random variable */

    /******************************************************************************//**
     * \fn Read statistics from text file, true = read statistics from file or \n
     * false = solve SROM problem to compute statistics.
     * \return flag
    **********************************************************************************/
    bool readStats() const
    {
        return (mStatistics.mReadStatsFromFile);
    }
};
// struct RandomVariable

/******************************************************************************//**
 * \struct DeterministicVariable
 * \brief Metadata used to describe a deterministic variable.
**********************************************************************************/
struct DeterministicVariable
{
    std::string mTag;        /*!< main variable attribute , e.g. Poisson's ratio */
    std::string mValue;      /*!< attribute's deterministic value */
    std::string mAttribute;  /*!< attribute's category, e.g. homogeneous or heterogeneous */
};
// struct DeterministicVariable

/******************************************************************************//**
 * \struct SampleProbabilityPairs
 * \brief Sample-Probability pairs defined by the Stochastic Reduced Order Model (SROM) problem.
**********************************************************************************/
struct SampleProbabilityPairs
{
    int mNumSamples; /*!< total number of samples */
    std::vector<double> mSamples; /*!< sample set */
    std::vector<double> mProbabilities; /*!< probability set  */
};
// struct SampleProbabilityPairs

/******************************************************************************//**
 * \struct SromVariable
 * \brief Stochastic Reduced Order Model (SROM) variable metadata for SROM problem.
**********************************************************************************/
struct SromVariable
{
    std::string mTag;        /*!< label, e.g. angle variation, elastic modulus */
    std::string mAttribute;  /*!< SROM variable attribute, e.g. rotation axis, homogeneous */
    Plato::srom::SampleProbabilityPairs mSampleProbPairs; /*!< sample-probability pair for this random variable */
};
// struct SromVariable

}
// namespace srom

}
// namespace Plato
