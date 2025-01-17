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
 * Plato_Aggregator.hpp
 *
 *  Created on: Jun 27, 2019
 */

#pragma once

#include "Plato_Console.hpp"
#include "Plato_LocalOperation.hpp"
#include "Plato_SharedData.hpp"

#include "Plato_SerializationHeaders.hpp"

namespace Plato
{

class InputData;

struct AggStruct
{
    Plato::data::layout_t mLayout; /*!< data layout, e.g. scalar value, scalar field, etc */
    std::string mOutputName; /*!< output argument name */
    std::vector<std::string> mInputNames; /*!< input argument name */

    template<class Archive>
    void serialize(Archive & aArchive, const unsigned int version)
    {
        aArchive & boost::serialization::make_nvp("Layout",mLayout);
        aArchive & boost::serialization::make_nvp("OutputName",mOutputName);
        aArchive & boost::serialization::make_nvp("InputNames",mInputNames);
    }   
};

/******************************************************************************//**
 * @brief Aggregate a quantity of interest
**********************************************************************************/
class Aggregator : public Plato::LocalOp
{

public:
    Aggregator() = default;
    Aggregator(const std::vector<double>& aWeights,
               const std::vector<std::string>& aWeightBases,
               const std::vector<std::string>& aWeightNormals,
               const std::vector<AggStruct>& aAggStructs,
               const std::string& aWeightMethod = "FIXED", 
               double aLimitWeight = 1, 
               bool aReportStatus = false);

    /******************************************************************************//**
     * @brief Constructor
     * @param [in] aPlatoApp PLATO application
     * @param [in] aNode input XML data
     **********************************************************************************/
    Aggregator(PlatoApp* aPlatoApp, Plato::InputData& aNode);

    /******************************************************************************//**
     * @brief perform local operation - aggregate values
    **********************************************************************************/
    void operator()();

    /******************************************************************************//**
     * @brief Return local operation's argument list
     * @param [out] aLocalArgs argument list
    **********************************************************************************/
    void getArguments(std::vector<Plato::LocalArg>& aLocalArgs);
    
    template<class Archive>
    void serialize(Archive & aArchive, const unsigned int version)
    {
        aArchive & boost::serialization::make_nvp("LocalOp",boost::serialization::base_object<LocalOp>(*this));
        aArchive & boost::serialization::make_nvp("LimitWeight",mLimitWeight);
        aArchive & boost::serialization::make_nvp("WeightBases",mWeightBases);
        aArchive & boost::serialization::make_nvp("WeightNormals",mWeightNormals);
        aArchive & boost::serialization::make_nvp("WeightMethod",mWeightMethod);
        aArchive & boost::serialization::make_nvp("Weights",mWeights);
        aArchive & boost::serialization::make_nvp("AggStruct",mAggStructs);
    }

private:
  
    bool mReportStatus = false; /*!< whether to write status to Plato::Console */
    double mLimitWeight = 1e9; /*!< weight's upper bound */
    std::vector<std::string> mWeightBases; /*!< weight bases */
    std::vector<std::string> mWeightNormals; /*!< weight normals */
    std::string mWeightMethod = "FIXED"; /*!< method - basically, how are weights applied */
    std::vector<double> mWeights; /*!< weights for each component */
    std::vector<AggStruct> mAggStructs; /*!< core data for each aggregated component */

    /******************************************************************************//**
     * @brief Return aggregator weights
    **********************************************************************************/
    decltype(mWeights) getWeights();

    /******************************************************************************//**
     * @brief Aggregate the member scalars and scalar fields
     * @param [in] aWeights current weights
    **********************************************************************************/
    void aggregate(const AggStruct& aAggStruct, decltype(mWeights) aWeights);

    /******************************************************************************//**
     * @brief Aggregate a scalar field
     * @param [in] aAggStruct struct containing inputs and output
     * @param [in] aWeights current weights
    **********************************************************************************/
    void aggregateScalarField(const AggStruct& aAggStruct, const decltype(mWeights)& aWeights);

    /******************************************************************************//**
     * @brief Aggregate a scalar
     * @param [in] aAggStruct struct containing inputs and output
     * @param [in] aWeights current weights
    **********************************************************************************/
    void aggregateScalar(const AggStruct& aAggStruct, const decltype(mWeights)& aWeights);

    /******************************************************************************//**
     * @brief Wrapper around Plato::Console::Alert()
     * @param [in] aStream stream containing message for console
    **********************************************************************************/
    void reportStatus(const std::stringstream& aStream) const;

    /******************************************************************************//**
     * @brief Wrapper around Plato::Console::Alert()
     * @param [in] aString string containing message for console
    **********************************************************************************/
    void reportStatus(const std::string& aStream) const;
};
// class Aggregator

}
// namespace Plato

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(Plato::Aggregator, "Aggregator")
