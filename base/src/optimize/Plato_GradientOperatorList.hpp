/*
 * Plato_GradientOperatorList.hpp
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
//
// *************************************************************************
//@HEADER
*/

#ifndef PLATO_GRADIENTOPERATORLIST_HPP_
#define PLATO_GRADIENTOPERATORLIST_HPP_

#include <vector>
#include <memory>
#include <cassert>

#include "Plato_CriterionList.hpp"
#include "Plato_AnalyticalGradient.hpp"

namespace Plato
{

template<typename ScalarType, typename OrdinalType = size_t>
class GradientOperatorList
{
public:
    GradientOperatorList() :
            mList()
    {
    }
    explicit GradientOperatorList(const std::shared_ptr<Plato::CriterionList<ScalarType, OrdinalType>> & aInput) :
            mList()
    {
        const OrdinalType tNumCriterion = aInput->size();
        for(OrdinalType tIndex = 0; tIndex < tNumCriterion; tIndex++)
        {
            mList.push_back(std::make_shared<Plato::AnalyticalGradient<ScalarType, OrdinalType>>(aInput->ptr(tIndex)));
        }
    }
    ~GradientOperatorList()
    {
    }

    OrdinalType size() const
    {
        return (mList.size());
    }
    void add(const std::shared_ptr<Plato::GradientOperator<ScalarType, OrdinalType>> & aInput)
    {
        mList.push_back(aInput);
    }
    Plato::GradientOperator<ScalarType, OrdinalType> & operator [](const OrdinalType & aIndex)
    {
        assert(aIndex < mList.size());
        assert(mList[aIndex].get() != nullptr);
        return (mList[aIndex].operator*());
    }
    const Plato::GradientOperator<ScalarType, OrdinalType> & operator [](const OrdinalType & aIndex) const
    {
        assert(aIndex < mList.size());
        assert(mList[aIndex].get() != nullptr);
        return (mList[aIndex].operator*());
    }
    std::shared_ptr<Plato::GradientOperatorList<ScalarType, OrdinalType>> create() const
    {
        assert(this->size() > static_cast<OrdinalType>(0));

        std::shared_ptr<Plato::GradientOperatorList<ScalarType, OrdinalType>> tOutput =
                std::make_shared<Plato::GradientOperatorList<ScalarType, OrdinalType>>();
        const OrdinalType tNumGradientOperators = this->size();
        for(OrdinalType tIndex = 0; tIndex < tNumGradientOperators; tIndex++)
        {
            assert(mList[tIndex].get() != nullptr);

            const std::shared_ptr<Plato::GradientOperator<ScalarType, OrdinalType>> & tGradientOperator = mList[tIndex];
            tOutput->add(tGradientOperator);
        }
        return (tOutput);
    }
    const std::shared_ptr<Plato::GradientOperator<ScalarType, OrdinalType>> & ptr(const OrdinalType & aIndex) const
    {
        assert(aIndex < mList.size());
        assert(mList[aIndex].get() != nullptr);
        return(mList[aIndex]);
    }

private:
    std::vector<std::shared_ptr<Plato::GradientOperator<ScalarType, OrdinalType>>> mList;

private:
    GradientOperatorList(const Plato::GradientOperatorList<ScalarType, OrdinalType>&);
    Plato::GradientOperatorList<ScalarType, OrdinalType> & operator=(const Plato::GradientOperatorList<ScalarType, OrdinalType>&);
};

} //namespace Plato

#endif /* PLATO_GRADIENTOPERATORLIST_HPP_ */