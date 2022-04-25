/*
 * XMLGeneratorStage.cpp
 *
 *  Created on: April 14, 2022
 */

#include "XMLGeneratorStage.hpp"
#include "XMLGeneratorUtilities.hpp"

namespace XMLGen
{
    
XMLGeneratorStage::XMLGeneratorStage
(const std::string& aName,
 const std::vector<std::shared_ptr<XMLGeneratorOperation>>& aOperations,
 std::shared_ptr<XMLGeneratorSharedData> aInputSharedData,
 std::shared_ptr<XMLGeneratorSharedData> aOutputSharedData):
 XMLGeneratorFileObject(aName),
 mInputSharedData(aInputSharedData),
 mOutputSharedData(aOutputSharedData),
 mOperations(aOperations)
{
}

void XMLGeneratorStage::write
(pugi::xml_document& aDocument)
{
    auto tStageNode = aDocument.append_child("Stage");
    
    addChild(tStageNode, "Name", name());

    this->appendInput(tStageNode);

    for(unsigned int iOperation = 0; iOperation < mOperations.size(); ++iOperation)
    {
        auto tForOrStageNode = mOperations[iOperation]->forNode(tStageNode,"Parameters");
        mOperations[iOperation]->write_interface(tForOrStageNode); 
    }

    this->appendOutput(tStageNode);
}

void XMLGeneratorStage::appendInput
(pugi::xml_node& aNode,
 const std::string& aTag)
{
    if(mInputSharedData)
    {
        auto tForOrStageNode = mInputSharedData->forNode(aNode,"Parameters");
        auto tSharedDataNode = tForOrStageNode.append_child("Input");
        if (aTag == "")
            XMLGen::append_children({"SharedDataName"},{mInputSharedData->name()},tSharedDataNode);
        else
            XMLGen::append_children({"Tag", "SharedDataName"},{aTag, mInputSharedData->name()},tSharedDataNode);
    }
}

void XMLGeneratorStage::appendOutput(pugi::xml_node& aNode)
{
    if(mOutputSharedData)
    {
        auto tForOrStageNode = mOutputSharedData->forNode(aNode,"Parameters");
        auto tSharedDataNode = tForOrStageNode.append_child("Output");
        XMLGen::append_children({"SharedDataName"},{mOutputSharedData->name()},tSharedDataNode);
    }
}

void XMLGeneratorStage::write_dakota
(pugi::xml_node& aDocument,
 const std::string& aStageTag)
{
    auto tStageNode = aDocument.append_child("Stage");
    
    addChild(tStageNode, "StageTag", aStageTag);
    addChild(tStageNode, "StageName", name());

    this->appendInput(tStageNode,"continuous");
    this->appendOutput(tStageNode);
}

}