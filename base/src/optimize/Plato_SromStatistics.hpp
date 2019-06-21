/*
 * Plato_SromStatistics.hpp
 *
 *  Created on: Jan 31, 2018
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

#ifndef PLATO_SROMSTATISTICS_HPP_
#define PLATO_SROMSTATISTICS_HPP_

#include <cmath>
#include <cassert>

#include "Plato_Vector.hpp"

#define _MATH_DEFINES_DEFINED

namespace Plato
{

/******************************************************************************//**
 * @brief Evaluate cumulative distribution function (CDF) at this sample point
 * @param [in] aMySample input sample point
 * @param [in] aSigma curvature correction
 * @param [in] aSamples set of samples
 * @param [in] aSamplesProbability set of sample probabilities
 * @return CDF value given a sample computed with a stochastic reduced order model
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
inline ScalarType compute_srom_cdf(const ScalarType & aMySample,
                                   const ScalarType & aSigma,
                                   const Plato::Vector<ScalarType, OrdinalType> & aSamples,
                                   const Plato::Vector<ScalarType, OrdinalType> & aSamplesProbability)
{
    ScalarType tSum = 0;
    OrdinalType tNumSamples = aSamples.size();
    for(OrdinalType tIndexJ = 0; tIndexJ < tNumSamples; tIndexJ++)
    {
        ScalarType tValue = (aMySample - aSamples[tIndexJ]) / (aSigma * std::sqrt(static_cast<ScalarType>(2)));
        tSum = tSum + aSamplesProbability[tIndexJ] *
                (static_cast<ScalarType>(0.5) * (static_cast<ScalarType>(1) + std::erf(tValue)));
    }
    return (tSum);
}
// function compute_srom_cdf

/******************************************************************************//**
 * @brief Evaluate n-th order raw moment
 * @param [in] aOrder raw moment order
 * @param [in] aSamples set of samples
 * @param [in] aSamplesProbability set of sample probabilities
 * @return evaluation of the n-th order raw moment
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
inline ScalarType compute_raw_moment(const ScalarType & aOrder,
                                     const Plato::Vector<ScalarType, OrdinalType> & aSamples,
                                     const Plato::Vector<ScalarType, OrdinalType> & aSamplesProbability)
{
    assert(aOrder >= static_cast<OrdinalType>(0));
    assert(aSamples.size() == aSamplesProbability.size());

    ScalarType tOutput = 0;
    OrdinalType tNumSamples = aSamples.size();
    for(OrdinalType tSampleIndex = 0; tSampleIndex < tNumSamples; tSampleIndex++)
    {
        tOutput = tOutput + (aSamplesProbability[tSampleIndex] * std::pow(aSamples[tSampleIndex], aOrder));
    }
    return (tOutput);
}
// function compute_raw_moment

/******************************************************************************//**
 * @brief Evaluate n-th order central moment
 * @param [in] aOrder raw moment order
 * @param [in] aSamples set of samples
 * @param [in] aSamplesProbability set of sample probabilities
 * @return evaluation of the n-th order raw moment
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
inline ScalarType compute_central_moment(const ScalarType & aOrder,
                                         const Plato::Vector<ScalarType, OrdinalType> & aSamples,
                                         const Plato::Vector<ScalarType, OrdinalType> & aSampleProbabilities)
{
    assert(aOrder >= static_cast<OrdinalType>(0));
    assert(aSamples.size() == aSampleProbabilities.size());

    const ScalarType tOrderRawMoment= 1.0;
    const ScalarType tSampleMean =
            Plato::compute_raw_moment<ScalarType, OrdinalType>(tOrderRawMoment, aSamples, aSampleProbabilities);

    ScalarType tOutput = 0;
    OrdinalType tNumSamples = aSamples.size();
    for(OrdinalType tSampleIndex = 0; tSampleIndex < tNumSamples; tSampleIndex++)
    {
        ScalarType tSampleMinusMean = aSamples[tSampleIndex] - tSampleMean;
        tOutput = tOutput + (aSampleProbabilities[tSampleIndex] * std::pow(tSampleMinusMean, aOrder));
    }
    return (tOutput);
}
// function compute_central_moment

}
// namespace Plato

#endif /* PLATO_SROMSTATISTICS_HPP_ */
