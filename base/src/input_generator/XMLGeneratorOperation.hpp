/*
 * XMLGeneratorOperation.hpp
 *
 *  Created on: April 12, 2022
 */
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <regex>

#include "pugixml.hpp"

#include "XMLGeneratorSharedData.hpp"

namespace XMLGen
{

    struct XMLGeneratorInputOutput
    {
        std::string mArgumentName;
        std::string mLayout;
        std::string mSize;
        std::shared_ptr<XMLGeneratorSharedData> mSharedData;
    };

class XMLGeneratorOperation
{
private:
    std::string mName;
    
protected:
    std::string mFunction;
    std::string mPerformerName;
    std::string mEvaluationTag;
    bool mChDir;
    bool mOnChange;
    int mConcurrentEvaluations;
    std::regex mTagExpression;

    void appendCommonChildren(pugi::xml_node &aOperationNode,std::string aEvaluationNumber);

public:
    XMLGeneratorOperation(const std::string& aName,
                          const std::string& aFunction,
                          int aConcurrentEvaluations);
    virtual void write_definition(pugi::xml_document& aDocument, std::string aEvaluationNumber = "") = 0;
    virtual void write_interface(pugi::xml_document& aDocument, std::string aEvaluationNumber = "") = 0;
    
    void addNameTag();
    std::string name(std::string aEvaluationNumber = "");
};

class XMLGeneratorOperationWait : public XMLGeneratorOperation
{
private:
    std::string mCommand; 
    
public:
    XMLGeneratorOperationWait(const std::string& aName,
                              const std::string& aFile,
                              int aConcurrentEvaluations);
    void write_definition(pugi::xml_document& aDocument, std::string aEvaluationNumber = "") override;
    void write_interface(pugi::xml_document& aDocument, std::string aEvaluationNumber = "") override;
};

class XMLGeneratorOperationGemmaMPISystemCall : public XMLGeneratorOperation
{
private:
    std::string mCommand; 
    std::string mNumRanks;
    std::string mArgument;
    
public:
    XMLGeneratorOperationGemmaMPISystemCall(const std::string& aInputDeck,
                                            const std::string& aNumRanks, 
                                            int aConcurrentEvaluations);
    void write_definition(pugi::xml_document& aDocument, std::string aEvaluationNumber = "") override;
    void write_interface(pugi::xml_document& aDocument, std::string aEvaluationNumber = "") override;
};

class XMLGeneratorOperationAprepro : public XMLGeneratorOperation
{
private:
    std::string mCommand; 
    std::vector<std::string> mArgument;
    std::vector<std::string> mOptions;
    XMLGeneratorInputOutput mInput;
public:
    XMLGeneratorOperationAprepro(const std::string& aInputDeck,
                                 const std::vector<std::string>& aOptions, 
                                 std::shared_ptr<XMLGeneratorSharedData> aSharedData, 
                                 int aConcurrentEvaluations);
    void write_definition(pugi::xml_document& aDocument, std::string aEvaluationNumber = "") override;
    void write_interface(pugi::xml_document& aDocument, std::string aEvaluationNumber = "") override;
};

class XMLGeneratorOperationHarvestDataFunction : public XMLGeneratorOperation
{
private:
    std::string mFile;
    std::string mOperation;
    std::string mColumn;
    
    
    XMLGeneratorInputOutput mOutput;
public:
    XMLGeneratorOperationHarvestDataFunction(const std::string& aFile,
                                             const std::string& aMathOperation,
                                             const std::string& aDataColumn,
                                             int aConcurrentEvaluations);
    void write_definition(pugi::xml_document& aDocument, std::string aEvaluationNumber = "") override;
    void write_interface(pugi::xml_document& aDocument, std::string aEvaluationNumber = "") override;
};

}