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
 * Plato_InitializeValues.hpp
 *
 *  Created on: Jun 30, 2019
 */

#pragma once

#include "Plato_LocalOperation.hpp"

class PlatoApp;

namespace Plato
{

class InputData;

/******************************************************************************//**
 * @brief Set all design variables to the same initial value
 **********************************************************************************/
class InitializeValues : public Plato::LocalOp
{

public:
    InitializeValues() = default;
    /******************************************************************************//**
     * @brief Constructor
     * @param [in] aPlatoApp PLATO application
     * @param [in] aNode input XML data
    **********************************************************************************/
    InitializeValues(PlatoApp* aPlatoApp, Plato::InputData & aNode);

    /******************************************************************************//**
     * @brief perform local operation - set all design variables to the same initial value
    **********************************************************************************/
    void operator()();

    /******************************************************************************//**
     * @brief Return local operation's argument list
     * @param [out] aLocalArgs argument list
    **********************************************************************************/
    void getArguments(std::vector<Plato::LocalArg> & aLocalArgs);

    double getValue(int aIndex){return mValues[aIndex];}
    double getValueUpperBound(int aIndex){return mUpperBounds[aIndex];}
    double getValueLowerBound(int aIndex){return mLowerBounds[aIndex];}


    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & aArchive, const unsigned int version)
    {
      aArchive & boost::serialization::make_nvp("LocalOp",boost::serialization::base_object<LocalOp>(*this));
      aArchive & boost::serialization::make_nvp("ValueNames",mValuesName);
      aArchive & boost::serialization::make_nvp("UpperBoundsName",mUpperBoundsName);
      aArchive & boost::serialization::make_nvp("LowerBoundsName",mLowerBoundsName);
      aArchive & boost::serialization::make_nvp("StringMethod",mStringMethod);
      aArchive & boost::serialization::make_nvp("CSMFileName",mCSMFileName);
      aArchive & boost::serialization::make_nvp("Values",mValues);
      aArchive & boost::serialization::make_nvp("LowerBounds",mLowerBounds);
      aArchive & boost::serialization::make_nvp("UpperBounds",mUpperBounds);
    }
    
private:

    std::string mValuesName; /*!< initial value argument name */
    std::string mUpperBoundsName; /*!< initial value argument name */
    std::string mLowerBoundsName; /*!< initial value argument name */
    std::string mStringMethod; /*!< method for initialization */
    std::string mCSMFileName; /*!< method for initialization */
    std::vector<double> mValues; /*!< input value */
    std::vector<double> mLowerBounds; /*!< value lower bound */
    std::vector<double> mUpperBounds; /*!< value upper bound */

};
// class InitializeValues;

}
// namespace Plato

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(Plato::InitializeValues, "InitializeValues")