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
 * Plato_SromLoadUtils.hpp
 *
 *  Created on: June 18, 2019
 */

#pragma once

#include <set>
#include <map>
#include <cmath>
#include <locale>
#include <cctype>
#include <cstdlib>
#include <iostream>

#include "Plato_RandomLoadMetadata.hpp"
#include "XMLGeneratorDataStruct.hpp"
#include "Plato_UniqueCounter.hpp"
#include "Plato_SromMetadata.hpp"
#include "Plato_SromSolve.hpp"
#include "Plato_Macros.hpp"

using namespace XMLGen;

namespace Plato
{

namespace srom
{

struct VariableType
{

    enum type_t
    {
        LOAD, MATERIAL, UNDEFINED
    };

};


struct RotationAxis
{

    enum type_t
    {
        x, y, z
    };
};

/******************************************************************************//**
 * \brief Return rotation axis
 * \param [in] aStringAxis rotation axis
 * \param [out] aEnumAxis rotation axis
**********************************************************************************/
inline void rotation_axis_string_to_enum(const std::string & aStringAxis, Plato::srom::RotationAxis::type_t & aEnumAxis)
{
    if(aStringAxis == "x")
    {
        aEnumAxis = Plato::srom::RotationAxis::x;
    }
    else if(aStringAxis == "y")
    {
        aEnumAxis = Plato::srom::RotationAxis::y;
    }
    else if(aStringAxis == "z")
    {
        aEnumAxis = Plato::srom::RotationAxis::z;
    }
    else
    {
        THROWERR("ROTATION AXIS IS NOT DEFINED. OPTIONS ARE: X, Y AND Z")
    }
}

/******************************************************************************//**
 * \brief transform string with variable type to an enum (Plato::srom::VariableType)
 * \param [in] aStringVarType string variable type
 * \param [out] aEnumVarType enum variable type
 * \return flag (true = no error, false = error, string variable type is not defined/supported)
**********************************************************************************/
inline bool variable_type_string_to_enum(const std::string& aStringVarType, Plato::srom::VariableType::type_t& aEnumVarType)
{
    if(aStringVarType == "material")
    {
        aEnumVarType = Plato::srom::VariableType::MATERIAL;
    }
    else if(aStringVarType == "load")
    {
        aEnumVarType = Plato::srom::VariableType::LOAD;
    }
    else
    {
        return (false);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Check if array's values are finite
 * \param [in] aInput array of values
 * \return flag (true = no error, false = error)
**********************************************************************************/
inline bool check_vector3d_values(const std::vector<double> & aInput)
{
    if(std::isfinite(aInput[0]) == false)
    {
        PRINTERR("X-COMPONENT IS NOT A FINITE NUMBER.\n");
        return (false);
    }
    else if(std::isfinite(aInput[1]) == false)
    {
        PRINTERR("Y-COMPONENT IS NOT A FINITE NUMBER.\n");
        return (false);
    }
    else if(std::isfinite(aInput[2]) == false)
    {
        PRINTERR("Z-COMPONENT IS NOT A FINITE NUMBER.\n");
        return (false);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Initialize counter used to set load identifiers (IDs)
 * \param [in] aLoadCases array of load cases
 * \param [in] aUniqueCounter IDs counter
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool initialize_load_id_counter(const std::vector<XMLGen::LoadCase> &aLoadCases,
                                       Plato::UniqueCounter &aUniqueCounter)
{
    if(aLoadCases.empty() == true)
    {
        PRINTERR("INPUT SET OF LOAD CASES IS EMPTY.\n");
        return (false);
    }

    aUniqueCounter.mark(0); // Mark 0 as true since we don't want to have a 0 ID
    for(size_t tIndex = 0; tIndex < aLoadCases.size(); tIndex++)
    {
        aUniqueCounter.mark(std::atoi(aLoadCases[tIndex].id.c_str()));
    }

    return (true);
}

/******************************************************************************//**
 * \brief Expand set of load cases into an array of single load cases
 * \param [in] aOldLoadCase old load case metadata
 * \param [out] aNewLoadCaseList expanded set of load cases - load case is reformatted
 *               to facilitate the creation of the stochastic reduced order models
 * \param [out] aUniqueLoadIDCounter load identifiers counter
 * \param [out] aOriginalToNewLoadCaseMap map between original load case IDs and new load case IDs
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool expand_single_load_case(const XMLGen::LoadCase &aOldLoadCase,
                                    std::vector<XMLGen::LoadCase> &aNewLoadCaseList,
                                    Plato::UniqueCounter &aUniqueLoadIDCounter,
                                    std::map<int, std::vector<int> > &aOriginalToNewLoadCaseMap)
{
    if(aOldLoadCase.loads.empty() == true)
    {
        std::ostringstream tMsg;
        tMsg << "LOAD CASE #" << aOldLoadCase.id << " HAS AN EMPTY LOADS SET.\n";
        PRINTERR(tMsg.str().c_str());
        return (false);
    }

    int tOriginalLoadCaseID = std::atoi(aOldLoadCase.id.c_str());
    for(size_t tLoadCaseIndex = 0; tLoadCaseIndex < aOldLoadCase.loads.size(); tLoadCaseIndex++)
    {
        std::string tIDString = aOldLoadCase.id;
        int tCurLoadCaseID = std::atoi(tIDString.c_str());
        // If this is a multi-load load case we need to generate a new load case with a new id.
        if(tLoadCaseIndex > 0)
        {
            tCurLoadCaseID = aUniqueLoadIDCounter.assignNextUnique();
            tIDString = std::to_string(tCurLoadCaseID);
        }
        aOriginalToNewLoadCaseMap[tOriginalLoadCaseID].push_back(aNewLoadCaseList.size());
        XMLGen::LoadCase tNewLoadCase = aOldLoadCase;
        tNewLoadCase.id = tIDString;
        tNewLoadCase.loads[0] = aOldLoadCase.loads[tLoadCaseIndex];
        tNewLoadCase.loads.resize(1);
        aNewLoadCaseList.push_back(tNewLoadCase);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Expand old set of load cases into an array of new load cases with one load
 * \param [in] aOldLoadCase old set of load cases
 * \param [in] aNewLoadCaseList new set of load cases re-formatted to create the
 *             stochastic reduced order models (srom)
 * \param [in] aOriginalToNewLoadCaseMap map between original load case IDs and new load case IDs
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool expand_load_cases(const std::vector<XMLGen::LoadCase> &aInputLoadCases,
                              std::vector<XMLGen::LoadCase> &aNewLoadCaseList,
                              std::map<int, std::vector<int> > &aOriginalToNewLoadCaseMap)
{
    Plato::UniqueCounter tUniqueLoadIDCounter;
    if(Plato::srom::initialize_load_id_counter(aInputLoadCases, tUniqueLoadIDCounter) == false)
    {
        PRINTERR("FAILED TO INITIALIZE ORIGINAL SET OF LOAD IDENTIFIERS.\n");
        return (false);
    }

    for(size_t tLoadCaseIndex = 0; tLoadCaseIndex < aInputLoadCases.size(); tLoadCaseIndex++)
    {
        const XMLGen::LoadCase& tOldLoadCase = aInputLoadCases[tLoadCaseIndex];
        Plato::srom::expand_single_load_case(tOldLoadCase, aNewLoadCaseList, tUniqueLoadIDCounter, aOriginalToNewLoadCaseMap);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Create a deterministic load from the input deterministic load case
 * \param [in] aLoadCase deterministic load case metadata (input load case has only
 *            one load, multiple loads are not expected)
 * \param [out] aLoad deterministic load metadata
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool create_deterministic_load_variable(const XMLGen::LoadCase &aLoadCase, Plato::srom::Load& aLoad)
{
    if(aLoadCase.loads.empty() == true)
    {
        std::ostringstream tMsg;
        tMsg << "LOAD CASE #" << aLoadCase.id << " HAS AN EMPTY LOADS SET.\n";
        PRINTERR(tMsg.str().c_str());
        return (false);
    }

    for(size_t i=0; i<aLoadCase.loads[0].values.size(); ++i)
        aLoad.mValues.push_back(aLoadCase.loads[0].values[i]);

    aLoad.mLoadID = aLoadCase.id;
    aLoad.mAppType = aLoadCase.loads[0].app_type;
    aLoad.mLoadType = aLoadCase.loads[0].type;
    aLoad.mAppID = std::atoi(aLoadCase.loads[0].app_id.c_str());

    return (true);
}

/******************************************************************************//**
 * \brief Get or create a random load given a random load case
 * \param [in] aLoadCase random load case metadata (load case only has one load,
 *              multiple loads are not expected)
 * \param [out] aRandomLoad set of random loads
 * \return random load identifier
**********************************************************************************/
inline int get_or_create_random_load_variable(const XMLGen::LoadCase &aLoadCase,
                                              std::vector<Plato::srom::Load> &aRandomLoads)
{
    for(size_t tRandomLoadIndex = 0; tRandomLoadIndex < aRandomLoads.size(); ++tRandomLoadIndex)
    {
        if(aRandomLoads[tRandomLoadIndex].mLoadID == aLoadCase.id)
            return (tRandomLoadIndex);
    }

    Plato::srom::Load tNewLoad;
    tNewLoad.mLoadType = aLoadCase.loads[0].type;
    tNewLoad.mAppType = aLoadCase.loads[0].app_type;
    tNewLoad.mAppID = std::atoi(aLoadCase.loads[0].app_id.c_str());
    tNewLoad.mAppName = aLoadCase.loads[0].app_name;
    tNewLoad.mLoadID = aLoadCase.id;
    for(size_t i=0; i<aLoadCase.loads[0].values.size(); ++i)
        tNewLoad.mValues.push_back(aLoadCase.loads[0].values[i]);
    aRandomLoads.push_back(tNewLoad);

    return (aRandomLoads.size() - 1);
}

/******************************************************************************//**
 * \brief Append a random variable, and corresponding statistics, to the random load.
 * \param [in] aRandomLoad random load metadata
 * \param [out] aRandomVariable random variable metadata
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline void add_random_variable_to_random_load(Plato::srom::Load &aRandomLoad,
                                               const XMLGen::Uncertainty &aRandomVariable)
{
    Plato::srom::RandomVariable tNewVariable;
    tNewVariable.tag(aRandomVariable.type);
    tNewVariable.attribute(aRandomVariable.axis);
    tNewVariable.mean(aRandomVariable.mean);
    tNewVariable.lower(aRandomVariable.lower);
    tNewVariable.upper(aRandomVariable.upper);
    tNewVariable.samples(aRandomVariable.num_samples);
    tNewVariable.distribution(aRandomVariable.distribution);
    tNewVariable.deviation(aRandomVariable.standard_deviation);
    aRandomLoad.mRandomVars.push_back(tNewVariable);
}

/******************************************************************************//**
 * \brief Create a random load and append it to the set of random load cases
 * \param [in] aRandomVariable random variable metadata
 * \param [in] aNewLoadCases set of load cases (deterministic plus random load cases)
 * \param [in] aOriginalToNewLoadCaseMap map from original load case identifier to new/expanded load case identifier
 * \param [out] aRandomLoadIDs set of random load case identifiers
 * \param [out] aRandomLoads set of random load cases
**********************************************************************************/
inline void create_random_loads_from_uncertainty(const XMLGen::Uncertainty& aRandomVariable,
                                                 const std::vector<XMLGen::LoadCase> &aNewLoadCases,
                                                 std::map<int, std::vector<int> > &aOriginalToNewLoadCaseMap,
                                                 std::set<int> &aRandomLoadIDs,
                                                 std::vector<Plato::srom::Load> &aRandomLoads)
{
    const int tRandomVarLoadID = std::atoi(aRandomVariable.id.c_str());
    for(size_t tIndexJ = 0; tIndexJ < aOriginalToNewLoadCaseMap[tRandomVarLoadID].size(); tIndexJ++)
    {
        const int tIndexIntoNewLoadCaseList = aOriginalToNewLoadCaseMap[tRandomVarLoadID][tIndexJ];
        std::string tCurLoadCaseIDString = aNewLoadCases[tIndexIntoNewLoadCaseList].id;
        const int tCurLoadCaseID = std::atoi(tCurLoadCaseIDString.c_str());
        aRandomLoadIDs.insert(tCurLoadCaseID);
        const int tIndexOfRandomLoad = get_or_create_random_load_variable(aNewLoadCases[tIndexIntoNewLoadCaseList], aRandomLoads);
        add_random_variable_to_random_load(aRandomLoads[tIndexOfRandomLoad], aRandomVariable);
    }
}

/******************************************************************************//**
 * \brief Create set of random loads
 * \param [in] aRandomVariable random variable metadata
 * \param [in] aNewLoadCases set of load cases (deterministic plus random load cases)
 * \param [in] aOriginalToNewLoadCaseMap map from original load case identifier to new/expanded load case identifier
 * \param [out] aRandomLoadIDs set of random load case identifiers
 * \param [out] aRandomLoads set of random load cases
**********************************************************************************/
inline void create_random_load_variables(const std::vector<XMLGen::Uncertainty> &aRandomVariables,
                                         const std::vector<XMLGen::LoadCase> &aNewLoadCases,
                                         std::map<int, std::vector<int> > &aOriginalToNewLoadCaseMap,
                                         std::set<int> &aRandomLoadIDs,
                                         std::vector<Plato::srom::Load> &aLoad)
{
    const size_t tNumRandomVars = aRandomVariables.size();
    for(size_t tRandomVarIndex = 0; tRandomVarIndex < tNumRandomVars; ++tRandomVarIndex)
    {
        const XMLGen::Uncertainty& tRandomVariable = aRandomVariables[tRandomVarIndex];
        Plato::srom::VariableType::type_t tVarType = Plato::srom::VariableType::UNDEFINED;
        Plato::srom::variable_type_string_to_enum(tRandomVariable.variable_type, tVarType);
        if(tVarType == Plato::srom::VariableType::LOAD)
        {
            Plato::srom::create_random_loads_from_uncertainty(tRandomVariable, aNewLoadCases, aOriginalToNewLoadCaseMap, aRandomLoadIDs, aLoad);
        }
    }
}

/******************************************************************************//**
 * \brief Create set of deterministic loads
 * \param [in] aNewLoadCases set of load cases (deterministic plus random load cases)
 * \param [in] aRandomLoadIDs set of random load case identifiers
 * \param [out] aDeterministicLoads set of deterministic load cases
**********************************************************************************/
inline void create_deterministic_load_variables(const std::vector<XMLGen::LoadCase> &aNewLoadCases,
                                                const std::set<int> & aRandomLoadIDs,
                                                std::vector<Plato::srom::Load> &aDeterministicLoads)
{
    for(size_t tLoadCaseIndex = 0; tLoadCaseIndex < aNewLoadCases.size(); tLoadCaseIndex++)
    {
        const int tCurLoadCaseID = std::atoi(aNewLoadCases[tLoadCaseIndex].id.c_str());
        if(aRandomLoadIDs.find(tCurLoadCaseID) == aRandomLoadIDs.end())
        {
            Plato::srom::Load tNewLoad;
            Plato::srom::create_deterministic_load_variable(aNewLoadCases[tLoadCaseIndex], tNewLoad);
            aDeterministicLoads.push_back(tNewLoad);
        }
    }
}

/******************************************************************************//**
 * \brief Generate array of deterministic and random loads in the format expected
 * by the stochastic reduced order model application programming interface.
 * \param [in] aInputLoadCases set of load cases created by the XML generator
 * \param [in] aUncertainties set of random variables created by the XML generator
 * \param [out] aLoads set of deterministic and random loads in the format expected
 *                    by the Stochastic Reduced Order Model (SROM) interface
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool generate_srom_load_inputs(const std::vector<XMLGen::LoadCase> &aInputLoadCases,
                                      const std::vector<XMLGen::Uncertainty> &aUncertainties,
                                      std::vector<Plato::srom::Load> &aLoads)
{
    std::vector<XMLGen::LoadCase> tNewSetLoadCases;
    std::map<int, std::vector<int> > tOriginalToNewLoadCaseMap;
    Plato::srom::expand_load_cases(aInputLoadCases, tNewSetLoadCases, tOriginalToNewLoadCaseMap);

    std::set<int> tRandomLoadIDs;
    Plato::srom::create_random_load_variables(aUncertainties,
                                              tNewSetLoadCases,
                                              tOriginalToNewLoadCaseMap,
                                              tRandomLoadIDs,
                                              aLoads);

    Plato::srom::create_deterministic_load_variables(tNewSetLoadCases, tRandomLoadIDs, aLoads);

    return (true);
}

/******************************************************************************//**
 * \brief Apply rotation matrix to a vector
 * \param [in] aRotatioAnglesInDegrees rotation angles in degrees
 * \param [out] aVectorToRotate rotated vector
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool apply_rotation_matrix(const std::vector<double>& aRotatioAnglesInDegrees,
                                  std::vector<double>& aVectorToRotate)
{
    const bool tBadNumberDetected = Plato::srom::check_vector3d_values(aVectorToRotate) == false;
    const bool tBadRotationDetected = Plato::srom::check_vector3d_values(aRotatioAnglesInDegrees) == false;
    if(tBadRotationDetected || tBadNumberDetected)
    {
        PRINTERR("A NON-FINITE NUMBER WAS DETECTED.\n");
        return (false);
    }

    // compute cosine/sine
    const double tCosAngleX = cos(aRotatioAnglesInDegrees[0] * M_PI / 180.0);
    const double tSinAngleX = sin(aRotatioAnglesInDegrees[0] * M_PI / 180.0);
    const double tCosAngleY = cos(aRotatioAnglesInDegrees[1] * M_PI / 180.0);
    const double tSinAngleY = sin(aRotatioAnglesInDegrees[1] * M_PI / 180.0);
    const double tCosAngleZ = cos(aRotatioAnglesInDegrees[2] * M_PI / 180.0);
    const double tSinAngleZ = sin(aRotatioAnglesInDegrees[2] * M_PI / 180.0);

    // compute all the components associated with the rotation matrix
    const double tA11 =  tCosAngleY * tCosAngleZ;
    const double tA12 = (-tCosAngleX * tSinAngleZ) + (tSinAngleX * tSinAngleY * tCosAngleZ);
    const double tA13 =  (tSinAngleX * tSinAngleZ) + (tCosAngleX * tSinAngleY * tCosAngleZ);

    const double tA21 =  tCosAngleY * tSinAngleZ;
    const double tA22 =  (tCosAngleX * tCosAngleZ) + (tSinAngleX * tSinAngleY * tSinAngleZ);
    const double tA23 = -(tSinAngleX * tCosAngleZ) + (tCosAngleX * tSinAngleY * tSinAngleZ);

    const double tA31 = -tSinAngleY;
    const double tA32 = tSinAngleX * tCosAngleY;
    const double tA33 = tCosAngleX * tCosAngleY;

    // apply rotation matrix to vector
    const double tMagnitudeX = aVectorToRotate[0];
    const double tMagnitudeY = aVectorToRotate[1];
    const double tMagnitudeZ = aVectorToRotate[2];
    aVectorToRotate[0] = (tMagnitudeX * tA11) + (tMagnitudeY * tA12) + (tMagnitudeZ * tA13);
    aVectorToRotate[1] = (tMagnitudeX * tA21) + (tMagnitudeY * tA22) + (tMagnitudeZ * tA23);
    aVectorToRotate[2] = (tMagnitudeX * tA31) + (tMagnitudeY * tA32) + (tMagnitudeZ * tA33);

    return (true);
}

/******************************************************************************//**
 * \brief Expand sample-probability pairs
 * \param [in] aMySampleProbPairs sample-probability pairs for this random load
 * \param [out] aMyXaxisSampleProbPairs sample-probability for a random rotation along the x-axis
 * \param [out] aMyYaxisSampleProbPairs sample-probability for a random rotation along the y-axis
 * \param [out] aMyZaxisSampleProbPairs sample-probability for a random rotation along the z-axis
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool expand_load_sample_probability_pair(const std::vector<Plato::srom::SromVariable> & aMySampleProbPairs,
                                                Plato::srom::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                                Plato::srom::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                                Plato::srom::SampleProbabilityPairs& aMyZaxisSampleProbPairs)
{
    if(aMySampleProbPairs.size() <= 0)
    {
        PRINTERR("INPUT SET OF RANDOM VARIABLES IS EMPTY.\n");
        return (false);
    }

    for(size_t tRandVarIndex = 0; tRandVarIndex < aMySampleProbPairs.size(); tRandVarIndex++)
    {
        Plato::srom::RotationAxis::type_t tMyAxis = Plato::srom::RotationAxis::x;
        Plato::srom::rotation_axis_string_to_enum(aMySampleProbPairs[tRandVarIndex].mAttribute, tMyAxis);
        if(tMyAxis == Plato::srom::RotationAxis::x)
        {
            aMyXaxisSampleProbPairs = aMySampleProbPairs[tRandVarIndex].mSampleProbPairs;
        }
        else if(tMyAxis == Plato::srom::RotationAxis::y)
        {
            aMyYaxisSampleProbPairs = aMySampleProbPairs[tRandVarIndex].mSampleProbPairs;
        }
        else
        {
            aMyZaxisSampleProbPairs = aMySampleProbPairs[tRandVarIndex].mSampleProbPairs;
        }
    }

    return (true);
}

/******************************************************************************//**
 * \brief Compute random rotations along the x, y and z axes
 * \param [in] aMyXaxisSampleProbPairs sample-probability for a random rotation along the x-axis
 * \param [in] aMyYaxisSampleProbPairs sample-probability for a random rotation along the y-axis
 * \param [in] aMyZaxisSampleProbPairs sample-probability for a random rotation along the z-axis
 * \param [out] aMyRandomRotations vector of random rotations
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool compute_random_rotations_about_xyz(const Plato::srom::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                               const Plato::srom::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                               const Plato::srom::SampleProbabilityPairs& aMyZaxisSampleProbPairs,
                                               std::vector<Plato::srom::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexI = 0; tIndexI < aMyXaxisSampleProbPairs.mSamples.size(); tIndexI++)
    {
        for(size_t tIndexJ = 0; tIndexJ < aMyYaxisSampleProbPairs.mSamples.size(); tIndexJ++)
        {
            for(size_t tIndexK = 0; tIndexK < aMyZaxisSampleProbPairs.mSamples.size(); tIndexK++)
            {
                Plato::srom::RandomRotations tMyRandomRotations;
                tMyRandomRotations.mProbability = aMyXaxisSampleProbPairs.mProbabilities[tIndexI]
                        * aMyYaxisSampleProbPairs.mProbabilities[tIndexJ] * aMyZaxisSampleProbPairs.mProbabilities[tIndexK];
                tMyRandomRotations.mRotations.resize(3, 0.0);
                tMyRandomRotations.mRotations[0] = aMyXaxisSampleProbPairs.mSamples[tIndexI];
                tMyRandomRotations.mRotations[1] = aMyYaxisSampleProbPairs.mSamples[tIndexJ];
                tMyRandomRotations.mRotations[2] = aMyZaxisSampleProbPairs.mSamples[tIndexK];
                aMyRandomRotations.push_back(tMyRandomRotations);
            }
        }
    }

    return (true);
}

/******************************************************************************//**
 * \brief Compute random rotations along the x and y axes
 * \param [in] aMyXaxisSampleProbPairs sample-probability for a random rotation along the x-axis
 * \param [in] aMyYaxisSampleProbPairs sample-probability for a random rotation along the y-axis
 * \param [out] aMyRandomRotations vector of random rotations
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool compute_random_rotations_about_xy(const Plato::srom::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                              const Plato::srom::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                              std::vector<Plato::srom::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexI = 0; tIndexI < aMyXaxisSampleProbPairs.mSamples.size(); tIndexI++)
    {
        for(size_t tIndexJ = 0; tIndexJ < aMyYaxisSampleProbPairs.mSamples.size(); tIndexJ++)
        {
            Plato::srom::RandomRotations tMyRandomRotations;
            tMyRandomRotations.mProbability = aMyXaxisSampleProbPairs.mProbabilities[tIndexI]
                    * aMyYaxisSampleProbPairs.mProbabilities[tIndexJ];
            tMyRandomRotations.mRotations.resize(3, 0.0);
            tMyRandomRotations.mRotations[0] = aMyXaxisSampleProbPairs.mSamples[tIndexI];
            tMyRandomRotations.mRotations[1] = aMyYaxisSampleProbPairs.mSamples[tIndexJ];
            aMyRandomRotations.push_back(tMyRandomRotations);
        }
    }

    return (true);
}

/******************************************************************************//**
 * \brief Compute random rotations along the x and z axes
 * \param [in] aMyXaxisSampleProbPairs sample-probability for a random rotation along the x-axis
 * \param [in] aMyZaxisSampleProbPairs sample-probability for a random rotation along the z-axis
 * \param [out] aMyRandomRotations vector of random rotations
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool compute_random_rotations_about_xz(const Plato::srom::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                              const Plato::srom::SampleProbabilityPairs& aMyZaxisSampleProbPairs,
                                              std::vector<Plato::srom::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexI = 0; tIndexI < aMyXaxisSampleProbPairs.mSamples.size(); tIndexI++)
    {
        for(size_t tIndexK = 0; tIndexK < aMyZaxisSampleProbPairs.mSamples.size(); tIndexK++)
        {
            Plato::srom::RandomRotations tMyRandomRotations;
            tMyRandomRotations.mProbability = aMyXaxisSampleProbPairs.mProbabilities[tIndexI]
                    * aMyZaxisSampleProbPairs.mProbabilities[tIndexK];
            tMyRandomRotations.mRotations.resize(3, 0.0);
            tMyRandomRotations.mRotations[0] = aMyXaxisSampleProbPairs.mSamples[tIndexI];
            tMyRandomRotations.mRotations[2] = aMyZaxisSampleProbPairs.mSamples[tIndexK];
            aMyRandomRotations.push_back(tMyRandomRotations);
        }
    }

    return (true);
}

/******************************************************************************//**
 * \brief Compute random rotations along the y and z axes
 * \param [in] aMyYaxisSampleProbPairs sample-probability for a random rotation along the y-axis
 * \param [in] aMyZaxisSampleProbPairs sample-probability for a random rotation along the z-axis
 * \param [out] aMyRandomRotations vector of random rotations
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool compute_random_rotations_about_yz(const Plato::srom::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                              const Plato::srom::SampleProbabilityPairs& aMyZaxisSampleProbPairs,
                                              std::vector<Plato::srom::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexJ = 0; tIndexJ < aMyYaxisSampleProbPairs.mSamples.size(); tIndexJ++)
    {
        for(size_t tIndexK = 0; tIndexK < aMyZaxisSampleProbPairs.mSamples.size(); tIndexK++)
        {
            Plato::srom::RandomRotations tMyRandomRotations;
            tMyRandomRotations.mProbability = aMyYaxisSampleProbPairs.mProbabilities[tIndexJ]
                    * aMyZaxisSampleProbPairs.mProbabilities[tIndexK];
            tMyRandomRotations.mRotations.resize(3, 0.0);
            tMyRandomRotations.mRotations[1] = aMyYaxisSampleProbPairs.mSamples[tIndexJ];
            tMyRandomRotations.mRotations[2] = aMyZaxisSampleProbPairs.mSamples[tIndexK];
            aMyRandomRotations.push_back(tMyRandomRotations);
        }
    }

    return (true);
}

/******************************************************************************//**
 * \brief Compute random rotations along the x axis
 * \param [in] aMyXaxisSampleProbPairs sample-probability for a random rotation along the x-axis
 * \param [out] aMyRandomRotations vector of random rotations
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool compute_random_rotations_about_x(const Plato::srom::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                             std::vector<Plato::srom::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexI = 0; tIndexI < aMyXaxisSampleProbPairs.mSamples.size(); tIndexI++)
    {
        Plato::srom::RandomRotations tMyRandomRotations;
        tMyRandomRotations.mProbability = aMyXaxisSampleProbPairs.mProbabilities[tIndexI];
        tMyRandomRotations.mRotations.resize(3, 0.0);
        tMyRandomRotations.mRotations[0] = aMyXaxisSampleProbPairs.mSamples[tIndexI];
        aMyRandomRotations.push_back(tMyRandomRotations);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Compute random rotations along the y axis
 * \param [in] aMyYaxisSampleProbPairs sample-probability for a random rotation along the x-axis
 * \param [out] aMyRandomRotations vector of random rotations
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool compute_random_rotations_about_y(const Plato::srom::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                             std::vector<Plato::srom::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexJ = 0; tIndexJ < aMyYaxisSampleProbPairs.mSamples.size(); tIndexJ++)
    {
        Plato::srom::RandomRotations tMyRandomRotations;
        tMyRandomRotations.mProbability = aMyYaxisSampleProbPairs.mProbabilities[tIndexJ];
        tMyRandomRotations.mRotations.resize(3, 0.0);
        tMyRandomRotations.mRotations[1] = aMyYaxisSampleProbPairs.mSamples[tIndexJ];
        aMyRandomRotations.push_back(tMyRandomRotations);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Compute random rotations along the z axis
 * \param [in] aMyYaxisSampleProbPairs sample-probability for a random rotation along the z-axis
 * \param [out] aMyRandomRotations vector of random rotations
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool compute_random_rotations_about_z(const Plato::srom::SampleProbabilityPairs& aMyZaxisSampleProbPairs,
                                             std::vector<Plato::srom::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexK = 0; tIndexK < aMyZaxisSampleProbPairs.mSamples.size(); tIndexK++)
    {
        Plato::srom::RandomRotations tMyRandomRotations;
        tMyRandomRotations.mProbability = aMyZaxisSampleProbPairs.mProbabilities[tIndexK];
        tMyRandomRotations.mRotations.resize(3, 0.0);
        tMyRandomRotations.mRotations[2] = aMyZaxisSampleProbPairs.mSamples[tIndexK];
        aMyRandomRotations.push_back(tMyRandomRotations);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Compute vector of random rotations
 * \param [in] aMyXaxisSampleProbPairs sample-probability for a random rotation along the x-axis
 * \param [in] aMyYaxisSampleProbPairs sample-probability for a random rotation along the y-axis
 * \param [in] aMyZaxisSampleProbPairs sample-probability for a random rotation along the z-axis
 * \param [out] aMyRandomRotations vector of random rotations
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool expand_random_rotations(const Plato::srom::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                    const Plato::srom::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                    const Plato::srom::SampleProbabilityPairs& aMyZaxisSampleProbPairs,
                                    std::vector<Plato::srom::RandomRotations> & aMyRandomRotations)
{
    aMyRandomRotations.clear();

    const bool tRotateAboutX = !aMyXaxisSampleProbPairs.mSamples.empty();
    const bool tRotateAboutY = !aMyYaxisSampleProbPairs.mSamples.empty();
    const bool tRotateAboutZ = !aMyZaxisSampleProbPairs.mSamples.empty();

    if(tRotateAboutX && tRotateAboutY && tRotateAboutZ)
    {
        Plato::srom::compute_random_rotations_about_xyz(aMyXaxisSampleProbPairs, aMyYaxisSampleProbPairs, aMyZaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(tRotateAboutX && tRotateAboutY && !tRotateAboutZ)
    {
        Plato::srom::compute_random_rotations_about_xy(aMyXaxisSampleProbPairs, aMyYaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(tRotateAboutX && !tRotateAboutY && tRotateAboutZ)
    {
        Plato::srom::compute_random_rotations_about_xz(aMyXaxisSampleProbPairs, aMyZaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(!tRotateAboutX && tRotateAboutY && tRotateAboutZ)
    {
        Plato::srom::compute_random_rotations_about_yz(aMyYaxisSampleProbPairs, aMyZaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(tRotateAboutX && !tRotateAboutY && !tRotateAboutZ)
    {
        Plato::srom::compute_random_rotations_about_x(aMyXaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(!tRotateAboutX && tRotateAboutY && !tRotateAboutZ)
    {
        Plato::srom::compute_random_rotations_about_y(aMyYaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(!tRotateAboutX && !tRotateAboutY && tRotateAboutZ)
    {
        Plato::srom::compute_random_rotations_about_z(aMyZaxisSampleProbPairs, aMyRandomRotations);
    }
    else
    {
        PRINTERR("INPUT SET OF SAMPLE PROBABILITY PAIRS ARE EMPTY. LOAD VECTOR IS DETERMINISTIC.\n");
        return (false);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Check the input arguments needed to compute the set of random loads
 * \param [in] aMyOriginalLoad original load vector (x, y and z components)
 * \param [in] aMyRandomRotations random rotations
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool check_expand_random_loads_inputs(const std::vector<double> & aMyOriginalLoad,
                                             const std::vector<Plato::srom::RandomRotations> & aMyRandomRotations)
{
    if(aMyRandomRotations.empty())
    {
        PRINTERR("VECTOR OF RANDOM ROTATIONS IS EMPTY.\n");
        return (false);
    }
    else if(Plato::srom::check_vector3d_values(aMyOriginalLoad) == false)
    {
        PRINTERR("A NON-FINITE NUMBER WAS DETECTED IN VECTOR 3D.\n");
        return (false);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Create the set of random loads given its corresponding set of random rotations
 * \param [in] aMyOriginalLoad original load vector (x, y and z components)
 * \param [in] aMyRandomRotations set of random rotations
 * \param [out] aMyRandomLoads set of random loads
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool expand_random_loads(const std::vector<double> & aMyOriginalLoad,
                                const std::vector<Plato::srom::RandomRotations> & aMyRandomRotations,
                                std::vector<Plato::srom::RandomLoad> & aMyRandomLoads)
{
    if(Plato::srom::check_expand_random_loads_inputs(aMyOriginalLoad, aMyRandomRotations) == false)
    {
        PRINTERR("ONE OF THE INPUT ARGUMENTS IS NOT PROPERLY DEFINED.\n");
        return (false);
    }

    const size_t tNumRandomLoads = aMyRandomRotations.size();
    for(size_t tIndex = 0; tIndex < tNumRandomLoads; tIndex++)
    {
        Plato::srom::RandomLoad tMyRandomLoad;
        tMyRandomLoad.mLoadValues = aMyOriginalLoad;
        tMyRandomLoad.mProbability = aMyRandomRotations[tIndex].mProbability;
        if(Plato::srom::apply_rotation_matrix(aMyRandomRotations[tIndex].mRotations, tMyRandomLoad.mLoadValues) == false)
        {
            PRINTERR("APPLICATION OF ROTATION MATRIX WAS UNSUCCESSFUL.\n");
            return (false);
        }
        aMyRandomLoads.push_back(tMyRandomLoad);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Create initial/first random load case
 * \param [in] aNewSetRandomLoads set of random loads for the input random variable
 * \param [out] aOldRandomLoadCases set of random load cases
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool update_initial_random_load_case(const std::vector<Plato::srom::RandomLoad> & aNewSetRandomLoads,
                                            std::vector<Plato::srom::RandomLoadCase> & aOldRandomLoadCases)
{
    std::vector<Plato::srom::RandomLoadCase> tNewSetRandomLoadCase;

    for(size_t tLoadIndex = 0; tLoadIndex < aNewSetRandomLoads.size(); tLoadIndex++)
    {
        Plato::srom::RandomLoadCase tNewRandomLoadCase;
        const Plato::srom::RandomLoad& tMyNewRandomLoad = aNewSetRandomLoads[tLoadIndex];
        tNewRandomLoadCase.mLoads.push_back(tMyNewRandomLoad);
        tNewRandomLoadCase.mProbability = tMyNewRandomLoad.mProbability;
        tNewSetRandomLoadCase.push_back(tNewRandomLoadCase);
    } // index over new random loads

    aOldRandomLoadCases = tNewSetRandomLoadCase;

    return (true);
}

/******************************************************************************//**
 * \brief Update set of random load cases
 * \param [in] aNewSetRandomLoads set of random loads for the input random variable
 * \param [out] aOldRandomLoadCases set of random load cases
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool update_random_load_cases(const std::vector<Plato::srom::RandomLoad> & aNewSetRandomLoads,
                                     std::vector<Plato::srom::RandomLoadCase> & aOldRandomLoadCases)
{
    std::vector<Plato::srom::RandomLoadCase> tNewSetRandomLoadCase;

    for(size_t tNewLoadIndex = 0; tNewLoadIndex < aNewSetRandomLoads.size(); tNewLoadIndex++)
    {
        const Plato::srom::RandomLoad& tMyNewRandomLoad = aNewSetRandomLoads[tNewLoadIndex];
        std::vector<Plato::srom::RandomLoadCase> tTempRandomLoadCases = aOldRandomLoadCases;
        for(size_t tOldLoadCaseIndex = 0; tOldLoadCaseIndex < aOldRandomLoadCases.size(); tOldLoadCaseIndex++)
        {
            tTempRandomLoadCases[tOldLoadCaseIndex].mLoads.push_back(tMyNewRandomLoad);
            Plato::srom::RandomLoadCase& tMyTempRandomLoadCase = tTempRandomLoadCases[tOldLoadCaseIndex];
            tMyTempRandomLoadCase.mProbability = tMyTempRandomLoadCase.mProbability * tMyNewRandomLoad.mProbability;
            tNewSetRandomLoadCase.push_back(tMyTempRandomLoadCase);
        } // index over old random load cases
    } // index over new random loads

    aOldRandomLoadCases = tNewSetRandomLoadCase;

    return (true);
}

/******************************************************************************//**
 * \brief Create the set of random load cases resulting from the solution to the
 *   stochastic reduced order model (SROM) problem
 * \param [in] aNewSetRandomLoads set of random loads for a given random variable
 * \param [out] aOldRandomLoadCases updated set of random load cases
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool expand_random_load_cases(const std::vector<Plato::srom::RandomLoad> & aNewSetRandomLoads,
                                     std::vector<Plato::srom::RandomLoadCase> & aOldRandomLoadCases)
{
    if(aNewSetRandomLoads.empty())
    {
        PRINTERR("THE NEW SET OF RANDOM LOADS IS EMPTY.\n");
        return (false);
    } // if statement

    if(aOldRandomLoadCases.empty())
    {
        if(Plato::srom::update_initial_random_load_case(aNewSetRandomLoads, aOldRandomLoadCases) == false)
        {
            PRINTERR("FUNCTION FAILED WHILE TRYING TO UPDATE THE FIRST RANDOM LOAD CASE.\n");
            return (false);
        } // if statement
    } // if-else statement
    else
    {
        if(Plato::srom::update_random_load_cases(aNewSetRandomLoads, aOldRandomLoadCases) == false)
        {
            PRINTERR("FUNCTION FAILED WHILE TRYING TO ADD A NEW RANDOM LOAD CASE.\n");
            return (false);
        } // if statement
    } // if-else statement

    return (true);
}

/******************************************************************************//**
 * \brief Create the set of random and deterministic loads
 * \param [in] aLoads set of random and deterministic loads
 * \param [out] aRandomLoads set of random loads
 * \param [out] aDeterministicLoads set of deterministic loads
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool expand_random_and_deterministic_loads(const std::vector<Plato::srom::Load>& aLoads,
                                                  std::vector<Plato::srom::Load>& aRandomLoads,
                                                  std::vector<Plato::srom::Load>& aDeterministicLoads)
{
    if(aLoads.empty())
    {
        PRINTERR("INPUT SET OF LOADS IS EMPTY.\n");
        return (false);
    } // if statement

    const size_t tNumLoads = aLoads.size();
    for(size_t tLoadIndex = 0; tLoadIndex < tNumLoads; tLoadIndex++)
    {
        const Plato::srom::Load& tLoad = aLoads[tLoadIndex];
        if(tLoad.mRandomVars.empty())
        {
            // deterministic load
            aDeterministicLoads.push_back(aLoads[tLoadIndex]);
        }
        else
        {
            // random load
            aRandomLoads.push_back(aLoads[tLoadIndex]);
        } // if-else statement
    }// for-loop

    return (true);
}

/******************************************************************************//**
 * \brief Check the parameters that define a load
 * \param [in] aLoads set of loads
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool check_load_parameters(const Plato::srom::Load& aLoad)
{
    if(std::isfinite(aLoad.mAppID) == false)
    {
        PRINTERR("APPLICATION IDENTIFIER IS NOT A FINITE NUMBER.\n");
        return (false);
    }

    if(aLoad.mAppType.empty())
    {
        PRINTERR("APPLICATION TYPE IS NOT DEFINE.\n");
        return (false);
    }

    if(aLoad.mLoadType.empty())
    {
        PRINTERR("LOAD TYPE IS NOT DEFINE.\n");
        return (false);
    }

    if(aLoad.mValues.empty())
    {
        PRINTERR("LOAD VALUES/COMPONENTS ARE NOT DEFINE, VALUES VECTOR IS EMPTY.\n");
        return (false);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Set load components (i.e. magnitudes)
 * \param [in] aInput array of load components string values
 * \param [out] aOutput array of load component values
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool set_load_components(const std::vector<std::string> & aInput, std::vector<double> & aOutput)
{
    if(aInput.empty())
    {
        PRINTERR("INPUT LOAD VECTOR IS EMPTY.\n");
        return (false);
    }

    if(aInput.size() != static_cast<size_t>(3))
    {
        std::ostringstream tMsg;
        tMsg << "INPUT DIMENSIONS = " << aInput.size() << ". CURRENTLY, ONLY 3-DIM PROBLEMS ARE SUPPORTED.\n";
        PRINTERR(tMsg.str().c_str());
        return (false);
    }

    aOutput.resize(3, 0.0);
    aOutput[0] = std::atof(aInput[0].c_str());
    aOutput[1] = std::atof(aInput[1].c_str());
    aOutput[2] = std::atof(aInput[2].c_str());

    return (true);
}

/******************************************************************************//**
 * \brief Set all the parameters used to identify a random load
 * \param [in] aOriginalLoad original load metadata
 * \param [out] aSetRandomLoads set of random loads
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool set_random_load_parameters(const Plato::srom::Load & aOriginalLoad, std::vector<Plato::srom::RandomLoad> & aSetRandomLoads)
{
    if(Plato::srom::check_load_parameters(aOriginalLoad) == false)
    {
        PRINTERR("ONE OR MORE LOAD PARAMETERS ARE NOT DEFINED.\n");
        return (false);
    }

    for(size_t tLoadIndex = 0; tLoadIndex < aSetRandomLoads.size(); tLoadIndex++)
    {
        aSetRandomLoads[tLoadIndex].mAppID = aOriginalLoad.mAppID;
        aSetRandomLoads[tLoadIndex].mAppName = aOriginalLoad.mAppName;
        aSetRandomLoads[tLoadIndex].mAppType = aOriginalLoad.mAppType;
        aSetRandomLoads[tLoadIndex].mLoadType = aOriginalLoad.mLoadType;
    }

    return (true);
}

/******************************************************************************//**
 * \brief Generate the set of random rotations associated with the input random variable
 * \param [in] aMySampleProbPairs sample-probability pairs
 * \param [out] aMySetRandomRotation set of random rotations
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool generate_set_random_rotations(const std::vector<Plato::srom::SromVariable> & aMySampleProbPairs,
                                          std::vector<Plato::srom::RandomRotations> & aMySetRandomRotation)
{
    Plato::srom::SampleProbabilityPairs tXaxisSampleProbPairs, tYaxisSampleProbPairs, tZaxisSampleProbPairs;
    if(Plato::srom::expand_load_sample_probability_pair(aMySampleProbPairs,  tXaxisSampleProbPairs, tYaxisSampleProbPairs, tZaxisSampleProbPairs) == false)
    {
        PRINTERR("FAILED TO EXPAND SAMPLE-PROBABILITY PAIRS.\n");
        return (false);
    }

    Plato::srom::expand_random_rotations(tXaxisSampleProbPairs, tYaxisSampleProbPairs, tZaxisSampleProbPairs, aMySetRandomRotation);

    return (true);
}

/******************************************************************************//**
 * \brief Generate the set of random loads associated with the input random variable
 * \param [in] aOriginalLoad        original load case with corresponding load attributes
 * \param [in] aMySetRandomRotation set of random rotations
 * \param [out] aSetRandomLoads     set of random loads constructed from input rotations information
 * \return flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool generate_set_random_loads(const Plato::srom::Load & aOriginalLoad,
                                      const std::vector<Plato::srom::RandomRotations> & aSetRandomRotations,
                                      std::vector<Plato::srom::RandomLoad> & aSetRandomLoads)
{
    std::vector<double> tMyOriginalLoadComponents;
    tMyOriginalLoadComponents.resize(3, 0.0);
    if(Plato::srom::set_load_components(aOriginalLoad.mValues, tMyOriginalLoadComponents) == false)
    {
        PRINTERR("FAILED TO SET ARRAY OF LOAD COMPONENTS.\n");
        return (false);
    }

    if(Plato::srom::expand_random_loads(tMyOriginalLoadComponents, aSetRandomRotations, aSetRandomLoads) == false)
    {
        PRINTERR("FAILED TO EXPAND SET OF RANDOM LOADS.\n");
        return (false);
    }

    if(Plato::srom::set_random_load_parameters(aOriginalLoad, aSetRandomLoads) == false)
    {
        PRINTERR("FAILED TO SET LOAD TYPE, APPLICATION TYPE AND APPLICATION IDENTIFIER.\n");
        return (false);
    }

    return (true);
}

/******************************************************************************//**
 * \brief Assign an identification number to each load case in the set.
 * \param [out] aSetLoadCases set of load cases
**********************************************************************************/
inline void assign_load_case_identification_number(std::vector<Plato::srom::RandomLoadCase> & aSetLoadCases)
{
    Plato::UniqueCounter tLoadCounter, tLoadCaseCounter;
    tLoadCounter.mark(0); tLoadCaseCounter.mark(0);

    for(size_t tLoadCaseIndex = 0; tLoadCaseIndex < aSetLoadCases.size(); tLoadCaseIndex++)
    {
        Plato::srom::RandomLoadCase& tMyLoadCase = aSetLoadCases[tLoadCaseIndex];
        tMyLoadCase.mLoadCaseID = tLoadCaseCounter.assignNextUnique();
        for(size_t tLoadIndex = 0; tLoadIndex < tMyLoadCase.mLoads.size(); tLoadIndex++)
        {
            tMyLoadCase.mLoads[tLoadIndex].mLoadID = tLoadCounter.assignNextUnique();
        }
    }
}

/******************************************************************************//**
 * \brief Check the parameters used to identified a deterministic load
 * \param [in] aDeterministicLoads set of deterministic loads
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool check_deterministic_loads(const std::vector<Plato::srom::Load>& aDeterministicLoads)
{
    for(size_t tIndex = 0; tIndex < aDeterministicLoads.size(); tIndex++)
    {
        if(Plato::srom::check_load_parameters(aDeterministicLoads[tIndex]) == false)
        {
            std::ostringstream tMsg;
            tMsg << "UNDEFINED PARAMETER FOR DETERMINISTIC LOAD #" << tIndex << ".\n";
            PRINTERR(tMsg.str().c_str());
            return (false);
        }
    }

    return (true);
}

/******************************************************************************//**
 * \brief Append the set of deterministic loads to the set of random load cases
 * \param [in] aDeterministicLoads set of deterministic loads
 * \param [out] aSetLoadCases set of random load cases
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline void append_deterministic_loads(const std::vector<Plato::srom::Load>& aDeterministicLoads,
                                       std::vector<Plato::srom::RandomLoadCase> & aSetLoadCases)
{
    if(aDeterministicLoads.empty())
    {
        return; // return, there are no deterministic loads to append
    }

    for(size_t tLoadCaseIndex = 0; tLoadCaseIndex < aSetLoadCases.size(); tLoadCaseIndex++)
    {
        Plato::srom::RandomLoadCase& tRandomLoadCase = aSetLoadCases[tLoadCaseIndex];
        for(size_t tLoadIndex = 0; tLoadIndex < aDeterministicLoads.size(); tLoadIndex++)
        {
            Plato::srom::RandomLoad tLoad;
            tLoad.mProbability = 1.0;
            tLoad.mAppID = aDeterministicLoads[tLoadIndex].mAppID;
            tLoad.mAppType = aDeterministicLoads[tLoadIndex].mAppType;
            tLoad.mLoadType = aDeterministicLoads[tLoadIndex].mLoadType;
            Plato::srom::set_load_components(aDeterministicLoads[tLoadIndex].mValues, tLoad.mLoadValues);
            tRandomLoadCase.mLoads.push_back(tLoad);
        }
    }
}

/******************************************************************************//**
 * \brief Create the set of the output set of random load cases
 * \param [in] aDeterministicLoads set of deterministic loads
 * \param [out] aSetLoadCases set of random load cases
 * \return error flag - function call was successful, true = no error, false = error
**********************************************************************************/
inline bool generate_output_random_load_cases
(const std::vector<Plato::srom::Load>& aDeterministicLoads,
 std::vector<Plato::srom::RandomLoadCase> & aSetRandomLoadCases)
{
    if(aSetRandomLoadCases.empty())
    {
        PRINTERR("SET OF RANDOM LOAD CASES IS EMPTY.\n");
        return (false);
    }

    if(Plato::srom::check_deterministic_loads(aDeterministicLoads) == false)
    {
        PRINTERR("AN ERROR WAS DETECTED WITH THE INPUT SET OF DETERMINISTIC LOADS.\n");
        return (false);
    }

    Plato::srom::append_deterministic_loads(aDeterministicLoads, aSetRandomLoadCases);
    Plato::srom::assign_load_case_identification_number(aSetRandomLoadCases);

    return (true);
}
// function generate_output_random_load_cases

/******************************************************************************//**
 * \fn build_load_sroms
 * \brief Build set of random load cases using Stochastic Reduced Order Models (SROMs).
 * \param [in] aLoads  set of loads
 * \param [in] aOutput set of random load cases
**********************************************************************************/
inline bool build_load_sroms
(const std::vector<Plato::srom::Load>& aLoads,
 std::vector<Plato::srom::RandomLoadCase>& aOutput)
{
    aOutput.clear();
    std::vector<Plato::srom::Load> tRandomLoads, tDeterministicLoads;
    if(Plato::srom::expand_random_and_deterministic_loads(aLoads, tRandomLoads, tDeterministicLoads) == false)
    {
        PRINTERR("FAILED TO GENERATE THE SETS OF RANDOM AND DETERMINISTIC LOADS.\n");
        return (false);
    }

    for(size_t tLoadIndex = 0; tLoadIndex < tRandomLoads.size(); tLoadIndex++)
    {
        Plato::srom::set_random_variables_id(tRandomLoads[tLoadIndex].mRandomVars);

        std::vector<Plato::srom::SromVariable> tMySampleProbPairs;
        if(Plato::srom::compute_sample_probability_pairs(tRandomLoads[tLoadIndex].mRandomVars, tMySampleProbPairs) == false)
        {
            std::ostringstream tMsg;
            tMsg << "FAILED TO COMPUTE THE SAMPLE-PROBABILITY PAIRS FOR LOAD #" << tLoadIndex << ".\n";
            PRINTERR(tMsg.str().c_str());
            return (false);
        }

        std::vector<Plato::srom::RandomRotations> tMySetRandomRotation;
        if(Plato::srom::generate_set_random_rotations(tMySampleProbPairs, tMySetRandomRotation) == false)
        {
            std::ostringstream tMsg;
            tMsg << "FAILED TO GENERATE SET OF RANDOM ROTATIONS FOR LOAD #" << tLoadIndex << ".\n";
            PRINTERR(tMsg.str().c_str());
            return (false);
        }

        std::vector<Plato::srom::RandomLoad> tMySetRandomLoads;
        if(Plato::srom::generate_set_random_loads(tRandomLoads[tLoadIndex], tMySetRandomRotation, tMySetRandomLoads) == false)
        {
            std::ostringstream tMsg;
            tMsg << "FAILED TO GENERATE SET OF RANDOM LOADS FOR LOAD #" << tLoadIndex << ".\n";
            PRINTERR(tMsg.str().c_str());
            return (false);
        }

        if(Plato::srom::expand_random_load_cases(tMySetRandomLoads, aOutput) == false)
        {
            std::ostringstream tMsg;
            tMsg << "FAILED TO EXPAND RANDOM LOAD CASES FOR LOAD #" << tLoadIndex << ".\n";
            PRINTERR(tMsg.str().c_str());
            return (false);
        }
    }

    if(Plato::srom::generate_output_random_load_cases(tDeterministicLoads, aOutput) == false)
    {
        PRINTERR("FAILED TO GENERATE SET OF OUTPUT RANDOM LOAD CASES.\n");
        return (false);
    }

    return(true);
}
// function build_load_sroms

}
// namespace srom

}
// namespace Plato
