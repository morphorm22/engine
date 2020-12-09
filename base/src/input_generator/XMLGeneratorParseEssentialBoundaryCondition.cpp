/*
 * XMLGeneratorParseEssentialBoundaryCondition.cpp
 *
 *  Created on: Dec 8, 2020
 */

#include <algorithm>

#include "XMLGeneratorParseEssentialBoundaryCondition.hpp"
#include "XMLGeneratorValidInputKeys.hpp"

namespace XMLGen
{

/*
std::string check_material_model_key
(const std::string& aKeyword)
{
    XMLGen::ValidMaterialModelKeys tValidKeys;
    auto tValue = tValidKeys.value(aKeyword);
    if(tValue.empty())
    {
        THROWERR(std::string("Check Material Model Key: Material model keyword '") + aKeyword + "' is not supported.")
    }
    return tValue;
}
*/

void ParseEssentialBoundaryCondition::insertCoreProperties()
{
    mTags.insert({ "id", { { {"id"}, ""}, "" } });
    mTags.insert({ "type", { { {"type"}, ""}, "" } });
    mTags.insert({ "location_type", { { {"location_type"}, ""}, "" } });
    mTags.insert({ "location_name", { { {"location_name"}, ""}, "" } });
    mTags.insert({ "degree_of_freedom", { { {"degree_of_freedom"}, ""}, "" } });
    mTags.insert({ "value", { { {"value"}, ""}, "" } });
}

void ParseEssentialBoundaryCondition::allocate()
{
    mTags.clear();
    this->insertCoreProperties();
}

void ParseEssentialBoundaryCondition::expandDofs()
{
    std::vector<XMLGen::EssentialBoundaryCondition> tNewEBCs;
    for(auto &tCurEBC : mData)
    {
        std::string tDofString = tCurEBC.value("degree_of_freedom");
        std::string tValueString = tCurEBC.value("value");
        std::vector<std::string> tDofTokens;
        std::vector<std::string> tValueTokens;
        XMLGen::parseTokens((char*)(tDofString.c_str()), tDofTokens); 
        XMLGen::parseTokens((char*)(tValueString.c_str()), tValueTokens); 
        if(tDofTokens.size() > 1)
        {
            if(tDofTokens.size() != tValueTokens.size())
            {
                THROWERR(std::string("Parse EssentialBoundaryCondition:expandDofs:  Number of Dofs does not equal the number of values. "))
            } 
            for(size_t i=0; i<tDofTokens.size(); ++i)
            {
                XMLGen::EssentialBoundaryCondition tNewEBC = tCurEBC;
                tNewEBC.property("degree_of_freedom", tDofTokens[i]);
                tNewEBC.property("value", tValueTokens[i]);
                tNewEBCs.push_back(tNewEBC);
            }
        }
        else
        {
            tNewEBCs.push_back(tCurEBC);
        }
    }
    mData = tNewEBCs;
}

void ParseEssentialBoundaryCondition::setEssentialBoundaryConditionIdentification(XMLGen::EssentialBoundaryCondition& aMetadata)
{
    if(aMetadata.id().empty())
    {
        auto tItr = mTags.find("id");
        if(tItr->second.first.second.empty())
        {
            THROWERR(std::string("Parse EssentialBoundaryCondition: essential boundary condition identification number is empty. ")
                + std::string("A unique essential boundary condition identification number must be assigned to an essential boundary condition block."))
        }
        aMetadata.id(tItr->second.first.second);
    }
}

void ParseEssentialBoundaryCondition::setType(XMLGen::EssentialBoundaryCondition& aMetadata)
{
    if(aMetadata.value("type").empty())
    {
        auto tItr = mTags.find("type");
        if(tItr->second.first.second.empty())
        {
            THROWERR("Parse EssentialBoundaryCondition: essential boundary condition type is empty.")
        }
        aMetadata.property("type", tItr->second.first.second);
    }
}

void ParseEssentialBoundaryCondition::setLocationType(XMLGen::EssentialBoundaryCondition& aMetadata)
{
    if(aMetadata.value("location_type").empty())
    {
        auto tItr = mTags.find("location_type");
        if(tItr->second.first.second.empty())
        {
            THROWERR("Parse EssentialBoundaryCondition: essential boundary condition location_type is empty.")
        }
        aMetadata.property("location_type", tItr->second.first.second);
    }
}

void ParseEssentialBoundaryCondition::setLocationName(XMLGen::EssentialBoundaryCondition& aMetadata)
{
    if(aMetadata.value("location_name").empty())
    {
        auto tItr = mTags.find("location_name");
        if(tItr->second.first.second.empty())
        {
            THROWERR("Parse EssentialBoundaryCondition: essential boundary condition location_name is empty.")
        }
        aMetadata.property("location_name", tItr->second.first.second);
    }
}

void ParseEssentialBoundaryCondition::setDegreeOfFreedom(XMLGen::EssentialBoundaryCondition& aMetadata)
{
    if(aMetadata.value("degree_of_freedom").empty())
    {
        auto tItr = mTags.find("degree_of_freedom");
        if(tItr->second.first.second.empty())
        {
            THROWERR("Parse EssentialBoundaryCondition: essential boundary condition degree_of_freedom is empty.")
        }
        aMetadata.property("degree_of_freedom", tItr->second.first.second);
    }
}

void ParseEssentialBoundaryCondition::setValue(XMLGen::EssentialBoundaryCondition& aMetadata)
{
    if(aMetadata.value("value").empty())
    {
        auto tItr = mTags.find("value");
        if(tItr->second.first.second.empty())
        {
            THROWERR("Parse EssentialBoundaryCondition: essential boundary condition value is empty.")
        }
        aMetadata.property("value", tItr->second.first.second);
    }
}

void ParseEssentialBoundaryCondition::setMetadata(XMLGen::EssentialBoundaryCondition& aMetadata)
{
    this->setEssentialBoundaryConditionIdentification(aMetadata);
    this->setType(aMetadata);
    this->setLocationType(aMetadata);
    this->setLocationName(aMetadata);
    this->setDegreeOfFreedom(aMetadata);
    this->setValue(aMetadata);
}

void ParseEssentialBoundaryCondition::checkUniqueIDs()
{
    std::vector<std::string> tIDs;
    for(auto& tEBC : mData)
    {
        tIDs.push_back(tEBC.id());
    }

    if(!XMLGen::unique(tIDs))
    {
        THROWERR("Parse EssentialBoundaryCondition: BoundaryCondition block identification numbers, i.e. IDs, are not unique.  BoundaryCondition block IDs must be unique.")
    }
}

std::vector<XMLGen::EssentialBoundaryCondition> ParseEssentialBoundaryCondition::data() const
{
    return mData;
}

void ParseEssentialBoundaryCondition::parse(std::istream &aInputFile)
{
    mData.clear();
    this->allocate();
    constexpr int MAX_CHARS_PER_LINE = 10000;
    std::vector<char> tBuffer(MAX_CHARS_PER_LINE);
    while (!aInputFile.eof())
    {
        // read an entire line into memory
        std::vector<std::string> tTokens;
        aInputFile.getline(tBuffer.data(), MAX_CHARS_PER_LINE);
        XMLGen::parse_tokens(tBuffer.data(), tTokens);
        XMLGen::to_lower(tTokens);

        std::string tBCBlockID;
        if (XMLGen::parse_single_value(tTokens, { "begin", "boundary_condition" }, tBCBlockID))
        {
            XMLGen::EssentialBoundaryCondition tMetadata;
            tMetadata.id(tBCBlockID);
            XMLGen::erase_tag_values(mTags);
            XMLGen::parse_input_metadata( { "end", "boundary_condition" }, aInputFile, mTags);
            this->setMetadata(tMetadata);
            mData.push_back(tMetadata);
        }
    }
    this->checkUniqueIDs();
}

}
// namespace XMLGen
