/*
 * XMLGeneratorOptimizationParametersMetadata.cpp
 *
 *  Created on: Jan 11, 2021
 */

#include "XMLG_Macros.hpp"
#include "Plato_FreeFunctions.hpp"
#include "XMLGeneratorParserUtilities.hpp"
#include "XMLGeneratorOptimizationParametersMetadata.hpp"

namespace XMLGen
{

std::string OptimizationParameters::getValue(const std::string& aTag) const
{
    auto tItr = mMetaData.find(aTag);
    if(tItr == mMetaData.end())
    {
        return "";
    }
    return tItr->second.mValue.front();
}

std::vector<std::string> OptimizationParameters::getValues(const std::string& aTag) const
{
    auto tItr = mMetaData.find(aTag);
    if(tItr == mMetaData.end())
    {
        return {""};
    }
    return tItr->second.mValue;
}

bool OptimizationParameters::getBool(const std::string& aTag) const
{
    auto tItr = mMetaData.find(aTag);
    if(tItr == mMetaData.end())
    {
        THROWERR(std::string("XML Generator OptimizationParameters Metadata: '") + aTag + "' keyword is not defined.")
    }
    return (XMLGen::transform_boolean_key(tItr->second.mValue.front()));
}

std::string OptimizationParameters::value(const std::string& aTag) const
{
    std::string tOutput = "";
    auto tTag = Plato::tolower(aTag);
    auto tItr = mMetaData.find(tTag);

    if(tItr == mMetaData.end() || tItr->second.mValue.front().empty())
    {
        PRINTEMPTYINFO(aTag, "Optimization Parameters");
    }
    else
    {
        tOutput = tItr->second.mValue.front();
        if(tItr->second.mIsDefault)
        {
            PRINTDEFAULTINFO(aTag, tOutput, "Optimization Parameters");
        }
    }

    return tOutput;
}

std::vector<std::string> OptimizationParameters::values(const std::string& aTag) const
{
    auto tTag = Plato::tolower(aTag);
    auto tItr = mMetaData.find(tTag);
    if(tItr == mMetaData.end())
    {
        PRINTEMPTYINFO(aTag, "Optimization Parameters");
    }
    return tItr->second.mValue;
}

std::vector<std::string> OptimizationParameters::tags() const
{
    std::vector<std::string> tTags;
    for(auto& tProperty : mMetaData)
    {
        tTags.push_back(tProperty.first);
    }
    return tTags;
}

void OptimizationParameters::append(const std::string& aTag, const std::string& aValue, const bool& aIsDefault)
{
    if (aTag.empty())
    {
        THROWERR(std::string("XML Generator OptimizationParameters Metadata: Parameter with tag '") + aTag + "' is empty.")
    }
    auto tTag = Plato::tolower(aTag);
    ValueData tValueData;
    tValueData.mValue.push_back(aValue);
    tValueData.mIsDefault = aIsDefault;
    mMetaData[aTag] = tValueData;
}

void OptimizationParameters::set(const std::string& aTag, const std::vector<std::string>& aList, const bool& aIsDefault)
{
    if (aTag.empty())
    {
        THROWERR(std::string("XML Generator OptimizationParameters Metadata: Parameter with tag '") + aTag + "' is empty.")
    }
    auto tTag = Plato::tolower(aTag);
    ValueData tValueData;
    tValueData.mValue = aList;
    tValueData.mIsDefault = aIsDefault;
    mMetaData[aTag] = tValueData;
}

bool OptimizationParameters::needsMeshMap() const
{
    if(mSymmetryPlaneOrigin.size() == 3 && mSymmetryPlaneNormal.size() == 3)
    {
        return true;
    }
    return false;
}

std::string OptimizationParameters::filter_before_symmetry_enforcement() const
{
    return (this->getValue("filter_before_symmetry_enforcement"));
}

std::string OptimizationParameters::mesh_map_filter_radius() const
{
    return (this->getValue("mesh_map_filter_radius"));
}

}
// namespace XMLGen
