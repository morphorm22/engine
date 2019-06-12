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
 * Plato_Test_UncertainLoadGeneratorXML.cpp
 *
 *  Created on: Jun 1, 2019
 */

#include "gtest/gtest.h"

#include <map>
#include <cmath>
#include <locale>
#include <cctype>
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "../../base/src/tools/Plato_UniqueCounter.hpp"
#include "../../base/src/tools/Plato_Vector3DVariations.hpp"
#include "../../base/src/optimize/Plato_SromProbDataStruct.hpp"
#include "../../base/src/optimize/Plato_SolveUncertaintyProblem.hpp"
#include "../../base/src/optimize/Plato_KelleySachsAugmentedLagrangianLightInterface.hpp"
#include "../../base/src/input_generator/XMLGeneratorDataStruct.hpp"

namespace Plato
{

enum VariableType
{
    VT_LOAD,
    VT_MATERIAL
};

class Variable
{

public:

    Variable();
    virtual ~Variable();
    virtual VariableType variableType() = 0;

    // Get/set functions for member data
    bool isRandom() { return  mIsRandom; }
    void isRandom(const bool &aNewValue) { mIsRandom = aNewValue; }
    std::string type() { return mType; }
    void type(const std::string &aNewType) { mType = aNewType; }
    std::string subType() { return mSubType; }
    void subType(const std::string &aNewSubType) { mSubType = aNewSubType; }
    std::string globalID() { return mGlobalID; }
    void globalID(const std::string &aNewGlobalID) { mGlobalID = aNewGlobalID; }
    std::string distribution() { return mDistribution; }
    void distribution(const std::string &aNewDistribution) { mDistribution = aNewDistribution; }
    std::string mean() { return mMean; }
    void mean(const std::string &aNewMean) { mMean = aNewMean; }
    std::string upperBound() { return mUpperBound; }
    void upperBound(const std::string &aNewUpperBound) { mUpperBound = aNewUpperBound; }
    std::string lowerBound() { return mLowerBound; }
    void lowerBound(const std::string &aNewLowerBound) { mLowerBound = aNewLowerBound; }
    std::string standardDeviation() { return mStandardDeviation; }
    void standardDeviation(const std::string &aNewStandardDeviation) { mStandardDeviation = aNewStandardDeviation; }
    std::string numSamples() { return mNumSamples; }
    void numSamples(const std::string &aNewNumSamples) { mNumSamples = aNewNumSamples; }

private:

    bool mIsRandom; /*!< specifies whether this variable has uncertainty */
    std::string mType; /*!< random variable type, e.g. random rotation */
    std::string mSubType; /*!< random variable subtype, e.g. rotation axis */
    std::string mGlobalID; /*!< random variable global identifier */
    std::string mDistribution; /*!< probability distribution */
    std::string mMean; /*!< probability distribution mean */
    std::string mUpperBound; /*!< probability distribution upper bound */
    std::string mLowerBound; /*!< probability distribution lower bound */
    std::string mStandardDeviation; /*!< probability distribution standard deviation */
    std::string mNumSamples; /*!< number of samples */
};

class SROMLoad : public Variable
{

public:

    SROMLoad();
    virtual ~SROMLoad();
    VariableType variableType() override { return VT_LOAD; }

private:

    Vector3D mLoadComponents;

};

class SROMMaterial : public Variable
{

public:

    SROMMaterial();
    virtual ~SROMMaterial();
    virtual VariableType variableType() override { return VT_MATERIAL; }

};

struct RandomVariable
{
    std::string mType; /*!< random variable type, e.g. random rotation */
    std::string mSubType; /*!< random variable subtype, e.g. rotation axis */
    std::string mGlobalID; /*!< random variable global identifier */
    std::string mDistribution; /*!< probability distribution */
    std::string mMean; /*!< probability distribution mean */
    std::string mUpperBound; /*!< probability distribution upper bound */
    std::string mLowerBound; /*!< probability distribution lower bound */
    std::string mStandardDeviation; /*!< probability distribution standard deviation */
    std::string mNumSamples; /*!< number of samples */
};

struct SampleProbabilityPairs
{
    int mNumSamples; /*!< total number of samples */
    std::vector<double> mSamples; /*!< sample set */
    std::vector<double> mProbabilities; /*!< probability set  */
};

struct SromRandomVariable
{
    std::string mType; /*!< random variable type, e.g. random rotation */
    std::string mSubType; /*!< random variable subtype, e.g. rotation axis */
    Plato::SampleProbabilityPairs mSampleProbPair; /*!< sample-probability pair for this random variable */
};

struct RandomRotations
{
    double mProbability; /*!< probability associated with this random rotation */
    Plato::Vector3D mRotations; /*!< vector of random rotations, e.g. /f$(\theta_x, \theta_y, \theta_z)/f$ */
};

struct RandomLoad
{
    int mLoadID;           /*!< random load vector global identifier */
    double mProbability;   /*!< probability associated with this random load */
    Plato::Vector3D mLoad; /*!< vector of random loads, e.g. /f$(f_x, f_y, f_z)/f$ */
};

struct RandomLoadCase
{
    int mLoadCaseID;                       /*!< random load case global identifier */
    double mProbability;                   /*!< probability associated with this random load case */
    std::vector<Plato::RandomLoad> mLoads; /*!< set of random loads associated with this random load case */
};

inline bool check_vector3d_values(const Plato::Vector3D & aMyOriginalLoad)
{
    if(std::isfinite(aMyOriginalLoad.mX) == false)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: X-COMPONENT IS NOT A FINITE NUMBER.\n";
        return (false);
    }
    else if(std::isfinite(aMyOriginalLoad.mY) == false)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: Y-COMPONENT IS NOT A FINITE NUMBER.\n";
        return (false);
    }
    else if(std::isfinite(aMyOriginalLoad.mZ) == false)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: Z-COMPONENT IS NOT A FINITE NUMBER.\n";
        return (false);
    }

    return (true);
}

inline void initialize_load_id_counter(const std::vector<LoadCase> &aLoadCases,
                                       UniqueCounter &aUniqueCounter)
{
    aUniqueCounter.mark(0); // Mark 0 as true since we don't want to have a 0 ID
    for(size_t i=0; i<aLoadCases.size(); ++i)
        aUniqueCounter.mark(std::atoi(aLoadCases[i].id.c_str()));
}

inline void expand_single_load_case(const LoadCase &aOldLoadCase,
                                    std::vector<LoadCase> &aNewLoadCaseList,
                                    UniqueCounter &aUniqueLoadIDCounter,
                                    std::map<int, std::vector<int> > &tOriginalToNewLoadCaseMap)
{
    int tOriginalLoadCaseID = std::atoi(aOldLoadCase.id.c_str());
    for(size_t j=0; j<aOldLoadCase.loads.size(); ++j)
    {
        std::string tIDString = aOldLoadCase.id;
        int tCurLoadCaseID = std::atoi(tIDString.c_str());
        // If this is a multi-load load case we need to generate
        // a new load case with a new id.
        if(j>0)
        {
            tCurLoadCaseID = aUniqueLoadIDCounter.assignNextUnique();
            tIDString = std::to_string(tCurLoadCaseID);
        }
        tOriginalToNewLoadCaseMap[tOriginalLoadCaseID].push_back(tCurLoadCaseID);
        LoadCase tNewLoadCase = aOldLoadCase;
        tNewLoadCase.id = tIDString;
        tNewLoadCase.loads[0] = aOldLoadCase.loads[j];
        tNewLoadCase.loads.resize(1);
        aNewLoadCaseList.push_back(tNewLoadCase);
    }
}

inline void expand_load_cases(const InputData &aInputData,
                              std::vector<LoadCase> &aNewLoadCaseList,
                              std::map<int, std::vector<int> > &tOriginalToNewLoadCaseMap)
{
    UniqueCounter tUniqueLoadIDCounter;
    initialize_load_id_counter(aInputData.load_cases, tUniqueLoadIDCounter);

    for(size_t i=0; i<aInputData.load_cases.size(); ++i)
        expand_single_load_case(aInputData.load_cases[i], aNewLoadCaseList, tUniqueLoadIDCounter,
                                tOriginalToNewLoadCaseMap);
}

inline bool generateSROMInput(const InputData &aInputData,
                              std::vector<std::shared_ptr<Variable> > &aSROMVariables)
{
    std::map<int, std::vector<int> > tOriginalToNewLoadCaseMap;

    // Make sure we don't have any multi-load load cases.  If we do,
    // expand them into load cases with single loads.
    std::vector<LoadCase> tNewLoadCases;
    expand_load_cases(aInputData, tNewLoadCases, tOriginalToNewLoadCaseMap);

    // Loop through uncertainties and add loads with uncertainties to the SROM input
    // data structures.
    for(size_t i=0; i<aInputData.uncertainties.size(); ++i)
    {
        const Uncertainty &tCurUncertainty = aInputData.uncertainties[i];
        const std::string tLoadIDString = tCurUncertainty.id;
        const int tLoadID = std::atoi(tLoadIDString.c_str());
        for(size_t j=0; j<tOriginalToNewLoadCaseMap[tLoadID].size(); ++j)
        {
            const int tCurLoadCaseID = tOriginalToNewLoadCaseMap[tLoadID][j];
            for(size_t k=0; k<tNewLoadCases.size(); ++k)
            {
                if(tNewLoadCases[k].id == tLoadIDString)
                {
                    std::shared_ptr<Variable> tNewLoad = std::make_shared<SROMLoad>();
                    tNewLoad->isRandom(true);
                    tNewLoad->type(tCurUncertainty.type);
                    tNewLoad->subType(tCurUncertainty.axis);
                    tNewLoad->globalID(tLoadIDString);
                    tNewLoad->distribution(tCurUncertainty.distribution);
                    tNewLoad->mean(tCurUncertainty.mean);
                    tNewLoad->upperBound(tCurUncertainty.upper);
                    tNewLoad->lowerBound(tCurUncertainty.lower);
                    tNewLoad->standardDeviation(tCurUncertainty.standard_deviation);
                    tNewLoad->numSamples(tCurUncertainty.num_samples);
                    aSROMVariables.push_back(tNewLoad);
                    k=tNewLoadCases.size(); // break out of the looping
                }
            }
        }
    }

    return true;
}


inline bool apply_rotation_matrix(const Plato::Vector3D& aRotatioAnglesInDegrees, Plato::Vector3D& aVectorToRotate)
{
    const bool tBadNumberDetected = Plato::check_vector3d_values(aVectorToRotate) == false;
    const bool tBadRotationDetected = Plato::check_vector3d_values(aRotatioAnglesInDegrees) == false;
    if(tBadRotationDetected || tBadNumberDetected)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: A NON-FINITE NUMBER WAS DETECTED.\n";
        return (false);
    }

    // compute cosine/sine
    const double tCosAngleX = cos(aRotatioAnglesInDegrees.mX * M_PI / 180.0);
    const double tSinAngleX = sin(aRotatioAnglesInDegrees.mX * M_PI / 180.0);
    const double tCosAngleY = cos(aRotatioAnglesInDegrees.mY * M_PI / 180.0);
    const double tSinAngleY = sin(aRotatioAnglesInDegrees.mY * M_PI / 180.0);
    const double tCosAngleZ = cos(aRotatioAnglesInDegrees.mZ * M_PI / 180.0);
    const double tSinAngleZ = sin(aRotatioAnglesInDegrees.mZ * M_PI / 180.0);

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
    const double tMagnitudeX = aVectorToRotate.mX;
    const double tMagnitudeY = aVectorToRotate.mY;
    const double tMagnitudeZ = aVectorToRotate.mZ;
    aVectorToRotate.mX = (tMagnitudeX * tA11) + (tMagnitudeY * tA12) + (tMagnitudeZ * tA13);
    aVectorToRotate.mY = (tMagnitudeX * tA21) + (tMagnitudeY * tA22) + (tMagnitudeZ * tA23);
    aVectorToRotate.mZ = (tMagnitudeX * tA31) + (tMagnitudeY * tA32) + (tMagnitudeZ * tA33);

    return (true);
}

/*inline bool register_all_load_ids(const std::vector<LoadCase> & aLoadCases, Plato::UniqueCounter & aUniqueLoadCounter)
{
    if(aLoadCases.size() <= 0)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: INPUT SET OF LOAD CASES IS EMPTY.\n";
        return (false);
    }

    const int tNumLoadCases = aLoadCases.size();
    for(int tPrivateLoadIndex = 0; tPrivateLoadIndex < tNumLoadCases; tPrivateLoadIndex++)
    {
        // register load case
        const int tMyLoadCaseId = std::atoi(aLoadCases[tPrivateLoadIndex].id.c_str());
        aUniqueLoadCounter.mark(tMyLoadCaseId);

        const std::vector<Load> & tMyLoadCaseLoads = aLoadCases[tPrivateLoadIndex].loads;
        const int tMyLoadCaseNumLoads = tMyLoadCaseLoads.size();
        for(int tLoadIndex = 0; tLoadIndex < tMyLoadCaseNumLoads; tLoadIndex++)
        {
            // register load id
            const int tMyLoadID = std::atoi(tMyLoadCaseLoads[tLoadIndex].load_id.c_str());
            aUniqueLoadCounter.mark(tMyLoadID);
        }
    }

    return (true);
}*/

inline bool define_distribution(const Plato::RandomVariable & aMyRandomVar, Plato::SromInputs<double> & aInput)
{
    if(aMyRandomVar.mDistribution == "normal")
    {
        aInput.mDistribution = Plato::DistrubtionName::normal;
    }
    else if(aMyRandomVar.mDistribution == "uniform")
    {
        aInput.mDistribution = Plato::DistrubtionName::uniform;
    }
    else if(aMyRandomVar.mDistribution == "beta")
    {
        aInput.mDistribution = Plato::DistrubtionName::beta;
    }
    else
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: DISTRIBUTION = " << aMyRandomVar.mDistribution
                 << " IS NOT DEFINED. OPTIONS ARE NORMAL, UNIFORM AND BETA.\n";
        return (false);
    }

    return (true);
}

inline bool check_input_mean(const Plato::RandomVariable & aMyRandomVar)
{
    if(aMyRandomVar.mMean.empty() == true)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: MEAN IS NOT DEFINED.\n";
        return (false);
    }
    return (true);
}

inline bool check_input_lower_bound(const Plato::RandomVariable & aMyRandomVar)
{
    if(aMyRandomVar.mLowerBound.empty() == true)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: LOWER BOUND IS NOT DEFINED.\n";
        return (false);
    }
    return (true);
}

