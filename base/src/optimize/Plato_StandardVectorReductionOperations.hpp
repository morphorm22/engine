/*
 * Plato_StandardVectorReductionOperations.hpp
 *
 *  Created on: Oct 17, 2017
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

#ifndef PLATO_STANDARDVECTORREDUCTIONOPERATIONS_HPP_
#define PLATO_STANDARDVECTORREDUCTIONOPERATIONS_HPP_

#include <vector>
#include <cassert>
#include <algorithm>

#include "Plato_Vector.hpp"
#include "Plato_ReductionOperations.hpp"

namespace Plato
{

/******************************************************************************//**
 * @brief Operations used to reduce the elements of an array into a single result.
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
class StandardVectorReductionOperations : public Plato::ReductionOperations<ScalarType, OrdinalType>
{
public:
    /******************************************************************************//**
     * @brief Constructor
    **********************************************************************************/
    StandardVectorReductionOperations()
    {
    }

    /******************************************************************************//**
     * @brief Destructor
    **********************************************************************************/
    virtual ~StandardVectorReductionOperations()
    {
    }

    /******************************************************************************//**
     * @brief Returns the maximum element in range.
     * @param [in] aInput array of elements
     * @return maximum value
    **********************************************************************************/
    ScalarType max(const Plato::Vector<ScalarType, OrdinalType> & aInput) const
    {
        assert(aInput.size() > 0);

        const ScalarType tValue = 0;
        const OrdinalType tSize = aInput.size();
        std::vector<ScalarType> tCopy(tSize, tValue);
        for(OrdinalType tIndex = 0; tIndex < tSize; tIndex++)
        {
            tCopy[tIndex] = aInput[tIndex];
        }

        ScalarType aMaxValue = *std::max_element(tCopy.begin(), tCopy.end());
        return (aMaxValue);
    }

    /******************************************************************************//**
     * @brief Returns the minimum element in range.
     * @param [in] aInput array of elements
     * @return minimum value
    **********************************************************************************/
    ScalarType min(const Plato::Vector<ScalarType, OrdinalType> & aInput) const
    {
        assert(aInput.size() > 0);

        const ScalarType tValue = 0;
        const OrdinalType tSize = aInput.size();
        std::vector<ScalarType> tCopy(tSize, tValue);
        for(OrdinalType tIndex = 0; tIndex < tSize; tIndex++)
        {
            tCopy[tIndex] = aInput[tIndex];
        }

        ScalarType aMinValue = *std::min_element(tCopy.begin(), tCopy.end());
        return (aMinValue);
    }

    /******************************************************************************//**
     * @brief Returns the sum of all the elements in the container.
     * @param [in] aInput array of elements
     * @return sum
    **********************************************************************************/
    ScalarType sum(const Plato::Vector<ScalarType, OrdinalType> & aInput) const
    {
        assert(aInput.size() > 0);

        const ScalarType tValue = 0;
        const OrdinalType tSize = aInput.size();
        std::vector<ScalarType> tCopy(tSize, tValue);
        for(OrdinalType tIndex = 0; tIndex < tSize; tIndex++)
        {
            tCopy[tIndex] = aInput[tIndex];
        }

        ScalarType tBaseValue = 0;
        ScalarType tSum = std::accumulate(tCopy.begin(), tCopy.end(), tBaseValue);
        return (tSum);
    }

    /******************************************************************************//**
     * @brief Computes minimum value and also the index attached to the minimum value.
     * @param [in] aInput array of elements
     * @param [out] aOutput struct with minimum value and the index attached to the minimum
    **********************************************************************************/
    void minloc(const Plato::Vector<ScalarType, OrdinalType> & aInput,
                Plato::ReductionOutputs<ScalarType, OrdinalType> & aOutput) const
    {
        aOutput.mOutputIndex = 0;
        aOutput.mOutputValue = aInput[0];
        const OrdinalType tMyNumElements = aInput.size();
        for(OrdinalType tIndex = 0; tIndex < tMyNumElements; tIndex++)
        {
            if(aInput[tIndex] < aOutput.mOutputValue)
            {
                aOutput.mOutputValue = aInput[tIndex];
                aOutput.mOutputIndex = tIndex;
            }
        }
        aOutput.mOutputRank = 0;
    }

    /******************************************************************************//**
     * @brief Returns a copy of a ReductionOperations instance
     * @return copy of this instance
    **********************************************************************************/
    std::shared_ptr<Plato::ReductionOperations<ScalarType, OrdinalType>> create() const
    {
        std::shared_ptr<Plato::ReductionOperations<ScalarType, OrdinalType>> tCopy =
                std::make_shared<StandardVectorReductionOperations<ScalarType, OrdinalType>>();
        return (tCopy);
    }

private:
    StandardVectorReductionOperations(const Plato::StandardVectorReductionOperations<ScalarType, OrdinalType> &);
    Plato::StandardVectorReductionOperations<ScalarType, OrdinalType> & operator=(const Plato::StandardVectorReductionOperations<ScalarType, OrdinalType> &);
};
// class StandardVectorReductionOperations

} //namespace Plato

#endif /* PLATO_STANDARDVECTORREDUCTIONOPERATIONS_HPP_ */
