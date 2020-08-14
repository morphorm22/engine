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
 * XMLGenerator.hpp
 *
 *  Created on: Sep 28, 2017
 *
 */

#ifndef SRC_XMLGENERATOR_HPP_
#define SRC_XMLGENERATOR_HPP_

#include <string>
#include <map>
#include <vector>
#include <fstream>

#include "Plato_Parser.hpp"
#include "XMLGeneratorUtilities.hpp"
#include "XMLGeneratorDataStruct.hpp"

namespace XMLGen
{

class XMLGenerator
{

public:
    XMLGenerator(const std::string &input_filename = "", bool use_launch = false, const XMLGen::Arch& arch = XMLGen::Arch::CEE);
    ~XMLGenerator();
    void generate();
    // const InputData& getInputData(){ return m_InputData; }
    // bool parseService(std::istream &fin);

protected:

    bool parseLoads(std::istream &fin);
    bool parseLoadsBlock(std::istream &fin);
    bool parseLoadLine(std::vector<std::string>& tokens);
    bool parseTractionLoad(std::vector<std::string>& tokens, XMLGen::Load& new_load);
    bool parsePressureLoad(std::vector<std::string>& tokens, XMLGen::Load& new_load);
    bool parseAccelerationLoad(std::vector<std::string>& tokens, XMLGen::Load& new_load);
    bool parseHeatFluxLoad(std::vector<std::string>& tokens, XMLGen::Load& new_load);
    bool parseForceLoad(std::vector<std::string>& tokens, XMLGen::Load& new_load);
    bool parseMeshSetNameOrID(size_t& aTokenIndex, std::vector<std::string>& tokens, XMLGen::Load& new_load);
    void getTokensFromLine(std::istream &fin, std::vector<std::string>& tokens);
    void parseBCs(std::istream &fin);
    void parseBCsBlock(std::istream &fin);
    void parseBCLine(std::vector<std::string>& tokens);
    void parseDisplacementBC(std::vector<std::string>& tokens, XMLGen::BC& new_bc);
    void parseTemperatureBC(std::vector<std::string>& tokens, XMLGen::BC& new_bc);
    // bool runSROMForUncertainVariables();
    // void setNumPerformers();
    void parseInputFile();
    bool parseMesh(std::istream &fin);
    // bool parseOutput(std::istream &fin);
    bool parseCodePaths(std::istream &fin);
    void parseMaterials(std::istream &fin);
    void parseCriteria(std::istream &fin);
    bool parseBlocks(std::istream &fin);
    // bool parseObjectives(std::istream &fin);
    // bool fillObjectiveAndPerfomerNames();
    // bool parseConstraints(std::istream &fin);
    // bool parseOptimizationParameters(std::istream &fin);
    // bool parseUncertainties(std::istream &fin);
    bool parseTokens(char *buffer, std::vector<std::string> &tokens);
    // void getUncertaintyFlags();
    // void outputOutputToFileStageForNewUncertaintyWorkflow(pugi::xml_document &doc,
    //                              bool &aHasUncertainties,
    //                              bool &aRequestedVonMises);
    std::string toLower(const std::string &s);
    std::string toUpper(const std::string &s);
    // bool find_tokens(std::vector<std::string> &tokens, const int &start_index, const char *str1, const char *str2);
    bool parseSingleValue(const std::vector<std::string> &aTokens,
                          const std::vector<std::string> &aInputStrings,
                          std::string &aReturnStringValue);

    bool parseSingleUnLoweredValue(const std::vector<std::string> &aTokens,
                                   const std::vector<std::string> &aUnLoweredTokens,
                                   const std::vector<std::string> &aInputStrings,
                                   std::string &aReturnStringValue);
    // void lookForPlatoAnalyzePerformers();
    // bool checkForNodesetSidesetNameConflicts();

    std::string m_InputFilename;
    XMLGen::InputData m_InputData;
private:

    // /******************************************************************************//**
    //  * @brief Initialize Plato problem options
    //  **********************************************************************************/
    // void initializePlatoProblemOptions();

    // void putLoadInLoadCase(XMLGen::Load& new_load);
    // bool putLoadInLoadCaseWithMatchingID(XMLGen::Load& new_load);
    // void createNewLoadCase(XMLGen::Load& new_load);
    // void writeInputFiles();
};

}

#endif /* SRC_XMLGENERATOR_HPP_ */