inline bool check_input_upper_bound(const Plato::RandomVariable & aMyRandomVar)
{
    if(aMyRandomVar.mUpperBound.empty() == true)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: UPPER BOUND IS NOT DEFINED.\n";
        return (false);
    }
    return (true);
}

inline bool check_input_standard_deviation(const Plato::RandomVariable & aMyRandomVar)
{
    if(aMyRandomVar.mStandardDeviation.empty() == true)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: STANDARD DEVIATION IS NOT DEFINED.\n";
        return (false);
    }
    return (true);
}

inline bool check_input_number_samples(const Plato::RandomVariable & aMyRandomVar)
{
    if(aMyRandomVar.mNumSamples.empty() == true)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: NUMBER OF SAMPLES IS NOT DEFINED.\n";
        return (false);
    }
    else if(std::atof(aMyRandomVar.mNumSamples.c_str()) <= 0)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: NUMBER OF SAMPLES SHOULD BE GREATER THAN ZERO. "
                 << "INPUT NUMBER OF SAMPLES = " << std::atof(aMyRandomVar.mNumSamples.c_str()) << ".\n";
        return (false);
    }

    return (true);
}

inline bool check_input_statistics(const Plato::RandomVariable & aMyRandomVar)
{
    std::locale tLocale;
    std::stringstream tOutput;
    for(std::string::size_type tIndex = 0; tIndex < aMyRandomVar.mDistribution.length(); ++tIndex)
    {
        tOutput << std::toupper(aMyRandomVar.mDistribution[tIndex], tLocale);
    }

    if(aMyRandomVar.mDistribution == "beta" || aMyRandomVar.mDistribution == "normal")
    {
        bool tIsMeanDefined = Plato::check_input_mean(aMyRandomVar);
        bool tIsLowerBoundDefined = Plato::check_input_lower_bound(aMyRandomVar);
        bool tIsUpperBoundDefined = Plato::check_input_upper_bound(aMyRandomVar);
        bool tIsNumSamplesDefined = Plato::check_input_number_samples(aMyRandomVar);
        bool tIsStandardDeviationDefined = Plato::check_input_standard_deviation(aMyRandomVar);
        bool tStatisticsDefined = tIsMeanDefined && tIsLowerBoundDefined && tIsUpperBoundDefined && tIsNumSamplesDefined
                && tIsStandardDeviationDefined;
        if(tStatisticsDefined == false)
        {
            std::cout<< "\nFILE: " << __FILE__
                     << "\nFUNCTION: " << __PRETTY_FUNCTION__
                     << "\nLINE:" << __LINE__
                     << "\nMESSAGE: FULL SET OF INPUT STATISTICS FOR THE " << tOutput.str().c_str()
                     << " DISTRIBUTION IS NOT PROPERLY DEFINED.\n";
            return (false);
        }
    }
    else if(aMyRandomVar.mDistribution == "uniform")
    {
        bool tIsLowerBoundDefined = Plato::check_input_lower_bound(aMyRandomVar);
        bool tIsUpperBoundDefined = Plato::check_input_upper_bound(aMyRandomVar);
        bool tIsNumSamplesDefined = Plato::check_input_number_samples(aMyRandomVar);
        bool tStatisticsDefined = tIsLowerBoundDefined && tIsUpperBoundDefined && tIsNumSamplesDefined;
        if(tStatisticsDefined == false)
        {
            std::cout<< "\nFILE: " << __FILE__
                     << "\nFUNCTION: " << __PRETTY_FUNCTION__
                     << "\nLINE:" << __LINE__
                     << "\nMESSAGE: SET OF INPUT STATISTICS FOR THE " << tOutput.str().c_str()
                     << " DISTRIBUTION IS NOT PROPERLY DEFINED.\n";
            return (false);
        }
    }

    return (true);
}

inline bool define_input_statistics(const Plato::RandomVariable & aMyRandomVar, Plato::SromInputs<double> & aInput)
{
    if(Plato::check_input_statistics(aMyRandomVar) == false)
    {
        return (false);
    }

    aInput.mMean = std::atof(aMyRandomVar.mMean.c_str());
    aInput.mLowerBound = std::atof(aMyRandomVar.mLowerBound.c_str());
    aInput.mUpperBound = std::atof(aMyRandomVar.mUpperBound.c_str());
    const double tStdDev = std::atof(aMyRandomVar.mStandardDeviation.c_str());
    aInput.mVariance = tStdDev * tStdDev;
    const int tNumSamples = std::atoi(aMyRandomVar.mNumSamples.c_str());
    aInput.mNumSamples = tNumSamples;

    return (true);
}

inline bool post_process_sample_probability_pairs(const std::vector<Plato::SromOutputs<double>> aMySromSolution,
                                                  const Plato::RandomVariable & aMyRandomVariable,
                                                  Plato::SromRandomVariable & aMySromRandomVariable)
{
    if(aMySromSolution.size() <= 0)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: SROM SOLUTION IS EMPTY.\n";
        return (false);
    }

    aMySromRandomVariable.mSampleProbPair.mSamples.clear();
    aMySromRandomVariable.mSampleProbPair.mProbabilities.clear();

    aMySromRandomVariable.mType = aMyRandomVariable.mType;
    aMySromRandomVariable.mSubType = aMyRandomVariable.mSubType;

    const size_t tNumSamples = aMySromSolution.size();
    aMySromRandomVariable.mSampleProbPair.mNumSamples = tNumSamples;
    aMySromRandomVariable.mSampleProbPair.mSamples.resize(tNumSamples);
    aMySromRandomVariable.mSampleProbPair.mProbabilities.resize(tNumSamples);

    for(size_t tIndex = 0; tIndex < tNumSamples; tIndex++)
    {
        aMySromRandomVariable.mSampleProbPair.mSamples[tIndex] = aMySromSolution[tIndex].mSampleValue;
        aMySromRandomVariable.mSampleProbPair.mProbabilities[tIndex] = aMySromSolution[tIndex].mSampleWeight;
    }

    return (true);
}

inline bool compute_uniform_random_variable_statistics(const Plato::SromInputs<double> & aSromInputs,
                                                       std::vector<Plato::SromOutputs<double>> & aSromOutputSet)
{
    aSromOutputSet.clear();

    const double tSampleProbability = static_cast<double>(1.0 / aSromInputs.mNumSamples);
    const double tDelta = (aSromInputs.mUpperBound - aSromInputs.mLowerBound) / static_cast<double>(aSromInputs.mNumSamples - 1);
    for(size_t tIndex = 0; tIndex < aSromInputs.mNumSamples; tIndex++)
    {
        Plato::SromOutputs<double> tSromOutputs;
        tSromOutputs.mSampleWeight = tSampleProbability;
        tSromOutputs.mSampleValue = aSromInputs.mLowerBound + (static_cast<double>(tIndex) * tDelta);
        aSromOutputSet.push_back(tSromOutputs);
    }
    return (true);
}

inline bool compute_random_variable_statistics(const Plato::SromInputs<double> & aSromInputs,
                                               std::vector<Plato::SromOutputs<double>> & aSromOutputs)
{
    switch(aSromInputs.mDistribution)
    {
        case Plato::DistrubtionName::beta:
        case Plato::DistrubtionName::normal:
        {
            // solve stochastic reduced order model sub-problem
            const bool tEnableOutput = true;
            Plato::AlgorithmInputsKSAL<double> tAlgoInputs;
            Plato::SromDiagnostics<double> tSromDiagnostics;
            Plato::solve_srom_problem(aSromInputs, tAlgoInputs, tSromDiagnostics, aSromOutputs, tEnableOutput);
            break;
        }
        case Plato::DistrubtionName::uniform:
        {
            Plato::compute_uniform_random_variable_statistics(aSromInputs, aSromOutputs);
            break;
        }
        default:
        case Plato::DistrubtionName::undefined:
        {
            std::cout<< "\nFILE: " << __FILE__
                     << "\nFUNCTION: " << __PRETTY_FUNCTION__
                     << "\nLINE:" << __LINE__
                     << "\nMESSAGE: INPUT DISTRIBUTION IS NOT SUPPORTED. OPTIONS ARE BETA, NORMAL AND UNIFORM.\n";
            return (false);
        }
    }

    return (true);
}

inline bool compute_sample_probability_pairs(const std::vector<Plato::RandomVariable> & aMySetRandomVars,
                                             std::vector<Plato::SromRandomVariable> & aMySampleProbPairs)
{
    if(aMySetRandomVars.size() <= 0)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: INPUT SET OF RANDOM VARIABLES IS EMPTY.\n";
        return (false);
    }

    aMySampleProbPairs.clear();

    const size_t tMyNumRandomVars = aMySetRandomVars.size();
    for(size_t tRandomVarIndex = 0; tRandomVarIndex < tMyNumRandomVars; tRandomVarIndex++)
    {
        // pose uncertainty
        Plato::SromInputs<double> tSromInputs;
        const Plato::RandomVariable & tMyRandomVar = aMySetRandomVars[tRandomVarIndex];
        if(Plato::define_distribution(tMyRandomVar, tSromInputs) == false)
        {
            std::cout<< "\nFILE: " << __FILE__
                     << "\nFUNCTION: " << __PRETTY_FUNCTION__
                     << "\nLINE:" << __LINE__
                     << "\nMESSAGE: PROBABILITY DISTIRBUTION WAS NOT DEFINED FOR RANDOM VARIABLE #"
                     << tRandomVarIndex << ".\n";
            return (false);
        }

        if(Plato::define_input_statistics(tMyRandomVar, tSromInputs) == false)
        {
            std::cout<< "\nFILE: " << __FILE__
                     << "\nFUNCTION: " << __PRETTY_FUNCTION__
                     << "\nLINE:" << __LINE__
                     << "\nMESSAGE: SET OF INPUT STATISTICS FOR THE SROM PROBLEM IS NOT PROPERLY DEFINED FOR RANDOM VARIABLE #"
                     << tRandomVarIndex << ".\n";
            return (false);
        }

        std::vector<Plato::SromOutputs<double>> tSromOutputs;
        if(Plato::compute_random_variable_statistics(tSromInputs, tSromOutputs) == false)
        {
            std::cout<< "\nFILE: " << __FILE__
                     << "\nFUNCTION: " << __PRETTY_FUNCTION__
                     << "\nLINE:" << __LINE__
                     << "\nMESSAGE: STATISTICS FOR RANDOM VARIABLE #" << tRandomVarIndex << " WERE NOT COMPUTED.\n";
            return (false);
        }

        Plato::SromRandomVariable tMySampleProbPairs;
        if(Plato::post_process_sample_probability_pairs(tSromOutputs, tMyRandomVar, tMySampleProbPairs) == false)
        {
            std::cout<< "\nFILE: " <<  __FILE__
                     << "\nFUNCTION: " << __PRETTY_FUNCTION__
                     << "\nLINE:" << __LINE__
                     << "\nMESSAGE: SAMPLE PROBABILITY PAIR POST PROCESSING FAILED FOR RANDOM VARIABLE #"
                     << tRandomVarIndex << ".\n";
            return (false);
        }
        aMySampleProbPairs.push_back(tMySampleProbPairs);
    }

    return (true);
}

inline bool post_process_random_load(const std::vector<Plato::SromRandomVariable> & aMySampleProbPairs,
                                     std::map<Plato::axis3D::axis3D, Plato::SampleProbabilityPairs> & aRotationAxisToSampleProbPairs)
{
    if(aMySampleProbPairs.size() <= 0)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: INPUT SET OF RANDOM VARIABLES IS EMPTY.\n";
        return (false);
    }

    const size_t tNumRandomVariables = aMySampleProbPairs.size();
    for(size_t tRandVarIndex = 0; tRandVarIndex < tNumRandomVariables; tRandVarIndex++)
    {
        Plato::axis3D::axis3D tMyAxis = Plato::axis3D::axis3D::x;
        Plato::axis3D_stringToEnum(aMySampleProbPairs[tRandVarIndex].mSubType, tMyAxis);
        aRotationAxisToSampleProbPairs[tMyAxis] = aMySampleProbPairs[tRandVarIndex].mSampleProbPair;
    }

    if(aMySampleProbPairs.size() != aRotationAxisToSampleProbPairs.size())
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: DUPLICATE RANDOM ROTATIONS WERE PROVIDED FOR A SINGLE LOAD."
                 << " RANDOM ROTATIONS ARE EXPECTED TO BE UNIQUE FOR A SINGLE LOAD.\n";
        return (false);
    }

    return (true);
}

inline bool set_random_rotations_about_xyz(const Plato::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                           const Plato::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                           const Plato::SampleProbabilityPairs& aMyZaxisSampleProbPairs,
                                           std::vector<Plato::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexI = 0; tIndexI < aMyXaxisSampleProbPairs.mSamples.size(); tIndexI++)
    {
        for(size_t tIndexJ = 0; tIndexJ < aMyYaxisSampleProbPairs.mSamples.size(); tIndexJ++)
        {
            for(size_t tIndexK = 0; tIndexK < aMyZaxisSampleProbPairs.mSamples.size(); tIndexK++)
            {
                Plato::RandomRotations tMyRandomRotations;
                tMyRandomRotations.mProbability = aMyXaxisSampleProbPairs.mProbabilities[tIndexI]
                        * aMyYaxisSampleProbPairs.mProbabilities[tIndexJ] * aMyZaxisSampleProbPairs.mProbabilities[tIndexK];
                tMyRandomRotations.mRotations.mX = aMyXaxisSampleProbPairs.mSamples[tIndexI];
                tMyRandomRotations.mRotations.mY = aMyYaxisSampleProbPairs.mSamples[tIndexJ];
                tMyRandomRotations.mRotations.mZ = aMyZaxisSampleProbPairs.mSamples[tIndexK];
                aMyRandomRotations.push_back(tMyRandomRotations);
            }
        }
    }

    return (true);
}

inline bool set_random_rotations_about_xy(const Plato::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                          const Plato::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                          std::vector<Plato::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexI = 0; tIndexI < aMyXaxisSampleProbPairs.mSamples.size(); tIndexI++)
    {
        for(size_t tIndexJ = 0; tIndexJ < aMyYaxisSampleProbPairs.mSamples.size(); tIndexJ++)
        {
            Plato::RandomRotations tMyRandomRotations;
            tMyRandomRotations.mProbability = aMyXaxisSampleProbPairs.mProbabilities[tIndexI]
                    * aMyYaxisSampleProbPairs.mProbabilities[tIndexJ];
            tMyRandomRotations.mRotations.mX = aMyXaxisSampleProbPairs.mSamples[tIndexI];
            tMyRandomRotations.mRotations.mY = aMyYaxisSampleProbPairs.mSamples[tIndexJ];
            aMyRandomRotations.push_back(tMyRandomRotations);
        }
    }

    return (true);
}

