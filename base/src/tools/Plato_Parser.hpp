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
 * Plato_Parser.hpp
 * 
 * Created on: Dec 13, 2016
 * 
 */

#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_

#include <string>
#include <vector>
#include "pugixml.hpp"
#include "Plato_InputData.hpp"

namespace Plato
{

class StageInputDataMng;
class OperationInputDataMng;
class OptimizerEngineStageData;



class Parser {
  public:
    virtual ~Parser(){}
    virtual InputData parseFile(const std::string& fileName) = 0;
    virtual InputData parseString(const std::string& inputString) = 0;
};

class PugiParser : public Parser {
  public:
    ~PugiParser(){}
    Plato::InputData parseFile(const std::string& fileName);
    Plato::InputData parseString(const std::string& inputString);
  private:
    Plato::InputData read(std::shared_ptr<pugi::xml_document> doc);
    void addChildren(const pugi::xml_node& node, InputData& inputData);
};

namespace Get
{
bool                Bool      (const Plato::InputData & aInputData, const std::string & aFieldname);
double              Double    (const Plato::InputData & aInputData, const std::string & aFieldname);
std::vector<double> Doubles   (const Plato::InputData & aInputData, const std::string & aKeyword);
int                 Int       (const Plato::InputData & aInputData, const std::string & aFieldname);
std::vector<int>    Ints      (const Plato::InputData & aInputData, const std::string & aKeyword);
std::string         String    (const Plato::InputData & aInputData, const std::string & aFieldname, bool aToUpper = false);
std::string         String    (      Plato::InputData & aInputData, const std::string & aFieldname, const std::string& aDefault);
Plato::InputData    InputData (Plato::InputData & aNode, const std::string & aNodename, unsigned int aIndex=0);
}

namespace Parse
{

/*** Pugi specific.  To be deprecated ***
 *
 *  These are still used in base/src/analyze
 */
typedef pugi::xml_node InputNode;

void toUppercase(std::string& aString);
void loadFile(pugi::xml_document & aInput);
bool getBool(const pugi::xml_node & aNode, const std::string & aFieldname);
double getDouble(const pugi::xml_node & aNode);
double getDouble(const pugi::xml_node & aNode, const std::string & aFieldname);
std::vector<double> getDoubles(const pugi::xml_node & aXML_Node, const std::string & aKeyword);
int getInt(const pugi::xml_node & aNode, const std::string & aFieldname);
int numChildren(const pugi::xml_node & aNode, const std::string & aFieldname);

std::string getString(pugi::xml_node & aNode, const std::string & aFieldname, const std::string& aDefault);
std::string getStringValue(const pugi::xml_node & aNode, bool aToUpper = false);
std::string getString(const pugi::xml_node & aNode, const std::string & aFieldname, bool aToUpper = false);
std::vector<std::string> getStrings(const pugi::xml_node & aXML_Node, const std::string & aKeyword);
/**** end Pugi specific functions *******/




void toUppercase(std::string& aString);
bool boolFromString(const std::string & strval);

void parseArgumentNameInputs    (const Plato::InputData & aInputData,  std::vector<std::string> & aInputs);
void parseArgumentNameOutputs   (const Plato::InputData & aOutputData, std::vector<std::string> & aOutputs);
void parseSharedDataNameInputs  (const Plato::InputData & tInputData,  std::vector<std::string> & aInputs);
void parseSharedDataNameOutputs (const Plato::InputData & tInputData,  std::vector<std::string> & aOutputs);
void parseStageData             (const Plato::InputData & aStageData,  Plato::StageInputDataMng & aStageInputDataMng);
void parseStageOperations       (const Plato::InputData & aStageData,  Plato::StageInputDataMng & aStageInputDataMng);

void parseConstraintValueNames(const Plato::InputData & aOptimizationNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseConstraintHessianNames(const Plato::InputData & aOptimizationNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseConstraintTargetValues(const Plato::InputData & aOptimizationNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseConstraintGradientNames(const Plato::InputData & aOptimizationNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseConstraintReferenceValues(const Plato::InputData & aOptimizationNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseConstraintReferenceValueNames(const Plato::InputData & aOptimizationNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseOperationData(const Plato::InputData & aOperationNode, Plato::OperationInputDataMng & aOperationData);

void parseObjectiveStagesData(const Plato::InputData & aObjectiveNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseLowerBounds(const Plato::InputData & aOptimizerNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseUpperBounds(const Plato::InputData & aOptimizerNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseInitialGuess(const Plato::InputData & aOptimizerNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseConstraints(const Plato::InputData & aConstraintNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseOptimizerStages(const Plato::InputData & aOptimizerNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseOptimizerOptions(const Plato::InputData & aOptimizerNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

void parseOptimizationVariablesNames(const Plato::InputData & aOptimizerNode, Plato::OptimizerEngineStageData & aOptimizerEngineStageData);

} // namespace Parse

} /* namespace Plato */

#endif /* SRC_PARSER_H_ */