inline bool set_random_rotations_about_xz(const Plato::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                          const Plato::SampleProbabilityPairs& aMyZaxisSampleProbPairs,
                                          std::vector<Plato::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexI = 0; tIndexI < aMyXaxisSampleProbPairs.mSamples.size(); tIndexI++)
    {
        for(size_t tIndexK = 0; tIndexK < aMyZaxisSampleProbPairs.mSamples.size(); tIndexK++)
        {
            Plato::RandomRotations tMyRandomRotations;
            tMyRandomRotations.mProbability = aMyXaxisSampleProbPairs.mProbabilities[tIndexI]
                    * aMyZaxisSampleProbPairs.mProbabilities[tIndexK];
            tMyRandomRotations.mRotations.mX = aMyXaxisSampleProbPairs.mSamples[tIndexI];
            tMyRandomRotations.mRotations.mZ = aMyZaxisSampleProbPairs.mSamples[tIndexK];
            aMyRandomRotations.push_back(tMyRandomRotations);
        }
    }

    return (true);
}

inline bool set_random_rotations_about_yz(const Plato::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                          const Plato::SampleProbabilityPairs& aMyZaxisSampleProbPairs,
                                          std::vector<Plato::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexJ = 0; tIndexJ < aMyYaxisSampleProbPairs.mSamples.size(); tIndexJ++)
    {
        for(size_t tIndexK = 0; tIndexK < aMyZaxisSampleProbPairs.mSamples.size(); tIndexK++)
        {
            Plato::RandomRotations tMyRandomRotations;
            tMyRandomRotations.mProbability = aMyYaxisSampleProbPairs.mProbabilities[tIndexJ]
                    * aMyZaxisSampleProbPairs.mProbabilities[tIndexK];
            tMyRandomRotations.mRotations.mY = aMyYaxisSampleProbPairs.mSamples[tIndexJ];
            tMyRandomRotations.mRotations.mZ = aMyZaxisSampleProbPairs.mSamples[tIndexK];
            aMyRandomRotations.push_back(tMyRandomRotations);
        }
    }

    return (true);
}

inline bool set_random_rotations_about_x(const Plato::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                         std::vector<Plato::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexI = 0; tIndexI < aMyXaxisSampleProbPairs.mSamples.size(); tIndexI++)
    {
        Plato::RandomRotations tMyRandomRotations;
        tMyRandomRotations.mProbability = aMyXaxisSampleProbPairs.mProbabilities[tIndexI];
        tMyRandomRotations.mRotations.mX = aMyXaxisSampleProbPairs.mSamples[tIndexI];
        aMyRandomRotations.push_back(tMyRandomRotations);
    }

    return (true);
}

inline bool set_random_rotations_about_y(const Plato::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                         std::vector<Plato::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexJ = 0; tIndexJ < aMyYaxisSampleProbPairs.mSamples.size(); tIndexJ++)
    {
        Plato::RandomRotations tMyRandomRotations;
        tMyRandomRotations.mProbability = aMyYaxisSampleProbPairs.mProbabilities[tIndexJ];
        tMyRandomRotations.mRotations.mY = aMyYaxisSampleProbPairs.mSamples[tIndexJ];
        aMyRandomRotations.push_back(tMyRandomRotations);
    }

    return (true);
}

inline bool set_random_rotations_about_z(const Plato::SampleProbabilityPairs& aMyZaxisSampleProbPairs,
                                         std::vector<Plato::RandomRotations> & aMyRandomRotations)
{
    for(size_t tIndexK = 0; tIndexK < aMyZaxisSampleProbPairs.mSamples.size(); tIndexK++)
    {
        Plato::RandomRotations tMyRandomRotations;
        tMyRandomRotations.mProbability = aMyZaxisSampleProbPairs.mProbabilities[tIndexK];
        tMyRandomRotations.mRotations.mZ = aMyZaxisSampleProbPairs.mSamples[tIndexK];
        aMyRandomRotations.push_back(tMyRandomRotations);
    }

    return (true);
}

inline bool expand_random_rotations(const Plato::SampleProbabilityPairs& aMyXaxisSampleProbPairs,
                                    const Plato::SampleProbabilityPairs& aMyYaxisSampleProbPairs,
                                    const Plato::SampleProbabilityPairs& aMyZaxisSampleProbPairs,
                                    std::vector<Plato::RandomRotations> & aMyRandomRotations)
{
    aMyRandomRotations.clear();

    const bool tRotateAboutX = !aMyXaxisSampleProbPairs.mSamples.empty();
    const bool tRotateAboutY = !aMyYaxisSampleProbPairs.mSamples.empty();
    const bool tRotateAboutZ = !aMyZaxisSampleProbPairs.mSamples.empty();

    if(tRotateAboutX && tRotateAboutY && tRotateAboutZ)
    {
        Plato::set_random_rotations_about_xyz(aMyXaxisSampleProbPairs, aMyYaxisSampleProbPairs, aMyZaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(tRotateAboutX && tRotateAboutY && !tRotateAboutZ)
    {
        Plato::set_random_rotations_about_xy(aMyXaxisSampleProbPairs, aMyYaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(tRotateAboutX && !tRotateAboutY && tRotateAboutZ)
    {
        Plato::set_random_rotations_about_xz(aMyXaxisSampleProbPairs, aMyZaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(!tRotateAboutX && tRotateAboutY && tRotateAboutZ)
    {
        Plato::set_random_rotations_about_yz(aMyYaxisSampleProbPairs, aMyZaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(tRotateAboutX && !tRotateAboutY && !tRotateAboutZ)
    {
        Plato::set_random_rotations_about_x(aMyXaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(!tRotateAboutX && tRotateAboutY && !tRotateAboutZ)
    {
        Plato::set_random_rotations_about_y(aMyYaxisSampleProbPairs, aMyRandomRotations);
    }
    else if(!tRotateAboutX && !tRotateAboutY && tRotateAboutZ)
    {
        Plato::set_random_rotations_about_z(aMyZaxisSampleProbPairs, aMyRandomRotations);
    }
    else
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: INPUT SET OF SAMPLE PROBABILITY PAIRS ARE EMPTY. LOAD VECTOR IS DETERMINISTIC.\n";
        return (false);
    }

    return (true);
}

inline bool check_expand_random_loads_inputs(const Plato::Vector3D & aMyOriginalLoad,
                                             const std::vector<Plato::RandomRotations> & aMyRandomRotations)
{
    if(aMyRandomRotations.empty())
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: VECTOR OF RANDOM ROTATIONS IS EMPTY.\n";
        return (false);
    }
    else if(Plato::check_vector3d_values(aMyOriginalLoad) == false)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: A NON-FINITE NUMBER WAS DETECTED IN VECTOR 3D.\n";
        return (false);
    }

    return (true);
}

inline bool expand_random_loads(const Plato::Vector3D & aMyOriginalLoad,
                                const std::vector<Plato::RandomRotations> & aMyRandomRotations,
                                Plato::UniqueCounter & aUniqueLoadCounter,
                                std::vector<Plato::RandomLoad> & aMyRandomLoads)
{
    if(Plato::check_expand_random_loads_inputs(aMyOriginalLoad, aMyRandomRotations) == false)
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: EXPAND RANDOM LOADS INPUTS ARE NOT VALID.\n";
        return (false);
    }

    const size_t tNumRandomLoads = aMyRandomRotations.size();
    for(size_t tIndex = 0; tIndex < tNumRandomLoads; tIndex++)
    {
        Plato::RandomLoad tMyRandomLoad;
        tMyRandomLoad.mLoad = aMyOriginalLoad;
        tMyRandomLoad.mProbability = aMyRandomRotations[tIndex].mProbability;
        if(Plato::apply_rotation_matrix(aMyRandomRotations[tIndex].mRotations, tMyRandomLoad.mLoad) == false)
        {
            std::cout<< "\nFILE: " << __FILE__
                     << "\nFUNCTION: " << __PRETTY_FUNCTION__
                     << "\nLINE:" << __LINE__
                     << "\nMESSAGE: APPLICATION OF ROTATION MATRIX WAS UNSUCCESSFUL.\n";
            return (false);
        }
        tMyRandomLoad.mLoadID = aUniqueLoadCounter.assignNextUnique();
        aMyRandomLoads.push_back(tMyRandomLoad);
    }

    return (true);
}

inline bool update_initial_random_load_case(const std::vector<Plato::RandomLoad> & aNewRandomLoads,
                                            std::vector<Plato::RandomLoadCase> & aOldRandomLoadCases,
                                            Plato::UniqueCounter & aUniqueLoadCounter)
{
    std::vector<Plato::RandomLoadCase> tNewSetRandomLoadCase;

    for(size_t tLoadIndex = 0; tLoadIndex < aNewRandomLoads.size(); tLoadIndex++)
    {
        Plato::RandomLoadCase tNewRandomLoadCase;
        const Plato::RandomLoad& tMyNewRandomLoad = aNewRandomLoads[tLoadIndex];
        tNewRandomLoadCase.mLoads.push_back(tMyNewRandomLoad);
        tNewRandomLoadCase.mProbability = tMyNewRandomLoad.mProbability;
        tNewRandomLoadCase.mLoadCaseID = aUniqueLoadCounter.assignNextUnique();
        tNewSetRandomLoadCase.push_back(tNewRandomLoadCase);
    } // index over new random loads

    aOldRandomLoadCases = tNewSetRandomLoadCase;

    return (true);
}

inline bool update_random_load_cases(const std::vector<Plato::RandomLoad> & aNewRandomLoads,
                                     std::vector<Plato::RandomLoadCase> & aOldRandomLoadCases,
                                     Plato::UniqueCounter & aUniqueLoadCounter)
{
    std::vector<Plato::RandomLoadCase> tNewSetRandomLoadCase;

    for(size_t tNewLoadIndex = 0; tNewLoadIndex < aNewRandomLoads.size(); tNewLoadIndex++)
    {
        const Plato::RandomLoad& tMyNewRandomLoad = aNewRandomLoads[tNewLoadIndex];
        std::vector<Plato::RandomLoadCase> tTempRandomLoadCases = aOldRandomLoadCases;
        for(size_t tOldLoadCaseIndex = 0; tOldLoadCaseIndex < aOldRandomLoadCases.size(); tOldLoadCaseIndex++)
        {
            tTempRandomLoadCases[tOldLoadCaseIndex].mLoads.push_back(tMyNewRandomLoad);
            Plato::RandomLoadCase& tMyTempRandomLoadCase = tTempRandomLoadCases[tOldLoadCaseIndex];
            tMyTempRandomLoadCase.mLoadCaseID = aUniqueLoadCounter.assignNextUnique();
            tMyTempRandomLoadCase.mProbability = tMyTempRandomLoadCase.mProbability * tMyNewRandomLoad.mProbability;
            tNewSetRandomLoadCase.push_back(tMyTempRandomLoadCase);
        } // index over old random load cases
    } // index over new random loads

    aOldRandomLoadCases = tNewSetRandomLoadCase;

    return (true);
}

inline bool expand_random_load_cases(const std::vector<Plato::RandomLoad> & aNewRandomLoads,
                                     std::vector<Plato::RandomLoadCase> & aOldRandomLoadCases,
                                     Plato::UniqueCounter & aUniqueLoadCounter)
{
    if(aNewRandomLoads.empty())
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: NEW VECTOR OF RANDOM LOADS IS EMPTY.\n";
        return (false);
    } // if-else statement
    else if(!aNewRandomLoads.empty() && aOldRandomLoadCases.empty())
    {
        std::cout<< "\nFILE: " << __FILE__
                 << "\nFUNCTION: " << __PRETTY_FUNCTION__
                 << "\nLINE:" << __LINE__
                 << "\nMESSAGE: NEW VECTOR OF RANDOM LOADS IS NOT EMPTY. "
                 << "HOWEVER, THE PREVIOUS SET OF LOAD CASES IS EMPTY. "
                 << "THIS USE CASE IS NOT VALID.\n";
        return (false);
    } // if-else statement

    if(aOldRandomLoadCases.empty())
    {
        if(Plato::update_initial_random_load_case(aNewRandomLoads, aOldRandomLoadCases, aUniqueLoadCounter) == false)
        {
            std::cout<< "\nFILE: " << __FILE__
                     << "\nFUNCTION: " << __PRETTY_FUNCTION__
                     << "\nLINE:" << __LINE__
                     << "\nMESSAGE: INITIAL RANDOM LOAD CASE WAS NOT UPDATED.\n";
            return (false);
        } // if statement
    } // if-else statement
    else
    {
        if(Plato::update_random_load_cases(aNewRandomLoads, aOldRandomLoadCases, aUniqueLoadCounter) == false)
        {
            std::cout<< "\nFILE: " << __FILE__
                     << "\nFUNCTION: " << __PRETTY_FUNCTION__
                     << "\nLINE:" << __LINE__
                     << "\nMESSAGE: NEW SET OF RANDOM LOAD CASES WAS NOT UPDATED.\n";
            return (false);
        } // if statement
    } // if-else statement

    return (true);
}


}

namespace PlatoUncertainLoadGeneratorXMLTest
{

TEST(PlatoTest, check_input_mean)
{
    Plato::RandomVariable tMyRandomVar;
    ASSERT_FALSE(Plato::check_input_mean(tMyRandomVar));

    tMyRandomVar.mMean = "5";
    ASSERT_TRUE(Plato::check_input_mean(tMyRandomVar));
}

TEST(PlatoTest, check_input_lower_bound)
{
    Plato::RandomVariable tMyRandomVar;
    ASSERT_FALSE(Plato::check_input_lower_bound(tMyRandomVar));

    tMyRandomVar.mLowerBound = "5";
    ASSERT_TRUE(Plato::check_input_lower_bound(tMyRandomVar));
}

TEST(PlatoTest, check_input_upper_bound)
{
    Plato::RandomVariable tMyRandomVar;
    ASSERT_FALSE(Plato::check_input_upper_bound(tMyRandomVar));

    tMyRandomVar.mUpperBound = "5";
    ASSERT_TRUE(Plato::check_input_upper_bound(tMyRandomVar));
}

TEST(PlatoTest, check_input_standard_deviation)
{
    Plato::RandomVariable tMyRandomVar;
    ASSERT_FALSE(Plato::check_input_standard_deviation(tMyRandomVar));

    tMyRandomVar.mStandardDeviation = "5";
    ASSERT_TRUE(Plato::check_input_standard_deviation(tMyRandomVar));
}

TEST(PlatoTest, check_input_number_samples)
{
    Plato::RandomVariable tMyRandomVar;
    ASSERT_FALSE(Plato::check_input_number_samples(tMyRandomVar));

    tMyRandomVar.mNumSamples = "-1";
    ASSERT_FALSE(Plato::check_input_number_samples(tMyRandomVar));

    tMyRandomVar.mNumSamples = "0";
    ASSERT_FALSE(Plato::check_input_number_samples(tMyRandomVar));

    tMyRandomVar.mNumSamples = "2";
    ASSERT_TRUE(Plato::check_input_number_samples(tMyRandomVar));
}

TEST(PlatoTest, check_input_statistics_uniform)
{
    // DEFINE INPUTS
    Plato::RandomVariable tMyRandomVar;
    tMyRandomVar.mType = "random rotation";
    tMyRandomVar.mSubType = "x";
    tMyRandomVar.mGlobalID = "1";
    tMyRandomVar.mDistribution = "uniform";

    // TEST UNDEFINED UPPER BOUND
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST UNDEFINED LOWER BOUND
    tMyRandomVar.mLowerBound = "";
    tMyRandomVar.mUpperBound = "135";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST UNDEFINED NUMBER OF SAMPLES
    tMyRandomVar.mNumSamples = "";
    tMyRandomVar.mLowerBound = "65";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST NUMBER OF SAMPLES = 0
    tMyRandomVar.mNumSamples = "0";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST NEGATIVE NUMBER OF SAMPLES
    tMyRandomVar.mNumSamples = "-1";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST SUCCESS
    tMyRandomVar.mNumSamples = "4";
    ASSERT_TRUE(Plato::check_input_statistics(tMyRandomVar));
}

TEST(PlatoTest, check_input_statistics_normal)
{
    // DEFINE INPUTS
    Plato::RandomVariable tMyRandomVar;
    tMyRandomVar.mType = "random rotation";
    tMyRandomVar.mSubType = "x";
    tMyRandomVar.mGlobalID = "1";
    tMyRandomVar.mNumSamples = "4";
    tMyRandomVar.mLowerBound = "65";
    tMyRandomVar.mUpperBound = "95";
    tMyRandomVar.mStandardDeviation = "5";
    tMyRandomVar.mDistribution = "normal";

    // TEST UNDEFINED MEAN
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST UNDEFINED UPPER BOUND
    tMyRandomVar.mMean = "80";
    tMyRandomVar.mUpperBound = "";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST UNDEFINED LOWER BOUND
    tMyRandomVar.mLowerBound = "";
    tMyRandomVar.mUpperBound = "95";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST UNDEFINED NUMBER OF SAMPLES
    tMyRandomVar.mNumSamples = "";
    tMyRandomVar.mLowerBound = "65";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST NUMBER OF SAMPLES = 0
    tMyRandomVar.mNumSamples = "0";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST NEGATIVE NUMBER OF SAMPLES
    tMyRandomVar.mNumSamples = "-1";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST UNDEFINED STANDARD DEVIATION
    tMyRandomVar.mNumSamples = "4";
    tMyRandomVar.mStandardDeviation = "";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST SUCCESS
    tMyRandomVar.mStandardDeviation = "5";
    ASSERT_TRUE(Plato::check_input_statistics(tMyRandomVar));
}

TEST(PlatoTest, check_input_statistics_beta)
{
    // DEFINE INPUTS
    Plato::RandomVariable tMyRandomVar;
    tMyRandomVar.mType = "random rotation";
    tMyRandomVar.mSubType = "x";
    tMyRandomVar.mGlobalID = "1";
    tMyRandomVar.mNumSamples = "4";
    tMyRandomVar.mLowerBound = "65";
    tMyRandomVar.mUpperBound = "95";
    tMyRandomVar.mStandardDeviation = "5";
    tMyRandomVar.mDistribution = "beta";

    // TEST UNDEFINED MEAN
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST UNDEFINED UPPER BOUND
    tMyRandomVar.mMean = "80";
    tMyRandomVar.mUpperBound = "";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST UNDEFINED LOWER BOUND
    tMyRandomVar.mLowerBound = "";
    tMyRandomVar.mUpperBound = "95";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST UNDEFINED NUMBER OF SAMPLES
    tMyRandomVar.mNumSamples = "";
    tMyRandomVar.mLowerBound = "65";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST NUMBER OF SAMPLES = 0
    tMyRandomVar.mNumSamples = "0";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST NEGATIVE NUMBER OF SAMPLES
    tMyRandomVar.mNumSamples = "-1";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST UNDEFINED STANDARD DEVIATION
    tMyRandomVar.mNumSamples = "4";
    tMyRandomVar.mStandardDeviation = "";
    ASSERT_FALSE(Plato::check_input_statistics(tMyRandomVar));

    // TEST SUCCESS
    tMyRandomVar.mStandardDeviation = "5";
    ASSERT_TRUE(Plato::check_input_statistics(tMyRandomVar));
}

TEST(PlatoTest, define_input_statistics)
{
    // DEFINE INPUTS
    Plato::RandomVariable tMyRandomVar;
    tMyRandomVar.mType = "random rotation";
    tMyRandomVar.mSubType = "x";
    tMyRandomVar.mGlobalID = "1";
    tMyRandomVar.mNumSamples = "4";
    tMyRandomVar.mLowerBound = "65";
    tMyRandomVar.mUpperBound = "95";
    tMyRandomVar.mStandardDeviation = "5";
    tMyRandomVar.mDistribution = "beta";

    // TEST UNDEFINED PARAMETER
    Plato::SromInputs<double> tSromInputs;
    ASSERT_FALSE(Plato::define_input_statistics(tMyRandomVar, tSromInputs));

    // TEST SUCCESS
    tMyRandomVar.mMean = "80";
    ASSERT_TRUE(Plato::define_input_statistics(tMyRandomVar, tSromInputs));

    ASSERT_EQ(4u, tSromInputs.mNumSamples);
    ASSERT_EQ(Plato::DistrubtionName::beta, tSromInputs.mDistribution);

    const double tTolerance = 1e-4;
    ASSERT_NEAR(80.0, tSromInputs.mMean, tTolerance);
    ASSERT_NEAR(25.0, tSromInputs.mVariance, tTolerance);
    ASSERT_NEAR(65.0, tSromInputs.mLowerBound, tTolerance);
    ASSERT_NEAR(95.0, tSromInputs.mUpperBound, tTolerance);
}

TEST(PlatoTest, define_distribution)
{
    Plato::RandomVariable tMyRandomVar;
    Plato::SromInputs<double> tSromInputs;

    tMyRandomVar.mDistribution = "normal";
    ASSERT_TRUE(Plato::define_distribution(tMyRandomVar, tSromInputs));
    ASSERT_EQ(Plato::DistrubtionName::normal, tSromInputs.mDistribution);

    tMyRandomVar.mDistribution = "beta";
    ASSERT_TRUE(Plato::define_distribution(tMyRandomVar, tSromInputs));
    ASSERT_EQ(Plato::DistrubtionName::beta, tSromInputs.mDistribution);

    tMyRandomVar.mDistribution = "uniform";
    ASSERT_TRUE(Plato::define_distribution(tMyRandomVar, tSromInputs));
    ASSERT_EQ(Plato::DistrubtionName::uniform, tSromInputs.mDistribution);

    tMyRandomVar.mDistribution = "lognormal";
    ASSERT_FALSE(Plato::define_distribution(tMyRandomVar, tSromInputs));
}

TEST(PlatoTest, compute_uniform_random_variable_statistics)
{
    Plato::SromInputs<double> tSromInputs;
    tSromInputs.mNumSamples = 4;
    tSromInputs.mLowerBound = 10.0;
    tSromInputs.mUpperBound = 20.0;
    tSromInputs.mDistribution = Plato::DistrubtionName::uniform;
    std::vector<Plato::SromOutputs<double>> tSromOutputs;
    ASSERT_TRUE(Plato::compute_uniform_random_variable_statistics(tSromInputs, tSromOutputs));

    // TEST RESULTS
    double tSum = 0;
    double tTolerance = 1e-4;
    std::vector<double> tGoldSamples = {10, 13.333333333333, 16.666666666667, 20.0};
    std::vector<double> tGoldProbabilities(tSromInputs.mNumSamples, 0.25);
    for(size_t tIndex = 0; tIndex < tSromInputs.mNumSamples; tIndex++)
    {
        tSum += tSromOutputs[tIndex].mSampleWeight;
        ASSERT_NEAR(tGoldSamples[tIndex], tSromOutputs[tIndex].mSampleValue, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tIndex], tSromOutputs[tIndex].mSampleWeight, tTolerance);
    }
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, compute_random_variable_statistics_error)
{
    Plato::SromInputs<double> tSromInputs;
    tSromInputs.mNumSamples = 4;
    tSromInputs.mLowerBound = 10.0;
    tSromInputs.mUpperBound = 20.0;
    tSromInputs.mDistribution = Plato::DistrubtionName::undefined;
    std::vector<Plato::SromOutputs<double>> tSromOutputs;
    ASSERT_FALSE(Plato::compute_random_variable_statistics(tSromInputs, tSromOutputs));
}

TEST(PlatoTest, compute_random_variable_statistics)
{
    Plato::SromInputs<double> tSromInputs;
    tSromInputs.mNumSamples = 4;
    tSromInputs.mLowerBound = 10.0;
    tSromInputs.mUpperBound = 20.0;
    tSromInputs.mDistribution = Plato::DistrubtionName::uniform;
    std::vector<Plato::SromOutputs<double>> tSromOutputs;
    ASSERT_TRUE(Plato::compute_random_variable_statistics(tSromInputs, tSromOutputs));

    // TEST RESULTS
    double tSum = 0;
    double tTolerance = 1e-4;
    std::vector<double> tGoldSamples = {10, 13.333333333333, 16.666666666667, 20.0};
    std::vector<double> tGoldProbabilities(tSromInputs.mNumSamples, 0.25);
    for(size_t tIndex = 0; tIndex < tSromInputs.mNumSamples; tIndex++)
    {
        tSum += tSromOutputs[tIndex].mSampleWeight;
        ASSERT_NEAR(tGoldSamples[tIndex], tSromOutputs[tIndex].mSampleValue, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tIndex], tSromOutputs[tIndex].mSampleWeight, tTolerance);
    }
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, compute_sample_probability_pairs_OneRandVar)
{
    // DEFINE INPUTS
    Plato::RandomVariable tMyRandomVar;
    tMyRandomVar.mType = "random rotation";
    tMyRandomVar.mSubType = "x";
    tMyRandomVar.mGlobalID = "1";
    tMyRandomVar.mDistribution = "beta";
    tMyRandomVar.mMean = "85";
    tMyRandomVar.mUpperBound = "65";
    tMyRandomVar.mLowerBound = "135";
    tMyRandomVar.mStandardDeviation = "15";
    tMyRandomVar.mNumSamples = "3";
    std::vector<Plato::RandomVariable> tRandomVarsSet;
    tRandomVarsSet.push_back(tMyRandomVar);

    // SOLVE SROM PROBLEM
    std::vector<Plato::SromRandomVariable> tMySampleProbPairs;
    Plato::compute_sample_probability_pairs(tRandomVarsSet, tMySampleProbPairs);

    // TEST RESULTS
    ASSERT_EQ(1u, tMySampleProbPairs.size());
    ASSERT_STREQ("x", tMySampleProbPairs[0].mSubType.c_str());
    ASSERT_STREQ("random rotation", tMySampleProbPairs[0].mType.c_str());
    ASSERT_EQ(3u, tMySampleProbPairs[0].mSampleProbPair.mNumSamples);
    ASSERT_EQ(3u, tMySampleProbPairs[0].mSampleProbPair.mSamples.size());
    ASSERT_EQ(3u, tMySampleProbPairs[0].mSampleProbPair.mProbabilities.size());

    // TEST RESULTS
    double tSum = 0;
    double tTolerance = 1e-4;
    const Plato::SampleProbabilityPairs& tSampleProbabilityPairs = tMySampleProbPairs[0].mSampleProbPair;
    std::vector<double> tGoldSamples = {102.6401761033302, 88.31771931721183, 76.83726384445947};
    std::vector<double> tGoldProbabilities = {0.18178392025984, 0.19071306890276, 0.62750198867198};
    for(size_t tIndex = 0; tIndex < tSampleProbabilityPairs.mNumSamples; tIndex++)
    {
        tSum += tSampleProbabilityPairs.mProbabilities[tIndex];
        ASSERT_NEAR(tGoldSamples[tIndex], tSampleProbabilityPairs.mSamples[tIndex], tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tIndex], tSampleProbabilityPairs.mProbabilities[tIndex], tTolerance);
    }
    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, compute_sample_probability_pairs_TwoRandVars)
{
    std::vector<Plato::RandomVariable> tRandomVarsSet;

    // DEFINE INPUTS
    Plato::RandomVariable tMyRandomVarOne;
    tMyRandomVarOne.mType = "random rotation";
    tMyRandomVarOne.mSubType = "x";
    tMyRandomVarOne.mGlobalID = "1";
    tMyRandomVarOne.mDistribution = "beta";
    tMyRandomVarOne.mMean = "85";
    tMyRandomVarOne.mLowerBound = "65";
    tMyRandomVarOne.mUpperBound = "135";
    tMyRandomVarOne.mStandardDeviation = "15";
    tMyRandomVarOne.mNumSamples = "3";
    tRandomVarsSet.push_back(tMyRandomVarOne);

    Plato::RandomVariable tMyRandomVarTwo;
    tMyRandomVarTwo.mType = "random rotation";
    tMyRandomVarTwo.mSubType = "y";
    tMyRandomVarTwo.mGlobalID = "1";
    tMyRandomVarTwo.mNumSamples = "4";
    tMyRandomVarTwo.mDistribution = "beta";
    tMyRandomVarTwo.mMean = "70";
    tMyRandomVarTwo.mLowerBound = "50";
    tMyRandomVarTwo.mUpperBound = "120";
    tMyRandomVarTwo.mStandardDeviation = "15";
    tRandomVarsSet.push_back(tMyRandomVarTwo);

    // SOLVE SROM PROBLEM
    std::vector<Plato::SromRandomVariable> tMySampleProbPairs;
    Plato::compute_sample_probability_pairs(tRandomVarsSet, tMySampleProbPairs);

    // TEST RESULTS - RANDOM VARIABLE ONE
    ASSERT_EQ(2u, tMySampleProbPairs.size());
    ASSERT_STREQ("x", tMySampleProbPairs[0].mSubType.c_str());
    ASSERT_STREQ("random rotation", tMySampleProbPairs[0].mType.c_str());
    ASSERT_EQ(3u, tMySampleProbPairs[0].mSampleProbPair.mNumSamples);
    ASSERT_EQ(3u, tMySampleProbPairs[0].mSampleProbPair.mSamples.size());
    ASSERT_EQ(3u, tMySampleProbPairs[0].mSampleProbPair.mProbabilities.size());

    double tSum = 0;
    double tTolerance = 1e-4;
    const Plato::SampleProbabilityPairs& tSampleProbabilityPairsOne = tMySampleProbPairs[0].mSampleProbPair;
    std::vector<double> tGoldSamplesOne = {79.56461506624, 95.1780010696, 104.3742043151};
    std::vector<double> tGoldProbabilitiesOne = {0.441549282785, 0.3256625620299, 0.2326524892665};
    for(size_t tIndex = 0; tIndex < tSampleProbabilityPairsOne.mNumSamples; tIndex++)
    {
        tSum += tSampleProbabilityPairsOne.mProbabilities[tIndex];
        ASSERT_NEAR(tGoldSamplesOne[tIndex], tSampleProbabilityPairsOne.mSamples[tIndex], tTolerance);
        ASSERT_NEAR(tGoldProbabilitiesOne[tIndex], tSampleProbabilityPairsOne.mProbabilities[tIndex], tTolerance);
    }
    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);

    // TEST RESULTS - RANDOM VARIABLE TWO
    ASSERT_STREQ("y", tMySampleProbPairs[1].mSubType.c_str());
    ASSERT_STREQ("random rotation", tMySampleProbPairs[1].mType.c_str());
    ASSERT_EQ(4u, tMySampleProbPairs[1].mSampleProbPair.mNumSamples);
    ASSERT_EQ(4u, tMySampleProbPairs[1].mSampleProbPair.mSamples.size());
    ASSERT_EQ(4u, tMySampleProbPairs[1].mSampleProbPair.mProbabilities.size());

    tSum = 0;
    tTolerance = 1e-4;
    const Plato::SampleProbabilityPairs& tSampleProbabilityPairsTwo = tMySampleProbPairs[1].mSampleProbPair;
    std::vector<double> tGoldSamplesTwo = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    std::vector<double> tGoldProbabilitiesTwo = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};
    for(size_t tIndex = 0; tIndex < tSampleProbabilityPairsTwo.mNumSamples; tIndex++)
    {
        tSum += tSampleProbabilityPairsTwo.mProbabilities[tIndex];
        ASSERT_NEAR(tGoldSamplesTwo[tIndex], tSampleProbabilityPairsTwo.mSamples[tIndex], tTolerance);
        ASSERT_NEAR(tGoldProbabilitiesTwo[tIndex], tSampleProbabilityPairsTwo.mProbabilities[tIndex], tTolerance);
    }
    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, compute_sample_probability_pairs_error_undefined_distribution)
{
    // DEFINE INPUTS
    Plato::RandomVariable tMyRandomVar;
    tMyRandomVar.mType = "random rotation";
    tMyRandomVar.mSubType = "x";
    tMyRandomVar.mGlobalID = "1";
    tMyRandomVar.mDistribution = "";
    tMyRandomVar.mMean = "85";
    tMyRandomVar.mUpperBound = "135";
    tMyRandomVar.mLowerBound = "65";
    tMyRandomVar.mStandardDeviation = "15";
    tMyRandomVar.mNumSamples = "3";
    std::vector<Plato::RandomVariable> tRandomVarsSet;
    tRandomVarsSet.push_back(tMyRandomVar);

    // SOLVE SROM PROBLEM
    std::vector<Plato::SromRandomVariable> tMySampleProbPairs;
    ASSERT_FALSE(Plato::compute_sample_probability_pairs(tRandomVarsSet, tMySampleProbPairs));
}

TEST(PlatoTest, compute_sample_probability_pairs_error_empty_random_var_set)
{
    // SOLVE SROM PROBLEM
    std::vector<Plato::RandomVariable> tRandomVarsSet;
    std::vector<Plato::SromRandomVariable> tMySampleProbPairs;
    ASSERT_FALSE(Plato::compute_sample_probability_pairs(tRandomVarsSet, tMySampleProbPairs));
}

TEST(PlatoTest, compute_sample_probability_pairs_error_undefined_input_statistics_uniform)
{
    // DEFINE INPUTS
    Plato::RandomVariable tMyRandomVar;
    tMyRandomVar.mType = "random rotation";
    tMyRandomVar.mSubType = "x";
    tMyRandomVar.mGlobalID = "1";
    tMyRandomVar.mDistribution = "uniform";
    tMyRandomVar.mLowerBound = "65";
    tMyRandomVar.mUpperBound = "";
    tMyRandomVar.mNumSamples = "3";
    std::vector<Plato::RandomVariable> tRandomVarsSet;
    tRandomVarsSet.push_back(tMyRandomVar);

    // TEST UNDEFINED UPPER BOUND
    std::vector<Plato::SromRandomVariable> tMySampleProbPairs;
    ASSERT_FALSE(Plato::compute_sample_probability_pairs(tRandomVarsSet, tMySampleProbPairs));

    // TEST UNDEFINED LOWER BOUND
    tRandomVarsSet[0].mLowerBound = "";
    tRandomVarsSet[0].mUpperBound = "135";
    ASSERT_FALSE(Plato::compute_sample_probability_pairs(tRandomVarsSet, tMySampleProbPairs));

    // TEST UNDEFINED NUMBER OF SAMPLES
    tRandomVarsSet[0].mNumSamples = "";
    tRandomVarsSet[0].mLowerBound = "65";
    ASSERT_FALSE(Plato::compute_sample_probability_pairs(tRandomVarsSet, tMySampleProbPairs));

    // TEST NUMBER OF SAMPLES = 0
    tRandomVarsSet[0].mNumSamples = "0";
    tRandomVarsSet[0].mLowerBound = "65";
    ASSERT_FALSE(Plato::compute_sample_probability_pairs(tRandomVarsSet, tMySampleProbPairs));

    // TEST NEGATIVE NUMBER OF SAMPLES
    tRandomVarsSet[0].mNumSamples = "-1";
    tRandomVarsSet[0].mLowerBound = "65";
    ASSERT_FALSE(Plato::compute_sample_probability_pairs(tRandomVarsSet, tMySampleProbPairs));
}

TEST(PlatoTest, post_process_random_load_error)
{
    // ERROR: ZERO INPUTS PROVIDED
    std::vector<Plato::SromRandomVariable> tSromRandomVariableSet;
    std::map<Plato::axis3D::axis3D, Plato::SampleProbabilityPairs> tRotationAxisToSampleProbPairs;
    ASSERT_FALSE(Plato::post_process_random_load(tSromRandomVariableSet, tRotationAxisToSampleProbPairs));

    // ERROR: INPUT RANDOM ROTATIONS ARE NOT UNIQUE
    Plato::SromRandomVariable tRandomLoadX;
    tRandomLoadX.mType = "random rotation";
    tRandomLoadX.mSubType = "x";
    tRandomLoadX.mSampleProbPair.mNumSamples = 4;
    tRandomLoadX.mSampleProbPair.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tRandomLoadX.mSampleProbPair.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    Plato::SromRandomVariable tRandomLoadY;
    tRandomLoadY.mType = "random rotation";
    tRandomLoadY.mSubType = "x"; // ERROR - OVERWRITES ORIGINAL INPUT
    tRandomLoadY.mSampleProbPair.mNumSamples = 3;
    tRandomLoadY.mSampleProbPair.mSamples = {79.56461506624, 95.1780010696, 104.3742043151};
    tRandomLoadY.mSampleProbPair.mProbabilities = {0.441549282785, 0.3256625620299, 0.2326524892665};

    tSromRandomVariableSet.push_back(tRandomLoadX);
    tSromRandomVariableSet.push_back(tRandomLoadY);
    ASSERT_FALSE(Plato::post_process_random_load(tSromRandomVariableSet, tRotationAxisToSampleProbPairs));
}

TEST(PlatoTest, post_process_random_load_OneRandomRotation)
{
    Plato::SromRandomVariable tRandomLoadX;
    tRandomLoadX.mType = "random rotation";
    tRandomLoadX.mSubType = "x";
    tRandomLoadX.mSampleProbPair.mNumSamples = 4;
    tRandomLoadX.mSampleProbPair.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tRandomLoadX.mSampleProbPair.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};
    std::vector<Plato::SromRandomVariable> tSromRandomVariableSet;
    tSromRandomVariableSet.push_back(tRandomLoadX);

    std::map<Plato::axis3D::axis3D, Plato::SampleProbabilityPairs> tRotationAxisToSampleProbPairs;
    ASSERT_TRUE(Plato::post_process_random_load(tSromRandomVariableSet, tRotationAxisToSampleProbPairs));

    // TEST RESULTS
    ASSERT_EQ(1u, tRotationAxisToSampleProbPairs.size());
    ASSERT_FALSE(tRotationAxisToSampleProbPairs.find(Plato::axis3D::x) == tRotationAxisToSampleProbPairs.end());
    ASSERT_TRUE(tRotationAxisToSampleProbPairs.find(Plato::axis3D::y) == tRotationAxisToSampleProbPairs.end());
    ASSERT_TRUE(tRotationAxisToSampleProbPairs.find(Plato::axis3D::z) == tRotationAxisToSampleProbPairs.end());

    const Plato::SampleProbabilityPairs& tSampleProbabilityPairs = tRotationAxisToSampleProbPairs.find(Plato::axis3D::x)->second;
    ASSERT_EQ(4u, tSampleProbabilityPairs.mNumSamples);

    const double tTolerance = 1e-4;
    for(size_t tIndex = 0; tIndex < tSampleProbabilityPairs.mNumSamples; tIndex++)
    {
        ASSERT_NEAR(tRandomLoadX.mSampleProbPair.mSamples[tIndex], tSampleProbabilityPairs.mSamples[tIndex], tTolerance);
        ASSERT_NEAR(tRandomLoadX.mSampleProbPair.mProbabilities[tIndex], tSampleProbabilityPairs.mProbabilities[tIndex], tTolerance);
    }
}

TEST(PlatoTest, post_process_random_load_TwoRandomRotations)
{
    Plato::SromRandomVariable tRandomLoadX;
    tRandomLoadX.mType = "random rotation";
    tRandomLoadX.mSubType = "x";
    tRandomLoadX.mSampleProbPair.mNumSamples = 4;
    tRandomLoadX.mSampleProbPair.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tRandomLoadX.mSampleProbPair.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    Plato::SromRandomVariable tRandomLoadY;
    tRandomLoadY.mType = "random rotation";
    tRandomLoadY.mSubType = "y";
    tRandomLoadY.mSampleProbPair.mNumSamples = 3;
    tRandomLoadY.mSampleProbPair.mSamples = {79.56461506624, 95.1780010696, 104.3742043151};
    tRandomLoadY.mSampleProbPair.mProbabilities = {0.441549282785, 0.3256625620299, 0.2326524892665};

    std::vector<Plato::SromRandomVariable> tSromRandomVariableSet;
    tSromRandomVariableSet.push_back(tRandomLoadX);
    tSromRandomVariableSet.push_back(tRandomLoadY);

    std::map<Plato::axis3D::axis3D, Plato::SampleProbabilityPairs> tRotationAxisToSampleProbPairs;
    ASSERT_TRUE(Plato::post_process_random_load(tSromRandomVariableSet, tRotationAxisToSampleProbPairs));

    // TEST RESULTS
    ASSERT_EQ(2u, tRotationAxisToSampleProbPairs.size());
    ASSERT_FALSE(tRotationAxisToSampleProbPairs.find(Plato::axis3D::x) == tRotationAxisToSampleProbPairs.end());
    ASSERT_FALSE(tRotationAxisToSampleProbPairs.find(Plato::axis3D::y) == tRotationAxisToSampleProbPairs.end());
    ASSERT_TRUE(tRotationAxisToSampleProbPairs.find(Plato::axis3D::z) == tRotationAxisToSampleProbPairs.end());

    // TEST RESULTS FOR ROTATION X SAMPLE-PROBABILITY PAIRS
    const double tTolerance = 1e-4;

    const Plato::SampleProbabilityPairs& tSampleProbabilityPairsX = tRotationAxisToSampleProbPairs.find(Plato::axis3D::x)->second;
    ASSERT_EQ(tRandomLoadX.mSampleProbPair.mNumSamples, tSampleProbabilityPairsX.mNumSamples);
    for(size_t tIndex = 0; tIndex < tSampleProbabilityPairsX.mNumSamples; tIndex++)
    {
        ASSERT_NEAR(tRandomLoadX.mSampleProbPair.mSamples[tIndex], tSampleProbabilityPairsX.mSamples[tIndex], tTolerance);
        ASSERT_NEAR(tRandomLoadX.mSampleProbPair.mProbabilities[tIndex], tSampleProbabilityPairsX.mProbabilities[tIndex], tTolerance);
    }

    // TEST RESULTS FOR ROTATION Y SAMPLE-PROBABILITY PAIRS
    const Plato::SampleProbabilityPairs& tSampleProbabilityPairsY = tRotationAxisToSampleProbPairs.find(Plato::axis3D::y)->second;
    ASSERT_EQ(tRandomLoadY.mSampleProbPair.mNumSamples, tSampleProbabilityPairsY.mNumSamples);
    for(size_t tIndex = 0; tIndex < tSampleProbabilityPairsY.mNumSamples; tIndex++)
    {
        ASSERT_NEAR(tRandomLoadY.mSampleProbPair.mSamples[tIndex], tSampleProbabilityPairsY.mSamples[tIndex], tTolerance);
        ASSERT_NEAR(tRandomLoadY.mSampleProbPair.mProbabilities[tIndex], tSampleProbabilityPairsY.mProbabilities[tIndex], tTolerance);
    }
}

TEST(PlatoTest, post_process_random_load_ThreeRandomRotations)
{
    Plato::SromRandomVariable tRandomLoadX;
    tRandomLoadX.mType = "random rotation";
    tRandomLoadX.mSubType = "x";
    tRandomLoadX.mSampleProbPair.mNumSamples = 4;
    tRandomLoadX.mSampleProbPair.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tRandomLoadX.mSampleProbPair.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    Plato::SromRandomVariable tRandomLoadY;
    tRandomLoadY.mType = "random rotation";
    tRandomLoadY.mSubType = "y";
    tRandomLoadY.mSampleProbPair.mNumSamples = 3;
    tRandomLoadY.mSampleProbPair.mSamples = {79.56461506624, 95.1780010696, 104.3742043151};
    tRandomLoadY.mSampleProbPair.mProbabilities = {0.441549282785, 0.3256625620299, 0.2326524892665};

    Plato::SromRandomVariable tRandomLoadZ;
    tRandomLoadZ.mType = "random rotation";
    tRandomLoadZ.mSubType = "z";
    tRandomLoadZ.mSampleProbPair.mNumSamples = 4;
    tRandomLoadZ.mSampleProbPair.mSamples = {10, 13.333333333333, 16.666666666667, 20.0};
    tRandomLoadZ.mSampleProbPair.mProbabilities = {0.25, 0.25, 0.25, 0.25};

    std::vector<Plato::SromRandomVariable> tSromRandomVariableSet;
    tSromRandomVariableSet.push_back(tRandomLoadX);
    tSromRandomVariableSet.push_back(tRandomLoadY);
    tSromRandomVariableSet.push_back(tRandomLoadZ);

    std::map<Plato::axis3D::axis3D, Plato::SampleProbabilityPairs> tRotationAxisToSampleProbPairs;
    ASSERT_TRUE(Plato::post_process_random_load(tSromRandomVariableSet, tRotationAxisToSampleProbPairs));

    // TEST RESULTS
    ASSERT_EQ(3u, tRotationAxisToSampleProbPairs.size());
    ASSERT_FALSE(tRotationAxisToSampleProbPairs.find(Plato::axis3D::x) == tRotationAxisToSampleProbPairs.end());
    ASSERT_FALSE(tRotationAxisToSampleProbPairs.find(Plato::axis3D::y) == tRotationAxisToSampleProbPairs.end());
    ASSERT_FALSE(tRotationAxisToSampleProbPairs.find(Plato::axis3D::z) == tRotationAxisToSampleProbPairs.end());

    // TEST RESULTS FOR ROTATION X SAMPLE-PROBABILITY PAIRS
    const double tTolerance = 1e-4;

    const Plato::SampleProbabilityPairs& tSampleProbabilityPairsX = tRotationAxisToSampleProbPairs.find(Plato::axis3D::x)->second;
    ASSERT_EQ(tRandomLoadX.mSampleProbPair.mNumSamples, tSampleProbabilityPairsX.mNumSamples);
    for(size_t tIndex = 0; tIndex < tSampleProbabilityPairsX.mNumSamples; tIndex++)
    {
        ASSERT_NEAR(tRandomLoadX.mSampleProbPair.mSamples[tIndex], tSampleProbabilityPairsX.mSamples[tIndex], tTolerance);
        ASSERT_NEAR(tRandomLoadX.mSampleProbPair.mProbabilities[tIndex], tSampleProbabilityPairsX.mProbabilities[tIndex], tTolerance);
    }

    // TEST RESULTS FOR ROTATION Y SAMPLE-PROBABILITY PAIRS
    const Plato::SampleProbabilityPairs& tSampleProbabilityPairsY = tRotationAxisToSampleProbPairs.find(Plato::axis3D::y)->second;
    ASSERT_EQ(tRandomLoadY.mSampleProbPair.mNumSamples, tSampleProbabilityPairsY.mNumSamples);
    for(size_t tIndex = 0; tIndex < tSampleProbabilityPairsY.mNumSamples; tIndex++)
    {
        ASSERT_NEAR(tRandomLoadY.mSampleProbPair.mSamples[tIndex], tSampleProbabilityPairsY.mSamples[tIndex], tTolerance);
        ASSERT_NEAR(tRandomLoadY.mSampleProbPair.mProbabilities[tIndex], tSampleProbabilityPairsY.mProbabilities[tIndex], tTolerance);
    }

    // TEST RESULTS FOR ROTATION Z SAMPLE-PROBABILITY PAIRS
    const Plato::SampleProbabilityPairs& tSampleProbabilityPairsZ = tRotationAxisToSampleProbPairs.find(Plato::axis3D::z)->second;
    ASSERT_EQ(tRandomLoadZ.mSampleProbPair.mNumSamples, tSampleProbabilityPairsZ.mNumSamples);
    for(size_t tIndex = 0; tIndex < tSampleProbabilityPairsZ.mNumSamples; tIndex++)
    {
        ASSERT_NEAR(tRandomLoadZ.mSampleProbPair.mSamples[tIndex], tSampleProbabilityPairsZ.mSamples[tIndex], tTolerance);
        ASSERT_NEAR(tRandomLoadZ.mSampleProbPair.mProbabilities[tIndex], tSampleProbabilityPairsZ.mProbabilities[tIndex], tTolerance);
    }
}

TEST(PlatoTest, expand_random_rotations_about_xyz)
{
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    tSampleProbPairSetX.mNumSamples = 4;
    tSampleProbPairSetX.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tSampleProbPairSetX.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    tSampleProbPairSetY.mNumSamples = 3;
    tSampleProbPairSetY.mSamples = {79.56461506624, 95.1780010696, 104.3742043151};
    tSampleProbPairSetY.mProbabilities = {0.441549282785, 0.3256625620299, 0.2326524892665};
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    tSampleProbPairSetZ.mNumSamples = 4;
    tSampleProbPairSetZ.mProbabilities = {0.25, 0.25, 0.25, 0.25};
    tSampleProbPairSetZ.mSamples = {10, 13.333333333333, 16.666666666667, 20.0};

    std::vector<Plato::RandomRotations> tRandomRotationSet;
    ASSERT_TRUE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tRandomRotationSet));

    // TEST NUMBER OF ROTATION VECTORS
    const size_t tNumRotationVectors = tSampleProbPairSetX.mNumSamples * tSampleProbPairSetY.mNumSamples * tSampleProbPairSetZ.mNumSamples;
    ASSERT_EQ(tNumRotationVectors, tRandomRotationSet.size()); // expects 48 rotation vectors
    std::vector<std::vector<double>> tSetGoldRotations =
        { {62.92995363352, 79.56461506624, 10}, {62.92995363352, 79.56461506624, 13.333333333333}, {62.92995363352, 79.56461506624, 16.666666666667}, {62.92995363352, 79.56461506624, 20},
          {62.92995363352,  95.1780010696, 10}, {62.92995363352,  95.1780010696, 13.333333333333}, {62.92995363352,  95.1780010696, 16.666666666667}, {62.92995363352,  95.1780010696, 20},
          {62.92995363352, 104.3742043151, 10}, {62.92995363352, 104.3742043151, 13.333333333333}, {62.92995363352, 104.3742043151, 16.666666666667}, {62.92995363352, 104.3742043151, 20},
          {69.67128118964, 79.56461506624, 10}, {69.67128118964, 79.56461506624, 13.333333333333}, {69.67128118964, 79.56461506624, 16.666666666667}, {69.67128118964, 79.56461506624, 20},
          {69.67128118964,  95.1780010696, 10}, {69.67128118964,  95.1780010696, 13.333333333333}, {69.67128118964,  95.1780010696, 16.666666666667}, {69.67128118964,  95.1780010696, 20},
          {69.67128118964, 104.3742043151, 10}, {69.67128118964, 104.3742043151, 13.333333333333}, {69.67128118964, 104.3742043151, 16.666666666667}, {69.67128118964, 104.3742043151, 20},
          {66.03455388567, 79.56461506624, 10}, {66.03455388567, 79.56461506624, 13.333333333333}, {66.03455388567, 79.56461506624, 16.666666666667}, {66.03455388567, 79.56461506624, 20},
          {66.03455388567,  95.1780010696, 10}, {66.03455388567,  95.1780010696, 13.333333333333}, {66.03455388567,  95.1780010696, 16.666666666667}, {66.03455388567,  95.1780010696, 20},
          {66.03455388567, 104.3742043151, 10}, {66.03455388567, 104.3742043151, 13.333333333333}, {66.03455388567, 104.3742043151, 16.666666666667}, {66.03455388567, 104.3742043151, 20},
          { 96.2527627689, 79.56461506624, 10}, { 96.2527627689, 79.56461506624, 13.333333333333}, { 96.2527627689, 79.56461506624, 16.666666666667}, { 96.2527627689, 79.56461506624, 20},
          { 96.2527627689,  95.1780010696, 10}, { 96.2527627689,  95.1780010696, 13.333333333333}, { 96.2527627689,  95.1780010696, 16.666666666667}, { 96.2527627689,  95.1780010696, 20},
          { 96.2527627689, 104.3742043151, 10}, { 96.2527627689, 104.3742043151, 13.333333333333}, { 96.2527627689, 104.3742043151, 16.666666666667}, { 96.2527627689, 104.3742043151, 20} };

    std::vector<double> tGoldProbabilities = { 0.040214307576243, 0.040214307576243, 0.040214307576243, 0.040214307576243,
                                               0.029659870248084, 0.029659870248084, 0.029659870248084, 0.029659870248084,
                                               0.021188934342120, 0.021188934342120, 0.021188934342120, 0.021188934342120,
                                               0.021685307558066, 0.021685307558066, 0.021685307558066, 0.021685307558066,
                                               0.015993894890335, 0.015993894890335, 0.015993894890335, 0.015993894890335,
                                               0.011425997007791, 0.011425997007791, 0.011425997007791, 0.011425997007791,
                                               0.025396048040355, 0.025396048040355, 0.025396048040355, 0.025396048040355,
                                               0.018730733788290, 0.018730733788290, 0.018730733788290, 0.018730733788290,
                                               0.013381187614786, 0.013381187614786, 0.013381187614786, 0.013381187614786,
                                               0.023089690523626, 0.023089690523626, 0.023089690523626, 0.023089690523626,
                                               0.017029690830826, 0.017029690830826, 0.017029690830826, 0.017029690830826,
                                               0.012165966939936, 0.012165966939936, 0.012165966939936, 0.012165966939936};

    double tSum = 0;
    double tTolerance = 1e-4;
    const size_t tTotalNumSamples = tSetGoldRotations.size();
    for(size_t tSampleIndex = 0; tSampleIndex < tTotalNumSamples; tSampleIndex++)
    {
        const std::vector<double>& tGoldRotations = tSetGoldRotations[tSampleIndex];
        ASSERT_NEAR(tGoldRotations[0], tRandomRotationSet[tSampleIndex].mRotations.mX, tTolerance);
        ASSERT_NEAR(tGoldRotations[1], tRandomRotationSet[tSampleIndex].mRotations.mY, tTolerance);
        ASSERT_NEAR(tGoldRotations[2], tRandomRotationSet[tSampleIndex].mRotations.mZ, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tSampleIndex], tRandomRotationSet[tSampleIndex].mProbability, tTolerance);
        tSum += tRandomRotationSet[tSampleIndex].mProbability;
    }

    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, expand_random_rotations_about_xy)
{
    // SET INPUTS
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    tSampleProbPairSetX.mNumSamples = 4;
    tSampleProbPairSetX.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tSampleProbPairSetX.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    tSampleProbPairSetY.mNumSamples = 3;
    tSampleProbPairSetY.mSamples = {79.56461506624, 95.1780010696, 104.3742043151};
    tSampleProbPairSetY.mProbabilities = {0.441549282785, 0.3256625620299, 0.2326524892665};

    // CALL FUNCTION
    std::vector<Plato::RandomRotations> tRandomRotationSet;
    ASSERT_TRUE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tRandomRotationSet));

    // TEST OUTPUTS
    ASSERT_EQ(12u, tRandomRotationSet.size());
    std::vector<std::vector<double>> tSetGoldRotations =
        { {62.92995363352, 79.56461506624, 0}, {62.92995363352,  95.1780010696, 0}, {62.92995363352, 104.3742043151, 0},
          {69.67128118964, 79.56461506624, 0}, {69.67128118964,  95.1780010696, 0}, {69.67128118964, 104.3742043151, 0},
          {66.03455388567, 79.56461506624, 0}, {66.03455388567,  95.1780010696, 0}, {66.03455388567, 104.3742043151, 0},
          {96.25276276890, 79.56461506624, 0}, {96.25276276890,  95.1780010696, 0}, {96.25276276890, 104.3742043151, 0}};

    std::vector<double> tGoldProbabilities = { 0.160857230304970, 0.118639480992335, 0.0847557373684797,
                                               0.0867412302322639, 0.0639755795613402, 0.0457039880311642,
                                               0.101584192161421, 0.0749229351531628, 0.0535247504591439,
                                               0.0923587620945064, 0.0681187633233061, 0.0486638677597447 };

    double tSum = 0;
    double tTolerance = 1e-4;
    const size_t tTotalNumSamples = tSetGoldRotations.size();
    for(size_t tSampleIndex = 0; tSampleIndex < tTotalNumSamples; tSampleIndex++)
    {
        const std::vector<double>& tGoldRotations = tSetGoldRotations[tSampleIndex];
        ASSERT_NEAR(tGoldRotations[0], tRandomRotationSet[tSampleIndex].mRotations.mX, tTolerance);
        ASSERT_NEAR(tGoldRotations[1], tRandomRotationSet[tSampleIndex].mRotations.mY, tTolerance);
        ASSERT_NEAR(tGoldRotations[2], tRandomRotationSet[tSampleIndex].mRotations.mZ, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tSampleIndex], tRandomRotationSet[tSampleIndex].mProbability, tTolerance);
        tSum += tRandomRotationSet[tSampleIndex].mProbability;
    }

    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, expand_random_rotations_about_xz)
{
    // SET INPUTS
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    tSampleProbPairSetX.mNumSamples = 4;
    tSampleProbPairSetX.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tSampleProbPairSetX.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    tSampleProbPairSetZ.mNumSamples = 3;
    tSampleProbPairSetZ.mSamples = {79.56461506624, 95.1780010696, 104.3742043151};
    tSampleProbPairSetZ.mProbabilities = {0.441549282785, 0.3256625620299, 0.2326524892665};

    // CALL FUNCTION
    std::vector<Plato::RandomRotations> tRandomRotationSet;
    ASSERT_TRUE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tRandomRotationSet));

    // TEST OUTPUTS
    ASSERT_EQ(12u, tRandomRotationSet.size());
    std::vector<std::vector<double>> tSetGoldRotations =
        { {62.92995363352, 0, 79.56461506624}, {62.92995363352, 0, 95.1780010696}, {62.92995363352, 0, 104.3742043151},
          {69.67128118964, 0, 79.56461506624}, {69.67128118964, 0, 95.1780010696}, {69.67128118964, 0, 104.3742043151},
          {66.03455388567, 0, 79.56461506624}, {66.03455388567, 0, 95.1780010696}, {66.03455388567, 0, 104.3742043151},
          {96.25276276890, 0, 79.56461506624}, {96.25276276890, 0, 95.1780010696}, {96.25276276890, 0, 104.3742043151}};

    std::vector<double> tGoldProbabilities = { 0.160857230304970, 0.118639480992335, 0.0847557373684797,
                                               0.0867412302322639, 0.0639755795613402, 0.0457039880311642,
                                               0.101584192161421, 0.0749229351531628, 0.0535247504591439,
                                               0.0923587620945064, 0.0681187633233061, 0.0486638677597447 };

    double tSum = 0;
    double tTolerance = 1e-4;
    const size_t tTotalNumSamples = tSetGoldRotations.size();
    for(size_t tSampleIndex = 0; tSampleIndex < tTotalNumSamples; tSampleIndex++)
    {
        const std::vector<double>& tGoldRotations = tSetGoldRotations[tSampleIndex];
        ASSERT_NEAR(tGoldRotations[0], tRandomRotationSet[tSampleIndex].mRotations.mX, tTolerance);
        ASSERT_NEAR(tGoldRotations[1], tRandomRotationSet[tSampleIndex].mRotations.mY, tTolerance);
        ASSERT_NEAR(tGoldRotations[2], tRandomRotationSet[tSampleIndex].mRotations.mZ, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tSampleIndex], tRandomRotationSet[tSampleIndex].mProbability, tTolerance);
        tSum += tRandomRotationSet[tSampleIndex].mProbability;
    }

    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, expand_random_rotations_about_yz)
{
    // SET INPUTS
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    tSampleProbPairSetY.mNumSamples = 4;
    tSampleProbPairSetY.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tSampleProbPairSetY.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    tSampleProbPairSetZ.mNumSamples = 3;
    tSampleProbPairSetZ.mSamples = {79.56461506624, 95.1780010696, 104.3742043151};
    tSampleProbPairSetZ.mProbabilities = {0.441549282785, 0.3256625620299, 0.2326524892665};

    // CALL FUNCTION
    std::vector<Plato::RandomRotations> tRandomRotationSet;
    ASSERT_TRUE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tRandomRotationSet));

    // TEST OUTPUTS
    ASSERT_EQ(12u, tRandomRotationSet.size());
    std::vector<std::vector<double>> tSetGoldRotations =
        { {0, 62.92995363352, 79.56461506624}, {0, 62.92995363352, 95.1780010696}, {0, 62.92995363352, 104.3742043151},
          {0, 69.67128118964, 79.56461506624}, {0, 69.67128118964, 95.1780010696}, {0, 69.67128118964, 104.3742043151},
          {0, 66.03455388567, 79.56461506624}, {0, 66.03455388567, 95.1780010696}, {0, 66.03455388567, 104.3742043151},
          {0, 96.25276276890, 79.56461506624}, {0, 96.25276276890, 95.1780010696}, {0, 96.25276276890, 104.3742043151}};

    std::vector<double> tGoldProbabilities = { 0.160857230304970, 0.118639480992335, 0.0847557373684797,
                                               0.0867412302322639, 0.0639755795613402, 0.0457039880311642,
                                               0.101584192161421, 0.0749229351531628, 0.0535247504591439,
                                               0.0923587620945064, 0.0681187633233061, 0.0486638677597447 };

    double tSum = 0;
    double tTolerance = 1e-4;
    const size_t tTotalNumSamples = tSetGoldRotations.size();
    for(size_t tSampleIndex = 0; tSampleIndex < tTotalNumSamples; tSampleIndex++)
    {
        const std::vector<double>& tGoldRotations = tSetGoldRotations[tSampleIndex];
        ASSERT_NEAR(tGoldRotations[0], tRandomRotationSet[tSampleIndex].mRotations.mX, tTolerance);
        ASSERT_NEAR(tGoldRotations[1], tRandomRotationSet[tSampleIndex].mRotations.mY, tTolerance);
        ASSERT_NEAR(tGoldRotations[2], tRandomRotationSet[tSampleIndex].mRotations.mZ, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tSampleIndex], tRandomRotationSet[tSampleIndex].mProbability, tTolerance);
        tSum += tRandomRotationSet[tSampleIndex].mProbability;
    }

    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, expand_random_rotations_about_x)
{
    // SET INPUTS
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    tSampleProbPairSetX.mNumSamples = 4;
    tSampleProbPairSetX.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tSampleProbPairSetX.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    // CALL FUNCTION
    std::vector<Plato::RandomRotations> tRandomRotationSet;
    ASSERT_TRUE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tRandomRotationSet));

    // TEST OUTPUTS
    ASSERT_EQ(4u, tRandomRotationSet.size());
    std::vector<std::vector<double>> tSetGoldRotations =
        { {62.92995363352, 0, 0}, {69.67128118964, 0, 0}, {66.03455388567, 0, 0}, {96.25276276890, 0, 0} };
    std::vector<double> tGoldProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    double tSum = 0;
    double tTolerance = 1e-4;
    const size_t tTotalNumSamples = tSetGoldRotations.size();
    for(size_t tSampleIndex = 0; tSampleIndex < tTotalNumSamples; tSampleIndex++)
    {
        const std::vector<double>& tGoldRotations = tSetGoldRotations[tSampleIndex];
        ASSERT_NEAR(tGoldRotations[0], tRandomRotationSet[tSampleIndex].mRotations.mX, tTolerance);
        ASSERT_NEAR(tGoldRotations[1], tRandomRotationSet[tSampleIndex].mRotations.mY, tTolerance);
        ASSERT_NEAR(tGoldRotations[2], tRandomRotationSet[tSampleIndex].mRotations.mZ, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tSampleIndex], tRandomRotationSet[tSampleIndex].mProbability, tTolerance);
        tSum += tRandomRotationSet[tSampleIndex].mProbability;
    }

    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, expand_random_rotations_about_y)
{
    // SET INPUTS
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    tSampleProbPairSetY.mNumSamples = 4;
    tSampleProbPairSetY.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tSampleProbPairSetY.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    // CALL FUNCTION
    std::vector<Plato::RandomRotations> tRandomRotationSet;
    ASSERT_TRUE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tRandomRotationSet));

    // TEST OUTPUTS
    ASSERT_EQ(4u, tRandomRotationSet.size());
    std::vector<std::vector<double>> tSetGoldRotations =
        { {0, 62.92995363352, 0}, {0, 69.67128118964, 0}, {0, 66.03455388567, 0}, {0, 96.25276276890, 0} };
    std::vector<double> tGoldProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    double tSum = 0;
    double tTolerance = 1e-4;
    const size_t tTotalNumSamples = tSetGoldRotations.size();
    for(size_t tSampleIndex = 0; tSampleIndex < tTotalNumSamples; tSampleIndex++)
    {
        const std::vector<double>& tGoldRotations = tSetGoldRotations[tSampleIndex];
        ASSERT_NEAR(tGoldRotations[0], tRandomRotationSet[tSampleIndex].mRotations.mX, tTolerance);
        ASSERT_NEAR(tGoldRotations[1], tRandomRotationSet[tSampleIndex].mRotations.mY, tTolerance);
        ASSERT_NEAR(tGoldRotations[2], tRandomRotationSet[tSampleIndex].mRotations.mZ, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tSampleIndex], tRandomRotationSet[tSampleIndex].mProbability, tTolerance);
        tSum += tRandomRotationSet[tSampleIndex].mProbability;
    }

    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, expand_random_rotations_about_z)
{
    // SET INPUTS
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    tSampleProbPairSetZ.mNumSamples = 4;
    tSampleProbPairSetZ.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tSampleProbPairSetZ.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    // CALL FUNCTION
    std::vector<Plato::RandomRotations> tRandomRotationSet;
    ASSERT_TRUE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tRandomRotationSet));

    // TEST OUTPUTS
    ASSERT_EQ(4u, tRandomRotationSet.size());
    std::vector<std::vector<double>> tSetGoldRotations =
        { {0, 0, 62.92995363352}, {0, 0, 69.67128118964}, {0, 0, 66.03455388567}, {0, 0, 96.25276276890} };
    std::vector<double> tGoldProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    double tSum = 0;
    double tTolerance = 1e-4;
    const size_t tTotalNumSamples = tSetGoldRotations.size();
    for(size_t tSampleIndex = 0; tSampleIndex < tTotalNumSamples; tSampleIndex++)
    {
        const std::vector<double>& tGoldRotations = tSetGoldRotations[tSampleIndex];
        ASSERT_NEAR(tGoldRotations[0], tRandomRotationSet[tSampleIndex].mRotations.mX, tTolerance);
        ASSERT_NEAR(tGoldRotations[1], tRandomRotationSet[tSampleIndex].mRotations.mY, tTolerance);
        ASSERT_NEAR(tGoldRotations[2], tRandomRotationSet[tSampleIndex].mRotations.mZ, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tSampleIndex], tRandomRotationSet[tSampleIndex].mProbability, tTolerance);
        tSum += tRandomRotationSet[tSampleIndex].mProbability;
    }

    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, expand_random_rotations_error)
{
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    std::vector<Plato::RandomRotations> tRandomRotationSet;
    ASSERT_FALSE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tRandomRotationSet));
}

TEST(PlatoTest, expand_random_loads_about_z)
{
    // SET INPUTS
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    tSampleProbPairSetZ.mNumSamples = 4;
    tSampleProbPairSetZ.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tSampleProbPairSetZ.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    // COMPUTE RANDOM ROTATIONS SET
    std::vector<Plato::RandomRotations> tMyRandomRotationsSet;
    ASSERT_TRUE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tMyRandomRotationsSet));

    // CALL EXPAND RANDOM LOADS FUNCTION - FUNCTION BEING TESTED
    Plato::UniqueCounter tUniqueLoadCounter;
    Plato::Vector3D tMyOriginalLoad(1, 1, 1);
    std::vector<Plato::RandomLoad> tMyRandomLoads;
    Plato::expand_random_loads(tMyOriginalLoad, tMyRandomRotationsSet, tUniqueLoadCounter, tMyRandomLoads);

    // TEST OUTPUTS
    ASSERT_EQ(4u, tMyRandomLoads.size());

    std::vector<std::vector<double>> tGoldLoads =
        { {-0.43537138598505315, 1.345530288126378, 1}, {-0.5903092061838121, 1.2851206328958531, 1},
          {-0.50760495686136231, 1.319976214850015, 1}, {-1.1029658951090628, 0.8851362800305180, 1} };
    std::vector<double> tGoldProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    double tSum = 0;
    double tTolerance = 1e-4;
    const size_t tTotalNumSamples = tMyRandomLoads.size();
    for(size_t tSampleIndex = 0; tSampleIndex < tTotalNumSamples; tSampleIndex++)
    {
        EXPECT_NEAR(tGoldLoads[tSampleIndex][0], tMyRandomLoads[tSampleIndex].mLoad.mX, tTolerance);
        EXPECT_NEAR(tGoldLoads[tSampleIndex][1], tMyRandomLoads[tSampleIndex].mLoad.mY, tTolerance);
        EXPECT_NEAR(tGoldLoads[tSampleIndex][2], tMyRandomLoads[tSampleIndex].mLoad.mZ, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tSampleIndex], tMyRandomLoads[tSampleIndex].mProbability, tTolerance);
        tSum += tMyRandomLoads[tSampleIndex].mProbability;
    }

    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, expand_random_loads_about_y)
{
    // SET INPUTS
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    tSampleProbPairSetY.mNumSamples = 4;
    tSampleProbPairSetY.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tSampleProbPairSetY.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    // COMPUTE RANDOM ROTATIONS SET
    std::vector<Plato::RandomRotations> tMyRandomRotationsSet;
    ASSERT_TRUE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tMyRandomRotationsSet));

    // CALL EXPAND RANDOM LOADS FUNCTION - FUNCTION BEING TESTED
    Plato::UniqueCounter tUniqueLoadCounter;
    Plato::Vector3D tMyOriginalLoad(1, 1, 1);
    std::vector<Plato::RandomLoad> tMyRandomLoads;
    Plato::expand_random_loads(tMyOriginalLoad, tMyRandomRotationsSet, tUniqueLoadCounter, tMyRandomLoads);

    // TEST OUTPUTS
    ASSERT_EQ(4u, tMyRandomLoads.size());

    std::vector<std::vector<double>> tGoldLoads =
        { {1.34553028812638, 1, -0.435371385985053}, {1.28512063289585, 1, -0.590309206183812},
          {1.31997621485002, 1, -0.507604956861362}, {0.88513628003051, 1, -1.10296589510906} };
    std::vector<double> tGoldProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    double tSum = 0;
    double tTolerance = 1e-4;
    const size_t tTotalNumSamples = tMyRandomLoads.size();
    for(size_t tSampleIndex = 0; tSampleIndex < tTotalNumSamples; tSampleIndex++)
    {
        EXPECT_NEAR(tGoldLoads[tSampleIndex][0], tMyRandomLoads[tSampleIndex].mLoad.mX, tTolerance);
        EXPECT_NEAR(tGoldLoads[tSampleIndex][1], tMyRandomLoads[tSampleIndex].mLoad.mY, tTolerance);
        EXPECT_NEAR(tGoldLoads[tSampleIndex][2], tMyRandomLoads[tSampleIndex].mLoad.mZ, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tSampleIndex], tMyRandomLoads[tSampleIndex].mProbability, tTolerance);
        tSum += tMyRandomLoads[tSampleIndex].mProbability;
    }

    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, expand_random_loads_about_x)
{
    // SET INPUTS
    Plato::SampleProbabilityPairs tSampleProbPairSetX;
    Plato::SampleProbabilityPairs tSampleProbPairSetY;
    Plato::SampleProbabilityPairs tSampleProbPairSetZ;
    tSampleProbPairSetX.mNumSamples = 4;
    tSampleProbPairSetX.mSamples = {62.92995363352, 69.67128118964, 66.03455388567, 96.2527627689};
    tSampleProbPairSetX.mProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    // COMPUTE RANDOM ROTATIONS SET
    std::vector<Plato::RandomRotations> tMyRandomRotationsSet;
    ASSERT_TRUE(Plato::expand_random_rotations(tSampleProbPairSetX, tSampleProbPairSetY, tSampleProbPairSetZ, tMyRandomRotationsSet));

    // CALL EXPAND RANDOM LOADS FUNCTION - FUNCTION BEING TESTED
    Plato::UniqueCounter tUniqueLoadCounter;
    Plato::Vector3D tMyOriginalLoad(1, 1, 1);
    std::vector<Plato::RandomLoad> tMyRandomLoads;
    Plato::expand_random_loads(tMyOriginalLoad, tMyRandomRotationsSet, tUniqueLoadCounter, tMyRandomLoads);

    // TEST OUTPUTS
    ASSERT_EQ(4u, tMyRandomLoads.size());

    std::vector<std::vector<double>> tGoldLoads =
        { {1, -0.435371385985053, 1.34553028812638}, {1, -0.590309206183812, 1.28512063289585},
          {1, -0.507604956861362, 1.31997621485002}, {1, -1.10296589510906, 0.885136280030518} };
    std::vector<double> tGoldProbabilities = {0.3643018720139, 0.1964474490484, 0.2300630894941, 0.2091697703866};

    double tSum = 0;
    double tTolerance = 1e-4;
    const size_t tTotalNumSamples = tMyRandomLoads.size();
    for(size_t tSampleIndex = 0; tSampleIndex < tTotalNumSamples; tSampleIndex++)
    {
        EXPECT_NEAR(tGoldLoads[tSampleIndex][0], tMyRandomLoads[tSampleIndex].mLoad.mX, tTolerance);
        EXPECT_NEAR(tGoldLoads[tSampleIndex][1], tMyRandomLoads[tSampleIndex].mLoad.mY, tTolerance);
        EXPECT_NEAR(tGoldLoads[tSampleIndex][2], tMyRandomLoads[tSampleIndex].mLoad.mZ, tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tSampleIndex], tMyRandomLoads[tSampleIndex].mProbability, tTolerance);
        tSum += tMyRandomLoads[tSampleIndex].mProbability;
    }

    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);
}

TEST(PlatoTest, expand_random_loads_error_1)
{
    // CALL EXPAND RANDOM LOADS FUNCTION - FAILURE IS EXPECTED SINCE SET OF RANDOM ROTATION IS EMPTY
    Plato::UniqueCounter tUniqueLoadCounter;
    Plato::Vector3D tMyOriginalLoad(1, 1, 1);
    std::vector<Plato::RandomLoad> tMyRandomLoads;
    std::vector<Plato::RandomRotations> tMyRandomRotationsSet;
    ASSERT_FALSE(Plato::expand_random_loads(tMyOriginalLoad, tMyRandomRotationsSet, tUniqueLoadCounter, tMyRandomLoads));
}

TEST(PlatoTest, expand_random_loads_error_2)
{
    // CALL EXPAND RANDOM LOADS FUNCTION - FAILURE IS EXPECTED DUE TO NaN X-COMPONENT
    Plato::UniqueCounter tUniqueLoadCounter;
    Plato::Vector3D tMyOriginalLoad(std::numeric_limits<double>::quiet_NaN(), 1, 1);
    std::vector<Plato::RandomRotations> tMyRandomRotationsSet;
    Plato::RandomRotations tMyRotation;
    tMyRotation.mRotations.mX = 0;
    tMyRotation.mRotations.mY = 0;
    tMyRotation.mRotations.mZ = 62.92995363352;
    tMyRandomRotationsSet.push_back(tMyRotation);
    std::vector<Plato::RandomLoad> tMyRandomLoads;
    ASSERT_FALSE(Plato::expand_random_loads(tMyOriginalLoad, tMyRandomRotationsSet, tUniqueLoadCounter, tMyRandomLoads));

    // CALL EXPAND RANDOM LOADS FUNCTION - FAILURE IS EXPECTED DUE TO NaN Y-COMPONENT
    tMyOriginalLoad.mX = 1; tMyOriginalLoad.mY = std::numeric_limits<double>::quiet_NaN();
    ASSERT_FALSE(Plato::expand_random_loads(tMyOriginalLoad, tMyRandomRotationsSet, tUniqueLoadCounter, tMyRandomLoads));

    // CALL EXPAND RANDOM LOADS FUNCTION - FAILURE IS EXPECTED DUE TO NaN Z-COMPONENT
    tMyOriginalLoad.mY = 1; tMyOriginalLoad.mZ = std::numeric_limits<double>::quiet_NaN();
    ASSERT_FALSE(Plato::expand_random_loads(tMyOriginalLoad, tMyRandomRotationsSet, tUniqueLoadCounter, tMyRandomLoads));
}

TEST(PlatoTest, expand_random_loads_error_3)
{
    // CALL EXPAND RANDOM LOADS FUNCTION - FAILURE IS EXPECTED DUE TO NaN X-COMPONENT
    Plato::UniqueCounter tUniqueLoadCounter;
    Plato::Vector3D tMyOriginalLoad(1, 1, 1);
    std::vector<Plato::RandomLoad> tMyRandomLoads;
    std::vector<Plato::RandomRotations> tMyRandomRotationsSet;
    Plato::RandomRotations tMyRotation;
    tMyRotation.mRotations.mX = std::numeric_limits<double>::quiet_NaN(); tMyRotation.mRotations.mY = 0; tMyRotation.mRotations.mZ = 62.92995363352;
    tMyRandomRotationsSet.push_back(tMyRotation);
    ASSERT_FALSE(Plato::expand_random_loads(tMyOriginalLoad, tMyRandomRotationsSet, tUniqueLoadCounter, tMyRandomLoads));

    // CALL EXPAND RANDOM LOADS FUNCTION - FAILURE IS EXPECTED DUE TO NaN Y-COMPONENT
    tMyRandomRotationsSet[0].mRotations.mX = 0; tMyRandomRotationsSet[0].mRotations.mY = std::numeric_limits<double>::quiet_NaN();
    ASSERT_FALSE(Plato::expand_random_loads(tMyOriginalLoad, tMyRandomRotationsSet, tUniqueLoadCounter, tMyRandomLoads));

    // CALL EXPAND RANDOM LOADS FUNCTION - FAILURE IS EXPECTED DUE TO NaN Z-COMPONENT
    tMyRandomRotationsSet[0].mRotations.mY = 0; tMyRandomRotationsSet[0].mRotations.mZ = std::numeric_limits<double>::quiet_NaN();
    ASSERT_FALSE(Plato::expand_random_loads(tMyOriginalLoad, tMyRandomRotationsSet, tUniqueLoadCounter, tMyRandomLoads));
}

TEST(PlatoTest, expand_load_cases)
{
    std::map<int, std::vector<int> > tOriginalToNewLoadCaseMap;
    InputData tInputData;
    LoadCase tLC1;
    Load tL1;
    tLC1.id = "2";
    tLC1.loads.push_back(tL1);
    tLC1.loads.push_back(tL1);
    tLC1.loads.push_back(tL1);
    tInputData.load_cases.push_back(tLC1);
    tLC1.id = "4";
    tLC1.loads.clear();
    tLC1.loads.push_back(tL1);
    tLC1.loads.push_back(tL1);
    tLC1.loads.push_back(tL1);
    tInputData.load_cases.push_back(tLC1);
    tLC1.id = "6";
    tLC1.loads.clear();
    tLC1.loads.push_back(tL1);
    tInputData.load_cases.push_back(tLC1);
    tLC1.id = "10";
    tLC1.loads.clear();
    tLC1.loads.push_back(tL1);
    tLC1.loads.push_back(tL1);
    tInputData.load_cases.push_back(tLC1);
    std::vector<LoadCase> tNewLoadCases;
    Plato::expand_load_cases(tInputData, tNewLoadCases, tOriginalToNewLoadCaseMap);
    ASSERT_EQ(tNewLoadCases.size(), 9);
    ASSERT_STREQ(tNewLoadCases[0].id.c_str(), "2");
    ASSERT_STREQ(tNewLoadCases[1].id.c_str(), "1");
    ASSERT_STREQ(tNewLoadCases[2].id.c_str(), "3");
    ASSERT_STREQ(tNewLoadCases[3].id.c_str(), "4");
    ASSERT_STREQ(tNewLoadCases[4].id.c_str(), "5");
    ASSERT_STREQ(tNewLoadCases[5].id.c_str(), "7");
    ASSERT_STREQ(tNewLoadCases[6].id.c_str(), "6");
    ASSERT_STREQ(tNewLoadCases[7].id.c_str(), "10");
    ASSERT_STREQ(tNewLoadCases[8].id.c_str(), "8");
    ASSERT_EQ(tOriginalToNewLoadCaseMap[2][0], 2);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[2][1], 1);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[2][2], 3);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[4][0], 4);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[4][1], 5);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[4][2], 7);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[6][0], 6);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[10][0], 10);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[10][1], 8);
}

TEST(PlatoTest, initialize_load_id_counter)
{
    std::vector<LoadCase> tLoadCases;
    LoadCase tLC1;
    tLC1.id = "3";
    tLoadCases.push_back(tLC1);
    Plato::UniqueCounter tUniqueLoadIDCounter;
    Plato::initialize_load_id_counter(tLoadCases, tUniqueLoadIDCounter);
    int tID = tUniqueLoadIDCounter.assignNextUnique();
    ASSERT_EQ(tID, 1);
    tID = tUniqueLoadIDCounter.assignNextUnique();
    ASSERT_EQ(tID, 2);
    tID = tUniqueLoadIDCounter.assignNextUnique();
    ASSERT_EQ(tID, 4);
}

TEST(PlatoTest, expand_single_load_case)
{
    // Check case of single load case with single load
    std::map<int, std::vector<int> > tOriginalToNewLoadCaseMap;
    LoadCase tOldLoadCase;
    tOldLoadCase.id = "88";
    Load tLoad1;
    tLoad1.app_id = "34";
    tLoad1.app_type = "nodeset";
    tLoad1.type = "traction";
    tLoad1.x = "0.0";
    tLoad1.y = "1.0";
    tLoad1.z = "33";
    tLoad1.scale = "24.5";
    tLoad1.load_id = "89";
    tOldLoadCase.loads.push_back(tLoad1);
    Plato::UniqueCounter tUniqueLoadIDCounter;
    tUniqueLoadIDCounter.mark(0);
    tUniqueLoadIDCounter.mark(88);
    std::vector<LoadCase> tNewLoadCaseList;
    Plato::expand_single_load_case(tOldLoadCase,tNewLoadCaseList,tUniqueLoadIDCounter,
                                   tOriginalToNewLoadCaseMap);
    ASSERT_EQ(tNewLoadCaseList.size(), 1);
    ASSERT_STREQ(tNewLoadCaseList[0].id.c_str(), "88");
    ASSERT_EQ(tNewLoadCaseList[0].loads.size(), 1);
    ASSERT_STREQ(tNewLoadCaseList[0].loads[0].load_id.c_str(), "89");
    ASSERT_STREQ(tNewLoadCaseList[0].loads[0].y.c_str(), "1.0");
    ASSERT_STREQ(tNewLoadCaseList[0].loads[0].x.c_str(), "0.0");
    ASSERT_STREQ(tNewLoadCaseList[0].loads[0].z.c_str(), "33");
    ASSERT_STREQ(tNewLoadCaseList[0].loads[0].type.c_str(), "traction");
    ASSERT_STREQ(tNewLoadCaseList[0].loads[0].app_type.c_str(), "nodeset");
    ASSERT_STREQ(tNewLoadCaseList[0].loads[0].app_id.c_str(), "34");
    ASSERT_STREQ(tNewLoadCaseList[0].loads[0].scale.c_str(), "24.5");
    ASSERT_EQ(tOriginalToNewLoadCaseMap[88][0], 88);
    // Check the case where a load case has more than one load
    Load tLoad2;
    tLoad2.app_id = "21";
    tLoad2.app_type = "sideset";
    tLoad2.type = "pressure";
    tLoad2.x = "44";
    tLoad2.y = "55";
    tLoad2.z = "66";
    tLoad2.scale = "12";
    tLoad2.load_id = "101";
    tOldLoadCase.loads.push_back(tLoad2);
    std::vector<LoadCase> tNewLoadCaseList2;
    tOriginalToNewLoadCaseMap.clear();
    Plato::expand_single_load_case(tOldLoadCase,tNewLoadCaseList2,tUniqueLoadIDCounter,
                                   tOriginalToNewLoadCaseMap);
    ASSERT_EQ(tNewLoadCaseList2.size(), 2);
    ASSERT_STREQ(tNewLoadCaseList2[1].id.c_str(), "1");
    ASSERT_STREQ(tNewLoadCaseList2[0].id.c_str(), "88");
    ASSERT_STREQ(tNewLoadCaseList2[1].loads[0].y.c_str(), "55");
    ASSERT_STREQ(tNewLoadCaseList2[1].loads[0].x.c_str(), "44");
    ASSERT_STREQ(tNewLoadCaseList2[1].loads[0].z.c_str(), "66");
    ASSERT_EQ(tOriginalToNewLoadCaseMap[88][0], 88);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[88][1], 1);
    // Check case where load case with multiple loads is is split and the ids of the resulting
    // load cases have to "straddle" original load case id
    LoadCase tLC1;
    tLC1.id = "2";
    Load tL1, tL2, tL3;
    tLC1.loads.push_back(tL1);
    tLC1.loads.push_back(tL2);
    tLC1.loads.push_back(tL3);
    Plato::UniqueCounter tIDCounter;
    tIDCounter.mark(0);
    tIDCounter.mark(2);
    std::vector<LoadCase> tNewList;
    tOriginalToNewLoadCaseMap.clear();
    Plato::expand_single_load_case(tLC1,tNewList,tIDCounter,
                                   tOriginalToNewLoadCaseMap);
    ASSERT_EQ(tNewList.size(), 3);
    ASSERT_STREQ(tNewList[0].id.c_str(), "2");
    ASSERT_STREQ(tNewList[1].id.c_str(), "1");
    ASSERT_STREQ(tNewList[2].id.c_str(), "3");
    ASSERT_EQ(tOriginalToNewLoadCaseMap[2][0], 2);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[2][1], 1);
    ASSERT_EQ(tOriginalToNewLoadCaseMap[2][2], 3);
}

}
