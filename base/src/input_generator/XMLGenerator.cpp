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
 * XMLGenerator.cpp
 *
 *  Created on: Sep 28, 2017
 *
 */

#include <set>
#include <string>
#include <iostream>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
#include <utility>
#include <string>
#include <map>
#include "XMLGenerator.hpp"
#include "XMLGeneratorUtilities.hpp"
#include "Plato_SolveUncertaintyProblem.hpp"
#include "Plato_UniqueCounter.hpp"
#include "Plato_Vector3DVariations.hpp"
#include "PlatoAnalyzeInputDeckWriter.hpp"
#include "SalinasInputDeckWriter.hpp"
#include "Plato_FreeFunctions.hpp"
#include "Plato_SromXMLUtils.hpp"
#include "Plato_SromXML.hpp"
#include "XMLG_Macros.hpp"
//#include "InterfaceXMLWriter.hpp"

namespace XMLGen
{


const int MAX_CHARS_PER_LINE = 10000;
const int MAX_TOKENS_PER_LINE = 5000;
const char* const DELIMITER = " \t";

void PrintUnrecognizedTokens(const std::vector<std::string> & unrecognizedTokens)
{
       std::cout << "Did not recognize: ";
       for (size_t ind = 0 ; ind < unrecognizedTokens.size(); ++ind)
               std::cout << unrecognizedTokens[ind].c_str() << " " ;
        std::cout << "\n";
}

/******************************************************************************/
XMLGenerator::XMLGenerator(const std::string &input_filename, bool use_launch, const XMLGen::Arch& arch) :
        m_InputFilename(input_filename),
        m_UseLaunch(use_launch),
        m_Arch(arch),
        m_InputData(),
        m_filterType_identity_generatorName("identity"),
        m_filterType_identity_XMLName("Identity"),
        m_filterType_kernel_generatorName("kernel"),
        m_filterType_kernel_XMLName("Kernel"),
        m_filterType_kernelThenHeaviside_generatorName("kernel then heaviside"),
        m_filterType_kernelThenHeaviside_XMLName("KernelThenHeaviside"),
        m_filterType_kernelThenTANH_generatorName("kernel then tanh"),
        m_filterType_kernelThenTANH_XMLName("KernelThenTANH")
/******************************************************************************/
{
  m_HasUncertainties = false;
  m_RequestedVonMisesOutput = false;
  m_UseNewPlatoAnalyzeUncertaintyWorkflow = false;
}

/******************************************************************************/
XMLGenerator::~XMLGenerator() 
/******************************************************************************/
{
}

/******************************************************************************/
bool XMLGenerator::runSROMForUncertainVariables()
/******************************************************************************/
{
    if(m_InputData.uncertainties.size() > 0)
    {
        std::vector<XMLGen::LoadCase> tNewLoadCases;
        std::vector<double> tLoadCaseProbabilities;
        if(m_InputData.objectives.size() > 1)
        {
          std::cout << "ERROR: Only one objective is supported for uncertain loads" << std::endl;
          return false;
        }
        if(m_InputData.objectives[0].code_name == "plato_analyze" && !m_UseNewPlatoAnalyzeUncertaintyWorkflow)
        {
          if(m_InputData.objectives[0].atmost_total_num_processors < m_InputData.uncertainties[0].num_samples)
          {
            std::cout << "Number of processors must be equal to number of samples "
                      << "with legacy plato analyze uncerainty workflow" << std::endl;
            return false;
          }
        }
        if(m_InputData.objectives[0].weight != "1")
        {
          std::cout << "Objective weight must be equal to 1 for uncertain loads" << std::endl;
        }

        std::vector<XMLGen::LoadCase> tCurObjLoadCases;
        std::vector<XMLGen::Uncertainty> tCurObjUncertainties;
        XMLGen::Objective &tCurObj = m_InputData.objectives[0];
        for(size_t j=0; j<tCurObj.load_case_ids.size(); ++j)
        {
            const std::string &tCurID = tCurObj.load_case_ids[j];
            for(size_t k=0; k<m_InputData.load_cases.size(); ++k)
            {
                if(m_InputData.load_cases[k].id == tCurID)
                {
                    tCurObjLoadCases.push_back(m_InputData.load_cases[k]);
                    k=m_InputData.load_cases.size();
                }
            }
            for(size_t k=0; k<m_InputData.uncertainties.size(); ++k)
            {
                if(m_InputData.uncertainties[k].id == tCurID)
                {
                  tCurObjUncertainties.push_back(m_InputData.uncertainties[k]);
                  mObjectiveLoadCaseIndexToUncertaintyIndex[j] = k;
                }
            }
        }

        std::vector<Plato::srom::Load> tLoads;
        Plato::generate_srom_load_inputs(tCurObjLoadCases,tCurObjUncertainties,tLoads);

        if(tCurObjUncertainties.size() > 0)
        {
            Plato::srom::InputMetaData tInputs;
            tInputs.mLoads = tLoads;
            Plato::srom::OutputMetaData tOutputs;
            Plato::generate_load_sroms(tInputs, tOutputs);

            int tStartingLoadCaseID = tNewLoadCases.size() + 1;
            tCurObj.load_case_ids.clear();
            tCurObj.load_case_weights.clear();
            for(size_t j=0; j<tOutputs.mLoadCases.size(); ++j)
            {
                XMLGen::LoadCase tNewLoadCase;
                tNewLoadCase.id = Plato::to_string(tStartingLoadCaseID);
                for(size_t k=0; k<tOutputs.mLoadCases[j].mLoads.size(); ++k)
                {
                    XMLGen::Load tNewLoad;
                    tNewLoad.type = tOutputs.mLoadCases[j].mLoads[k].mLoadType;
                    tNewLoad.app_type = tOutputs.mLoadCases[j].mLoads[k].mAppType;
                    tNewLoad.app_id = Plato::to_string(tOutputs.mLoadCases[j].mLoads[k].mAppID);
                    tNewLoad.app_name = tOutputs.mLoadCases[j].mLoads[k].mAppName;
                    for(size_t h=0; h<tOutputs.mLoadCases[j].mLoads[k].mLoadValues.size(); ++h)
                        tNewLoad.values.push_back(Plato::to_string(tOutputs.mLoadCases[j].mLoads[k].mLoadValues[h]));
                    tNewLoad.load_id = Plato::to_string(tOutputs.mLoadCases[j].mLoads[k].mLoadID);
                    tNewLoadCase.loads.push_back(tNewLoad);
                }
                tNewLoadCases.push_back(tNewLoadCase);
                tLoadCaseProbabilities.push_back(tOutputs.mLoadCases[j].mProbability);
                tCurObj.load_case_ids.push_back(Plato::to_string(tStartingLoadCaseID));
                tCurObj.load_case_weights.push_back("1");
                tStartingLoadCaseID++;
            }
        }
        else
        {
            std::cout << "Objective has no associated uncertainty" << std::endl;
            return false;
        }

        m_InputData.load_cases = tNewLoadCases;
        m_InputData.load_case_probabilities = tLoadCaseProbabilities;
        if(!generateUncertaintyMetaData())
          return false;
    }


    return true;
}

/******************************************************************************/
bool XMLGenerator::generateUncertaintyMetaData()
/******************************************************************************/
{
  if(!getSizeOfLoadCases())
    return false;
  if(!getIndicesOfDeterministicAndRandomVariables())
    return false;
  m_UncertaintyMetaData.numSamples = m_InputData.load_cases.size();
  if(m_UseNewPlatoAnalyzeUncertaintyWorkflow)
    m_UncertaintyMetaData.numPeformers = std::atoi(m_InputData.objectives[0].num_ranks.c_str());
  else
    m_UncertaintyMetaData.numPeformers = std::atoi(m_InputData.objectives[0].atmost_total_num_processors.c_str());
  if(m_UncertaintyMetaData.numSamples % m_UncertaintyMetaData.numPeformers != 0)
  {
    std::cout << "Number of samples must divide evenly into number of processors" << std::endl;
    return false;
  }
    
  return true;
}

/******************************************************************************/
bool XMLGenerator::getSizeOfLoadCases()
/******************************************************************************/
{
  std::vector<XMLGen::LoadCase> tLoadCases = m_InputData.load_cases;

  size_t tLoadCaseSize = tLoadCases[0].loads.size();

  for(auto load_case:tLoadCases)
  {
    std::vector<XMLGen::Load> tLoads = load_case.loads;
    if(tLoads.size() != tLoadCaseSize)
    {
      std::cout << "Error: Uncertainty workflow only supports load cases that all have the same size" << std::endl;
      return false;
    }
  }

  m_UncertaintyMetaData.numVariables = tLoadCaseSize;

  return true;
}

/******************************************************************************/
bool XMLGenerator::getIndicesOfDeterministicAndRandomVariables()
/******************************************************************************/
{
  std::vector<XMLGen::LoadCase> tLoadCases = m_InputData.load_cases;
  if(tLoadCases.size() == 0)
  {
    std::cout << "Error: Must have more than zero samples" << std::endl;
  }

  // only one sample so every load is "deterministic"
  if(tLoadCases.size() == 1)
  {
    for(size_t i = 0; i < m_UncertaintyMetaData.numVariables; ++i)
      m_UncertaintyMetaData.deterministicVariableIndices.push_back(i);
    return true;
  }

  XMLGen::LoadCase tLoadCaseZero = tLoadCases[0];
  XMLGen::LoadCase tLoadCaseOne = tLoadCases[1];

  for(size_t i = 0; i < m_UncertaintyMetaData.numVariables; ++i)
  {
    XMLGen::Load tLoadZero = tLoadCaseZero.loads[i];
    XMLGen::Load tLoadOne = tLoadCaseOne.loads[i];

    if(tLoadZero.app_name != tLoadOne.app_name)
    {
      std::cout << "Error: Uncertainty workflow only supports load cases that all have the same format" << std::endl;
      return false;
    }

    if(tLoadZero.values == tLoadOne.values)
      m_UncertaintyMetaData.deterministicVariableIndices.push_back(i);
    else
      m_UncertaintyMetaData.randomVariableIndices.push_back(i);
  }

  verifyDeterministicLoadsHaveSameValuesAcrossAllLoadCases();
  verifyEachLoadHaSSameAppNameAcrossAllLoadCases();
  verifyEachLoadIsATractionLoad();

  return true;
}

/******************************************************************************/
bool XMLGenerator::verifyEachLoadIsATractionLoad()
/******************************************************************************/
{
  std::vector<XMLGen::LoadCase> tLoadCases = m_InputData.load_cases;
  for(auto load_case:tLoadCases)
  {
    for(auto load:load_case.loads)
    {
      if(load.type != "traction")
      {
        std::cout << "ERROR: Only traction loads with uncertainty are currently supported in new uncertainty workflow" << std::endl;
        return false;
      }
    }
  }
  return true;
}

/******************************************************************************/
bool XMLGenerator::verifyDeterministicLoadsHaveSameValuesAcrossAllLoadCases()
/******************************************************************************/
{
  std::vector<XMLGen::LoadCase> tLoadCases = m_InputData.load_cases;
  for(auto deterministicVariableIndex:m_UncertaintyMetaData.deterministicVariableIndices)
  {
    std::vector<std::string> tDeterministicLoadValues = tLoadCases[0].loads[deterministicVariableIndex].values;
    for(auto load_case:tLoadCases)
    {
      XMLGen::Load tDeterministicLoad = load_case.loads[deterministicVariableIndex];
      if(tDeterministicLoad.values != tDeterministicLoadValues)
      {
        std::cout << "Error: Uncertainty workflow only supports load cases that all have the same format" << std::endl;
        return false;
      }
    }
  }
  return true;
}

/******************************************************************************/
bool XMLGenerator::verifyEachLoadHaSSameAppNameAcrossAllLoadCases()
/******************************************************************************/
{
  std::vector<XMLGen::LoadCase> tLoadCases = m_InputData.load_cases;
  for(size_t loadIndex = 0; loadIndex < m_UncertaintyMetaData.numVariables; ++loadIndex)
  {
    std::string tAppName = tLoadCases[0].loads[loadIndex].app_name;
    for(auto load_case:tLoadCases)
    {
      XMLGen::Load tLoad = load_case.loads[loadIndex];
      if(tLoad.app_name != tAppName)
      {
        std::cout << "Error: Uncertainty workflow only supports load cases that all have the same format" << std::endl;
        return false;
      }
    }
  }
  return true;
}

/******************************************************************************/
bool XMLGenerator::parseCSMFile()
/******************************************************************************/
{
    bool tRet = true;

    std::ifstream tInputStream;
    tInputStream.open(m_InputData.csm_filename.c_str());
    if(tInputStream.good())
    {
        tRet = parseCSMFileFromStream(tInputStream);
        tInputStream.close();
    }
    else
        tRet = false;

    // Build a tesselation and exodus filenames from the csm filename
    size_t tPosition = m_InputData.csm_filename.find(".csm");
    if(tPosition != std::string::npos)
    {
        m_InputData.csm_tesselation_filename = m_InputData.csm_filename.substr(0, tPosition);
        m_InputData.csm_tesselation_filename += ".eto";
        m_InputData.csm_exodus_filename = m_InputData.csm_filename.substr(0, tPosition);
        m_InputData.csm_exodus_filename += ".exo";
    }
    else
    {
        std::cout << "\n\nError: CSM filename did not have .csm extension\n\n";
        tRet = false;
    }

    return tRet;
}

/******************************************************************************/
bool XMLGenerator::parseCSMFileFromStream(std::istream &aStream)
/******************************************************************************/
{
    bool tRet = true;

    char tBuffer[MAX_CHARS_PER_LINE];

    // read each line of the file (could optimize this to not read the whole file)
    while(!aStream.eof())
    {
        // read an entire line into memory
        aStream.getline(tBuffer, MAX_CHARS_PER_LINE);

        char *tCharPointer = std::strtok(tBuffer, " ");

        // skip comments
        if(tCharPointer && tCharPointer[0] == '#')
            continue;

        if(tCharPointer && std::strcmp(tCharPointer, "despmtr") == 0)
        {
            // Get the variable name
            tCharPointer = std::strtok(0, " ");
            if(!tCharPointer)
                break;
            // Get the variable value
            tCharPointer = std::strtok(0, " ");
            m_InputData.mShapeDesignVariableValues.push_back(tCharPointer);
            m_InputData.num_shape_design_variables++;
        }
    }

    return tRet;
}

/******************************************************************************/
bool XMLGenerator::generate()
/******************************************************************************/
{
    if(!parseFile())
    {
        std::cout << "Failed to parse input file." << std::endl;
        return false;
    }

    if(m_InputData.optimization_type == "shape" && m_InputData.csm_filename.length() > 0)
    {
        if(!parseCSMFile())
        {
            std::cout << "Failed to parse CSM file" << std::endl;
            return false;
        }
    }

    getUncertaintyFlags(m_InputData, m_HasUncertainties, m_RequestedVonMisesOutput);

    if(!runSROMForUncertainVariables())
    {
        std::cout << "Failed to expand uncertainties in file generation" << std::endl;
        return false;
    }

    // NOTE: modifies objectives to resolves distribution
    if(!distributeObjectivesForGenerate())
    {
        std::cout << "Failed to distribute objectives in file generation" << std::endl;
        return false;
    }

    lookForPlatoAnalyzePerformers();

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow)
    {

      if(!generateDefinesXML())
      {
        std::cout << "Failed to generate defines.xml" << std::endl;
        return false;
      }
    }

    if(m_InputData.optimization_type == "shape" && m_InputData.mPlatoAnalyzePerformerExists)
    {
        if(!generatePlatoAnalyzeShapeDefinesXML())
        {
            std::cout << "Failed to generate defines.xml" << std::endl;
            return false;
        }

        if(!generatePlatoAnalyzeShapeInterfaceXML())
        {
            std::cout << "Failed to generate interface.xml" << std::endl;
            return false;
        }
    }
    else
    {
        if(!generateInterfaceXML())
        {
            std::cout << "Failed to generate interface.xml" << std::endl;
            return false;
        }
    }

    if(m_InputData.optimization_type == "shape" && m_InputData.mPlatoAnalyzePerformerExists)
    {
        if(!generatePlatoMainOperationsXMLForShape())
        {
            std::cout << "Failed to generate plato_main_operations.xml" << std::endl;
            return false;
        }
    }
    else
    {
        if(!generatePlatoMainOperationsXML())
        {
            std::cout << "Failed to generate plato_main_operations.xml" << std::endl;
            return false;
        }
    }

    if(!generatePlatoMainInputDeckXML())
    {
        std::cout << "Failed to generate plato_main_input_deck.xml" << std::endl;
        return false;
    }

    if(!generatePerformerOperationsXML())
    {
        std::cout << "Failed to generate performer_operations.xml" << std::endl;
        return false;
    }

    if(!generatePhysicsInputDecks())
    {
        std::cout << "Failed to generate physics input decks" << std::endl;
        return false;
    }

    if(m_InputData.optimization_type == "shape" && m_InputData.mPlatoAnalyzePerformerExists)
    {
        if(!generatePlatoESPInputDeckXML())
        {
            std::cout << "Failed to generate plato_esp_input_deck.xml" << std::endl;
            return false;
        }
    }

    if(m_InputData.optimization_type == "shape" && m_InputData.mPlatoAnalyzePerformerExists)
    {
        if(!generatePlatoAnalyzeShapeLaunchScript())
        {
            std::cout << "Failed to generate mpirun.source" << std::endl;
            return false;
        }
    }
    else
    {
        if(!generateLaunchScript())
        {
            std::cout << "Failed to generate mpirun.source" << std::endl;
            return false;
        }
    }

    std::cout << "Successfully wrote XML files." << std::endl;
    return true;
}

/******************************************************************************/
void XMLGenerator::lookForPlatoAnalyzePerformers()
/******************************************************************************/
{
    m_InputData.mPlatoAnalyzePerformerExists = false;
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        if(m_InputData.objectives[i].code_name == "plato_analyze")
        {
            m_InputData.mPlatoAnalyzePerformerExists = true;
            break;
        }
    }
}

/******************************************************************************/
bool XMLGenerator::distributeObjectivesForGenerate()
/******************************************************************************/
{
    // for each objective, consider if should distribute
    size_t objective_index = 0u;
    if(m_HasUncertainties && m_InputData.objectives.size() != 1)
    {
      std::cout << "Uncertainty problems only support one objective" << std::endl;
      return false;
    }
    while(objective_index < m_InputData.objectives.size())
    {
        const std::string thisObjective_distributeType = m_InputData.objectives[objective_index].distribute_objective_type;
        if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && thisObjective_distributeType != "")
        {
          std::cout << "Objective distribution is not supported with new plato analyze uncertainty workflow" << std::endl;
          return false;
        }
        if(thisObjective_distributeType == "")
        {
            // no distribute; nothing to do for this objective.
        }
        else if(thisObjective_distributeType == "atmost")
        {
            // distribute by "atmost" rule

            // get inputs to distributed
            const size_t total_number_of_tasks = m_InputData.objectives[objective_index].load_case_ids.size();
            const int num_processors_in_group = std::atoi(m_InputData.objectives[objective_index].num_procs.c_str());
            const int atmost_processor_count =
                    std::atoi(m_InputData.objectives[objective_index].atmost_total_num_processors.c_str());
            if(num_processors_in_group <= 0 || atmost_processor_count <= 0)
            {
                std::cout << "ERROR:XMLGenerator:distributeObjectives: read a non-positive processor count.\n";
                return false;
            }

            // divide up distributed objective
            const size_t num_distributed_objectives = Plato::divide_up_atmost_processors(total_number_of_tasks,
                                                                                         num_processors_in_group,
                                                                                         atmost_processor_count);
            // store original load ids and weights
            const std::vector<std::string> orig_load_case_ids = m_InputData.objectives[objective_index].load_case_ids;
            const std::vector<std::string> orig_load_case_weights = m_InputData.objectives[objective_index].load_case_weights;

            const size_t num_loads_this_original_objective = orig_load_case_ids.size();
            if(num_loads_this_original_objective != orig_load_case_weights.size())
            {
                std::cout << "ERROR:XMLGenerator:distributeObjectives: "
                          << "Found length mismatch between load case ids and weights.\n";
                return false;
            }

            // clear load ids/weights in preparation for re-allocation
            m_InputData.objectives[objective_index].load_case_ids.clear();
            m_InputData.objectives[objective_index].load_case_weights.clear();

            // clear distribute type in preparation for completion
            m_InputData.objectives[objective_index].distribute_objective_type = "";
            m_InputData.objectives[objective_index].atmost_total_num_processors = "";

            // pushback indices for this distributed objective
            std::vector<size_t> fromOriginal_newDistributed_objectiveIndices(1u, objective_index);
            for(size_t distributed_index = 1u; distributed_index < num_distributed_objectives; distributed_index++)
            {
                fromOriginal_newDistributed_objectiveIndices.push_back(m_InputData.objectives.size());
                m_InputData.objectives.push_back(m_InputData.objectives[objective_index]);
            }

            // stride load case ids and weights across distributed objectives
            size_t strided_distributed_index = 0u;
            for(size_t abstract_load_id = 0u; abstract_load_id < num_loads_this_original_objective; abstract_load_id++)
            {
                // resolve strided index to objective index
                const size_t this_distributed_objectiveIndex =
                        fromOriginal_newDistributed_objectiveIndices[strided_distributed_index];

                // transfer ids/weights
                m_InputData.objectives[this_distributed_objectiveIndex].load_case_ids.push_back(orig_load_case_ids[abstract_load_id]);
                m_InputData.objectives[this_distributed_objectiveIndex].load_case_weights.push_back(orig_load_case_weights[abstract_load_id]);

                // remove name in distributed
                m_InputData.objectives[this_distributed_objectiveIndex].name="";
                m_InputData.objectives[this_distributed_objectiveIndex].performer_name="";

                // advance distributed index
                strided_distributed_index = (strided_distributed_index + 1) % num_distributed_objectives;
            }
        }

        // advance considered objective
        objective_index++;
    }

    // assign names
    const bool filling_did_pass = fillObjectiveAndPerfomerNames();
    if(!filling_did_pass)
    {
        std::cout << "ERROR:XMLGenerator:distributeObjectives: Failed to fill objective and performer names.\n";
        return false;
    }

    return true;
}

/******************************************************************************/
bool XMLGenerator::generateLaunchScript()
/******************************************************************************/
{
    if(m_Arch == XMLGen::Arch::SUMMIT)
      generateSummitLaunchScripts();
    else
    {
      FILE *fp=fopen("mpirun.source", "w");
      if(!fp)
      {
          return true;
      }

      std::string num_opt_procs = "1";
      if(!m_InputData.num_opt_processors.empty())
          num_opt_procs = m_InputData.num_opt_processors;

      // For restarts where we need to call prune_and_refine to do a variable transfer, prune, refine, or
      // any combination of the above we need to add an mpirun call for this first so it will run as
      // a pre-processor on the input mesh.
      int tNumRefines = 0;
      if(m_InputData.number_refines != "")
          tNumRefines = std::atoi(m_InputData.number_refines.c_str());
      if(tNumRefines > 0 ||
              (m_InputData.initial_guess_filename != "" && m_InputData.initial_guess_field_name != ""))
      {
          // Determine how many processors to use for the prune_and_refine run.
          std::string tNumberPruneAndRefineProcsString = "1";
          int tNumberPruneAndRefineProcs = 1;
          if(m_InputData.number_prune_and_refine_processors != "" &&
                  m_InputData.number_prune_and_refine_processors != "0")
          {
              tNumberPruneAndRefineProcsString = m_InputData.number_prune_and_refine_processors;
              tNumberPruneAndRefineProcs = std::atoi(tNumberPruneAndRefineProcsString.c_str());
          }
          else
          {
              // Find the max number of objective procs.
              for(size_t i=0; i<m_InputData.objectives.size(); ++i)
              {
                  if(!m_InputData.objectives[i].num_procs.empty())
                  {
                      int tNumProcs = std::atoi(m_InputData.objectives[i].num_procs.c_str());
                      if(tNumProcs > tNumberPruneAndRefineProcs)
                      {
                          tNumberPruneAndRefineProcsString = m_InputData.objectives[i].num_procs;
                          tNumberPruneAndRefineProcs = tNumProcs;
                      }
                  }
              }
          }

          // First decompose the files that will be involved below
          if(tNumberPruneAndRefineProcs > 1)
          {
              fprintf(fp, "decomp -p %d %s\n", tNumberPruneAndRefineProcs, m_InputData.mesh_name.c_str());
              if(m_InputData.initial_guess_filename != "")
                  fprintf(fp, "decomp -p %d %s\n", tNumberPruneAndRefineProcs, m_InputData.initial_guess_filename.c_str());
          }

          std::string tPruneString = "0";
          std::string tNumRefinesString = "0";
          std::string tNumBufferLayersString = "2";
          if(m_InputData.prune_mesh == "true")
              tPruneString = "1";
          if(m_InputData.number_refines != "")
              tNumRefinesString = m_InputData.number_refines;
          if(m_InputData.number_buffer_layers != "")
              tNumBufferLayersString = m_InputData.number_buffer_layers;

          std::string tCommand;
          std::string tPruneAndRefineExe = "prune_and_refine";
          if(m_InputData.prune_and_refine_path.length() > 0)
              tPruneAndRefineExe = m_InputData.prune_and_refine_path;
          if(m_UseLaunch)
              tCommand = "launch -n " + tNumberPruneAndRefineProcsString + " " + tPruneAndRefineExe;
          else
              tCommand = "mpiexec -np " + tNumberPruneAndRefineProcsString + " " + tPruneAndRefineExe;
          if(m_InputData.initial_guess_filename != "")
              tCommand += (" --mesh_with_variable=" + m_InputData.initial_guess_filename);
          tCommand += (" --mesh_to_be_pruned=" + m_InputData.mesh_name);
          tCommand += (" --result_mesh=" + m_InputData.run_mesh_name);
          if(m_InputData.initial_guess_field_name != "")
              tCommand += (" --field_name=" + m_InputData.initial_guess_field_name);
          tCommand += (" --number_of_refines=" + tNumRefinesString);
          tCommand += (" --number_of_buffer_layers=" + tNumBufferLayersString);
          tCommand += (" --prune_mesh=" + tPruneString);

          fprintf(fp, "%s\n", tCommand.c_str());

          // Now concatenate the input mesh file again since we don't know what other decompositions will be needed.
          if(tNumberPruneAndRefineProcs > 1)
          {
              // Build the extension string we will need.
              std::string tExtensionString = "." + tNumberPruneAndRefineProcsString + ".";
              for(size_t g=0; g<tNumberPruneAndRefineProcsString.length(); ++g)
                  tExtensionString += "0";
              fprintf(fp, "epu -auto %s%s\n", m_InputData.run_mesh_name.c_str(), tExtensionString.c_str());
          }
      }

      // remember if the run_mesh has been decomposed to this processor count
      std::map<std::string,int> hasBeenDecompedToThisCount;

      // Now do the decomps for the TO run.
      if(num_opt_procs.compare("1") != 0) {
          if(++hasBeenDecompedToThisCount[num_opt_procs] == 1)
          {
              fprintf(fp, "decomp -p %s %s\n", num_opt_procs.c_str(), m_InputData.run_mesh_name.c_str());
          }
      }
      if(m_InputData.initial_guess_filename != "" && num_opt_procs.compare("1") != 0)
          fprintf(fp, "decomp -p %s %s\n", num_opt_procs.c_str(), m_InputData.initial_guess_filename.c_str());
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
          std::string num_procs = "4";
          if(!m_InputData.objectives[i].num_procs.empty())
              num_procs = m_InputData.objectives[i].num_procs;
          if(num_procs.compare("1") != 0)
          {
              if(++hasBeenDecompedToThisCount[num_procs] == 1)
              {
                  fprintf(fp, "decomp -p %s %s\n", num_procs.c_str(), m_InputData.run_mesh_name.c_str());
              }
              if(m_InputData.objectives[i].ref_frf_file.length() > 0)
                  fprintf(fp, "decomp -p %s %s\n", num_procs.c_str(), m_InputData.objectives[i].ref_frf_file.c_str());
          }
      }

#ifndef USING_OPEN_MPI
      std::string envString = "-env";
      std::string separationString = " ";
#else
      std::string envString = "-x";
      std::string separationString = "=";
#endif

      std::string tLaunchString = "";
      std::string tNumProcsString = "";
      if(m_UseLaunch)
      {
          tLaunchString = "launch";
          tNumProcsString = "-n";
      }
      else
      {
          tLaunchString = "mpiexec";
          tNumProcsString = "-np";
      }

      if(m_InputData.optimization_type == "shape")
      {
          fprintf(fp, "python aflr.py %s %s %s ", m_InputData.csm_filename.c_str(), m_InputData.csm_exodus_filename.c_str(), m_InputData.csm_tesselation_filename.c_str());
          for(size_t i=0; i<m_InputData.mShapeDesignVariableValues.size(); ++i)
              fprintf(fp, "%s ", m_InputData.mShapeDesignVariableValues[i].c_str());
          fprintf(fp, "\n");
      }

      // Now add the main mpirun call.
      fprintf(fp, "%s %s %s %s PLATO_PERFORMER_ID%s0 \\\n", tLaunchString.c_str(), tNumProcsString.c_str(), num_opt_procs.c_str(), envString.c_str(),separationString.c_str());
      fprintf(fp, "%s PLATO_INTERFACE_FILE%sinterface.xml \\\n", envString.c_str(),separationString.c_str());
      fprintf(fp, "%s PLATO_APP_FILE%splato_main_operations.xml \\\n", envString.c_str(),separationString.c_str());
      if(m_InputData.plato_main_path.length() != 0)
          fprintf(fp, "%s plato_main_input_deck.xml \\\n", m_InputData.plato_main_path.c_str());
      else
          fprintf(fp, "plato_main plato_main_input_deck.xml \\\n");
      if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
      {

          fprintf(fp, ": %s %s %s PLATO_PERFORMER_ID%s1 \\\n", tNumProcsString.c_str(), Plato::to_string(m_UncertaintyMetaData.numPeformers).c_str(), envString.c_str(),separationString.c_str());
          fprintf(fp, "%s PLATO_INTERFACE_FILE%sinterface.xml \\\n", envString.c_str(),separationString.c_str());
          fprintf(fp, "%s PLATO_APP_FILE%splato_analyze_operations.xml \\\n", envString.c_str(),separationString.c_str());
        if(m_InputData.plato_analyze_path.length() != 0)
          fprintf(fp, "%s --input-config=plato_analyze_input_deck.xml \\\n", m_InputData.plato_analyze_path.c_str());
        else
          fprintf(fp, "analyze_MPMD --input-config=plato_analyze_input_deck.xml \\\n");
      }
      else
      {
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        {
          const XMLGen::Objective& cur_obj = m_InputData.objectives[i];
          if(!cur_obj.num_procs.empty())
          {
            fprintf(fp, ": %s %s %s PLATO_PERFORMER_ID%s%d \\\n", tNumProcsString.c_str(), cur_obj.num_procs.c_str(), envString.c_str(),separationString.c_str(), (int)(i+1));
          }
          else
            fprintf(fp, ": %s 4 %s PLATO_PERFORMER_ID%s%d \\\n",  tNumProcsString.c_str(), envString.c_str(),separationString.c_str(),(int)(i+1));
          fprintf(fp, "%s PLATO_INTERFACE_FILE%sinterface.xml \\\n", envString.c_str(),separationString.c_str());
          fprintf(fp, "%s PLATO_APP_FILE%s%s_operations_%s.xml \\\n", envString.c_str(),separationString.c_str(),cur_obj.code_name.c_str(),
              cur_obj.name.c_str());
          if(!cur_obj.code_name.compare("sierra_sd"))
          {
            if(m_InputData.sierra_sd_path.length() != 0)
              fprintf(fp, "%s sierra_sd_input_deck_%s.i \\\n", m_InputData.sierra_sd_path.c_str(), cur_obj.name.c_str());
            else
              fprintf(fp, "plato_sd_main sierra_sd_input_deck_%s.i \\\n", cur_obj.name.c_str());
          }
          else if(!cur_obj.code_name.compare("lightmp"))
          {
            if(m_InputData.lightmp_path.length() != 0)
              fprintf(fp, "%s lightmp_input_deck_%s.i \\\n", m_InputData.lightmp_path.c_str(), cur_obj.name.c_str());
          }
          else if(!cur_obj.code_name.compare("albany"))
          {
            if(m_InputData.albany_path.length() != 0)
              fprintf(fp, "%s albany_input_deck_%s.i \\\n", m_InputData.albany_path.c_str(), cur_obj.name.c_str());
            else
              fprintf(fp, "albany albany_input_deck_%s.i \\\n", cur_obj.name.c_str());
          }
          else if(!cur_obj.code_name.compare("plato_analyze"))
          {
            if(m_InputData.plato_analyze_path.length() != 0)
              fprintf(fp, "%s --input-config=plato_analyze_input_deck_%s.xml \\\n", m_InputData.plato_analyze_path.c_str(), cur_obj.name.c_str());
            else
              fprintf(fp, "analyze_MPMD --input-config=plato_analyze_input_deck_%s.xml \\\n", cur_obj.name.c_str());
          }
        }
      }

      fclose(fp);
    }
    return true;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoAnalyzeShapeLaunchScript()
/******************************************************************************/
{
    FILE *fp=fopen("mpirun.source", "w");
    if(!fp)
    {
        return true;
    }

    std::string num_opt_procs = "1";
    if(!m_InputData.num_opt_processors.empty())
        num_opt_procs = m_InputData.num_opt_processors;

    // remember if the run_mesh has been decomposed to this processor count
    std::map<std::string,int> hasBeenDecompedToThisCount;

    // Now do the decomps for the TO run.
    if(num_opt_procs.compare("1") != 0) 
    {
        if(++hasBeenDecompedToThisCount[num_opt_procs] == 1)
        {
            fprintf(fp, "decomp -p %s %s\n", num_opt_procs.c_str(), m_InputData.run_mesh_name.c_str());
        }
    }
    if(m_InputData.initial_guess_filename != "" && num_opt_procs.compare("1") != 0)
        fprintf(fp, "decomp -p %s %s\n", num_opt_procs.c_str(), m_InputData.initial_guess_filename.c_str());
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        std::string num_procs = "4";
        if(!m_InputData.objectives[i].num_procs.empty())
            num_procs = m_InputData.objectives[i].num_procs;
        if(num_procs.compare("1") != 0)
        {
            if(++hasBeenDecompedToThisCount[num_procs] == 1)
            {
                fprintf(fp, "decomp -p %s %s\n", num_procs.c_str(), m_InputData.run_mesh_name.c_str());
            }
            if(m_InputData.objectives[i].ref_frf_file.length() > 0)
                fprintf(fp, "decomp -p %s %s\n", num_procs.c_str(), m_InputData.objectives[i].ref_frf_file.c_str());
        }
    }

#ifndef USING_OPEN_MPI
    std::string envString = "-env";
    std::string separationString = " ";
#else
    std::string envString = "-x";
    std::string separationString = "=";
#endif

    std::string tLaunchString = "";
    std::string tNumProcsString = "";
    if(m_UseLaunch)
    {
        tLaunchString = "launch";
        tNumProcsString = "-n";
    }
    else
    {
        tLaunchString = "mpiexec";
        tNumProcsString = "-np";
    }

    fprintf(fp, "python aflr.py %s %s %s ", m_InputData.csm_filename.c_str(), m_InputData.csm_exodus_filename.c_str(), m_InputData.csm_tesselation_filename.c_str());
    for(size_t i=0; i<m_InputData.mShapeDesignVariableValues.size(); ++i)
        fprintf(fp, "%s ", m_InputData.mShapeDesignVariableValues[i].c_str());
    fprintf(fp, "\n");

    // Now add the main mpirun call.
    
    // PlatoMain
    fprintf(fp, "%s %s %s %s PLATO_PERFORMER_ID%s0 \\\n", tLaunchString.c_str(), tNumProcsString.c_str(), num_opt_procs.c_str(), envString.c_str(),separationString.c_str());
    fprintf(fp, "%s PLATO_INTERFACE_FILE%sinterface.xml \\\n", envString.c_str(),separationString.c_str());
    fprintf(fp, "%s PLATO_APP_FILE%splato_main_operations.xml \\\n", envString.c_str(),separationString.c_str());
    if(m_InputData.plato_main_path.length() != 0)
        fprintf(fp, "%s plato_main_input_deck.xml \\\n", m_InputData.plato_main_path.c_str());
    else
        fprintf(fp, "plato_main plato_main_input_deck.xml \\\n");

    // PlatoESP performers
    fprintf(fp, ": %s %d %s PLATO_PERFORMER_ID%s1 \\\n", tNumProcsString.c_str(), m_InputData.num_shape_design_variables, envString.c_str(),separationString.c_str());
    fprintf(fp, "%s PLATO_INTERFACE_FILE%sinterface.xml \\\n", envString.c_str(),separationString.c_str());
    fprintf(fp, "%s PLATO_APP_FILE%splato_esp_operations.xml \\\n", envString.c_str(),separationString.c_str());
    fprintf(fp, "PlatoESP plato_esp_input_deck.xml \\\n");

    // Physics performers
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        const XMLGen::Objective& cur_obj = m_InputData.objectives[i];
        if(!cur_obj.num_procs.empty())
            fprintf(fp, ": %s %s %s PLATO_PERFORMER_ID%s%d \\\n", tNumProcsString.c_str(), cur_obj.num_procs.c_str(), envString.c_str(),separationString.c_str(), (int)(i+2));
        else
            fprintf(fp, ": %s 4 %s PLATO_PERFORMER_ID%s%d \\\n",  tNumProcsString.c_str(), envString.c_str(),separationString.c_str(),(int)(i+2));
        fprintf(fp, "%s PLATO_INTERFACE_FILE%sinterface.xml \\\n", envString.c_str(),separationString.c_str());
        fprintf(fp, "%s PLATO_APP_FILE%s%s_operations_%s.xml \\\n", envString.c_str(),separationString.c_str(),cur_obj.code_name.c_str(),
              cur_obj.name.c_str());
        if(m_InputData.plato_analyze_path.length() != 0)
            fprintf(fp, "%s --input-config=plato_analyze_input_deck_%s.xml \\\n", m_InputData.plato_analyze_path.c_str(), cur_obj.name.c_str());
        else
            fprintf(fp, "analyze_MPMD --input-config=plato_analyze_input_deck_%s.xml \\\n", cur_obj.name.c_str());
    }

    fclose(fp);
    return true;
}
/******************************************************************************/
bool XMLGenerator::generateSummitLaunchScripts()
/******************************************************************************/
{
  if(m_InputData.objectives.size() > 99)
  {
    std::cout << "ERROR: Number of Samples must be less than 100" << std::endl;
    return false;
  }
  for(auto objective:m_InputData.objectives)
  {
    if(objective.code_name != "plato_analyze")
      std::cout << "ERROR: Summit output is only supported for Plato Analyze performers" << std::endl;
  }
  generatePerformerBashScripts();
  generateJSRunScript();
  generateBatchScript();

  return true;
}

/******************************************************************************/
void XMLGenerator::generatePerformerBashScripts()
/******************************************************************************/
{
  generateEngineBashScript();
  generateAnalyzeBashScripts();
}

/******************************************************************************/
void XMLGenerator::generateAnalyzeBashScripts()
/******************************************************************************/
{
  if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
  {
    std::ofstream analyzeBash;
    std::string filename = "analyze.sh";
    analyzeBash.open(filename);
    analyzeBash << "export PLATO_PERFORMER_ID=1\n";
    analyzeBash << "export PLATO_INTERFACE_FILE=interface.xml\n";
    analyzeBash << "export PLATO_APP_FILE=plato_analyze_operations.xml\n";
    analyzeBash << "\n";
    analyzeBash << "analyze_MPMD --input-config=plato_analyze_input_deck.xml\n";

    analyzeBash.close();
  }
  else
  {
    for(size_t i = 1; i <= m_InputData.objectives.size(); ++i)
    {
      std::ofstream analyzeBash;
      std::string filename = "analyze" + Plato::to_string(i) + ".sh";
      analyzeBash.open(filename);
      analyzeBash << "export PLATO_PERFORMER_ID=" << i << "\n";
      analyzeBash << "export PLATO_INTERFACE_FILE=interface.xml\n";
      analyzeBash << "export PLATO_APP_FILE=plato_analyze_operations_" << i << ".xml\n";
      analyzeBash << "\n";
      analyzeBash << "analyze_MPMD --input-config=plato_analyze_input_deck_" << i << ".xml\n";

      analyzeBash.close();
    }
  }
}

/******************************************************************************/
void XMLGenerator::generateEngineBashScript()
/******************************************************************************/
{
  std::ofstream engineBash;
  engineBash.open("engine.sh");
  engineBash << "export PLATO_PERFORMER_ID=0\n";
  engineBash << "export PLATO_INTERFACE_FILE=interface.xml\n";
  engineBash << "export PLATO_APP_FILE=plato_main_operations.xml\n";
  engineBash << "\n";
  engineBash << "PlatoMain plato_main_input_deck.xml";
  engineBash.close();
}

/******************************************************************************/
void XMLGenerator::generateJSRunScript()
/******************************************************************************/
{
  std::ofstream jsrun;
  jsrun.open("jsrun.source");
  jsrun << "1 : eng : bash engine.sh\n";
  if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
  {
    jsrun << Plato::to_string(m_UncertaintyMetaData.numPeformers) << " : per : bash analyze.sh\n";
  }
  else
  {
    for(size_t i = 1; i <= m_InputData.objectives.size(); ++i)
    {
      jsrun << "1 : per" << i << " : bash analyze" << i << ".sh\n";
    }
  }
  jsrun.close();
}

/******************************************************************************/
void XMLGenerator::generateBatchScript()
/******************************************************************************/
{
  std::ofstream batchFile;
  batchFile.open ("plato.batch");
  batchFile << "#!/bin/bash\n";
  batchFile << "# LSF Directives\n";
  batchFile << "#BSUB -P <PROJECT>\n";
  batchFile << "#BSUB -W 0:00\n";

  size_t tNumNodesNeeded = computeNumberOfNodesNeeded();

  batchFile << "#BSUB -nnodes " << tNumNodesNeeded << "\n";
  batchFile << "#BSUB -J plato\n";
  batchFile << "#BSUB -o plato.%J\n";
  batchFile << "\n";
  batchFile << "cd <path/to/working/directory>\n";
  batchFile << "date\n";
  batchFile << "jsrun -A eng -n1 -a1 -c1 -g0\n";

  if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
  {
    batchFile << "jsrun -A per -n" << Plato::to_string(m_UncertaintyMetaData.numPeformers) << " -a1 -c1 -g1\n";
  }
  else
  {
    for(size_t i = 1; i <= m_InputData.objectives.size(); ++i)
    {
      batchFile << "jsrun -A per" << i << " -n1 -a1 -c1 -g1\n";
    }
  }

  batchFile << "jsrun -f jsrun.source\n";
  

  batchFile.close();
}

/******************************************************************************/
size_t XMLGenerator::computeNumberOfNodesNeeded()
/******************************************************************************/
{
  size_t tNumGPUsNeeded;
  if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    tNumGPUsNeeded = m_UncertaintyMetaData.numPeformers;
  else
    tNumGPUsNeeded = m_InputData.objectives.size();
  size_t tNumGPUsPerNode = 6;
  size_t tNumNodesNeeded = tNumGPUsNeeded/tNumGPUsPerNode;
  if(tNumGPUsNeeded % tNumGPUsPerNode != 0)
    ++tNumNodesNeeded;
  return tNumNodesNeeded;
}

/******************************************************************************/
bool XMLGenerator::generateSalinasInputDecks(std::ostringstream *aStringStream)
/******************************************************************************/
{
    SalinasInputDeckWriter tWriter(m_InputData);
    tWriter.generate(m_HasUncertainties,m_RequestedVonMisesOutput,aStringStream);
    return true;
}
/******************************************************************************/
bool XMLGenerator::generateAlbanyInputDecks()
/******************************************************************************/
{
    char string_var[200];
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        const XMLGen::Objective& cur_obj = m_InputData.objectives[i];
        if(!cur_obj.code_name.compare("albany"))
        {
            char buf[200];
            sprintf(buf, "albany_input_deck_%s.i", cur_obj.name.c_str());
            pugi::xml_document doc;

            // Version entry
            pugi::xml_node tmp_node = doc.append_child(pugi::node_declaration);
            tmp_node.set_name("xml");
            pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
            tmp_att.set_value("1.0");

            pugi::xml_node n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12;
            pugi::xml_attribute a1, a2, a3;
            n1 = doc.append_child("ParameterList");
            n2 = n1.append_child("ParameterList");
            n2.append_attribute("name") = "Problem";
            if(!cur_obj.type.compare("maximize stiffness"))
                addNTVParameter(n2, "Name", "string", "LinearElasticity 3D");
            else if(!cur_obj.type.compare("maximize heat conduction"))
                addNTVParameter(n2, "Name", "string", "Poissons Equation 3D");

            // Topologies block
            n3 = n2.append_child("ParameterList");
            n3.append_attribute("name") = "Topologies";
            size_t num_topologies = 1;
            sprintf(string_var, "%d", (int)num_topologies);
            addNTVParameter(n3, "Number of Topologies", "int", string_var);
            for(size_t j=0; j<num_topologies; ++j)
            {
                n4 = n3.append_child("ParameterList");
                sprintf(string_var, "Topology %d", (int)j);
                n4.append_attribute("name") = string_var;
                addNTVParameter(n4, "Topology Name", "string", "Rho");
                addNTVParameter(n4, "Entity Type", "string", "State Variable");
                addNTVParameter(n4, "Bounds", "Array(double)", "{0.0,1.0}");
                addNTVParameter(n4, "Initial Value", "double", m_InputData.initial_density_value);
                n5 = n4.append_child("ParameterList");
                n5.append_attribute("name") = "Functions";
                size_t num_functions = 1;
                sprintf(string_var, "%d", (int)num_functions);
                addNTVParameter(n5, "Number of Functions", "int", string_var);
                for(size_t k=0; k<num_functions; ++k)
                {
                    n6 = n5.append_child("ParameterList");
                    sprintf(string_var, "Function %d", (int)k);
                    n6.append_attribute("name") = string_var;
                    addNTVParameter(n6, "Function Type", "string", "SIMP");
                    addNTVParameter(n6, "Minimum", "double", "0.001");
                    addNTVParameter(n6, "Penalization Parameter", "double", m_InputData.materials[0].penalty_exponent);
                }
                addNTVParameter(n4, "Spatial Filter", "int", "0");
            }

            // Configuration block
            n3 = n2.append_child("ParameterList");
            n3.append_attribute("name") = "Configuration";
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Element Blocks";
            size_t num_blocks = m_InputData.blocks.size();
            sprintf(string_var, "%d", (int)num_blocks);
            addNTVParameter(n4, "Number of Element Blocks", "int", string_var);
            for(size_t j=0; j<num_blocks; ++j)
            {
                n5 = n4.append_child("ParameterList");
                sprintf(string_var, "Element Block %d", (int)j);
                n5.append_attribute("name") = string_var;
                sprintf(string_var, "block_%s", m_InputData.blocks[j].block_id.c_str());
                addNTVParameter(n5, "Name", "string", string_var);
                n6 = n5.append_child("ParameterList");
                n6.append_attribute("name") = "Material";
                for(size_t n=0; n<m_InputData.materials.size(); n++)
                {
                    if(m_InputData.materials[n].material_id.compare(m_InputData.blocks[j].material_id) == 0)
                    {
                        if(!m_InputData.materials[n].youngs_modulus.empty())
                            addNTVParameter(n6, "Elastic Modulus", "double", m_InputData.materials[n].youngs_modulus);
                        if(!m_InputData.materials[n].poissons_ratio.empty())
                            addNTVParameter(n6, "Poissons Ratio", "double", m_InputData.materials[n].poissons_ratio);
                        if(!m_InputData.materials[n].thermal_conductivity.empty())
                            addNTVParameter(n6, "Isotropic Modulus", "double", m_InputData.materials[n].thermal_conductivity);
                        break;
                    }
                }
            }

            // Dirichlet BCs
            n3 = n2.append_child("ParameterList");
            n3.append_attribute("name") = "Dirichlet BCs";
            for(size_t j=0; j<cur_obj.bc_ids.size(); j++)
            {
                bool found = false;
                XMLGen::BC cur_bc;
                std::string cur_bc_id = cur_obj.bc_ids[j];
                for(size_t qq=0; qq<m_InputData.bcs.size(); ++qq)
                {
                    if(cur_bc_id == m_InputData.bcs[qq].bc_id)
                    {
                        found = true;
                        cur_bc = m_InputData.bcs[qq];
                    }
                }
                if(found)
                {
                    if(!cur_obj.type.compare("maximize stiffness"))
                    {
                        if(cur_bc.dof.empty())
                        {
                            // applying fixed boundary conditions to "sideset"s is currently supported
                            if(cur_bc.app_type != "nodeset")
                            {
                                std::cout << "ERROR:XMLGenerator:generateAlbanyInputDecks: "
                                          << "Albany boundary conditions can only be applied to \"nodeset\" types.\n";
                            }
                            else
                            {
                                sprintf(string_var, "DBC on NS nodelist_%s for DOF %s", cur_bc.app_id.c_str(), "X");
                                addNTVParameter(n3, string_var, "double", "0.0");
                                sprintf(string_var, "DBC on NS nodelist_%s for DOF %s", cur_bc.app_id.c_str(), "Y");
                                addNTVParameter(n3, string_var, "double", "0.0");
                                sprintf(string_var, "DBC on NS nodelist_%s for DOF %s", cur_bc.app_id.c_str(), "Z");
                                addNTVParameter(n3, string_var, "double", "0.0");
                            }
                        }
                        else
                        {
                            if(cur_bc.value.empty())
                            {
                                sprintf(string_var, "DBC on NS nodelist_%s for DOF %s",
                                        cur_bc.app_id.c_str(), cur_bc.dof.c_str());
                                addNTVParameter(n3, string_var, "double", "0.0");
                            }
                            else
                            {
                                sprintf(string_var, "DBC on NS nodelist_%s for DOF %s",
                                        cur_bc.app_id.c_str(), cur_bc.dof.c_str());
                                addNTVParameter(n3, string_var, "double", cur_bc.value);

                            }
                        }
                    }
                    else if(!cur_obj.type.compare("maximize heat conduction"))
                    {
                        if(cur_bc.value.empty())
                        {
                            sprintf(string_var, "DBC on NS nodelist_%s for DOF P",
                                    cur_bc.app_id.c_str());
                            addNTVParameter(n3, string_var, "double", "0.0");
                        }
                        else
                        {
                            sprintf(string_var, "DBC on NS nodelist_%s for DOF P",
                                    cur_bc.app_id.c_str());
                            addNTVParameter(n3, string_var, "double", cur_bc.value);
                        }
                    }
                }
            }

            // Neumann BCs
            n3 = n2.append_child("ParameterList");
            n3.append_attribute("name") = "Neumann BCs";
            for(size_t j=0; j<cur_obj.load_case_ids.size(); j++)
            {
                bool found = false;
                XMLGen::LoadCase cur_load_case;
                std::string cur_load_id = cur_obj.load_case_ids[j];
                for(size_t qq=0; qq<m_InputData.load_cases.size(); ++qq)
                {
                    if(cur_load_id == m_InputData.load_cases[qq].id)
                    {
                        found = true;
                        cur_load_case = m_InputData.load_cases[qq];
                    }
                }
                if(found)
                {
                    for(size_t e=0; e<cur_load_case.loads.size(); e++)
                    {
                        XMLGen::Load cur_load = cur_load_case.loads[e];
                        if(!cur_obj.type.compare("maximize stiffness"))
                        {
                            if(cur_load.type == "traction")
                            {
                                sprintf(string_var, "NBC on SS surface_%s for DOF all set (t_x, t_y, t_z)",
                                        cur_load.app_id.c_str());
                                char tmp_buf[200];
                                double x = std::atof(cur_load.values[0].c_str());
                                double y = std::atof(cur_load.values[1].c_str());
                                double z = std::atof(cur_load.values[2].c_str());
                                sprintf(tmp_buf, "{%lf,%lf,%lf}", x, y, z);
                                addNTVParameter(n3, string_var, "Array(double)", tmp_buf);
                            }
                        }
                        else if(!cur_obj.type.compare("maximize heat conduction"))
                        {
                            if(cur_load.type == "heat") // for "heat flux"
                            {
                                sprintf(string_var, "NBC on SS surface_%s for DOF P set (dudx, dudy, dudz)",
                                        cur_load.app_id.c_str());
                                char tmp_buf[200];
                                sprintf(tmp_buf, "{%s,0.0,0.0}", cur_load.values[0].c_str());
                                addNTVParameter(n3, string_var, "Array(double)", tmp_buf);
                            }
                        }
                    }
                }
            }

            // Topology Weights
            n3 = n2.append_child("ParameterList");
            n3.append_attribute("name") = "Apply Topology Weight Functions";
            size_t num_fields = 1;
            sprintf(string_var, "%d", (int)num_fields);
            addNTVParameter(n3, "Number of Fields", "int", string_var);
            for(size_t j=0; j<num_fields; ++j)
            {
                sprintf(string_var, "Field %d", (int)j);
                n4 = n3.append_child("ParameterList");
                n4.append_attribute("name") = string_var;
                if(!cur_obj.type.compare("maximize stiffness"))
                {
                    addNTVParameter(n4, "Name", "string", "Stress");
                    addNTVParameter(n4, "Layout", "string", "QP Tensor");
                }
                else if(!cur_obj.type.compare("maximize heat conduction"))
                {
                    addNTVParameter(n4, "Name", "string", "kinVar");
                    addNTVParameter(n4, "Layout", "string", "QP Vector");
                }
                addNTVParameter(n4, "Topology Index", "int", "0");
                addNTVParameter(n4, "Function Index", "int", "0");

            }

            // Response Functions
            n3 = n2.append_child("ParameterList");
            n3.append_attribute("name") = "Response Functions";
            size_t num_response_vectors = 1;
            sprintf(string_var, "%d", (int)num_response_vectors);
            addNTVParameter(n3, "Number of Response Vectors", "int", string_var);
            for(size_t j=0; j<num_fields; ++j)
            {
                sprintf(string_var, "Response Vector %d", (int)j);
                n4 = n3.append_child("ParameterList");
                n4.append_attribute("name") = string_var;
                addNTVParameter(n4, "Name", "string", "Stiffness Objective");
                if(!cur_obj.type.compare("maximize stiffness"))
                {
                    addNTVParameter(n4, "Gradient Field Name", "string", "Strain");
                    addNTVParameter(n4, "Gradient Field Layout", "string", "QP Tensor");
                    addNTVParameter(n4, "Work Conjugate Name", "string", "Stress");
                    addNTVParameter(n4, "Work Conjugate Layout", "string", "QP Tensor");
                }
                else if(!cur_obj.type.compare("maximize heat conduction"))
                {
                    addNTVParameter(n4, "Gradient Field Name", "string", "Phi Gradient");
                    addNTVParameter(n4, "Gradient Field Layout", "string", "QP Vector");
                    addNTVParameter(n4, "Work Conjugate Name", "string", "kinVar");
                    addNTVParameter(n4, "Work Conjugate Layout", "string", "QP Vector");
                }
                addNTVParameter(n4, "Topology Index", "int", "0");
                addNTVParameter(n4, "Function Index", "int", "0");
                sprintf(string_var, "R%d", (int)j);
                addNTVParameter(n4, "Response Name", "string", string_var);
                sprintf(string_var, "dR%ddRho", (int)j);
                addNTVParameter(n4, "Response Derivative Name", "string", string_var);
            }

            // Discretization
            n2 = n1.append_child("ParameterList");
            n2.append_attribute("name") = "Discretization";
            addNTVParameter(n2, "Method", "string", "Ioss");
            sprintf(string_var, "%s", m_InputData.run_mesh_name.c_str());
            addNTVParameter(n2, "Exodus Input File Name", "string", string_var);
            sprintf(string_var, "%s_alb_%s.exo", m_InputData.run_mesh_name_without_extension.c_str(), cur_obj.name.c_str());
            addNTVParameter(n2, "Exodus Output File Name", "string", string_var);
            addNTVParameter(n2, "Separate Evaluators by Element Block", "bool", "true");

            // Piro
            n2 = n1.append_child("ParameterList");
            n2.append_attribute("name") = "Piro";
            n3 = n2.append_child("ParameterList");
            n3.append_attribute("name") = "LOCA";
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Bifurcation";
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Constraints";
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Predictor";
            n5 = n4.append_child("ParameterList");
            n5.append_attribute("name") = "First Step Predictor";
            n5 = n4.append_child("ParameterList");
            n5.append_attribute("name") = "Last Step Predictor";
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Step Size";
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Stepper";
            n5 = n4.append_child("ParameterList");
            n5.append_attribute("name") = "Eigensolver";
            n3 = n2.append_child("ParameterList");
            n3.append_attribute("name") = "NOX";
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Status Tests";
            addNTVParameter(n4, "Test Type", "string", "Combo");
            addNTVParameter(n4, "Combo Type", "string", "OR");
            addNTVParameter(n4, "Number of Tests", "int", "2");
            n5 = n4.append_child("ParameterList");
            n5.append_attribute("name") = "Test 0";
            addNTVParameter(n5, "Test Type", "string", "NormF");
            addNTVParameter(n5, "Norm Type", "string", "Two Norm");
            addNTVParameter(n5, "Scale Type", "string", "Scaled");
            addNTVParameter(n5, "Tolerance", "double", "1e-8");
            n5 = n4.append_child("ParameterList");
            n5.append_attribute("name") = "Test 1";
            addNTVParameter(n5, "Test Type", "string", "MaxIters");
            addNTVParameter(n5, "Maximum Iterations", "int", m_InputData.max_iterations.c_str());
            n5 = n4.append_child("ParameterList");
            n5.append_attribute("name") = "Test 2";
            addNTVParameter(n5, "Test Type", "string", "NormUpdate");
            addNTVParameter(n5, "Norm Type", "string", "Two Norm");
            addNTVParameter(n5, "Scale Type", "string", "Scaled");
            addNTVParameter(n5, "Tolerance", "double", "1e-8");
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Direction";
            addNTVParameter(n4, "Method", "string", "Newton");
            n5 = n4.append_child("ParameterList");
            n5.append_attribute("name") = "Newton";
            addNTVParameter(n5, "Forcing Term Method", "string", "Constant");
            addNTVParameter(n5, "Rescue Bad Newton Solve", "bool", "1");
            n6 = n5.append_child("ParameterList");
            n6.append_attribute("name") = "Stratimikos Linear Solver";
            n7 = n6.append_child("ParameterList");
            n7.append_attribute("name") = "NOX Stratimikos Options";
            n7 = n6.append_child("ParameterList");
            n7.append_attribute("name") = "Stratimikos";
            addNTVParameter(n7, "Linear Solver Type", "string", "Belos");
            n8 = n7.append_child("ParameterList");
            n8.append_attribute("name") = "Linear Solver Types";
            n9 = n8.append_child("ParameterList");
            n9.append_attribute("name") = "AztecOO";
            n10 = n9.append_child("ParameterList");
            n10.append_attribute("name") = "Forward Solve";
            n11 = n10.append_child("ParameterList");
            n11.append_attribute("name") = "AztecOO Settings";
            addNTVParameter(n11, "Aztec Solver", "string", "GMRES");
            addNTVParameter(n11, "Convergence Test", "string", "r0");
            addNTVParameter(n11, "Size of Krylov Subspace", "int", "200");
            addNTVParameter(n11, "Output Frequency", "int", "10");
            addNTVParameter(n10, "Max Iterations", "int", "200");
            addNTVParameter(n10, "Tolerance", "double", "1e-5");
            n9 = n8.append_child("ParameterList");
            n9.append_attribute("name") = "Belos";
            addNTVParameter(n9, "Solver Type", "string", "Block GMRES");
            n10 = n9.append_child("ParameterList");
            n10.append_attribute("name") = "Solver Types";
            n11 = n10.append_child("ParameterList");
            n11.append_attribute("name") = "Block GMRES";
            addNTVParameter(n11, "Convergence Tolerance", "double", "1e-12");
            addNTVParameter(n11, "Output Frequency", "int", "2");
            addNTVParameter(n11, "Output Style", "int", "1");
            addNTVParameter(n11, "Verbosity", "int", "0");
            addNTVParameter(n11, "Maximum Iterations", "int", "200");
            addNTVParameter(n11, "Block Size", "int", "1");
            addNTVParameter(n11, "Num Blocks", "int", "200");
            addNTVParameter(n11, "Flexible Gmres", "bool", "0");
            addNTVParameter(n7, "Preconditioner Type", "string", "Ifpack2");
            n8 = n7.append_child("ParameterList");
            n8.append_attribute("name") = "Preconditioner Types";
            n9 = n8.append_child("ParameterList");
            n9.append_attribute("name") = "Ifpack2";
            addNTVParameter(n9, "Overlap", "int", "2");
            addNTVParameter(n9, "Prec Type", "string", "ILUT");
            n10 = n9.append_child("ParameterList");
            n10.append_attribute("name") = "Ifpack2 Settings";
            addNTVParameter(n10, "fact: drop tolerance", "double", "0");
            addNTVParameter(n10, "fact: ilut level-of-fill", "double", "1");
            n10 = n9.append_child("ParameterList");
            n10.append_attribute("name") = "VerboseObject";
            addNTVParameter(n10, "Verbosity Level", "string", "medium");
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Line Search";
            n5 = n4.append_child("ParameterList");
            n5.append_attribute("name") = "Full Step";
            addNTVParameter(n5, "Full Step", "double", "1");
            addNTVParameter(n4, "Method", "string", "Full Step");
            addNTVParameter(n3, "Nonlinear Solver", "string", "Line Search Based");
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Printing";
            addNTVParameter(n4, "Output Information", "int", "103");
            addNTVParameter(n4, "Output Precision", "int", "3");
            addNTVParameter(n4, "Output Processor", "int", "0");
            n4 = n3.append_child("ParameterList");
            n4.append_attribute("name") = "Solver Options";
            addNTVParameter(n4, "Status Test Check Type", "string", "Minimal");



            // Write the file to disk
            doc.save_file(buf, "  ");
        }
    }
    return true;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoAnalyzeInputDecks(std::ostringstream *aStringStream)
/******************************************************************************/
{
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
        if(!generatePlatoAnalyzeInputDeckForNewUncertaintyWorkflow())
            return false;
    }
    else
    {
        PlatoAnalyzeInputDeckWriter tInputDeckWriter(m_InputData);
        if(m_InputData.optimization_type == "shape" && m_InputData.mPlatoAnalyzePerformerExists)
        {
            return tInputDeckWriter.generateForShape(aStringStream);
        }
        else
        {
            return tInputDeckWriter.generate(aStringStream);
        }
    }
    return true;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoAnalyzeInputDeckForNewUncertaintyWorkflow()
/******************************************************************************/
{
  pugi::xml_document doc;
  pugi::xml_node tmp_node = doc.append_child(pugi::node_declaration);
  tmp_node.set_name("xml");
  pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
  tmp_att.set_value("1.0");

  pugi::xml_node n1, n2, n3, n4;
  n1 = doc.append_child("ParameterList");
  n1.append_attribute("name") = "Problem";
  addNTVParameter(n1, "Physics", "string", "Plato Driver");
  addNTVParameter(n1, "Spatial Dimension", "int", "3");
  addNTVParameter(n1, "Input Mesh", "string", m_InputData.run_mesh_name);

  n2 = n1.append_child("ParameterList");
  n2.append_attribute("name") = "Plato Problem";
  addNTVParameter(n2, "Physics", "string", "Mechanical");
  addNTVParameter(n2, "PDE Constraint", "string", "Elliptic");
  addNTVParameter(n2, "Constraint", "string", "My Volume");
  addNTVParameter(n2, "Objective", "string", "My Internal Elastic Energy");
  addNTVParameter(n2, "Self-Adjoint", "bool", "true");

  n3 = n2.append_child("ParameterList");
  n3.append_attribute("name") = "My Volume";
  addNTVParameter(n3, "Type", "string", "Scalar Function");
  addNTVParameter(n3, "Scalar Function Type", "string", "Volume");

  n4 = n3.append_child("ParameterList");
  n4.append_attribute("name") = "Penalty Function";
  addNTVParameter(n4, "Type", "string", "SIMP");
  addNTVParameter(n4, "Exponent", "double", "1.0");
  addNTVParameter(n4, "Minimum Value", "double", "0.0");

  n3 = n2.append_child("ParameterList");
  n3.append_attribute("name") = "My Internal Elastic Energy";
  addNTVParameter(n3, "Type", "string", "Scalar Function");
  addNTVParameter(n3, "Scalar Function Type", "string", "Internal Elastic Energy");

  n4 = n3.append_child("ParameterList");
  n4.append_attribute("name") = "Penalty Function";
  addNTVParameter(n4, "Type", "string", "SIMP");
  addNTVParameter(n4, "Exponent", "double", "3.0");
  addNTVParameter(n4, "Minimum Value", "double", "1.0e-3");

  n3 = n2.append_child("ParameterList");
  n3.append_attribute("name") = "Elliptic";

  n4 = n3.append_child("ParameterList");
  n4.append_attribute("name") = "Penalty Function";
  addNTVParameter(n4, "Type", "string", "SIMP");
  addNTVParameter(n4, "Exponent", "double", "3.0");
  addNTVParameter(n4, "Minimum Value", "double", "1.0e-3");

  n3 = n2.append_child("ParameterList");
  n3.append_attribute("name") = "Material Model";

  n4 = n3.append_child("ParameterList");
  n4.append_attribute("name") = "Isotropic Linear Elastic";
  addNTVParameter(n4, "Poissons Ratio", "double", m_InputData.materials[0].poissons_ratio.c_str());
  addNTVParameter(n4, "Youngs Modulus", "double", m_InputData.materials[0].youngs_modulus.c_str());


  n3 = n2.append_child("ParameterList");
  n3.append_attribute("name") = "Natural Boundary Conditions";

  for(size_t tRandomLoadIndex = 0; tRandomLoadIndex < m_UncertaintyMetaData.randomVariableIndices.size(); ++tRandomLoadIndex)
  {
    n4 = n3.append_child("ParameterList");
    std::string tRandomLoadName = "Random Traction Vector Boundary Condition " + Plato::to_string(tRandomLoadIndex);
    n4.append_attribute("name") = tRandomLoadName.c_str();
    addNTVParameter(n4, "Type", "string", "Uniform");
    addNTVParameter(n4, "Values", "Array(double)", "{0.0, 0.0, 0.0}");
    std::string tMeshSetName = m_InputData.load_cases[0].loads[tRandomLoadIndex].app_name;
    addNTVParameter(n4, "Sides", "string", tMeshSetName);
  }

  for(size_t tDeterministicLoadIndex = 0; tDeterministicLoadIndex < m_UncertaintyMetaData.deterministicVariableIndices.size(); ++tDeterministicLoadIndex)
  {
    n4 = n3.append_child("ParameterList");
    std::string tDeterministicLoadName = "Deterministic Traction Vector Boundary Condition " + Plato::to_string(tDeterministicLoadIndex);
    n4.append_attribute("name") = tDeterministicLoadName.c_str();
    addNTVParameter(n4, "Type", "string", "Uniform");
    std::vector<std::string> tDeterministicLoadValues = m_InputData.load_cases[0].loads[tDeterministicLoadIndex].values;

    std::string tValuesInputString = "{";
    for(size_t i = 0; i < tDeterministicLoadValues.size(); ++i)
    {
      tValuesInputString += tDeterministicLoadValues[i];
      if(i != tDeterministicLoadValues.size() - 1)
        tValuesInputString += ", ";
    }
  
    addNTVParameter(n4, "Values", "Array(double)", "{0.0, 0.0, 0.0}");
    std::string tMeshSetName = m_InputData.load_cases[0].loads[tDeterministicLoadIndex].app_name;
    addNTVParameter(n4, "Sides", "string", tMeshSetName);
  }



  n3 = n2.append_child("ParameterList");
  n3.append_attribute("name") = "Essential Boundary Conditions";

  for(size_t i = 0; i < m_InputData.bcs.size(); ++i)
  {
    XMLGen::BC bc = m_InputData.bcs[i];
    if(bc.type != "displacement")
    {
      std::cout << "Error: Only fixed displacement boundary conditions are currently supported for new uncertainty workflow" << std::endl;
      return false;
    }
    if(bc.dof == "x")
    {
      n4 = n3.append_child("ParameterList");
      std::string tBCName = "X Fixed Displacement Boundary Condition " + Plato::to_string(i);
      n4.append_attribute("name") = tBCName.c_str();
      if(bc.value.empty())
        addNTVParameter(n4, "Type", "string", "Zero Value");
      else
      {
        addNTVParameter(n4, "Type", "string", "Fixed Value");
        addNTVParameter(n4, "Value", "double", bc.value);
      }
      addNTVParameter(n4, "Index", "int", "0");
      addNTVParameter(n4, "Sides", "string", bc.app_name);
    }
    else if(bc.dof == "y")
    {
      n4 = n3.append_child("ParameterList");
      std::string tBCName = "Y Fixed Displacement Boundary Condition " + Plato::to_string(i);
      n4.append_attribute("name") = tBCName.c_str();
      if(bc.value.empty())
        addNTVParameter(n4, "Type", "string", "Zero Value");
      else
      {
        addNTVParameter(n4, "Type", "string", "Fixed Value");
        addNTVParameter(n4, "Value", "double", bc.value);
      }
      addNTVParameter(n4, "Index", "int", "1");
      addNTVParameter(n4, "Sides", "string", bc.app_name);
    }
    else if(bc.dof == "z")
    {
      n4 = n3.append_child("ParameterList");
      std::string tBCName = "Z Fixed Displacement Boundary Condition " + Plato::to_string(i);
      n4.append_attribute("name") = tBCName.c_str();
      if(bc.value.empty())
        addNTVParameter(n4, "Type", "string", "Zero Value");
      else
      {
        addNTVParameter(n4, "Type", "string", "Fixed Value");
        addNTVParameter(n4, "Value", "double", bc.value);
      }
      addNTVParameter(n4, "Index", "int", "2");
      addNTVParameter(n4, "Sides", "string", bc.app_name);
    }
    else if(bc.dof.empty())
    {
      n4 = n3.append_child("ParameterList");
      std::string tBCName = "X Fixed Displacement Boundary Condition " + Plato::to_string(i);
      n4.append_attribute("name") = tBCName.c_str();
      addNTVParameter(n4, "Type", "string", "Zero Value");
      addNTVParameter(n4, "Index", "int", "0");
      addNTVParameter(n4, "Sides", "string", bc.app_name);

      n4 = n3.append_child("ParameterList");
      tBCName = "Y Fixed Displacement Boundary Condition " + Plato::to_string(i);
      n4.append_attribute("name") = tBCName.c_str();
      addNTVParameter(n4, "Type", "string", "Zero Value");
      addNTVParameter(n4, "Index", "int", "1");
      addNTVParameter(n4, "Sides", "string", bc.app_name);

      n4 = n3.append_child("ParameterList");
      tBCName = "Z Fixed Displacement Boundary Condition " + Plato::to_string(i);
      n4.append_attribute("name") = tBCName.c_str();
      addNTVParameter(n4, "Type", "string", "Zero Value");
      addNTVParameter(n4, "Index", "int", "2");
      addNTVParameter(n4, "Sides", "string", bc.app_name);
    }
  }

  char buf[200];
  sprintf(buf, "plato_analyze_input_deck.xml");
  doc.save_file(buf, "  ");

  return true;
}

/******************************************************************************/
bool XMLGenerator::generateLightMPInputDecks()
/******************************************************************************/
{
    pugi::xml_node node1, node2, node3;
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        const XMLGen::Objective& cur_obj = m_InputData.objectives[i];
        if(!cur_obj.code_name.compare("lightmp"))
        {
            char buf[200];
            pugi::xml_document doc;

            // Version entry
            pugi::xml_node tmp_node = doc.append_child(pugi::node_declaration);
            tmp_node.set_name("xml");
            pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
            tmp_att.set_value("1.0");

            for(size_t n=0; n<m_InputData.materials.size(); ++n)
            {
                node1 = doc.append_child("material");
                node2 = node1.append_child("id");
                node3 = node2.append_child(pugi::node_pcdata);
                node3.set_value(m_InputData.materials[n].material_id.c_str());

                node1 = node1.append_child("linear_elastic");
                node2 = node1.append_child("poissons_ratio");
                node3 = node2.append_child(pugi::node_pcdata);
                node3.set_value(m_InputData.materials[n].poissons_ratio.c_str());
                node2 = node1.append_child("youngs_modulus");
                node3 = node2.append_child(pugi::node_pcdata);
                node3.set_value(m_InputData.materials[n].youngs_modulus.c_str());
            }

            node1 = doc.append_child("mesh");
            node2 = node1.append_child("type");
            node3 = node2.append_child(pugi::node_pcdata);
            node3.set_value("unstructured");
            node2 = node1.append_child("format");
            node3 = node2.append_child(pugi::node_pcdata);
            node3.set_value("exodus");
            node2 = node1.append_child("mesh");
            node3 = node2.append_child(pugi::node_pcdata);
            node3.set_value(m_InputData.run_mesh_name.c_str());
            // lightmp just needs one block specified with an integration rule--don't need all of the blocks listed here
            node2 = node1.append_child("block");
            node3 = node2.append_child("index");
            pugi::xml_node node4 = node3.append_child(pugi::node_pcdata);
            node4.set_value(m_InputData.blocks[0].block_id.c_str());
            node3 = node2.append_child("integration");
            node4 = node3.append_child("type");
            pugi::xml_node node5 = node4.append_child(pugi::node_pcdata);
            node5.set_value("gauss");
            node4 = node3.append_child("order");
            node5 = node4.append_child(pugi::node_pcdata);
            node5.set_value("2");
            node3 = node2.append_child("material");
            node4 = node3.append_child(pugi::node_pcdata);
            node4.set_value(m_InputData.blocks[0].material_id.c_str());

            node1 = doc.append_child("output");
            node2 = node1.append_child("file");
            node3 = node2.append_child(pugi::node_pcdata);
            sprintf(buf, "lightmp_output_%s", cur_obj.name.c_str());
            node3.set_value(buf);
            node2 = node1.append_child("format");
            node3 = node2.append_child(pugi::node_pcdata);
            node3.set_value("exodus");

            node1 = doc.append_child("control");
            node2 = node1.append_child("time");
            node3 = node2.append_child("start");
            node4 = node3.append_child(pugi::node_pcdata);
            node4.set_value("0.0");
            node3 = node2.append_child("end");
            node4 = node3.append_child(pugi::node_pcdata);
            node4.set_value("0.01");
            node3 = node2.append_child("timestep");
            node4 = node3.append_child(pugi::node_pcdata);
            node4.set_value("0.001");

            node1 = doc.append_child("physics");
            node2 = node1.append_child("solid_statics");
            node3 = node2.append_child("solver");
            node4 = node3.append_child("output_interval");
            node5 = node4.append_child(pugi::node_pcdata);
            node5.set_value("100");
            node4 = node3.append_child("tolerance");
            node5 = node4.append_child(pugi::node_pcdata);
            node5.set_value("1e-9");
            node4 = node3.append_child("iterations");
            node5 = node4.append_child(pugi::node_pcdata);
            node5.set_value("1000");
            node3 = node2.append_child("boundary_conditions");
            pugi::xml_node node6;
            for(size_t j=0; j<cur_obj.bc_ids.size(); ++j)
            {
                bool found = false;
                XMLGen::BC cur_bc;
                std::string cur_bc_id = cur_obj.bc_ids[j];
                for(size_t qq=0; qq<m_InputData.bcs.size(); ++qq)
                {
                    if(cur_bc_id == m_InputData.bcs[qq].bc_id)
                    {
                        found = true;
                        cur_bc = m_InputData.bcs[qq];
                    }
                }
                if(found)
                {
                    if(cur_bc.dof.empty())
                    {
                        node4 = node3.append_child("displacement");
                        node5 = node4.append_child("nodeset");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value(cur_bc.app_id.c_str());
                        node5 = node4.append_child("direction");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("x");
                        node5 = node4.append_child("value");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("0.0");
                        node5 = node4.append_child("scale");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("1.0");
                        node4 = node3.append_child("displacement");
                        node5 = node4.append_child("nodeset");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value(cur_bc.app_id.c_str());
                        node5 = node4.append_child("direction");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("y");
                        node5 = node4.append_child("value");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("0.0");
                        node5 = node4.append_child("scale");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("1.0");
                        node4 = node3.append_child("displacement");
                        node5 = node4.append_child("nodeset");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value(cur_bc.app_id.c_str());
                        node5 = node4.append_child("direction");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("z");
                        node5 = node4.append_child("value");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("0.0");
                        node5 = node4.append_child("scale");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("1.0");
                    }
                    else
                    {
                        std::string tValue;
                        if(cur_bc.value.empty())
                        {
                            tValue = "0.0";
                        }
                        else
                        {
                            tValue = cur_bc.value;
                        }
                        node4 = node3.append_child("displacement");
                        node5 = node4.append_child("nodeset");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value(cur_bc.app_id.c_str());
                        node5 = node4.append_child("direction");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value(cur_bc.dof.c_str());
                        node5 = node4.append_child("value");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value(tValue.c_str());
                        node5 = node4.append_child("scale");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("1.0");
                    }
                }
            }
            for(size_t j=0; j<cur_obj.load_case_ids.size(); ++j)
            {
                bool found = false;
                XMLGen::LoadCase cur_load_case;
                std::string cur_load_id = cur_obj.load_case_ids[j];
                for(size_t qq=0; found == false && qq<m_InputData.load_cases.size(); ++qq)
                {
                    if(cur_load_id == m_InputData.load_cases[qq].id)
                    {
                        found = true;
                        cur_load_case = m_InputData.load_cases[qq];
                    }
                }
                if(found)
                {
                    for(size_t e=0; e<cur_load_case.loads.size(); ++e)
                    {
                        XMLGen::Load cur_load = cur_load_case.loads[e];
                        node4 = node3.append_child("traction");
                        node5 = node4.append_child("nodeset");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value(cur_load.app_id.c_str());
                        node5 = node4.append_child("direction");
                        node6 = node5.append_child(pugi::node_pcdata);
                        double x = std::atof(cur_load.values[0].c_str());
                        double y = std::atof(cur_load.values[1].c_str());
                        double z = std::atof(cur_load.values[2].c_str());
                        double magx = fabs(x);
                        double magy = fabs(y);
                        double magz = fabs(z);
                        std::string value = "0.0";
                        if(magx > magy && magx > magz)
                        {
                            node6.set_value("x");
                            value = Plato::to_string(x);
                        }
                        else if(magy > magx && magy > magz)
                        {
                            node6.set_value("y");
                            value = Plato::to_string(y);
                        }
                        else if(magz > magx && magz > magy)
                        {
                            node6.set_value("z");
                            value = Plato::to_string(z);
                        }
                        node5 = node4.append_child("value");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value(value.c_str());
                        node5 = node4.append_child("scale");
                        node6 = node5.append_child(pugi::node_pcdata);
                        node6.set_value("1.0");
                    }
                }
            }

            // Write the file to disk
            sprintf(buf, "lightmp_input_deck_%s.i", cur_obj.name.c_str());
            doc.save_file(buf, "  ");
        }
    }
    return true;
}
/******************************************************************************/
bool XMLGenerator::generatePhysicsInputDecks()
/******************************************************************************/
{
    if(!generateSalinasInputDecks())
      return false;
    if(!generateAlbanyInputDecks())
      return false;
    if(!generateLightMPInputDecks())
      return false;
    if(!generatePlatoAnalyzeInputDecks())
      return false;
    return true;
}

/******************************************************************************/
bool XMLGenerator::parseTokens(char *buffer, std::vector<std::string> &tokens)
/******************************************************************************/
{
    const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0
    int n = 0;

    // parse the line
    token[0] = strtok(buffer, DELIMITER); // first token

    // If there is a comment...
    if(token[0] && strlen(token[0]) > 1 && token[0][0] == '/' && token[0][1] == '/')
    {
        tokens.clear();
        return true;
    }

    if (token[0]) // zero if line is blank
    {
        for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
        {
            token[n] = strtok(0, DELIMITER); // subsequent tokens
            if (!token[n])
                break; // no more tokens
        }
    }
    for(int i=0; i<n; ++i)
        tokens.push_back(token[i]);

    return true;
}

/******************************************************************************/
bool XMLGenerator::parseObjectives(std::istream &fin)
/******************************************************************************/
{
    std::vector<std::string> tInputStringList;
    char buf[MAX_CHARS_PER_LINE];
    std::vector<std::string> tokens;
    std::string tStringValue;

    // read each line of the file
    while (!fin.eof())
    {
        // read an entire line into memory
        fin.getline(buf, MAX_CHARS_PER_LINE);
        tokens.clear();
        parseTokens(buf, tokens);

        // process the tokens
        if(tokens.size() > 0)
        {
            for(size_t j=0; j<tokens.size(); ++j)
                tokens[j] = toLower(tokens[j]);

            if(parseSingleValue(tokens, tInputStringList = {"begin","objective"}, tStringValue))
            {
                XMLGen::Objective new_objective;
                new_objective.weight="1";
                new_objective.normalize_objective = "true";
                // found an objective. parse it.
                // parse the rest of the objective
                while (!fin.eof())
                {
                    tokens.clear();
                    fin.getline(buf, MAX_CHARS_PER_LINE);
                    parseTokens(buf, tokens);
                    // process the tokens
                    if(tokens.size() > 0)
                    {
                        std::vector<std::string> unlowered_tokens = tokens;

                        for(size_t j=0; j<tokens.size(); ++j)
                            tokens[j] = toLower(tokens[j]);

                        if(parseSingleValue(tokens, tInputStringList = {"end","objective"}, tStringValue))
                        {
                            break;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"type"}, tStringValue))
                        {
                            if(tokens.size() < 2)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No type specified after \"type\" keyword.\n";
                                return false;
                            }
                            new_objective.type = tokens[1];
                            for(size_t j=2; j<tokens.size(); ++j)
                            {
                                new_objective.type += " ";
                                new_objective.type += tokens[j];
                            }
                            if((new_objective.type == "stress constrained mass minimization")  &&
                               new_objective.analysis_solver_tolerance.length() == 0)
                            {
                                // Set a default in case one isn't set by user.
                                new_objective.analysis_solver_tolerance = "1e-7";
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"name"}, tStringValue))
                        {
                            if(tokens.size() < 2)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No name specified after \"name\" keyword.\n";
                                return false;
                            }
                            new_objective.name = tokens[1];
                            for(size_t j=2; j<tokens.size(); ++j)
                            {
                                new_objective.name += " ";
                                new_objective.name += tokens[j];
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"boundary","condition","ids"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"boundary condition ids\" keywords.\n";
                                return false;
                            }
                            for(size_t j=3; j<tokens.size(); ++j)
                            {
                                new_objective.bc_ids.push_back(tokens[j]);
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"stress","limit"}, tStringValue))
                        {
                            if(tokens.size() < 3)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"stress limit\" keywords.\n";
                                return false;
                            }
                            if(new_objective.relative_stress_limit != "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: Only specify \"relative stress limit\" or \"stress limit\".\n";
                                return false;
                            }
                            new_objective.stress_limit = tokens[2];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"relative","stress","limit"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"relative stress limit\" keywords.\n";
                                return false;
                            }
                            if(new_objective.stress_limit != "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: Only specify \"relative stress limit\" or \"stress limit\".\n";
                                return false;
                            }
                            new_objective.relative_stress_limit = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"scmm", "constraint", "exponent"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"scmm constraint exponent\" keywords.\n";
                                return false;
                            }
                            new_objective.scmm_constraint_exponent = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"scmm", "penalty", "expansion", "factor"}, tStringValue))
                        {
                            if(tokens.size() < 5)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"scmm penalty expansion factor\" keywords.\n";
                                return false;
                            }
                            new_objective.scmm_penalty_expansion_factor = tokens[4];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"scmm", "initial", "penalty"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"scmm initial penalty\" keywords.\n";
                                return false;
                            }
                            new_objective.scmm_initial_penalty = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"stress","p","norm","power"}, tStringValue))
                        {
                            if(tokens.size() < 5)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"stress p norm power\" keywords.\n";
                                return false;
                            }
                            new_objective.stress_p_norm_power = tokens[4];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"volume","misfit","target"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"volume misfit target\" keywords.\n";
                                return false;
                            }
                            new_objective.volume_misfit_target = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"stress","ramp","factor"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"stress ramp factor\" keywords.\n";
                                return false;
                            }
                            new_objective.stress_ramp_factor = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"limit","power","min"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"limit power min\" keywords.\n";
                                return false;
                            }
                            new_objective.limit_power_min = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"limit","power","max"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"limit power max\" keywords.\n";
                                return false;
                            }
                            new_objective.limit_power_max = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"limit","power","feasible","bias"}, tStringValue))
                        {
                            if(tokens.size() < 5)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"limit power feasible bias\" keywords.\n";
                                return false;
                            }
                            new_objective.limit_power_feasible_bias = tokens[4];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"limit","power","feasible","slope"}, tStringValue))
                        {
                            if(tokens.size() < 5)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"limit power feasible slope\" keywords.\n";
                                return false;
                            }
                            new_objective.limit_power_feasible_slope = tokens[4];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"limit","power","infeasible","bias"}, tStringValue))
                        {
                            if(tokens.size() < 5)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"limit power infeasible bias\" keywords.\n";
                                return false;
                            }
                            new_objective.limit_power_infeasible_bias = tokens[4];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"limit","power","infeasible","slope"}, tStringValue))
                        {
                            if(tokens.size() < 5)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"limit power infeasible slope\" keywords.\n";
                                return false;
                            }
                            new_objective.limit_power_infeasible_slope = tokens[4];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"limit","reset","subfrequency"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"limit reset subfrequency\" keywords.\n";
                                return false;
                            }
                            new_objective.limit_reset_subfrequency = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"limit","reset","count"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"limit reset count\" keywords.\n";
                                return false;
                            }
                            new_objective.limit_reset_count = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"inequality","allowable","feasibility","upper"}, tStringValue))
                        {
                            if(tokens.size() < 5)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"inequality allowable feasibility upper\" keywords.\n";
                                return false;
                            }
                            new_objective.inequality_allowable_feasiblity_upper = tokens[4];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"inequality","feasibility","scale"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"inequality feasibility scale\" keywords.\n";
                                return false;
                            }
                            new_objective.inequality_feasibility_scale = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"inequality","infeasibility","scale"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"inequality infeasibility scale\" keywords.\n";
                                return false;
                            }
                            new_objective.inequality_infeasibility_scale = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"stress","inequality","power"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"stress inequality power\" keywords.\n";
                                return false;
                            }
                            new_objective.stress_inequality_power = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"stress","favor","final"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"stress favor final\" keywords.\n";
                                return false;
                            }
                            new_objective.stress_favor_final = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"stress","favor","updates"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"stress favor updates\" keywords.\n";
                                return false;
                            }
                            new_objective.stress_favor_updates = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"volume","penalty","power"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"volume penalty power\" keywords.\n";
                                return false;
                            }
                            new_objective.volume_penalty_power = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"volume","penalty","divisor"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"volume penalty divisor\" keywords.\n";
                                return false;
                            }
                            new_objective.volume_penalty_divisor = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"volume","penalty","bias"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"volume penalty bias\" keywords.\n";
                                return false;
                            }
                            new_objective.volume_penalty_bias = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"load","ids"}, tStringValue))
                        {
                            if(tokens.size() < 3)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"load ids\" keywords.\n";
                                return false;
                            }
                            for(size_t j=2; j<tokens.size(); ++j)
                            {
                                new_objective.load_case_ids.push_back(tokens[j]);
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"load","case","weights"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"load case weights\" keywords.\n";
                                return false;
                            }
                            for(size_t j=3; j<tokens.size(); ++j)
                            {
                                new_objective.load_case_weights.push_back(tokens[j]);
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"code"}, tStringValue))
                        {
                            if(tokens.size() < 2)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No code specified after \"code\" keyword.\n";
                                return false;
                            }
                            new_objective.code_name = tokens[1];
                            for(size_t j=2; j<tokens.size(); ++j)
                            {
                                new_objective.code_name += " ";
                                new_objective.code_name += tokens[j];
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"complex","error","measure"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No complex error measure value specified after \"complex error measure\" keywords.\n";
                                return false;
                            }
                            new_objective.complex_error_measure = tokens[3];
                            for(size_t j=4; j<tokens.size(); ++j)
                            {
                                new_objective.complex_error_measure += " ";
                                new_objective.complex_error_measure += tokens[j];
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"output","for","plotting"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No outputs specified after \"output for plotting\" keywords.\n";
                                return false;
                            }
                            for(size_t j=3; j<tokens.size(); ++j)
                            {
                                new_objective.output_for_plotting.push_back(tokens[j]);
                            }
                        }
                        else if (parseSingleValue(tokens, tInputStringList = {"ls","tet","type"}, tStringValue))
                        {
                            new_objective.convert_to_tet10 = tokens[3];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"number","processors"}, tStringValue))
                        {
                            if(tStringValue == "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"number processors\" keywords.\n";
                                return false;
                            }
                            new_objective.num_procs = tStringValue;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"weight"}, tStringValue))
                        {
                            if(tStringValue == "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"weight\" keyword.\n";
                                return false;
                            }
                            new_objective.weight = tStringValue;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"multi","load","case"}, tStringValue))
                        {
                            if(tStringValue == "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"multi load case\" keywords.\n";
                                return false;
                            }
                            new_objective.multi_load_case = tStringValue;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"frf","match","nodesets"}, tStringValue))
                        {
                            if(tokens.size() < 4)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"frf match nodesets\" keywords.\n";
                                return false;
                            }
                            for(size_t j=3; j<tokens.size(); ++j)
                                new_objective.frf_match_nodesets.push_back(tokens[j]);
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"weightmass","scale","factor"}, tStringValue))
                        {
                            if(tStringValue == "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"weightmass scale factor\" keywords.\n";
                                return false;
                            }
                            new_objective.wtmass_scale_factor = tStringValue;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"analysis","solver","tolerance"}, tStringValue))
                        {
                            if(tStringValue == "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"analysis solver tolerance\" keywords.\n";
                                return false;
                            }
                            new_objective.analysis_solver_tolerance = tStringValue;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"normalize","objective"}, tStringValue))
                        {
                            if(tStringValue == "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"normalize objective\" keywords.\n";
                                return false;
                            }
                            new_objective.normalize_objective = tStringValue;
                        }
                        else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"reference","frf","file"}, tStringValue))
                        {
                            if(tStringValue == "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"reference frf file\" keywords.\n";
                                return false;
                            }
                            new_objective.ref_frf_file = tStringValue;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"raleigh","damping","alpha"}, tStringValue))
                        {
                            if(tStringValue == "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"raleigh damping alpha\" keywords.\n";
                                return false;
                            }
                            new_objective.raleigh_damping_alpha = tStringValue;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"raleigh","damping","beta"}, tStringValue))
                        {
                            if(tStringValue == "")
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No value specified after \"raleigh damping beta\" keywords.\n";
                                return false;
                            }
                            new_objective.raleigh_damping_beta = tStringValue;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"distribute","objective"}, tStringValue))
                        {
                            if(tokens.size() < 3)
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: No case specified after \"repeat objective\" keywords.\n";
                                return false;
                            }
                            if(tokens[2]=="none")
                            {
                                // distribute objective none
                                // 0          1         2
                                new_objective.distribute_objective_type="";
                            }
                            else if(tokens[2]=="at")
                            {
                                // distribute objective at most {number} processors
                                // 0          1         2  3    4        5
                                if(tokens.size() < 6)
                                {
                                    std::cout << "ERROR:XMLGenerator:parseObjectives: Unmatched case specified after \"repeat objective\" keywords.\n";
                                    return false;
                                }
                                if(tokens[3]!="most" || tokens[5]!="processors")
                                {
                                    std::cout << "ERROR:XMLGenerator:parseObjectives: Unmatched case specified after \"repeat objective\" keywords.\n";
                                    return false;
                                }
                                new_objective.distribute_objective_type = "atmost";
                                new_objective.atmost_total_num_processors = tokens[4];
                            }
                            else
                            {
                                std::cout << "ERROR:XMLGenerator:parseObjectives: Unmatched case specified after \"repeat objective\" keywords.\n";
                                return false;
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"analyze","new","workflow"}, tStringValue))
                        {
                          if(tokens[3] == "true")
                            m_UseNewPlatoAnalyzeUncertaintyWorkflow = true;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"number", "ranks"}, tStringValue))
                        {
                          new_objective.num_ranks = tokens[2];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"begin","frequency"}, tStringValue))
                        {
                            while (!fin.eof())
                            {
                                tokens.clear();
                                fin.getline(buf, MAX_CHARS_PER_LINE);
                                parseTokens(buf, tokens);
                                // process the tokens
                                if(tokens.size() > 0)
                                {
                                    for(size_t j=0; j<tokens.size(); ++j)
                                        tokens[j] = toLower(tokens[j]);

                                    if(parseSingleValue(tokens, tInputStringList = {"end","frequency"}, tStringValue))
                                    {
                                        if(new_objective.freq_min == "" ||
                                                new_objective.freq_max == "" ||
                                                new_objective.freq_step == "")
                                        {
                                            std::cout << "ERROR:XMLGenerator:parseObjectives: Insufficient info in \"frequency block\".\n";
                                            return false;
                                        }
                                        break;
                                    }
                                    else
                                    {
                                        if(parseSingleValue(tokens, tInputStringList = {"min"}, tStringValue))
                                        {
                                            new_objective.freq_min = tStringValue;
                                        }
                                        else if(parseSingleValue(tokens, tInputStringList = {"max"}, tStringValue))
                                        {
                                            new_objective.freq_max = tStringValue;
                                        }
                                        else if(parseSingleValue(tokens, tInputStringList = {"step"}, tStringValue))
                                        {
                                            new_objective.freq_step = tStringValue;
                                        }
                                        else
                                        {
                                            PrintUnrecognizedTokens(tokens);
                                            std::cout << "ERROR:XMLGenerator:parseObjectives: Unrecognized keyword in \"frequency block\".\n";
                                            return false;
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            PrintUnrecognizedTokens(tokens);
                            std::cout << "ERROR:XMLGenerator:parseObjectives: Unrecognized keywords.\n";
                            return false;
                        }
                    }
                }

                // do checking of this objective

                // check (number of load ids) versus (number of load weights)
                const size_t num_load_ids = new_objective.load_case_ids.size();
                const size_t num_load_case_weights = new_objective.load_case_weights.size();
                if(num_load_case_weights == 0u)
                {
                    // replace with constant weighting
                    const double constantWeightFraction = 1.0;
                    const std::string uniformWeightFraction_str = Plato::to_string(constantWeightFraction);
                    new_objective.load_case_weights.assign(num_load_ids, uniformWeightFraction_str);
                    // must be constant at 1.0 rather than (1.0/num_cases) to allow
                    // multi-load to be equivalent to multi-objective. We don't know within
                    // this objective how many other loads should be considered "equally".
                }
                else if(num_load_ids != num_load_case_weights)
                {
                    std::cout << "ERROR:XMLGenerator:parseObjectives: Length mismatch in load case ids and weights.\n";
                    return false;
                }

                // check that (distribute objective) must have (multi load case true)
                const bool have_distributed_objective =
                        (new_objective.distribute_objective_type != "");
                const bool have_multiLoadCaseTrue =
                        (new_objective.multi_load_case == "true");
                if(have_distributed_objective && !have_multiLoadCaseTrue)
                {
                    std::cout << "ERROR:XMLGenerator:parseObjectives: "
                            << "Parsed input is ambiguous.\n"
                            << "Distributed objectives must have \"multi load case true\".\n"
                            << "Or set \"distribute objective none\".\n";
                    return false;
                }

                // place objective in array
                m_InputData.objectives.push_back(new_objective);
            }
        }
    }

    // assign names
    const bool filling_did_pass = fillObjectiveAndPerfomerNames();
    if(!filling_did_pass)
    {
        std::cout << "ERROR:XMLGenerator:parseObjectives: Failed to fill objective and performer names.\n";
        return false;
    }

    return true;
}

/******************************************************************************/
bool XMLGenerator::fillObjectiveAndPerfomerNames()
/******************************************************************************/
{
    // assigns objective names to yet un-named objectives
    // assigns performer name to each objective

    char buf2[200];
    size_t num_objs = m_InputData.objectives.size();
    // If there were objectives without names add a default name
    for(size_t i=0; i<num_objs; ++i)
    {
        // For each code name we will make sure there are names set
        std::string cur_code_name = m_InputData.objectives[i].code_name;
        int num_cur_code_objs=0;
        for(size_t j=i; j<num_objs; ++j)
        {
            if(!m_InputData.objectives[j].code_name.compare(cur_code_name))
            {
                num_cur_code_objs++;
                if(m_InputData.objectives[j].name.empty())
                {
                    sprintf(buf2, "%d", num_cur_code_objs);
                    m_InputData.objectives[j].name = buf2;
                }
            }
        }
    }
    // Set the performer names
    for(size_t i=0; i<num_objs; ++i)
    {
        m_InputData.objectives[i].performer_name =
                m_InputData.objectives[i].code_name +
                "_" + m_InputData.objectives[i].name;
    }

    return true;
}

/******************************************************************************/
bool XMLGenerator::parseLoads(std::istream &fin)
/******************************************************************************/
{
    std::vector<std::string> tInputStringList;
    std::vector<std::string> tokens;
    std::string tStringValue;
    bool load_block_found = false;

    // read each line of the file
    while (!fin.eof())
    {
        getTokensFromLine(fin,tokens);

        // process the tokens
        if(tokens.size() > 0)
        {
            if(parseSingleValue(tokens, tInputStringList = {"begin","loads"}, tStringValue))
            {
              if(!parseLoadsBlock(fin))
                return false;
              load_block_found = true;
            }
        }
    }

    if(!load_block_found)
    {
      std::cout << "ERROR:XMLGenerator:parseLoads: No load block found \n";
      return false;
    }

    return true;
}

/******************************************************************************/
bool XMLGenerator::parseLoadsBlock(std::istream &fin)
/******************************************************************************/
{
  std::vector<std::string> tInputStringList;
  std::vector<std::string> tokens;
  std::string tStringValue;
  
  while (!fin.eof())
  {
    getTokensFromLine(fin,tokens);

    if(tokens.size() > 0)
    {
      for(size_t j=0; j<tokens.size(); ++j)
          tokens[j] = toLower(tokens[j]);
      if(parseSingleValue(tokens, tInputStringList = {"end","loads"}, tStringValue))
        break;
      else
        if(!parseLoadLine(tokens))
          return false;
    }
  }
  return true;
}

/******************************************************************************/
void XMLGenerator::getTokensFromLine(std::istream &fin, std::vector<std::string>& tokens)
/******************************************************************************/
{
    char buf[MAX_CHARS_PER_LINE];
    
    tokens.clear();
    fin.getline(buf, MAX_CHARS_PER_LINE);
    parseTokens(buf, tokens);
}

/******************************************************************************/
bool XMLGenerator::parseLoadLine(std::vector<std::string>& tokens)
/******************************************************************************/
{
    XMLGen::Load new_load;
    new_load.type = tokens[0];
    bool return_status = true;

    if(!new_load.type.compare("traction"))
      return_status = parseTractionLoad(tokens,new_load);
    else if(!new_load.type.compare("pressure"))
      return_status = parsePressureLoad(tokens,new_load);
    else if(!new_load.type.compare("acceleration"))
      return_status = parseAccelerationLoad(tokens,new_load);
    else if(!new_load.type.compare("heat"))
      return_status = parseHeatFluxLoad(tokens,new_load);
    else if(!new_load.type.compare("force"))
      return_status = parseForceLoad(tokens,new_load);
    else
    {
        PrintUnrecognizedTokens(tokens);
        std::cout << "ERROR:XMLGenerator:parseLoads: Unrecognized load type.\n";
        return false;
    }

    if(return_status)
      putLoadInLoadCase(new_load);

    return return_status;
}

/******************************************************************************/
bool XMLGenerator::parseTractionLoad(std::vector<std::string>& tokens, XMLGen::Load& new_load)
/******************************************************************************/
{
    size_t tMin_parameters = 10;
    if(tokens.size() < tMin_parameters)
    {
      std::cout << "ERROR:XMLGenerator:parseLoads: Wrong number of parameters specified for \"traction\" load.\n";
      return false;
    }

    size_t tTokenIndex = 0;

    new_load.app_type = tokens[++tTokenIndex];

    if(parseMeshSetNameOrID(tTokenIndex,tokens,new_load))
    {
      if(!parseMeshSetNameOrID(tTokenIndex,tokens,new_load))
        --tTokenIndex;
    }
    else
    {
      new_load.app_name = "";
      new_load.app_id = tokens[tTokenIndex];
    }

    if(tokens[++tTokenIndex] != "value")
    {
      std::cout << "ERROR:XMLGenerator:parseLoads: \"value\" keyword not specified after sideset id.\n";
      return false;
    }

    new_load.values.push_back(tokens[++tTokenIndex]);
    new_load.values.push_back(tokens[++tTokenIndex]);
    new_load.values.push_back(tokens[++tTokenIndex]);

    if(tokens[++tTokenIndex] != "load" || tokens[++tTokenIndex] != "id")
    {
      std::cout << "ERROR:XMLGenerator:parseLoads: \"load id\" keywords not specified after value components.\n";
      return false;
    }

    new_load.load_id = tokens[++tTokenIndex];


    return true;
}

/******************************************************************************/
bool XMLGenerator::parseMeshSetNameOrID(size_t& aTokenIndex, std::vector<std::string>& tokens, XMLGen::Load& new_load)
/******************************************************************************/
{
    if(tokens[++aTokenIndex] == "id")
    {
      new_load.app_id = tokens[++aTokenIndex];
      return true;
    }
    else if(tokens[aTokenIndex] == "name")
    {
      new_load.app_name = tokens[++aTokenIndex];
      return true;
    }
    else
      return false;
}

/******************************************************************************/
bool XMLGenerator::parsePressureLoad(std::vector<std::string>& tokens, XMLGen::Load& new_load)
/******************************************************************************/
{
  size_t tMin_parameters = 8;
  if(tokens.size() < tMin_parameters)
  {
    std::cout << "ERROR:XMLGenerator:parseLoads: Wrong number of parameters specified for \"traction\" load.\n";
    return false;
  }

  size_t tTokenIndex = 0;
  new_load.app_type = tokens[++tTokenIndex];
  if(new_load.app_type != "sideset")
  {
      std::cout << "ERROR:XMLGenerator:parseLoads: Pressures can currently only be specified on sidesets.\n";
      return false;
  }

  if(parseMeshSetNameOrID(tTokenIndex,tokens,new_load))
  {
    if(!parseMeshSetNameOrID(tTokenIndex,tokens,new_load))
      --tTokenIndex;
  }
  else
  {
    new_load.app_name = "";
    new_load.app_id = tokens[tTokenIndex];
  }

  if(tokens[++tTokenIndex] != "value")
  {
      std::cout << "ERROR:XMLGenerator:parseLoads: \"value\" keyword not specified after sideset id.\n";
      return false;
  }
  new_load.values.push_back(tokens[++tTokenIndex]);
  if(tokens[++tTokenIndex] != "load" || tokens[++tTokenIndex] != "id")
  {
      std::cout << "ERROR:XMLGenerator:parseLoads: \"load id\" keywords not specified after value components.\n";
      return false;
  }
  new_load.load_id = tokens[++tTokenIndex];

  return true;
}

/******************************************************************************/
bool XMLGenerator::parseAccelerationLoad(std::vector<std::string>& tokens, XMLGen::Load& new_load)
/******************************************************************************/
{
  size_t tMin_parameters = 7;
  if(tokens.size() != tMin_parameters)
  {
    std::cout << "ERROR:XMLGenerator:parseLoads: Wrong number of parameters specified for \"traction\" load.\n";
    return false;
  }
  new_load.app_type = "body";
  new_load.values.push_back(tokens[1]);
  new_load.values.push_back(tokens[2]);
  new_load.values.push_back(tokens[3]);
  if(tokens[4] != "load" || tokens[5] != "id")
  {
      std::cout << "ERROR:XMLGenerator:parseLoads: \"load id\" keywords not specified after acceleration components.\n";
      return false;
  }
  new_load.load_id = tokens[6];
  return true;
}

/******************************************************************************/
bool XMLGenerator::parseHeatFluxLoad(std::vector<std::string>& tokens, XMLGen::Load& new_load)
/******************************************************************************/
{
  size_t tMin_parameters = 9;
  if(tokens.size() < tMin_parameters)
  {
    std::cout << "ERROR:XMLGenerator:parseLoads: Wrong number of parameters specified for \"traction\" load.\n";
    return false;
  }
  size_t tTokenIndex = 0;
  if(!tokens[++tTokenIndex].compare("flux"))
  {
      new_load.app_type = tokens[++tTokenIndex];
      if(new_load.app_type != "sideset")
      {
          std::cout << "ERROR:XMLGenerator:parseLoads: Heat flux can only be specified on sidesets currently.\n";
          return false;
      }

      if(parseMeshSetNameOrID(tTokenIndex,tokens,new_load))
      {
        if(!parseMeshSetNameOrID(tTokenIndex,tokens,new_load))
          --tTokenIndex;
      }
      else
      {
        new_load.app_name = "";
        new_load.app_id = tokens[tTokenIndex];
      }

      if(tokens[++tTokenIndex] != "value")
      {
          std::cout << "ERROR:XMLGenerator:parseLoads: 'value' keyword not specified after sideset id\n";
          return false;
      }
      new_load.values.push_back(tokens[++tTokenIndex]);
      if(tokens[++tTokenIndex] != "load" || tokens[++tTokenIndex] != "id")
      {
          std::cout << "ERROR:XMLGenerator:parseLoads: \"load id\" keywords not specified after value components.\n";
          return false;
      }
      new_load.load_id = tokens[++tTokenIndex];
  }
  else
  {
      std::cout << "ERROR:XMLGenerator:parseLoads: \"flux\" keyword must follow \"heat\" keyword.\n";
      return false;
  }

  return true;
}

/******************************************************************************/
bool XMLGenerator::parseForceLoad(std::vector<std::string>& tokens, XMLGen::Load& new_load)
/******************************************************************************/
{
  size_t tMin_parameters = 10;
  if(tokens.size() < tMin_parameters)
  {
    std::cout << "ERROR:XMLGenerator:parseLoads: Wrong number of parameters specified for \"traction\" load.\n";
    return false;
  }
  size_t tTokenIndex = 0;
  new_load.app_type = tokens[++tTokenIndex];
  if(new_load.app_type != "sideset" && new_load.app_type != "nodeset")
  {
      std::cout << "ERROR:XMLGenerator:parseLoads: Forces can only be applied to nodesets or sidesets currently.\n";
      return false;
  }

  if(parseMeshSetNameOrID(tTokenIndex,tokens,new_load))
  {
    if(!parseMeshSetNameOrID(tTokenIndex,tokens,new_load))
      --tTokenIndex;
  }
  else
  {
    new_load.app_name = "";
    new_load.app_id = tokens[tTokenIndex];
  }
  
  if(tokens[++tTokenIndex] != "value")
  {
      std::cout << "ERROR:XMLGenerator:parseLoads: \"value\" keyword not specified after nodeset or sideset id.\n";
      return false;
  }
  new_load.values.push_back(tokens[++tTokenIndex]);
  new_load.values.push_back(tokens[++tTokenIndex]);
  new_load.values.push_back(tokens[++tTokenIndex]);
  if(tokens[++tTokenIndex] != "load" || tokens[++tTokenIndex] != "id")
  {
      std::cout << "ERROR:XMLGenerator:parseLoads: \"load id\" keywords not specified after value components.\n";
      return false;
  }
  new_load.load_id = tokens[++tTokenIndex];

  return true;
}

/******************************************************************************/
void XMLGenerator::putLoadInLoadCase(XMLGen::Load& new_load)
/******************************************************************************/
{
  bool found_load_case = putLoadInLoadCaseWithMatchingID(new_load);

  if(!found_load_case)
    createNewLoadCase(new_load);
}

/******************************************************************************/
bool XMLGenerator::putLoadInLoadCaseWithMatchingID(XMLGen::Load& new_load)
/******************************************************************************/
{
  for(size_t h=0; h<m_InputData.load_cases.size(); ++h)
      if(m_InputData.load_cases[h].id == new_load.load_id)
      {
        m_InputData.load_cases[h].loads.push_back(new_load);
        return true;
      }
  return false;
}

/******************************************************************************/
void XMLGenerator::createNewLoadCase(XMLGen::Load& new_load)
/******************************************************************************/
{
  XMLGen::LoadCase new_load_case;
  new_load_case.id = new_load.load_id;
  new_load_case.loads.push_back(new_load);
  m_InputData.load_cases.push_back(new_load_case);
}


/******************************************************************************/
bool XMLGenerator::parseUncertainties(std::istream &fin)
/******************************************************************************/
{
    std::vector<std::string> tInputStringList;
    char buf[MAX_CHARS_PER_LINE];
    std::vector<std::string> tokens;
    std::string tStringValue;
    const std::string error_prestring = "ERROR:XMLGenerator:parseUncertainties: ";

    // read each line of the file
    while (!fin.eof())
    {
        // read an entire line into memory
        fin.getline(buf, MAX_CHARS_PER_LINE);
        tokens.clear();
        parseTokens(buf, tokens);

        // process the tokens
        if(tokens.size() > 0)
        {
            for(size_t j=0; j<tokens.size(); ++j)
                tokens[j] = toLower(tokens[j]);

            if(parseSingleValue(tokens, tInputStringList = {"begin","uncertainty"}, tStringValue))
            {
                XMLGen::Uncertainty new_uncertainty;
                new_uncertainty.variable_type = "load"; // default until user is allowed to set it
                // found an uncertainty. parse it.
                while (!fin.eof())
                {
                    tokens.clear();
                    fin.getline(buf, MAX_CHARS_PER_LINE);
                    parseTokens(buf, tokens);
                    // process the tokens
                    if(tokens.size() > 0)
                    {
                        std::vector<std::string> unlowered_tokens = tokens;

                        for(size_t j=0; j<tokens.size(); ++j)
                            tokens[j] = toLower(tokens[j]);

                        // begin uncertainty
                        //      variable_type load or material
                        //      type angle variation
                        //      axis STRING
                        //      load INTEGER
                        //      distribution STRING
                        //      mean VALUE
                        //      lower bound VALUE
                        //      upper bound VALUE
                        //      standard deviation VALUE
                        //      num samples INTEGER
                        // end uncertainty

                        if(parseSingleValue(tokens, tInputStringList = {"end","uncertainty"}, tStringValue))
                        {
                            break;
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"type"}, tStringValue))
                        {
                            if(tokens.size() < 2)
                            {
                                std::cout << error_prestring << "No type specified after \"type\" keyword.\n";
                                return false;
                            }
                            new_uncertainty.type = tokens[1];
                            for(size_t j=2; j<tokens.size(); ++j)
                            {
                                new_uncertainty.type += " ";
                                new_uncertainty.type += tokens[j];
                            }
                            if(new_uncertainty.type != "angle variation")
                            {
                                std::cout << error_prestring << "Unmatched uncertainty type.\n";
                                return false;
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"axis"}, tStringValue))
                        {
                            if(tokens.size() < 2)
                            {
                                std::cout << error_prestring << "No axis specified after \"axis\" keyword.\n";
                                return false;
                            }
                            new_uncertainty.axis = tokens[1];
                            if(new_uncertainty.axis != "x" &&
                                    new_uncertainty.axis != "y" &&
                                    new_uncertainty.axis != "z")
                            {
                                std::cout << error_prestring << "Unmatched uncertainty axis.\n";
                                return false;
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"load"}, tStringValue))
                        {
                            if(tokens.size() < 2)
                            {
                                std::cout << error_prestring << "No load specified after \"load\" keyword.\n";
                                return false;
                            }
                            new_uncertainty.id = tokens[1];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"variable", "type"}, tStringValue))
                        {
                            if(tokens.size() < 3)
                            {
                                std::cout << error_prestring << "No load specified after \"variable type\" keyword.\n";
                                return false;
                            }
                            new_uncertainty.variable_type = tokens[2];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"distribution"}, tStringValue))
                        {
                            if(tokens.size() < 2)
                            {
                                std::cout << error_prestring << "No distribution specified after \"distribution\" keyword.\n";
                                return false;
                            }
                            new_uncertainty.distribution = tokens[1];
                            if(new_uncertainty.distribution != "beta" &&
                                    new_uncertainty.distribution != "uniform" &&
                                    new_uncertainty.distribution != "normal")
                            {
                                std::cout << error_prestring << "Unmatched uncertainty distribution.\n";
                                return false;
                            }
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"mean"}, tStringValue))
                        {
                            if(tokens.size() < 2)
                            {
                                std::cout << error_prestring << "No mean specified after \"mean\" keyword.\n";
                                return false;
                            }
                            new_uncertainty.mean = tokens[1];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"lower", "bound"}, tStringValue))
                        {
                            if(tokens.size() < 3)
                            {
                                std::cout << error_prestring << "No lower bound specified after \"lower bound\" keyword.\n";
                                return false;
                            }
                            new_uncertainty.lower = tokens[2];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"upper", "bound"}, tStringValue))
                        {
                            if(tokens.size() < 3)
                            {
                                std::cout << error_prestring << "No load specified after \"upper bound\" keyword.\n";
                                return false;
                            }
                            new_uncertainty.upper = tokens[2];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"standard", "deviation"}, tStringValue))
                        {
                            if(tokens.size() < 3)
                            {
                                std::cout << error_prestring << "No standard deviation specified after \"standard deviation\" keyword.\n";
                                return false;
                            }
                            new_uncertainty.standard_deviation = tokens[2];
                        }
                        else if(parseSingleValue(tokens, tInputStringList = {"num", "samples"}, tStringValue))
                        {
                            if(tokens.size() < 2)
                            {
                                std::cout << error_prestring << "No samples specified after \"num samples\" keyword.\n";
                                return false;
                            }
                            new_uncertainty.num_samples = tokens[2];
                        }
                        else
                        {
                            PrintUnrecognizedTokens(tokens);
                            std::cout << "ERROR:XMLGenerator:parseUncertainties: Unrecognized keywords.\n";
                            return false;
                        }
                    }
                }

                // check that uncertainty well specified
                if(new_uncertainty.type == "angle variation")
                {
                    if(new_uncertainty.axis != "x" &&
                            new_uncertainty.axis != "y" &&
                            new_uncertainty.axis != "z")
                    {
                        std::cout << error_prestring << "Angular variation requires valid \"axis\" keyword.\n";
                        return false;
                    }
                }
                else
                {
                    std::cout << error_prestring << "Unmatched or absent uncertainty \"type\" keyword.\n";
                    return false;
                }

                // check that distribution well specified
                if(new_uncertainty.distribution == "beta")
                {
                    if(new_uncertainty.lower == "")
                    {
                        std::cout << error_prestring << "Beta distribution requires \"lower\" keyword.\n";
                        return false;
                    }
                    if(new_uncertainty.upper == "")
                    {
                        std::cout << error_prestring << "Beta distribution requires \"upper\" keyword.\n";
                        return false;
                    }
                    if(new_uncertainty.standard_deviation == "")
                    {
                        std::cout << error_prestring << "Beta distribution requires \"standard deviation\" keyword.\n";
                        return false;
                    }
                    if(new_uncertainty.mean == "")
                    {
                        std::cout << error_prestring << "Beta distribution requires \"mean\" keyword.\n";
                        return false;
                    }
                }
                else if(new_uncertainty.distribution == "uniform")
                {
                    if(new_uncertainty.lower == "")
                    {
                        std::cout << error_prestring << "Uniform distribution requires \"lower\" keyword.\n";
                        return false;
                    }
                    if(new_uncertainty.upper == "")
                    {
                        std::cout << error_prestring << "Uniform distribution requires \"upper\" keyword.\n";
                        return false;
                    }
                }
                else if(new_uncertainty.distribution == "normal")
                {
                    if(new_uncertainty.standard_deviation == "")
                    {
                        std::cout << error_prestring << "Normal distribution requires \"standard deviation\" keyword.\n";
                        return false;
                    }
                    if(new_uncertainty.mean == "")
                    {
                        std::cout << error_prestring << "Normal distribution requires \"mean\" keyword.\n";
                        return false;
                    }
                }
                else
                {
                    return false;
                }

                // check samples
                if(new_uncertainty.num_samples == "")
                {
                    std::cout << error_prestring << "Uncertainty requires \"num samples\" keyword.\n";
                    return false;
                }

                m_InputData.uncertainties.push_back(new_uncertainty);
            }
        }
    }

    return true;
}

/******************************************************************************/
bool XMLGenerator::parseBCs(std::istream &fin)
/******************************************************************************/
{
    std::vector<std::string> tInputStringList;
    std::vector<std::string> tokens;
    std::string tStringValue;
    bool bc_block_found = false;

    // read each line of the file
    while (!fin.eof())
    {
        getTokensFromLine(fin,tokens);

        // process the tokens
        if(tokens.size() > 0)
        {
            if(parseSingleValue(tokens, tInputStringList = {"begin","boundary","conditions"}, tStringValue))
            {
              if(!parseBCsBlock(fin))
                return false;
              bc_block_found = true;
            }
        }
    }

    if(!bc_block_found)
    {
      std::cout << "ERROR:XMLGenerator:parseLoads: No boundary condition block found \n";
      return false;
    }

    return true;
}

/******************************************************************************/
bool XMLGenerator::parseBCsBlock(std::istream &fin)
/******************************************************************************/
{
  std::vector<std::string> tInputStringList;
  std::vector<std::string> tokens;
  std::string tStringValue;
  
  while (!fin.eof())
  {
    getTokensFromLine(fin,tokens);

    if(tokens.size() > 0)
    {
      for(size_t j=0; j<tokens.size(); ++j)
          tokens[j] = toLower(tokens[j]);
      if(parseSingleValue(tokens, tInputStringList = {"end","boundary","conditions"}, tStringValue))
        break;
      else
        if(!parseBCLine(tokens))
          return false;
    }
  }
  return true;
}

/******************************************************************************/
bool XMLGenerator::parseBCLine(std::vector<std::string>& tokens)
/******************************************************************************/
{
    XMLGen::BC new_bc;
    bool return_status = true;

    if(tokens.size() < 7)
    {
        std::cout << "ERROR:XMLGenerator:parseBCs: Not enough parameters were specified for BC in \"boundary conditions\" block.\n";
        return false;
    }
    if(tokens[0] != "fixed")
    {
        std::cout << "ERROR:XMLGenerator:parseBCs: First boundary condition token must be \"fixed\".\n";
        return false;
    }
    new_bc.type = tokens[1];

    if(!new_bc.type.compare("displacement"))
      return_status = parseDisplacementBC(tokens,new_bc);
    else if(!new_bc.type.compare("temperature"))
      return_status = parseTemperatureBC(tokens,new_bc);
    else
    {
        PrintUnrecognizedTokens(tokens);
        std::cout << "ERROR:XMLGenerator:parseLoads: Unrecognized boundary condition type.\n";
        return false;
    }

    m_InputData.bcs.push_back(new_bc);

    return return_status;
}

/******************************************************************************/
bool XMLGenerator::parseDisplacementBC(std::vector<std::string>& tokens, XMLGen::BC& new_bc)
/******************************************************************************/
{
  // Potential syntax:
  // fixed displacement nodeset/sideset 1 bc id 1                 // all dofs have fixed disp of 0.0
  // fixed displacement nodeset/sideset 1 <x,y,z> bc id 1         // x, y, or z dof has fixed disp of 0.0
  // fixed displacement nodeset/sideset 1 <x,y,z> 3.0 bc id 1     // x, y, or z dof has fixed disp of 3.0

  size_t tTokenIndex = 1;
  bool tNameOrIDSpecified = false;

  if(tokens[++tTokenIndex] != "nodeset" && tokens[tTokenIndex] != "sideset")
  {
    std::cout << "ERROR:XMLGenerator:parseBCs: Boundary conditions can only be applied to \"nodeset\" or \"sideset\" types.\n";
    return false;
  }
  new_bc.app_type = tokens[tTokenIndex];

  if(tokens[++tTokenIndex] == "id")
  {
    new_bc.app_id = tokens[++tTokenIndex];
    tNameOrIDSpecified = true;
  }
  else if(tokens[tTokenIndex] == "name")
  {
    new_bc.app_name = tokens[++tTokenIndex];
    tNameOrIDSpecified = true;
  }
  else
  {
    new_bc.app_id = tokens[tTokenIndex];
    new_bc.app_name = "";
  }
  
  if(tNameOrIDSpecified)
  {
    if(tokens[++tTokenIndex] == "id")
      new_bc.app_id = tokens[++tTokenIndex];
    else if(tokens[tTokenIndex] == "name")
      new_bc.app_name = tokens[++tTokenIndex];
    else
      --tTokenIndex;
  }

  new_bc.dof = "";
  new_bc.value = "";
  if(tokens[++tTokenIndex] != "bc")
  {
    if(tokens[tTokenIndex] != "x" && tokens[tTokenIndex] != "y" && tokens[tTokenIndex] != "z")
    {
        std::cout << "ERROR:XMLGenerator:parseBCs: Boundary condition degree of freedom must be either \"x\", \"y\", or \"z\".\n";
        return false;
    }
    new_bc.dof = tokens[tTokenIndex];
    if(tokens[++tTokenIndex] != "bc")
    {
        new_bc.value = tokens[tTokenIndex];
        tTokenIndex += 3;
        new_bc.bc_id = tokens[tTokenIndex];
    }
    else
    {
        tTokenIndex += 2;
        new_bc.bc_id = tokens[tTokenIndex];
    }
  }
  else
  {
    tTokenIndex += 2;
    new_bc.bc_id = tokens[tTokenIndex];
  }

  return true;
}

/******************************************************************************/
bool XMLGenerator::parseTemperatureBC(std::vector<std::string>& tokens, XMLGen::BC& new_bc)
/******************************************************************************/
{
  // Potential syntax:
  // fixed temperature nodeset 1 bc id 1
  // fixed temperature nodeset 1 value 25.0 bc id 1
  if(tokens[2] != "nodeset" && tokens[2] != "sideset")
  {
      std::cout << "ERROR:XMLGenerator:parseBCs: Boundary conditions can only be applied to \"nodeset\" or \"sideset\" types.\n";
      return false;
  }
  new_bc.app_type = tokens[2];

  size_t tOffset = 0;
  bool name_or_id_specified = false;
  if(tokens[3] == "id")
  {
    ++tOffset;
    new_bc.app_id = tokens[3+tOffset];
    name_or_id_specified = true;
  }
  else if(tokens[3] == "name")
  {
    ++tOffset;
    new_bc.app_name = tokens[3+tOffset];
    name_or_id_specified = true;
  }
  else
  {
    new_bc.app_id = tokens[3];
    new_bc.app_name = "";
  }

  if(name_or_id_specified)
  {
    if(tokens[4+tOffset] == "id")
    {
      ++tOffset;
      new_bc.app_id = tokens[4+tOffset];
    }
    else if(tokens[4+tOffset] == "name")
    {
      ++tOffset;
      new_bc.app_name = tokens[4+tOffset];
    }
  }

  new_bc.value = "";
  if(tokens[4+tOffset] != "bc")
  {
      if(tokens[4+tOffset] != "value")
      {
          std::cout << "ERROR:XMLGenerator:parseBCs: Invalid BC syntax.\n";
          return false;
      }
      new_bc.value = tokens[5+tOffset];
      if(tokens[6+tOffset] != "bc")
      {
          std::cout << "ERROR:XMLGenerator:parseBCs: Invalid BC syntax.\n";
          return false;
      }
      new_bc.bc_id = tokens[8+tOffset];
  }
  else
      new_bc.bc_id = tokens[6+tOffset];

  return true;
}

/******************************************************************************/
bool XMLGenerator::parseSingleValue(const std::vector<std::string> &aTokens,
                                    const std::vector<std::string> &aInputStrings,
                                    std::string &aReturnStringValue)
/******************************************************************************/
{
    size_t i;

    aReturnStringValue = "";

    if(aInputStrings.size() < 1 || aTokens.size() < 1 || aTokens.size() < aInputStrings.size())
        return false;

    for(i=0; i<aInputStrings.size(); ++i)
    {
        if(aTokens[i].compare(aInputStrings[i]))
        {
            return false;
        }
    }

    if(aTokens.size() == (aInputStrings.size() + 1))
        aReturnStringValue = aTokens[i];

    return true;
}

/******************************************************************************/
bool XMLGenerator::parseSingleUnLoweredValue(const std::vector<std::string> &aTokens,
                                             const std::vector<std::string> &aUnLoweredTokens,
                                    const std::vector<std::string> &aInputStrings,
                                    std::string &aReturnStringValue)
/******************************************************************************/
{
    size_t i;

    aReturnStringValue = "";

    assert(aTokens.size() == aUnLoweredTokens.size());

    if(aInputStrings.size() < 1 || aTokens.size() < 1 || aTokens.size() < aInputStrings.size())
        return false;

    for(i=0; i<aInputStrings.size(); ++i)
    {
        if(aTokens[i].compare(aInputStrings[i]))
        {
            return false;
        }
    }

    if(aTokens.size() == (aInputStrings.size() + 1))
        aReturnStringValue = aUnLoweredTokens[i];

    return true;
}

/******************************************************************************/
void XMLGenerator::initializePlatoProblemOptions()
{
    m_InputData.output_frequency="5";
    m_InputData.discretization="density";
    m_InputData.initial_density_value="0.5";
    m_InputData.optimization_algorithm="oc";
    m_InputData.output_method="epu";
    m_InputData.check_gradient = "false";
    m_InputData.check_hessian = "false";
    m_InputData.filter_type = "kernel";
    m_InputData.filter_power = "1";

    m_InputData.mInnerKKTtoleranceGCMMA = "";
    m_InputData.mOuterKKTtoleranceGCMMA = "";
    m_InputData.mInnerControlStagnationToleranceGCMMA = "";
    m_InputData.mOuterControlStagnationToleranceGCMMA = "";
    m_InputData.mOuterObjectiveStagnationToleranceGCMMA = "";
    m_InputData.mMaxInnerIterationsGCMMA = "";
    m_InputData.mOuterStationarityToleranceGCMMA = "";
    m_InputData.mInitialMovingAsymptotesScaleFactorGCMMA = "";

    m_InputData.mMaxRadiusScale = "";
    m_InputData.mInitialRadiusScale = "";
    m_InputData.mMaxTrustRegionRadius = "";
    m_InputData.mMinTrustRegionRadius = "";
    m_InputData.mMaxTrustRegionIterations = "5";
    m_InputData.mTrustRegionExpansionFactor = "";
    m_InputData.mTrustRegionContractionFactor = "";
    m_InputData.mTrustRegionRatioLowKS = "";
    m_InputData.mTrustRegionRatioMidKS = "";
    m_InputData.mTrustRegionRatioUpperKS = "";

    m_InputData.mUseMeanNorm = "";
    m_InputData.mAugLagPenaltyParam = "";
    m_InputData.mFeasibilityTolerance = "";
    m_InputData.mAugLagPenaltyParamScale = "";
    m_InputData.mMaxNumAugLagSubProbIter = "";

    m_InputData.mHessianType = "disabled";
    m_InputData.mLimitedMemoryStorage = "8";
    m_InputData.mDisablePostSmoothingKS = "true";
    m_InputData.mProblemUpdateFrequency = "5";
    m_InputData.mOuterGradientToleranceKS = "";
    m_InputData.mOuterStationarityToleranceKS = "";
    m_InputData.mOuterStagnationToleranceKS = "";
    m_InputData.mOuterControlStagnationToleranceKS = "";
    m_InputData.mOuterActualReductionToleranceKS = "";

    m_InputData.filter_heaviside_min = "";
    m_InputData.filter_heaviside_update = "";
    m_InputData.filter_heaviside_max = "";

    m_InputData.filter_projection_start_iteration = "";
    m_InputData.filter_projection_update_interval = "";
    m_InputData.filter_use_additive_continuation = "";

    m_InputData.write_restart_file = "False";
    m_InputData.optimization_type = "topology";
}

/******************************************************************************/
bool XMLGenerator::parseOptimizationParameters(std::istream &fin)
{
  // Initialize variables
  this->initializePlatoProblemOptions();

  std::string tStringValue;
  std::vector<std::string> tInputStringList;
  // read each line of the file
  while (!fin.eof())
  {
    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);
    std::vector<std::string> tokens;
    parseTokens(buf, tokens);

    // process the tokens
    if(tokens.size() > 0)
    {
      for(size_t j=0; j<tokens.size(); ++j)
        tokens[j] = toLower(tokens[j]);

      if(parseSingleValue(tokens, tInputStringList = {"begin","optimization","parameters"}, tStringValue))
      {
        // found optimization parameters
        while (!fin.eof())
        {
          fin.getline(buf, MAX_CHARS_PER_LINE);
          tokens.clear();
          parseTokens(buf, tokens);
          // process the tokens
          if(tokens.size() > 0)
          {
            std::vector<std::string> unlowered_tokens = tokens;

            for(size_t j=0; j<tokens.size(); ++j)
              tokens[j] = toLower(tokens[j]);

            if(parseSingleValue(tokens, tInputStringList = {"end","optimization","parameters"}, tStringValue))
            {
              break;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"begin","material_box"}, tStringValue))
            {
              while (!fin.eof())
              {
                fin.getline(buf, MAX_CHARS_PER_LINE);
                tokens.clear();
                parseTokens(buf, tokens);
                // process the tokens
                if(tokens.size() > 0)
                {
                  for(size_t j=0; j<tokens.size(); ++j)
                    tokens[j] = toLower(tokens[j]);

                  if(parseSingleValue(tokens, tInputStringList = {"end","material_box"}, tStringValue))
                  {
                    if(m_InputData.levelset_material_box_min == "" ||
                        m_InputData.levelset_material_box_max == "")
                    {
                      std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: Not enough info in \"material_box\" block.\n";
                      return false;
                    }
                    break;
                  }
                  else if(parseSingleValue(tokens, tInputStringList = {"min", "coords"}, tStringValue))
                  {
                    if(tokens.size() != 5)
                    {
                      std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: Wrong number of params in \"min coords\" line.\n";
                      return false;
                    }
                    for(size_t j=2; j<5; ++j)
                    {
                      m_InputData.levelset_material_box_min += tokens[j];
                      if(j<4)
                        m_InputData.levelset_material_box_min += " ";
                    }
                  }
                  else if(parseSingleValue(tokens, tInputStringList = {"max", "coords"}, tStringValue))
                  {
                    if(tokens.size() != 5)
                    {
                      std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: Wrong number of params in \"max coords\" line.\n";
                      return false;
                    }
                    for(size_t j=2; j<5; ++j)
                    {
                      m_InputData.levelset_material_box_max += tokens[j];
                      if(j<4)
                        m_InputData.levelset_material_box_max += " ";
                    }
                  }
                }
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"objective","number","standard","deviations"}, tStringValue))
            {
              if(tokens.size() < 5)
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"objective number standard deviations\" keywords.\n";
                return false;
              }
              m_InputData.objective_number_standard_deviations = tokens[4];
            }
            else if(parseSingleValue(tokens, tInputStringList = {"output", "standard","deviations"}, tStringValue))
            {
              if(tokens.size() < 4)
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No outputs specified after \"output standard deviations\" keywords.\n";
                return false;
              }
              for(size_t j=3; j<tokens.size(); ++j)
              {
                m_InputData.mStandardDeviations.push_back(tokens[j]);
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"initial","density","value"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"initial density value\" keyword(s).\n";
                return false;
              }
              m_InputData.initial_density_value = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"optimization","type"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"optimization type\" keyword(s).\n";
                return false;
              }
              m_InputData.optimization_type = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"csm","file"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"csm file\" keyword(s).\n";
                return false;
              }
              m_InputData.csm_filename = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"use","normalization","in","aggregator"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"use normalization in aggregator\" keyword(s).\n";
                return false;
              }
              m_InputData.mUseNormalizationInAggregator = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"create","levelset","spheres"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"create levelset spheres\" keyword(s).\n";
                return false;
              }
              m_InputData.create_levelset_spheres = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"write","restart","file"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"write restart file\" keyword(s).\n";
                return false;
              }
              if(tStringValue == "true")
                m_InputData.write_restart_file = "True";
            }
            else if(parseSingleValue(tokens, tInputStringList = {"levelset","initialization","method"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"levelset initialization method\" keyword(s).\n";
                return false;
              }
              m_InputData.levelset_initialization_method = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"max","iterations"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"max iterations\" keyword(s).\n";
                return false;
              }
              m_InputData.max_iterations = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"restart","iteration"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"restart iteration\" keyword(s).\n";
                return false;
              }
              m_InputData.restart_iteration = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"initial","guess","filename"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"initial guess filename\" keyword(s).\n";
                return false;
              }
              m_InputData.initial_guess_filename = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"initial","guess","field","name"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"initial guess field name\" keyword(s).\n";
                return false;
              }
              m_InputData.initial_guess_field_name = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"prune","mesh"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"prune mesh\" keyword(s).\n";
                return false;
              }
              m_InputData.prune_mesh = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"number","buffer","layers"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"number buffer layers\" keyword(s).\n";
                return false;
              }
              m_InputData.number_buffer_layers = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"number","prune","and","refine","processors"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"number prune and refine processors\" keyword(s).\n";
                return false;
              }
              m_InputData.number_prune_and_refine_processors = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"number","refines"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"number refines\" keyword(s).\n";
                return false;
              }
              m_InputData.number_refines = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"mma","move","limit"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"mma move limit\" keyword(s).\n";
                return false;
              }
              m_InputData.mMMAMoveLimit = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"mma","control","stagnation","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"mma control stagnation tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mMMAControlStagnationTolerance = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"mma","objective","stagnation","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"mma objective stagnation tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mMMAObjectiveStagnationTolerance = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"mma","asymptote","expansion"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"mma asymptote expansion\" keyword(s).\n";
                return false;
              }
              m_InputData.mMMAAsymptoteExpansion = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"mma","asymptote","contraction"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"mma asymptote contraction\" keyword(s).\n";
                return false;
              }
              m_InputData.mMMAAsymptoteContraction = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"mma","max","sub","problem","iterations"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"mma max sub problem iterations\" keyword(s).\n";
                return false;
              }
              m_InputData.mMMAMaxNumSubProblemIterations = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"mma","max","trust","region","iterations"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"mma max trust region iterations\" keyword(s).\n";
                return false;
              }
              m_InputData.mMMAMaxTrustRegionIterations = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","max","trust","region","iterations"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks max trust region iterations\" keyword(s).\n";
                return false;
              }
              m_InputData.mMaxTrustRegionIterations = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","trust","region","expansion","factor"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks trust region expansion factor\" keyword(s).\n";
                return false;
              }
              m_InputData.mTrustRegionExpansionFactor = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","trust","region","contraction","factor"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks trust region contraction factor\" keyword(s).\n";
                return false;
              }
              m_InputData.mTrustRegionContractionFactor = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","outer","gradient","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks outer gradient tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mOuterGradientToleranceKS = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","outer","stationarity","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks outer stationarity tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mOuterStationarityToleranceKS = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","outer","stagnation","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks outer stagnation tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mOuterStagnationToleranceKS = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","outer","control","stagnation","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks outer control stagnation tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mOuterControlStagnationToleranceKS = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","disable","post","smoothing"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks disable post smoothing\" keyword(s).\n";
                return false;
              }
              m_InputData.mDisablePostSmoothingKS = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","outer","actual","reduction","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks outer actual reduction tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mOuterActualReductionToleranceKS = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","trust","region","ratio","low"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks trust region ratio low\" keyword(s).\n";
                return false;
              }
              m_InputData.mTrustRegionRatioLowKS = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","trust","region","ratio","mid"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks trust region ratio mid\" keyword(s).\n";
                return false;
              }
              m_InputData.mTrustRegionRatioMidKS = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","trust","region","ratio","high"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks trust region ratio high\" keyword(s).\n";
                return false;
              }
              m_InputData.mTrustRegionRatioUpperKS = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","initial","radius","scale"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks initial radius scale\" keyword(s).\n";
                return false;
              }
              m_InputData.mInitialRadiusScale = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks","max","radius","scale"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks max radius scale\" keyword(s).\n";
                return false;
              }
              m_InputData.mMaxRadiusScale = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"problem","update","frequency"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"problem update frequency\" keyword(s).\n";
                return false;
              }
              m_InputData.mProblemUpdateFrequency = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"gcmma","max","inner","iterations"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"gcmma max inner iterations\" keyword(s).\n";
                return false;
              }
              m_InputData.mMaxInnerIterationsGCMMA = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"gcmma","inner","kkt","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"gcmma inner kkt tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mInnerKKTtoleranceGCMMA = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"gcmma","inner","control","stagnation","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"gcmma inner control stagnation tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mInnerControlStagnationToleranceGCMMA = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"gcmma","outer","kkt","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"gcmma outer kkt tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mOuterKKTtoleranceGCMMA = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"gcmma","outer","control","stagnation","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"gcmma control stagnation tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mOuterControlStagnationToleranceGCMMA = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"gcmma","outer","objective","stagnation","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"gcmma outer objective stagnation tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mOuterObjectiveStagnationToleranceGCMMA = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"gcmma","outer","stationarity","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"gcmma outer stationarity tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mOuterStationarityToleranceGCMMA = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"gcmma","initial","moving","asymptotes","scale","factor"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"gcmma initial moving asymptotes scale factor\" keyword(s).\n";
                return false;
              }
              m_InputData.mInitialMovingAsymptotesScaleFactorGCMMA = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"levelset","sphere","packing", "factor"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"levelset sphere packing factor\" keyword(s).\n";
                return false;
              }
              m_InputData.levelset_sphere_packing_factor = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"levelset","sphere","radius"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"levelset sphere radius\" keyword(s).\n";
                return false;
              }
              m_InputData.levelset_sphere_radius = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"levelset","nodesets"}, tStringValue))
            {
              if(tokens.size() < 3)
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"levelset nodesets\" keyword(s).\n";
                return false;
              }
              for(size_t j=2; j<tokens.size(); ++j)
              {
                m_InputData.levelset_nodesets.push_back(tokens[j]);
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"output","frequency"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"output frequency\" keyword(s).\n";
                return false;
              }
              m_InputData.output_frequency = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"output","method"}, tStringValue))
            {
              if(tokens.size() < 3)
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"output method\" keyword(s).\n";
                return false;
              }
              m_InputData.output_method = tokens[2];
              for(size_t j=3; j<tokens.size(); ++j)
              {
                m_InputData.output_method += " ";
                m_InputData.output_method += tokens[j];
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"fixed","blocks"}, tStringValue))
            {
              if(tokens.size() < 3)
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"fixed blocks\" keyword(s).\n";
                return false;
              }
              for(size_t j=2; j<tokens.size(); ++j)
              {
                m_InputData.fixed_block_ids.push_back(tokens[j]);
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"fixed","sidesets"}, tStringValue))
            {
              if(tokens.size() < 3)
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"fixed sidesets\" keyword(s).\n";
                return false;
              }
              for(size_t j=2; j<tokens.size(); ++j)
              {
                m_InputData.fixed_sideset_ids.push_back(tokens[j]);
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"fixed","nodesets"}, tStringValue))
            {
              if(tokens.size() < 3)
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"fixed nodesets\" keyword(s).\n";
                return false;
              }
              for(size_t j=2; j<tokens.size(); ++j)
              {
                m_InputData.fixed_nodeset_ids.push_back(tokens[j]);
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"number","processors"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"number processors\" keyword(s).\n";
                return false;
              }
              m_InputData.num_opt_processors = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","type"}, tStringValue))
            {
              // retrieve input
              m_InputData.filter_type = "";
              for(size_t j=2; j<tokens.size(); ++j)
              {
                if(j!=2)
                {
                  m_InputData.filter_type += " ";
                }
                m_InputData.filter_type += tokens[j];
              }

              // check input is valid
              if(m_InputData.filter_type != m_filterType_identity_generatorName &&
                  m_InputData.filter_type != m_filterType_kernel_generatorName &&
                  m_InputData.filter_type != m_filterType_kernelThenHeaviside_generatorName &&
                  m_InputData.filter_type != m_filterType_kernelThenTANH_generatorName)
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: \"filter type\" did not match allowed types which include:\n\t"
                  <<"\""<<m_filterType_identity_generatorName<<"\","
                  <<"\""<<m_filterType_kernel_generatorName<<"\","
                  <<"\""<<m_filterType_kernelThenHeaviside_generatorName<<"\""
                  <<"\""<<m_filterType_kernelThenTANH_generatorName<<"\""
                  <<".\n";
                return false;
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","heaviside","min"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"filter heaviside min\" keyword(s).\n";
                return false;
              }
              if(m_InputData.filter_heaviside_min!="")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: \"filter heaviside scale\" and \"filter heaviside min\" both specified.\n"
                  <<"\tOnly specify one of them. \"max/min/update\" are for updating continuation problems. \"scale\" is used otherwise.\n";
                return false;
              }
              m_InputData.filter_heaviside_min = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","heaviside","update"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"filter heaviside update\" keyword(s).\n";
                return false;
              }
              m_InputData.filter_heaviside_update = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","heaviside","max"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"filter heaviside max\" keyword(s).\n";
                return false;
              }
              if(m_InputData.filter_heaviside_max!="")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: \"filter heaviside scale\" and \"filter heaviside max\" both specified."
                  <<"\tOnly specify one of them. \"max/min/update\" are for updating continuation problems. \"scale\" is used otherwise.\n";
                return false;
              }
              m_InputData.filter_heaviside_max = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","heaviside","scale"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"filter heaviside scale\" keyword(s).\n";
                return false;
              }
              if(m_InputData.filter_heaviside_min!="")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: \"filter heaviside scale\" and \"filter heaviside min\" both specified."
                  <<"\tOnly specify one of them. \"max/min/update\" are for updating continuation problems. \"scale\" is used otherwise.\n";
                return false;
              }
              if(m_InputData.filter_heaviside_max!="")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: \"filter heaviside scale\" and \"filter heaviside max\" both specified."
                  <<"\tOnly specify one of them. \"max/min/update\" are for updating continuation problems. \"scale\" is used otherwise.\n";
                return false;
              }
              m_InputData.filter_heaviside_min = tStringValue;
              m_InputData.filter_heaviside_max = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","radial","power"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"filter power\" keyword(s).\n";
                return false;
              }
              m_InputData.filter_power = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","radius","scale"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"filter radius scale\" keyword(s).\n";
                return false;
              }
              m_InputData.filter_radius_scale = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","radius","absolute"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"filter radius absolute\" keyword(s).\n";
                return false;
              }
              m_InputData.filter_radius_absolute = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","projection","start","iteration"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"filter projection start iteration\" keyword(s).\n";
                return false;
              }
              m_InputData.filter_projection_start_iteration = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","projection","update","interval"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"filter projection update interval\" keyword(s).\n";
                return false;
              }
              m_InputData.filter_projection_update_interval = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"filter","use","additive","continuation"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"filter use additive continuation\" keyword(s).\n";
                return false;
              }
              m_InputData.filter_use_additive_continuation = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"algorithm"}, tStringValue))
            {
              if(tokens.size() < 2)
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"algorithm\" keyword(s).\n";
                return false;
              }
              m_InputData.optimization_algorithm = tokens[1];
              for(size_t j=2; j<tokens.size(); ++j)
              {
                m_InputData.optimization_algorithm += " ";
                m_InputData.optimization_algorithm += tokens[j];
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"discretization"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"discretization\" keyword(s).\n";
                return false;
              }
              m_InputData.discretization = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"check","gradient"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"check gradient\" keyword(s).\n";
                return false;
              }
              m_InputData.check_gradient = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"check","hessian"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"check hessian\" keyword(s).\n";
                return false;
              }
              m_InputData.check_hessian = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"hessian","type"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"hessian type\" keyword(s).\n";
                return false;
              }
              m_InputData.mHessianType = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"limited","memory","storage"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"limited memory storage\" keyword(s).\n";
                return false;
              }
              m_InputData.mLimitedMemoryStorage = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"use","mean","norm"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"use mean norm\" keyword(s).\n";
                return false;
              }
              m_InputData.mUseMeanNorm = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"al","penalty","parameter"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"al penalty parameter\" keyword(s).\n";
                return false;
              }
              m_InputData.mAugLagPenaltyParam = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"feasibility","tolerance"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"feasibility tolerance\" keyword(s).\n";
                return false;
              }
              m_InputData.mFeasibilityTolerance = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"al","penalty","scale","factor"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"al penalty scale factor\" keyword(s).\n";
                return false;
              }
              m_InputData.mAugLagPenaltyParamScale = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"al","max","subproblem","iterations"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"al max subproblem iterations\" keyword(s).\n";
                return false;
              }
              m_InputData.mMaxNumAugLagSubProbIter = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"max","trust","region","radius"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"max trust region radius\" keyword(s).\n";
                return false;
              }
              m_InputData.mMaxTrustRegionRadius = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"ks", "min","trust","region","radius"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: No value specified after \"ks min trust region radius\" keyword(s).\n";
                return false;
              }
              m_InputData.mMinTrustRegionRadius = tStringValue;
            }
            else
            {
              PrintUnrecognizedTokens(tokens);
              std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: Unrecognized keyword.\n";
              return false;
            }
          }
        }
      }
    }
  }

  // If there is a restart iteration but no filename specified then we will
  // assume we are coming from the gui and we will use the default "restart_XXX.exo"
  // filename, iteration 1 from the file, and field name "optimizationdofs".
  if(m_InputData.restart_iteration != "" &&
      m_InputData.restart_iteration != "0")
  {
    if(m_InputData.initial_guess_filename == "" &&
        m_InputData.initial_guess_field_name == "")
    {
      // This block indicates that we are coming from the gui so only the
      // restart iteration was specified.  We will fill in the other values
      // based on what we know the gui will be providing for the run.
      m_InputData.initial_guess_filename = "restart_" + m_InputData.restart_iteration + ".exo";
      m_InputData.restart_iteration = "1";
      m_InputData.initial_guess_field_name = "optimizationdofs";
    }
    else
    {
      // This block indicates that the user is manually setting up the
      // restart file and so we depend on him having specified a filename
      // and field name.  If either of these is empty we need to error out.
      if(m_InputData.initial_guess_field_name == "" ||
          m_InputData.initial_guess_filename == "")
      {
        std::cout << "ERROR:XMLGenerator:parseOptimizationParameters: You must specify a valid initial guess mesh filename and a valid field name on that mesh from which initial values will be obtained.\n";
        return false;
      }
    }
  }

  // If "use normalization in aggregator" was not specified choose a default value based on
  // the optimization algorithm.
  if(m_InputData.mUseNormalizationInAggregator.length() == 0)
  {
    if(m_InputData.optimization_algorithm == "ksal" ||
        m_InputData.optimization_algorithm == "rol ksal")
    {
      m_InputData.mUseNormalizationInAggregator = "true";
    }
    else
    {
      m_InputData.mUseNormalizationInAggregator = "false";
    }
  }

  return true;
}

/******************************************************************************/
bool XMLGenerator::parseMesh(std::istream &fin)
  /******************************************************************************/
{
  std::string tStringValue;
  std::vector<std::string> tInputStringList;

  // read each line of the file
  while (!fin.eof())
  {
    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);
    std::vector<std::string> tokens;
    parseTokens(buf, tokens);

    // process the tokens
    if(tokens.size() > 0)
    {
      for(size_t j=0; j<tokens.size(); ++j)
        tokens[j] = toLower(tokens[j]);

      if(parseSingleValue(tokens, tInputStringList = {"begin","mesh"}, tStringValue))
      {
        // found mesh block
        while (!fin.eof())
        {
          fin.getline(buf, MAX_CHARS_PER_LINE);
          tokens.clear();
          parseTokens(buf, tokens);
          // process the tokens
          if(tokens.size() > 0)
          {
            std::vector<std::string> unlowered_tokens = tokens;

            for(size_t j=0; j<tokens.size(); ++j)
              tokens[j] = toLower(tokens[j]);

            if(parseSingleValue(tokens, tInputStringList = {"end","mesh"}, tStringValue))
            {
              break;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"name"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseMesh: No value specified after \"name\" keyword.\n";
                return false;
              }
              m_InputData.mesh_name = tStringValue;

              // find last dot in filename, get mesh filename base from this
              size_t loc = tStringValue.find_last_of('.');
              if(loc == std::string::npos)
              {
                // mesh name: mesh_file
                // without extension: mesh_file
                m_InputData.mesh_name_without_extension = m_InputData.mesh_name;
              }
              else if(tStringValue[loc] == '.')
              {
                // mesh name: some_file.gen
                // without extension: some_file
                m_InputData.mesh_name_without_extension = tStringValue.substr(0,loc);
                m_InputData.mesh_extension = tStringValue.substr(loc);
              }
              else
              {
                // I don't know when this case will ever occur
                m_InputData.mesh_name_without_extension = m_InputData.mesh_name;
              }
            }
            else
            {
              PrintUnrecognizedTokens(tokens);
              std::cout << "ERROR:XMLGenerator:parseMesh: Unrecognized keyword.\n";
              return false;
            }
          }
        }
      }
    }
  }
  return true;
}
/******************************************************************************/
bool XMLGenerator::parseCodePaths(std::istream &fin)
  /******************************************************************************/
{
  std::string tStringValue;
  std::vector<std::string> tInputStringList;

  // read each line of the file
  while (!fin.eof())
  {
    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);
    std::vector<std::string> tokens;
    parseTokens(buf, tokens);

    // process the tokens
    if(tokens.size() > 0)
    {
      for(size_t j=0; j<tokens.size(); ++j)
        tokens[j] = toLower(tokens[j]);

      if(parseSingleValue(tokens, tInputStringList = {"begin","paths"}, tStringValue))
      {
        // found mesh block
        while (!fin.eof())
        {
          fin.getline(buf, MAX_CHARS_PER_LINE);
          tokens.clear();
          parseTokens(buf, tokens);
          // process the tokens
          if(tokens.size() > 0)
          {
            std::vector<std::string> unlowered_tokens = tokens;

            for(size_t j=0; j<tokens.size(); ++j)
              tokens[j] = toLower(tokens[j]);

            if(parseSingleValue(tokens, tInputStringList = {"end","paths"}, tStringValue))
            {
              break;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"code","sierra_sd"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseCodePaths: No value specified after \"code sierra_sd\" keywords.\n";
                return false;
              }
              m_InputData.sierra_sd_path = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"code","platomain"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseCodePaths: No value specified after \"code platomain\" keywords.\n";
                return false;
              }
              m_InputData.plato_main_path = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"code","lightmp"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseCodePaths: No value specified after \"code lightmp\" keywords.\n";
                return false;
              }
              m_InputData.lightmp_path = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"code","prune_and_refine"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseCodePaths: No value specified after \"code prune_and_refine\" keywords.\n";
                return false;
              }
              m_InputData.prune_and_refine_path = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"code","albany"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseCodePaths: No value specified after \"code albany\" keywords.\n";
                return false;
              }
              m_InputData.albany_path = tStringValue;
            }
            else if(parseSingleUnLoweredValue(tokens, unlowered_tokens, tInputStringList = {"code","plato_analyze"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseCodePaths: No value specified after \"code plato_analyze\" keywords.\n";
                return false;
              }
              m_InputData.plato_analyze_path = tStringValue;
            }
            else
            {
              PrintUnrecognizedTokens(tokens);
              std::cout << "ERROR:XMLGenerator:parseCodePaths: Unrecognized keyword.\n";
              return false;
            }
          }
        }
      }
    }
  }
  return true;
}
/******************************************************************************/
bool XMLGenerator::parseBlocks(std::istream &fin)
  /******************************************************************************/
{
  std::string tStringValue;
  std::vector<std::string> tInputStringList;

  // read each line of the file
  while (!fin.eof())
  {
    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);
    std::vector<std::string> tokens;
    parseTokens(buf, tokens);

    // process the tokens
    if(tokens.size() > 0)
    {
      for(size_t j=0; j<tokens.size(); ++j)
        tokens[j] = toLower(tokens[j]);

      if(parseSingleValue(tokens, tInputStringList = {"begin","block"}, tStringValue))
      {
        XMLGen::Block new_block;
        if(tStringValue == "")
        {
          std::cout << "ERROR:XMLGenerator:parseBlocks: No block id specified.\n";
          return false;
        }
        new_block.block_id = tStringValue;
        // found mesh block
        while (!fin.eof())
        {
          fin.getline(buf, MAX_CHARS_PER_LINE);
          tokens.clear();
          parseTokens(buf, tokens);
          // process the tokens
          if(tokens.size() > 0)
          {
            for(size_t j=0; j<tokens.size(); ++j)
              tokens[j] = toLower(tokens[j]);

            if(parseSingleValue(tokens, tInputStringList = {"end","block"}, tStringValue))
            {
              if(new_block.block_id == "")
              {
                std::cout << "ERROR:XMLGenerator:parseBlocks: Block id was not specified for block.\n";
                return false;
              }
              break;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"material"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseBlocks: No value specified after \"material\" keyword.\n";
                return false;
              }
              new_block.material_id = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"element","type"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseBlocks: No value specified after \"element type\" keywords.\n";
                return false;
              }
              new_block.element_type = tStringValue;
            }
            else
            {
              PrintUnrecognizedTokens(tokens);
              std::cout << "ERROR:XMLGenerator:parseBlocks: Unrecognized keyword.\n";
              return false;
            }
          }
        }
        m_InputData.blocks.push_back(new_block);
      }
    }
  }
  return true;
}
/******************************************************************************/
bool XMLGenerator::parseMaterials(std::istream &fin)
  /******************************************************************************/
{
  std::string tStringValue;
  std::vector<std::string> tInputStringList;

  // read each line of the file
  while (!fin.eof())
  {
    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);
    std::vector<std::string> tokens;
    parseTokens(buf, tokens);

    // process the tokens
    if(tokens.size() > 0)
    {
      for(size_t j=0; j<tokens.size(); ++j)
        tokens[j] = toLower(tokens[j]);

      if(parseSingleValue(tokens, tInputStringList = {"begin","material"}, tStringValue))
      {
        XMLGen::Material new_material;
        new_material.penalty_exponent = "3.0";
        if(tStringValue == "")
        {
          std::cout << "ERROR:XMLGenerator:parseMaterials: No material id specified.\n";
          return false;
        }
        new_material.material_id = tStringValue;
        // found mesh block
        while (!fin.eof())
        {
          fin.getline(buf, MAX_CHARS_PER_LINE);
          tokens.clear();
          parseTokens(buf, tokens);
          // process the tokens
          if(tokens.size() > 0)
          {
            for(size_t j=0; j<tokens.size(); ++j)
              tokens[j] = toLower(tokens[j]);

            if(parseSingleValue(tokens, tInputStringList = {"end","material"}, tStringValue))
            {
              if(new_material.material_id == "")
              {
                std::cout << "ERROR:XMLGenerator:parseMaterials: Material ids was not specified for material.\n";
                return false;
              }
              break;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"penalty","exponent"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseMaterials: No value specified after \"penalty exponent\" keywords.\n";
                return false;
              }
              new_material.penalty_exponent = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"youngs","modulus"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseMaterials: No value specified after \"youngs modulus\" keywords.\n";
                return false;
              }
              new_material.youngs_modulus = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"specific","heat"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseMaterials: No value specified after \"specific heat\" keywords.\n";
                return false;
              }
              new_material.specific_heat = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"poissons","ratio"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseMaterials: No value specified after \"poissons ratio\" keywords.\n";
                return false;
              }
              new_material.poissons_ratio = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"thermal","conductivity","coefficient"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseMaterials: No value specified after \"thermal conductivity coefficient\" keywords.\n";
                return false;
              }
              new_material.thermal_conductivity = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"thermal","expansion","coefficient"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseMaterials: No value specified after \"thermal expansion coefficient\" keywords.\n";
                return false;
              }
              new_material.thermal_expansion = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"reference","temperature"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseMaterials: No value specified after \"reference temperature\" keywords.\n";
                return false;
              }
              new_material.reference_temperature = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"density"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseMaterials: No value specified after \"density\" keywords.\n";
                return false;
              }
              new_material.density = tStringValue;
            }
            else
            {
              PrintUnrecognizedTokens(tokens);
              std::cout << "ERROR:XMLGenerator:parseMaterials: Unrecognized keyword.\n";
              return false;
            }
          }
        }
        m_InputData.materials.push_back(new_material);
      }
    }
  }
  return true;
}
/******************************************************************************/
bool XMLGenerator::parseConstraints(std::istream &fin)
  /******************************************************************************/
{
  std::string tStringValue;
  std::vector<std::string> tInputStringList;

  // read each line of the file
  while (!fin.eof())
  {
    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);
    std::vector<std::string> tokens;
    parseTokens(buf, tokens);

    // process the tokens
    if(tokens.size() > 0)
    {
      for(size_t j=0; j<tokens.size(); ++j)
        tokens[j] = toLower(tokens[j]);

      if(parseSingleValue(tokens, tInputStringList = {"begin","constraint"}, tStringValue))
      {
        XMLGen::Constraint new_constraint;
        // found constraint
        while (!fin.eof())
        {
          fin.getline(buf, MAX_CHARS_PER_LINE);
          tokens.clear();
          parseTokens(buf, tokens);
          // process the tokens
          if(tokens.size() > 0)
          {
            for(size_t j=0; j<tokens.size(); ++j)
              tokens[j] = toLower(tokens[j]);

            if(parseSingleValue(tokens, tInputStringList = {"end","constraint"}, tStringValue))
            {
              break;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"type"}, tStringValue))
            {
              if(tokens.size() < 2)
              {
                std::cout << "ERROR:XMLGenerator:parseConstraints: Not enough params after \"type\" keyword.\n";
                return false;
              }
              new_constraint.type = tokens[1];
              for(size_t j=2; j<tokens.size(); ++j)
              {
                new_constraint.type += " ";
                new_constraint.type += tokens[j];
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"name"}, tStringValue))
            {
              if(tokens.size() < 2)
              {
                std::cout << "ERROR:XMLGenerator:parseConstraints: Not enough params after \"name\" keyword.\n";
                return false;
              }
              new_constraint.name = tokens[1];
              for(size_t j=2; j<tokens.size(); ++j)
              {
                new_constraint.name += " ";
                new_constraint.name += tokens[j];
              }
            }
            else if(parseSingleValue(tokens, tInputStringList = {"volume","fraction"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseConstraints: Not value specified for \"volume fraction\".\n";
                return false;
              }
              new_constraint.volume_fraction = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"volume","absolute"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseConstraints: Not value specified for \"volume absolute\".\n";
                return false;
              }
              new_constraint.volume_absolute = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"surface","area","sideset","id"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseConstraints: Not value specified for \"surface area sideset id\".\n";
                return false;
              }
              new_constraint.surface_area_ssid = tStringValue;
            }
            else if(parseSingleValue(tokens, tInputStringList = {"surface","area"}, tStringValue))
            {
              if(tStringValue == "")
              {
                std::cout << "ERROR:XMLGenerator:parseConstraints: Not value specified for \"surface area\".\n";
                return false;
              }
              new_constraint.surface_area = tStringValue;
            }
            else
            {
              std::cout << "ERROR:XMLGenerator:parseConstraints: Invalid keyword.\n";
              return false;
            }
          }
        }
        m_InputData.constraints.push_back(new_constraint);
      }
    }
  }

  return true;
}

bool XMLGenerator::find_tokens(std::vector<std::string> &tokens,
    const int &start_index,
    const char *str1,
    const char *str2)
{
  return true;
}

/******************************************************************************/
bool XMLGenerator::parseFile()
  /******************************************************************************/
{
  std::ifstream fin;
  fin.open(m_InputFilename.c_str()); // open a file
  if (!fin.good())
  {
    std::cout << "Failed to open " << m_InputFilename << "." << std::endl;
    return false; // exit if file not found
  }

  parseBCs(fin);
  fin.close();
  fin.open(m_InputFilename.c_str()); // open a file
  parseLoads(fin);
  fin.close();
  fin.open(m_InputFilename.c_str()); // open a file
  parseObjectives(fin);
  fin.close();
  fin.open(m_InputFilename.c_str()); // open a file
  parseOptimizationParameters(fin);
  fin.close();
  fin.open(m_InputFilename.c_str()); // open a file
  parseConstraints(fin);
  fin.close();
  fin.open(m_InputFilename.c_str()); // open a file
  parseMesh(fin);
  fin.close();
  fin.open(m_InputFilename.c_str()); // open a file
  parseMaterials(fin);
  fin.close();
  fin.open(m_InputFilename.c_str()); // open a file
  parseBlocks(fin);
  fin.close();
  fin.open(m_InputFilename.c_str()); // open a file
  parseCodePaths(fin);
  fin.close();
  fin.open(m_InputFilename.c_str()); // open a file
  parseUncertainties(fin);
  fin.close();

  // If we will need to run the prune_and_refine executable for any
  // reason we need to have our "run" mesh name not be the same
  // as the input mesh name.
  int tNumRefines = 0;
  if(m_InputData.number_refines != "")
    tNumRefines = std::atoi(m_InputData.number_refines.c_str());
  if(tNumRefines > 0 ||
      (m_InputData.initial_guess_filename != "" && m_InputData.initial_guess_field_name != ""))
  {
    m_InputData.run_mesh_name_without_extension = m_InputData.mesh_name_without_extension + "_mod";
    m_InputData.run_mesh_name = m_InputData.run_mesh_name_without_extension;
    if(m_InputData.mesh_extension != "")
      m_InputData.run_mesh_name += m_InputData.mesh_extension;
  }
  else
  {
    m_InputData.run_mesh_name = m_InputData.mesh_name;
    m_InputData.run_mesh_name_without_extension = m_InputData.mesh_name_without_extension;
  }

  return true;
}

/******************************************************************************/
std::string XMLGenerator::toLower(const std::string &s)
  /******************************************************************************/
{
  char buffer[500];
  std::string ret;
  if(s.size() > 500)
    ret = "";
  else
  {
    size_t i;
    for(i=0; i<s.size(); ++i)
    {
      buffer[i] = tolower(s[i]);
    }
    buffer[i] = '\0';
    ret = buffer;
  }
  return ret;
}

/******************************************************************************/
std::string XMLGenerator::toUpper(const std::string &s)
  /******************************************************************************/
{
  char buffer[500];
  std::string ret;
  if(s.size() > 500)
    ret = "";
  else
  {
    size_t i;
    for(i=0; i<s.size(); ++i)
    {
      buffer[i] = toupper(s[i]);
    }
    buffer[i] = '\0';
    ret = buffer;
  }
  return ret;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoMainInputDeckXML()
  /******************************************************************************/
{
  pugi::xml_document doc;
  pugi::xml_node tmp_node1, tmp_node2, tmp_node3;

  // Version entry
  pugi::xml_node tmp_node = doc.append_child(pugi::node_declaration);
  tmp_node.set_name("xml");
  pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
  tmp_att.set_value("1.0");

  // mesh
  pugi::xml_node mesh_node = doc.append_child("mesh");
  addChild(mesh_node, "type", "unstructured");
  addChild(mesh_node, "format", "exodus");

  // See if this is an Alexa (plato analyze) run and
  // add the ignore_node_map entry if it is.
  bool tAlexaRun = false;
  for(size_t i=0; i<m_InputData.objectives.size(); ++i)
  {
    XMLGen::Objective cur_obj = m_InputData.objectives[i];
    if(cur_obj.code_name == "plato_analyze")
    {
      tAlexaRun = true;
      break;
    }
  }
  if(tAlexaRun)
    addChild(mesh_node, "ignore_node_map", "true");

  addChild(mesh_node, "mesh", m_InputData.run_mesh_name.c_str());
  // just need one block specified here
  if(m_InputData.blocks.size() > 0)
  {
    tmp_node = mesh_node.append_child("block");
    addChild(tmp_node, "index", m_InputData.blocks[0].block_id.c_str());
    tmp_node1 = tmp_node.append_child("integration");
    addChild(tmp_node1, "type", "gauss");
    addChild(tmp_node1, "order", "2");
    addChild(tmp_node, "material", m_InputData.blocks[0].material_id.c_str());
  }

  // output
  tmp_node = doc.append_child("output");
  addChild(tmp_node, "file", "platomain");
  addChild(tmp_node, "format", "exodus");

  // Write the file to disk
  doc.save_file("plato_main_input_deck.xml", "  ");

  return true;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoESPInputDeckXML()
/******************************************************************************/
{
    pugi::xml_document doc;

    // Version entry
    pugi::xml_node tmp_node1 = doc.append_child(pugi::node_declaration);
    tmp_node1.set_name("xml");
    pugi::xml_attribute tmp_att = tmp_node1.append_attribute("version");
    tmp_att.set_value("1.0");

    // ESP
    tmp_node1 = doc.append_child("ESP");
    addChild(tmp_node1, "ModelFileName", m_InputData.csm_filename);
    addChild(tmp_node1, "TessFileName", m_InputData.csm_tesselation_filename);
    addChild(tmp_node1, "ParameterIndex", "0");

    // Write the file to disk
    doc.save_file("plato_esp_input_deck.xml", "  ");

    return true;
}

/******************************************************************************/
bool XMLGenerator::generatePerformerOperationsXML()
/******************************************************************************/
{
    generateSalinasOperationsXML();
    generateAlbanyOperationsXML();
    generateLightMPOperationsXML();
    if(m_InputData.optimization_type == "shape" && m_InputData.mPlatoAnalyzePerformerExists)
    {
        generatePlatoAnalyzeOperationsXMLForShape();
        generatePlatoESPOperationsXMLForShape();
    }
    else
    {
        generatePlatoAnalyzeOperationsXML();
    }
    generateAMGXInput();
    generateROLInput();
    return true;
}

/******************************************************************************/
void XMLGenerator::generateROLInput()
  /******************************************************************************/
{
  if(m_InputData.optimization_algorithm == "rol ksal" ||
      m_InputData.optimization_algorithm == "rol ksbc")
  {
    pugi::xml_document doc;
    pugi::xml_node n1, n2, n3;
    pugi::xml_attribute a1;

    // Version entry
    n1 = doc.append_child(pugi::node_declaration);
    n1.set_name("xml");
    a1 = n1.append_attribute("version");
    a1.set_value("1.0");

    n1 = doc.append_child("ParameterList");
    n1.append_attribute("name") = "Inputs";

    n2 = n1.append_child("ParameterList");
    n2.append_attribute("name") = "Problem";

    n2 = n1.append_child("ParameterList");
    n2.append_attribute("name") = "General";
    n3 = n2.append_child("ParameterList");
    n3.append_attribute("name") = "Secant";
    addNTVParameter(n3, "Type", "string", "Limited-Memory BFGS");
    addNTVParameter(n3, "Use as Preconditioner", "bool", "false");
    addNTVParameter(n3, "Use as Hessian", "bool", "true");
    addNTVParameter(n3, "Maximum Storage", "int", "20");
    addNTVParameter(n3, "Barzilai-Borwein", "int", "1");

    n2 = n1.append_child("ParameterList");
    n2.append_attribute("name") = "Step";

    // Trust region
    n3 = n2.append_child("ParameterList");
    n3.append_attribute("name") = "Trust Region";
    addNTVParameter(n3, "Subproblem Solver", "string", "Truncated CG");
    addNTVParameter(n3, "Subproblem Model", "string", "Kelley-Sachs");
    addNTVParameter(n3, "Initial Radius", "double", "-1.0");
    addNTVParameter(n3, "Maximum Radius", "double", "1.0e8");
    addNTVParameter(n3, "Step Acceptance Threshold", "double", "0.05");
    addNTVParameter(n3, "Radius Shrinking Threshold", "double", "0.05");
    addNTVParameter(n3, "Radius Growing Threshold", "double", "0.9");
    addNTVParameter(n3, "Radius Shrinking Rate (Negative rho)", "double", "0.0625");
    addNTVParameter(n3, "Radius Shrinking Rate (Positive rho)", "double", "0.25");
    addNTVParameter(n3, "Radius Growing Rate", "double", "2.5");
    addNTVParameter(n3, "Safeguard Size", "double", "100.0");

    // Augmented Lagrangian
    n3 = n2.append_child("ParameterList");
    n3.append_attribute("name") = "Augmented Lagrangian";
    addNTVParameter(n3, "Use Scaled Augmented Lagrangian", "bool", "false");
    addNTVParameter(n3, "Level of Hessian Approximation", "int", "0");
    addNTVParameter(n3, "Use Default Problem Scaling", "bool", "true");
    addNTVParameter(n3, "Objective Scaling", "double", "1.0");
    addNTVParameter(n3, "Constraint Scaling", "double", "1.0");
    addNTVParameter(n3, "Use Default Initial Penalty Parameter", "bool", "true");
    addNTVParameter(n3, "Initial Penalty Parameter", "double", "1.0e1");
    addNTVParameter(n3, "Penalty Parameter Growth Factor", "double", "1.0e1");
    addNTVParameter(n3, "Maximum Penalty Parameter", "double", "1.0e8");
    addNTVParameter(n3, "Initial Optimality Tolerance", "double", "1.0");
    addNTVParameter(n3, "Optimality Tolerance Update Exponent", "double", "1.0");
    addNTVParameter(n3, "Optimality Tolerance Decrease Exponent", "double", "1.0");
    addNTVParameter(n3, "Initial Feasibility Tolerance", "double", "1.0");
    addNTVParameter(n3, "Feasibility Tolerance Update Exponent", "double", "0.1");
    addNTVParameter(n3, "Feasibility Tolerance Decrease Exponent", "double", "0.9");
    addNTVParameter(n3, "Print Intermediate Optimization History", "bool", "false");
    addNTVParameter(n3, "Subproblem Step Type", "string", "Trust Region");
    addNTVParameter(n3, "Subproblem Iteration Limit", "int", "20");

    n2 = n1.append_child("ParameterList");
    n2.append_attribute("name") = "Status Test";
    addNTVParameter(n2, "Iteration Limit", "int", m_InputData.max_iterations);

    // Write the file to disk
    doc.save_file("rol_inputs.xml", "  ");
  }
}

/******************************************************************************/
void XMLGenerator::generateAMGXInput()
  /******************************************************************************/
{
  int num_plato_analyze_objs = 0;
  for(size_t i=0; i<m_InputData.objectives.size(); ++i)
  {
    if(!m_InputData.objectives[i].code_name.compare("plato_analyze"))
    {
      num_plato_analyze_objs++;
    }
  }

  if(num_plato_analyze_objs)
  {
    FILE *fp = fopen("amgx.json", "w");
    if(fp)
    {
      fprintf(fp, "{\n");
      fprintf(fp, "\"config_version\": 2,\n");
      fprintf(fp, "\"solver\": {\n");
      fprintf(fp, "\"preconditioner\": {\n");
      fprintf(fp, "\"print_grid_stats\": 1,\n");
      fprintf(fp, "\"algorithm\": \"AGGREGATION\",\n");
      fprintf(fp, "\"print_vis_data\": 0,\n");
      fprintf(fp, "\"max_matching_iterations\": 50,\n");
      fprintf(fp, "\"max_unassigned_percentage\": 0.01,\n");
      fprintf(fp, "\"solver\": \"AMG\",\n");
      fprintf(fp, "\"smoother\": {\n");
      fprintf(fp, "\"relaxation_factor\": 0.78,\n");
      fprintf(fp, "\"scope\": \"jacobi\",\n");
      fprintf(fp, "\"solver\": \"BLOCK_JACOBI\",\n");
      fprintf(fp, "\"monitor_residual\": 0,\n");
      fprintf(fp, "\"print_solve_stats\": 0\n");
      fprintf(fp, "},\n");
      fprintf(fp, "\"print_solve_stats\": 0,\n");
      fprintf(fp, "\"dense_lu_num_rows\": 64,\n");
      fprintf(fp, "\"presweeps\": 1,\n");
      fprintf(fp, "\"selector\": \"SIZE_8\",\n");
      fprintf(fp, "\"coarse_solver\": \"DENSE_LU_SOLVER\",\n");
      fprintf(fp, "\"coarsest_sweeps\": 2,\n");
      fprintf(fp, "\"max_iters\": 1,\n");
      fprintf(fp, "\"monitor_residual\": 0,\n");
      fprintf(fp, "\"store_res_history\": 0,\n");
      fprintf(fp, "\"scope\": \"amg\",\n");
      fprintf(fp, "\"max_levels\": 100,\n");
      fprintf(fp, "\"postsweeps\": 1,\n");
      fprintf(fp, "\"cycle\": \"W\"\n");
      fprintf(fp, "},\n");
      fprintf(fp, "\"solver\": \"PBICGSTAB\",\n");
      fprintf(fp, "\"print_solve_stats\": 0,\n");
      fprintf(fp, "\"obtain_timings\": 0,\n");
      fprintf(fp, "\"max_iters\": 1000,\n");
      fprintf(fp, "\"monitor_residual\": 1,\n");
      fprintf(fp, "\"convergence\": \"ABSOLUTE\",\n");
      fprintf(fp, "\"scope\": \"main\",\n");
      fprintf(fp, "\"tolerance\": 1e-12,\n");
      fprintf(fp, "\"norm\": \"L2\"\n");
      fprintf(fp, "}\n");
      fprintf(fp, "}\n");
      fclose(fp);
    }
  }
}

/******************************************************************************/
bool XMLGenerator::generateLightMPOperationsXML()
  /******************************************************************************/
{
  int num_lightmp_objs = 0;
  for(size_t i=0; i<m_InputData.objectives.size(); ++i)
  {
    if(!m_InputData.objectives[i].code_name.compare("lightmp"))
    {
      num_lightmp_objs++;

      pugi::xml_document doc;
      pugi::xml_node tmp_node, tmp_node1;

      // Version entry
      tmp_node = doc.append_child(pugi::node_declaration);
      tmp_node.set_name("xml");
      pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
      tmp_att.set_value("1.0");

      /* light mp doesn't currently support this
      // Cache State
      tmp_node = doc.append_child("Operation");
      addChild(tmp_node, "Function", "Cache State");
      addChild(tmp_node, "Name", "Cache State");
      */

      // InternalEnergy
      tmp_node = doc.append_child("Operation");
      addChild(tmp_node, "Function", "InternalEnergy");
      addChild(tmp_node, "Name", "Compute Internal Energy");
      tmp_node1 = tmp_node.append_child("Input");
      addChild(tmp_node1, "ArgumentName", "Topology");
      tmp_node1 = tmp_node.append_child("Output");
      addChild(tmp_node1, "ArgumentName", "Internal Energy");
      tmp_node1 = tmp_node.append_child("Output");
      addChild(tmp_node1, "ArgumentName", "Internal Energy Gradient");
      addChild(tmp_node, "PenaltyModel", "SIMP");
      tmp_node1 = tmp_node.append_child("SIMP");
      if(m_InputData.materials.size() > 0)
      {
        addChild(tmp_node1, "PenaltyExponent", m_InputData.materials[0].penalty_exponent.c_str());
      }
      addChild(tmp_node1, "MinimumValue", "0.001");

      if(m_InputData.discretization == "levelset")
      {
        // Compute Surface Area
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "Compute Surface Area");
        addChild(tmp_node, "Name", "Compute Surface Area");
        tmp_node1 = tmp_node.append_child("Input");
        addChild(tmp_node1, "ArgumentName", "Topology");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Surface Area");

        // Compute Surface Area Gradient
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "Compute Surface Area Gradient");
        addChild(tmp_node, "Name", "Compute Surface Area Gradient");
        tmp_node1 = tmp_node.append_child("Input");
        addChild(tmp_node1, "ArgumentName", "Topology");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Surface Area Gradient");
      }

      char buf[200];
      sprintf(buf, "lightmp_operations_%s.xml", m_InputData.objectives[i].name.c_str());
      // Write the file to disk
      doc.save_file(buf, "  ");
    }
  }

  return true;
}

/******************************************************************************/
bool XMLGenerator::generateAlbanyOperationsXML()
  /******************************************************************************/
{
  int num_albany_objs = 0;
  for(size_t i=0; i<m_InputData.objectives.size(); ++i)
  {
    if(!m_InputData.objectives[i].code_name.compare("albany"))
    {
      num_albany_objs++;

      pugi::xml_document doc;
      pugi::xml_node tmp_node, tmp_node1;

      // Version entry
      tmp_node = doc.append_child(pugi::node_declaration);
      tmp_node.set_name("xml");
      pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
      tmp_att.set_value("1.0");

      // Cache State
      /* For now Albany isn't handling this correctly.
         tmp_node = doc.append_child("Operation");
         addChild(tmp_node, "Function", "Cache State");
         addChild(tmp_node, "Name", "Cache State");
         */

      // InternalEnergy
      tmp_node = doc.append_child("Operation");
      addChild(tmp_node, "Function", "AlbanyResponse");
      addChild(tmp_node, "Name", "Compute Internal Energy");
      tmp_node1 = tmp_node.append_child("Input");
      addChild(tmp_node1, "LocalName", "Rho");
      addChild(tmp_node1, "Type", "Field");
      addChild(tmp_node1, "ArgumentName", "Topology");
      tmp_node1 = tmp_node.append_child("Output");
      addChild(tmp_node1, "LocalName", "R0");
      addChild(tmp_node1, "Type", "Value");
      addChild(tmp_node1, "ArgumentName", "Internal Energy");
      tmp_node1 = tmp_node.append_child("Output");
      addChild(tmp_node1, "LocalName", "dR0dRho 0");
      addChild(tmp_node1, "Type", "Field");
      addChild(tmp_node1, "ArgumentName", "Internal Energy Gradient");

      if(m_InputData.discretization == "levelset")
      {
        // Compute Surface Area
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "Compute Surface Area");
        addChild(tmp_node, "Name", "Compute Surface Area");
        tmp_node1 = tmp_node.append_child("Input");
        addChild(tmp_node1, "ArgumentName", "Topology");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Surface Area");

        // Compute Surface Area Gradient
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "Compute Surface Area Gradient");
        addChild(tmp_node, "Name", "Compute Surface Area Gradient");
        tmp_node1 = tmp_node.append_child("Input");
        addChild(tmp_node1, "ArgumentName", "Topology");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Surface Area Gradient");
      }

      char buf[200];
      sprintf(buf, "albany_operations_%s.xml", m_InputData.objectives[i].name.c_str());
      // Write the file to disk
      doc.save_file(buf, "  ");
    }
  }

  return true;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoAnalyzeOperationsXML()
  /******************************************************************************/
{
  int num_plato_analyze_objs = 0;
  if(m_InputData.optimization_type == "topology")
  {
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_document doc;
      pugi::xml_node tmp_node, tmp_node1;

      // Version entry
      tmp_node = doc.append_child(pugi::node_declaration);
      tmp_node.set_name("xml");
      pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
      tmp_att.set_value("1.0");

      // ComputeSolution
      tmp_node = doc.append_child("Operation");
      addChild(tmp_node, "Function", "ComputeSolution");
      addChild(tmp_node, "Name", "Compute Displacement Solution");
      tmp_node1 = tmp_node.append_child("Input");
      addChild(tmp_node1, "ArgumentName", "Topology");

      // ComputeObjectiveValue
      tmp_node = doc.append_child("Operation");
      addChild(tmp_node, "Function", "ComputeObjectiveValue");
      addChild(tmp_node, "Name", "Compute Objective Value");
      tmp_node1 = tmp_node.append_child("Input");
      addChild(tmp_node1, "ArgumentName", "Topology");
      tmp_node1 = tmp_node.append_child("Output");
      addChild(tmp_node1, "ArgumentName", "Objective Value");

      for(size_t tRandomLoadIndex = 0; tRandomLoadIndex < m_UncertaintyMetaData.randomVariableIndices.size(); ++tRandomLoadIndex)
      {
        tmp_node1 = tmp_node.append_child("Parameter");
        std::string tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "X";
        addChild(tmp_node1, "ArgumentName", tLoadName);
        std::string tPathToLoadInInputFile = "[Plato Problem]:[Natural Boundary Conditions]:[Random Traction Vector Boundary Condition " + Plato::to_string(tRandomLoadIndex) + "]:Values(0)";
        addChild(tmp_node1, "Target", tPathToLoadInInputFile);
        addChild(tmp_node1, "InitialValue", "0.0");
        tmp_node1 = tmp_node.append_child("Parameter");
        tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "Y";
        addChild(tmp_node1, "ArgumentName", tLoadName);
        tPathToLoadInInputFile = "[Plato Problem]:[Natural Boundary Conditions]:[Random Traction Vector Boundary Condition " + Plato::to_string(tRandomLoadIndex) + "]:Values(1)";
        addChild(tmp_node1, "Target", tPathToLoadInInputFile);
        addChild(tmp_node1, "InitialValue", "0.0");
        tmp_node1 = tmp_node.append_child("Parameter");
        tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "Z";
        addChild(tmp_node1, "ArgumentName", tLoadName);
        tPathToLoadInInputFile = "[Plato Problem]:[Natural Boundary Conditions]:[Random Traction Vector Boundary Condition " + Plato::to_string(tRandomLoadIndex) + "]:Values(2)";
        addChild(tmp_node1, "Target", tPathToLoadInInputFile);
        addChild(tmp_node1, "InitialValue", "0.0");
      }

      // ComputeObjectiveGradient
      tmp_node = doc.append_child("Operation");
      addChild(tmp_node, "Function", "ComputeObjective");
      addChild(tmp_node, "Name", "Compute Objective Gradient");
      tmp_node1 = tmp_node.append_child("Input");
      addChild(tmp_node1, "ArgumentName", "Topology");
      tmp_node1 = tmp_node.append_child("Output");
      addChild(tmp_node1, "ArgumentName", "Objective Gradient");

      for(size_t tRandomLoadIndex = 0; tRandomLoadIndex < m_UncertaintyMetaData.randomVariableIndices.size(); ++tRandomLoadIndex)
      {
        tmp_node1 = tmp_node.append_child("Parameter");
        std::string tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "X";
        addChild(tmp_node1, "ArgumentName", tLoadName);
        std::string tPathToLoadInInputFile = "[Plato Problem]:[Natural Boundary Conditions]:[Random Traction Vector Boundary Condition " + Plato::to_string(tRandomLoadIndex) + "]:Values(0)";
        addChild(tmp_node1, "Target", tPathToLoadInInputFile);
        addChild(tmp_node1, "InitialValue", "0.0");
        tmp_node1 = tmp_node.append_child("Parameter");
        tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "Y";
        addChild(tmp_node1, "ArgumentName", tLoadName);
        tPathToLoadInInputFile = "[Plato Problem]:[Natural Boundary Conditions]:[Random Traction Vector Boundary Condition " + Plato::to_string(tRandomLoadIndex) + "]:Values(1)";
        addChild(tmp_node1, "Target", tPathToLoadInInputFile);
        addChild(tmp_node1, "InitialValue", "0.0");
        tmp_node1 = tmp_node.append_child("Parameter");
        tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "Z";
        addChild(tmp_node1, "ArgumentName", tLoadName);
        tPathToLoadInInputFile = "[Plato Problem]:[Natural Boundary Conditions]:[Random Traction Vector Boundary Condition " + Plato::to_string(tRandomLoadIndex) + "]:Values(2)";
        addChild(tmp_node1, "Target", tPathToLoadInInputFile);
        addChild(tmp_node1, "InitialValue", "0.0");
      }


      // WriteOutput
      tmp_node = doc.append_child("Operation");
      addChild(tmp_node, "Function", "WriteOutput");
      addChild(tmp_node, "Name", "Write Output");

      char buf[200];
      sprintf(buf, "plato_analyze_operations.xml");
      // Write the file to disk
      doc.save_file(buf, "  ");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        if(m_InputData.objectives[i].code_name == "plato_analyze")
        {
          Objective tCurObjective = m_InputData.objectives[i];
          num_plato_analyze_objs++;

          pugi::xml_document doc;
          pugi::xml_node tmp_node, tmp_node1;

          // Version entry
          tmp_node = doc.append_child(pugi::node_declaration);
          tmp_node.set_name("xml");
          pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
          tmp_att.set_value("1.0");

          // ComputeSolution
          tmp_node = doc.append_child("Operation");
          addChild(tmp_node, "Function", "ComputeSolution");
          addChild(tmp_node, "Name", "Compute Displacement Solution");
          tmp_node1 = tmp_node.append_child("Input");
          addChild(tmp_node1, "ArgumentName", "Topology");

          // ComputeObjectiveValue
          tmp_node = doc.append_child("Operation");
          addChild(tmp_node, "Function", "ComputeObjectiveValue");
          addChild(tmp_node, "Name", "Compute Objective Value");
          tmp_node1 = tmp_node.append_child("Input");
          addChild(tmp_node1, "ArgumentName", "Topology");
          tmp_node1 = tmp_node.append_child("Output");
          addChild(tmp_node1, "ArgumentName", "Objective Value");

          // ComputeObjectiveGradient
          tmp_node = doc.append_child("Operation");
          addChild(tmp_node, "Function", "ComputeObjectiveGradient");
          addChild(tmp_node, "Name", "Compute Objective Gradient");
          tmp_node1 = tmp_node.append_child("Input");
          addChild(tmp_node1, "ArgumentName", "Topology");
          tmp_node1 = tmp_node.append_child("Output");
          addChild(tmp_node1, "ArgumentName", "Objective Gradient");

          // WriteOutput
          tmp_node = doc.append_child("Operation");
          addChild(tmp_node, "Function", "WriteOutput");
          addChild(tmp_node, "Name", "Write Output");
          for(size_t j=0; j<tCurObjective.output_for_plotting.size(); ++j)
          {
            tmp_node1 = tmp_node.append_child("Output");
            if(tCurObjective.output_for_plotting[j] == "dispx")
              addChild(tmp_node1, "ArgumentName", "Solution X");
            else if(tCurObjective.output_for_plotting[j] == "dispy")
              addChild(tmp_node1, "ArgumentName", "Solution Y");
            else if(tCurObjective.output_for_plotting[j] == "dispz")
              addChild(tmp_node1, "ArgumentName", "Solution Z");
            else if(tCurObjective.output_for_plotting[j] == "temperature")
              addChild(tmp_node1, "ArgumentName", "Solution");
          }

          char buf[200];
          sprintf(buf, "plato_analyze_operations_%s.xml", m_InputData.objectives[i].name.c_str());
          // Write the file to disk
          doc.save_file(buf, "  ");
        }
      }
    }
  }
  else if(m_InputData.optimization_type == "shape")
  {
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
      if(m_InputData.objectives[i].code_name == "plato_analyze")
      {
        Objective tCurObjective = m_InputData.objectives[i];
        num_plato_analyze_objs++;

        pugi::xml_document doc;
        pugi::xml_node tmp_node, tmp_node1;

        // Version entry
        tmp_node = doc.append_child(pugi::node_declaration);
        tmp_node.set_name("xml");
        pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
        tmp_att.set_value("1.0");

        // ComputeSolution
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "ComputeSolution");
        addChild(tmp_node, "Name", "Compute Displacement Solution");

        // ComputeObjectiveValue
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "ComputeObjectiveValue");
        addChild(tmp_node, "Name", "Compute Objective Value");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Objective Value");

        // ComputeObjectiveGradient
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "ComputeObjectiveP");
        addChild(tmp_node, "Name", "Compute Objective Gradient");
        addChild(tmp_node, "ESPName", "Design Geometry");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Objective Gradient");

        // ComputeObjective
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "ComputeObjectiveP");
        addChild(tmp_node, "Name", "Compute Objective");
        addChild(tmp_node, "ESPName", "Design Geometry");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Objective Value");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Objective Gradient");

        // ComputeConstraintValue
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "ComputeConstraintValue");
        addChild(tmp_node, "Name", "Compute Constraint Value");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Constraint Value");

        // ComputeConstraintGradient
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "ComputeConstraintGradientP");
        addChild(tmp_node, "Name", "Compute Constraint Gradient");
        addChild(tmp_node, "ESPName", "Design Geometry");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Constraint Gradient");

        // ComputeConstraint
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "ComputeConstraintP");
        addChild(tmp_node, "Name", "Compute Constraint");
        addChild(tmp_node, "ESPName", "Design Geometry");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Constraint Value");
        tmp_node1 = tmp_node.append_child("Output");
        addChild(tmp_node1, "ArgumentName", "Constraint Gradient");

        // Reinitialize on Change Operation
        tmp_node = doc.append_child("Operation");
        addChild(tmp_node, "Function", "ReinitializeESP");
        addChild(tmp_node, "Name", "Reinitialize on Change");
        addChild(tmp_node, "OnChange", "true");
        tmp_node1 = tmp_node.append_child("Input");
        addChild(tmp_node1, "ArgumentName", "Parameters");
        addChild(tmp_node, "ESPName", "Design Geometry");
        tmp_node1 = tmp_node.append_child("Parameter");
        addChild(tmp_node1, "ArgumentName", "Input Mesh");
        addChild(tmp_node1, "Target", "Input Mesh");
        addChild(tmp_node1, "InitialValue", m_InputData.csm_exodus_filename);

        tmp_node = doc.append_child("ESP");
        addChild(tmp_node, "Name", "Design Geometry");
        addChild(tmp_node, "ModelFileName", m_InputData.csm_filename);
        addChild(tmp_node, "TessFileName", m_InputData.csm_tesselation_filename);

        char buf[200];
        sprintf(buf, "plato_analyze_operations_%s.xml", m_InputData.objectives[i].name.c_str());
        // Write the file to disk
        doc.save_file(buf, "  ");
      }
    }
  }

    return true;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoAnalyzeOperationsXMLForShape()
/******************************************************************************/
{
    int num_plato_analyze_objs = 0;
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        if(m_InputData.objectives[i].code_name == "plato_analyze")
        {
            Objective tCurObjective = m_InputData.objectives[i];
            num_plato_analyze_objs++;

            pugi::xml_document doc;
            pugi::xml_node tmp_node, tmp_node1;
            pugi::xml_node tForNode;

            // Version entry
            tmp_node = doc.append_child(pugi::node_declaration);
            tmp_node.set_name("xml");
            pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
            tmp_att.set_value("1.0");

            // include defines.xml
            pugi::xml_node def_node = doc.append_child("include");
            def_node.append_attribute("filename") = "defines.xml";

            // Compute Constraint Sensitivity
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Name", "Compute Constraint Sensitivity");
            addChild(tmp_node, "Function", "MapConstraintGradientX");
            tForNode = tmp_node.append_child("For");
            tForNode.append_attribute("var") = "I";
            tForNode.append_attribute("in") = "Parameters";
            tmp_node1 = tForNode.append_child("Input");
            addChild(tmp_node1, "ArgumentName", "Parameter Sensitivity {I}");
            tmp_node1 = tmp_node.append_child("Output");
            addChild(tmp_node1, "ArgumentName", "Constraint Sensitivity");

            // Compute Objective Sensitivity
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Name", "Compute Objective Sensitivity");
            addChild(tmp_node, "Function", "MapObjectiveGradientX");
            tForNode = tmp_node.append_child("For");
            tForNode.append_attribute("var") = "I";
            tForNode.append_attribute("in") = "Parameters";
            tmp_node1 = tForNode.append_child("Input");
            addChild(tmp_node1, "ArgumentName", "Parameter Sensitivity {I}");
            tmp_node1 = tmp_node.append_child("Output");
            addChild(tmp_node1, "ArgumentName", "Objective Sensitivity");

            // ComputeObjectiveValue
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Name", "Compute Objective Value");
            addChild(tmp_node, "Function", "ComputeObjectiveValue");
            tmp_node1 = tmp_node.append_child("Output");
            addChild(tmp_node1, "ArgumentName", "Objective Value");

            // ComputeObjectiveGradient
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Name", "Compute Objective Gradient");
            addChild(tmp_node, "Function", "ComputeObjectiveX");

            // ComputeConstraintValue
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Name", "Compute Constraint Value");
            addChild(tmp_node, "Function", "ComputeConstraintValue");
            tmp_node1 = tmp_node.append_child("Output");
            addChild(tmp_node1, "ArgumentName", "Constraint Value");

            // ComputeConstraintGradient
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Name", "Compute Constraint Gradient");
            addChild(tmp_node, "Function", "ComputeConstraintX");

            // Reinitialize on Change Operation
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Name", "Reinitialize on Change");
            addChild(tmp_node, "Function", "Reinitialize");
            addChild(tmp_node, "OnChange", "true");
            tmp_node1 = tmp_node.append_child("Input");
            addChild(tmp_node1, "ArgumentName", "Parameters");
            addChild(tmp_node1, "SharedDataName", "Design Parameters");

            char buf[200];
            sprintf(buf, "plato_analyze_operations_%s.xml", m_InputData.objectives[i].name.c_str());
            // Write the file to disk
            doc.save_file(buf, "  ");
        }
    }

    return true;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoESPOperationsXMLForShape()
/******************************************************************************/
{
    pugi::xml_document doc;
    pugi::xml_node tmp_node, tmp_node1;

    // Version entry
    tmp_node = doc.append_child(pugi::node_declaration);
    tmp_node.set_name("xml");
    pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
    tmp_att.set_value("1.0");

    // Timers
    tmp_node = doc.append_child("Timers");
    addChild(tmp_node, "time", "true");

    // Compute Constraint Sensitivity
    tmp_node = doc.append_child("Operation");
    addChild(tmp_node, "Name", "Compute Parameter Sensitivity on Change");
    addChild(tmp_node, "Function", "ComputeParameterSensitivity");
    addChild(tmp_node, "OnChange", "true");
    tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Parameters");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Parameter Sensitivity");
    tmp_node1 = tmp_node.append_child("Parameter");
    addChild(tmp_node1, "ArgumentName", "Parameter Index");
    addChild(tmp_node1, "InitialValue", "0");

    // Write the file to disk
    doc.save_file("plato_esp_operations.xml", "  ");

    return true;
}

/******************************************************************************/
bool XMLGenerator::generateSalinasOperationsXML()
/******************************************************************************/
{
    int num_sierra_sd_objs = 0;
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        if(!m_InputData.objectives[i].code_name.compare("sierra_sd"))
        {
            XMLGen::Objective cur_obj = m_InputData.objectives[i];
            num_sierra_sd_objs++;

            pugi::xml_document doc;
            pugi::xml_node tmp_node1;

            // Version entry
            pugi::xml_node tmp_node = doc.append_child(pugi::node_declaration);
            tmp_node.set_name("xml");
            pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
            tmp_att.set_value("1.0");

            // Update Problem
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Function", "Update Problem");
            addChild(tmp_node, "Name", "Update Problem");

            // Cache State
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Function", "Cache State");
            addChild(tmp_node, "Name", "Cache State");
            if(cur_obj.multi_load_case == "true")
            {
                for(size_t k=0; k<cur_obj.load_case_ids.size(); k++)
                {
                    char buffer[100];
                    sprintf(buffer, "%lu", k);
                    for(size_t j=0; j<cur_obj.output_for_plotting.size(); j++)
                    {
                        tmp_node1 = tmp_node.append_child("Output");
                        addChild(tmp_node1, "ArgumentName", cur_obj.output_for_plotting[j] + buffer);
                    }
                }
            }
            else
            {
                for(size_t j=0; j<cur_obj.output_for_plotting.size(); j++)
                {
                    tmp_node1 = tmp_node.append_child("Output");
                    addChild(tmp_node1, "ArgumentName", cur_obj.output_for_plotting[j] + "0");
                }
            }

            // Displacement
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Function", "Displacement");
            addChild(tmp_node, "Name", "Compute Displacement");
            tmp_node1 = tmp_node.append_child("Topology");
            addChild(tmp_node1, "Name", "Topology");
            addChild(tmp_node, "PenaltyModel", "SIMP");
            tmp_node1 = tmp_node.append_child("SIMP");
            if(m_InputData.materials.size() > 0)
            {
                addChild(tmp_node1, "PenaltyExponent", m_InputData.materials[0].penalty_exponent.c_str());
            }
            addChild(tmp_node1, "MinimumValue", "0.001");

            // InternalEnergy
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Function", "InternalEnergy");
            addChild(tmp_node, "Name", "Compute Objective");
            tmp_node1 = tmp_node.append_child("Topology");
            addChild(tmp_node1, "Name", "Topology");
            addChild(tmp_node, "PenaltyModel", "SIMP");
            tmp_node1 = tmp_node.append_child("SIMP");
            if(m_InputData.materials.size() > 0)
            {
                addChild(tmp_node1, "PenaltyExponent", m_InputData.materials[0].penalty_exponent.c_str());
            }
            addChild(tmp_node1, "MinimumValue", "0.001");
            tmp_node1 = tmp_node.append_child("OutputValue");
            addChild(tmp_node1, "Name", "Internal Energy");

            // InternalEnergyGradient
            tmp_node = doc.append_child("Operation");
            addChild(tmp_node, "Function", "InternalEnergyGradient");
            addChild(tmp_node, "Name", "Compute Gradient");
            tmp_node1 = tmp_node.append_child("Topology");
            addChild(tmp_node1, "Name", "Topology");
            addChild(tmp_node, "PenaltyModel", "SIMP");
            tmp_node1 = tmp_node.append_child("SIMP");
            if(m_InputData.materials.size() > 0)
            {
                addChild(tmp_node1, "PenaltyExponent", m_InputData.materials[0].penalty_exponent.c_str());
            }
            addChild(tmp_node1, "MinimumValue", "0.001");
            tmp_node1 = tmp_node.append_child("OutputGradient");
            addChild(tmp_node1, "Name", "Internal Energy Gradient");

            if(m_InputData.optimization_algorithm =="ksbc" ||
               m_InputData.optimization_algorithm == "ksal" ||
               m_InputData.optimization_algorithm == "rol ksal" ||
               m_InputData.optimization_algorithm == "rol ksbc")
            {
                // InternalEnergyHessian
                tmp_node = doc.append_child("Operation");
                addChild(tmp_node, "Function", "InternalEnergyHessian");
                addChild(tmp_node, "Name", "Compute HessianTimesVector");
                tmp_node1 = tmp_node.append_child("Topology");
                addChild(tmp_node1, "Name", "Topology");
                tmp_node1 = tmp_node.append_child("DescentDirection");
                addChild(tmp_node1, "Name", "Descent Direction");
                addChild(tmp_node, "PenaltyModel", "SIMP");
                tmp_node1 = tmp_node.append_child("SIMP");
                if(m_InputData.materials.size() > 0)
                {
                    addChild(tmp_node1, "PenaltyExponent", m_InputData.materials[0].penalty_exponent.c_str());
                }
                addChild(tmp_node1, "MinimumValue", "0.001");
                tmp_node1 = tmp_node.append_child("OutputHessian");
                addChild(tmp_node1, "Name", "HessianTimesVector");
            }

            if(m_InputData.discretization == "levelset")
            {
                // Compute Surface Area
                tmp_node = doc.append_child("Operation");
                addChild(tmp_node, "Function", "Compute Surface Area");
                addChild(tmp_node, "Name", "Compute Surface Area");
                tmp_node1 = tmp_node.append_child("Topology");
                addChild(tmp_node1, "Name", "Topology");
                tmp_node1 = tmp_node.append_child("OutputValue");
                addChild(tmp_node1, "Name", "SurfaceArea");

                // Compute Surface Area Gradient
                tmp_node = doc.append_child("Operation");
                addChild(tmp_node, "Function", "Compute Surface Area Gradient");
                addChild(tmp_node, "Name", "Compute Surface Area Gradient");
                tmp_node1 = tmp_node.append_child("Topology");
                addChild(tmp_node1, "ArgumentName", "Topology");
                tmp_node1 = tmp_node.append_child("OutputValue");
                addChild(tmp_node1, "Name", "SurfaceAreaGradient");

            }

            char buf[200];
            sprintf(buf, "sierra_sd_operations_%s.xml", m_InputData.objectives[i].name.c_str());
            // Write the file to disk
            doc.save_file(buf, "  ");
        }
    }

    return true;
}

/******************************************************************************/
void XMLGenerator::addStochasticObjectiveValueOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node1;
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "MeanPlusStdDev");
    addChild(tmp_node, "Name", "Stochastic Objective Value");
    addChild(tmp_node, "Layout", "Scalar");
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node.append_child("For");
      for_node.append_attribute("var") = "performerIndex";
      for_node.append_attribute("in") = "Performers";
      for_node = for_node.append_child("For");
      for_node.append_attribute("var") = "PerformerSampleIndex";
      for_node.append_attribute("in") = "PerformerSamples";
      pugi::xml_node input_node = for_node.append_child("Input");
      addChild(input_node, "ArgumentName", "Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
      addChild(input_node, "Probability", "{Probabilities[{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}]}");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        XMLGen::Objective cur_obj = m_InputData.objectives[i];
        tmp_node1 = tmp_node.append_child("Input");
        std::string tTmpString = "";
        if(cur_obj.code_name == "plato_analyze")
          tTmpString += "Objective Value ";
        else
          tTmpString += "Internal Energy ";
        tTmpString += Plato::to_string(i+1);
        addChild(tmp_node1, "ArgumentName", tTmpString);
        addChild(tmp_node1, "Probability", Plato::to_string(m_InputData.load_case_probabilities[i]).c_str());
      }
    }
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "Statistic", "mean");
    addChild(tmp_node1, "ArgumentName", "objective_mean");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "Statistic", "std_dev");
    addChild(tmp_node1, "ArgumentName", "objective_std_dev");

    tmp_node1 = tmp_node.append_child("Output");
    std::string tFieldName = "mean_plus_";
    tFieldName += m_InputData.objective_number_standard_deviations;
    tFieldName += "_std_dev";
    addChild(tmp_node1, "Statistic", tFieldName);
    tFieldName = "Objective Mean Plus ";
    tFieldName += m_InputData.objective_number_standard_deviations;
    tFieldName += " StdDev";
    addChild(tmp_node1, "ArgumentName", tFieldName);
}

/******************************************************************************/
void XMLGenerator::addVonMisesStatisticsOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node1;
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "MeanPlusStdDev");
    addChild(tmp_node, "Name", "VonMises Statistics");
    addChild(tmp_node, "Layout", "Element Field");
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node.append_child("For");
      for_node.append_attribute("var") = "performerIndex";
      for_node.append_attribute("in") = "Performers";
      for_node = for_node.append_child("For");
      for_node.append_attribute("var") = "PerformerSampleIndex";
      for_node.append_attribute("in") = "PerformerSamples";
      pugi::xml_node input_node = for_node.append_child("Input");
      addChild(input_node, "ArgumentName", "plato_analyze_{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}_vonmises");
      addChild(input_node, "Probability", "{Probabilities[{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}]}");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        XMLGen::Objective cur_obj = m_InputData.objectives[i];
        tmp_node1 = tmp_node.append_child("Input");
        addChild(tmp_node1, "ArgumentName", cur_obj.performer_name + "_vonmises");
        addChild(tmp_node1, "Probability", Plato::to_string(m_InputData.load_case_probabilities[i]).c_str());
      }
    }
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "Statistic", "mean");
    addChild(tmp_node1, "ArgumentName", "von_mises_mean");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "Statistic", "std_dev");
    addChild(tmp_node1, "ArgumentName", "von_mises_std_dev");
    for(size_t j=0; j<m_InputData.mStandardDeviations.size(); ++j)
    {
        tmp_node1 = tmp_node.append_child("Output");
        std::string tTmpString = "mean_plus_";
        tTmpString += m_InputData.mStandardDeviations[j];
        tTmpString += "_std_dev";
        addChild(tmp_node1, "Statistic", tTmpString);
        tTmpString = "VonMises Mean Plus ";
        tTmpString += m_InputData.mStandardDeviations[j];
        tTmpString += " StdDev";
        addChild(tmp_node1, "ArgumentName", tTmpString);
    }
}

/******************************************************************************/
void XMLGenerator::addFilterControlOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "Filter");
    addChild(tmp_node, "Name", "FilterControl");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Field");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Filtered Field");
    addChild(tmp_node, "Gradient", "False");
}

/******************************************************************************/
bool XMLGenerator::addAggregateGradientOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    char tBuffer[100];
    pugi::xml_node tmp_node2;
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "Aggregator");
    addChild(tmp_node, "Name", "AggregateGradient");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Aggregate");
    addChild(tmp_node1, "Layout", "Nodal Field");
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "sampleIndex";
      for_node.append_attribute("in") = "Samples";
      tmp_node2 = for_node.append_child("Input"); 
      addChild(tmp_node2, "ArgumentName", "Field {sampleIndex}");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
          tmp_node2 = tmp_node1.append_child("Input");
          std::string tTmpString = "Field ";
          tTmpString += Plato::to_string(i+1);
          addChild(tmp_node2, "ArgumentName", tTmpString);
      }
    }
    tmp_node2 = tmp_node1.append_child("Output");
    addChild(tmp_node2, "ArgumentName", "Field");

    tmp_node1 = tmp_node.append_child("Weighting");
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "sampleIndex";
      for_node.append_attribute("in") = "Samples";
      tmp_node2 = for_node.append_child("Weight"); 
      addChild(tmp_node2, "Value", "1");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
          tmp_node2 = tmp_node1.append_child("Weight");
          addChild(tmp_node2, "Value", m_InputData.objectives[i].weight.c_str());
      }
    }
    if(m_InputData.mUseNormalizationInAggregator == "true")
    {
      if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
      {
        std::cerr << "Error: Normalization not supported for uncertain loads" << std::endl;
        return false;
      }
        tmp_node2 = tmp_node1.append_child("Normals");
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        {
            pugi::xml_node tmp_node3 = tmp_node2.append_child("Input");
            sprintf(tBuffer, "Normalization Factor %d", (int)(i+1));
            addChild(tmp_node3, "ArgumentName", tBuffer);
        }
    }

    return true;
}

/******************************************************************************/
bool XMLGenerator::addAggregateEnergyOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    char tBuffer[100];
    pugi::xml_node tmp_node2;
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "Aggregator");
    addChild(tmp_node, "Name", "AggregateEnergy");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Aggregate");
    addChild(tmp_node1, "Layout", "Value");

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "sampleIndex";
      for_node.append_attribute("in") = "Samples";
      tmp_node2 = for_node.append_child("Input"); 
      addChild(tmp_node2, "ArgumentName", "Value {sampleIndex}");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        tmp_node2 = tmp_node1.append_child("Input");
        std::string tTmpString = "Value ";
        tTmpString += Plato::to_string(i+1);
        addChild(tmp_node2, "ArgumentName", tTmpString);
      }
    }

    tmp_node2 = tmp_node1.append_child("Output");
    addChild(tmp_node2, "ArgumentName", "Value");

    tmp_node1 = tmp_node.append_child("Aggregate");
    addChild(tmp_node1, "Layout", "Nodal Field");

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "sampleIndex";
      for_node.append_attribute("in") = "Samples";
      tmp_node2 = for_node.append_child("Input"); 
      addChild(tmp_node2, "ArgumentName", "Field {sampleIndex}");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        tmp_node2 = tmp_node1.append_child("Input");
        std::string tTmpString = "Field ";
        tTmpString += Plato::to_string(i+1);
        addChild(tmp_node2, "ArgumentName", tTmpString);
      }
    }


    tmp_node2 = tmp_node1.append_child("Output");
    addChild(tmp_node2, "ArgumentName", "Field");

    tmp_node1 = tmp_node.append_child("Weighting");

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "sampleIndex";
      for_node.append_attribute("in") = "Samples";
      tmp_node2 = for_node.append_child("Weight"); 
      addChild(tmp_node2, "Value", "1");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
          tmp_node2 = tmp_node1.append_child("Weight");
          addChild(tmp_node2, "Value", m_InputData.objectives[i].weight.c_str());
      }
    }

    if(m_InputData.mUseNormalizationInAggregator == "true")
    {
        if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
        {
          std::cerr << "Error: Normalization is not supported with uncertain loads" << std::endl;
          return false;
        }
        tmp_node2 = tmp_node1.append_child("Normals");
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        {
            pugi::xml_node tmp_node3 = tmp_node2.append_child("Input");
            sprintf(tBuffer, "Normalization Factor %d", (int)(i+1));
            addChild(tmp_node3, "ArgumentName", tBuffer);
        }
    }

    return true;
}

/******************************************************************************/
bool XMLGenerator::addAggregateValuesOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    char tBuffer[100];
    pugi::xml_node tmp_node2;
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "Aggregator");
    addChild(tmp_node, "Name", "AggregateValues");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Aggregate");
    addChild(tmp_node1, "Layout", "Value");
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "sampleIndex";
      for_node.append_attribute("in") = "Samples";
      tmp_node2 = for_node.append_child("Input"); 
      addChild(tmp_node2, "ArgumentName", "Value {sampleIndex}");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
          tmp_node2 = tmp_node1.append_child("Input");
          std::string tTmpString = "Value ";
          tTmpString += Plato::to_string(i+1);
          addChild(tmp_node2, "ArgumentName", tTmpString);
      }
    }
    tmp_node2 = tmp_node1.append_child("Output");
    addChild(tmp_node2, "ArgumentName", "Values");

    tmp_node1 = tmp_node.append_child("Weighting");
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "sampleIndex";
      for_node.append_attribute("in") = "Samples";
      tmp_node2 = for_node.append_child("Weight"); 
      addChild(tmp_node2, "Value", "1");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
          tmp_node2 = tmp_node1.append_child("Weight");
          addChild(tmp_node2, "Value", m_InputData.objectives[i].weight.c_str());
      }
    }
    if(m_InputData.mUseNormalizationInAggregator == "true")
    {
        if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
        {
          std::cerr << "Error: Normalization is not supported with uncertain loads" << std::endl;
          return false;
        }
        tmp_node2 = tmp_node1.append_child("Normals");
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        {
            pugi::xml_node tmp_node3 = tmp_node2.append_child("Input");
            sprintf(tBuffer, "Normalization Factor %d", (int)(i+1));
            addChild(tmp_node3, "ArgumentName", tBuffer);
        }
    }

    return true;
}

/******************************************************************************/
bool XMLGenerator::addAggregateHessianOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    char tBuffer[100];
    pugi::xml_node tmp_node2;
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "Aggregator");
    addChild(tmp_node, "Name", "AggregateHessian");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Aggregate");
    addChild(tmp_node1, "Layout", "Nodal Field");
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "sampleIndex";
      for_node.append_attribute("in") = "Samples";
      tmp_node2 = for_node.append_child("Input"); 
      addChild(tmp_node2, "ArgumentName", "Field {sampleIndex}");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
          tmp_node2 = tmp_node1.append_child("Input");
          std::string tTmpString = "Field ";
          tTmpString += Plato::to_string(i+1);
          addChild(tmp_node2, "ArgumentName", tTmpString);
      }
    }
    tmp_node2 = tmp_node1.append_child("Output");
    addChild(tmp_node2, "ArgumentName", "Field");

    tmp_node1 = tmp_node.append_child("Weighting");
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "sampleIndex";
      for_node.append_attribute("in") = "Samples";
      tmp_node2 = for_node.append_child("Weight"); 
      addChild(tmp_node2, "Value", "1");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
          tmp_node2 = tmp_node1.append_child("Weight");
          addChild(tmp_node2, "Value", m_InputData.objectives[i].weight.c_str());
      }
    }
    if(m_InputData.mUseNormalizationInAggregator == "true")
    {
        if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
        {
          std::cerr << "Error: Normalization is not supported with uncertain loads" << std::endl;
          return false;
        }
        tmp_node2 = tmp_node1.append_child("Normals");
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        {
            pugi::xml_node tmp_node3 = tmp_node2.append_child("Input");
            sprintf(tBuffer, "Normalization Factor %d", (int)(i+1));
            addChild(tmp_node3, "ArgumentName", tBuffer);
        }
    }

    return true;
}

/******************************************************************************/
void XMLGenerator::addSetLowerBoundsOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "SetLowerBounds");
    addChild(tmp_node, "Discretization", m_InputData.discretization);
    addChild(tmp_node, "Name", "Calculate Lower Bounds");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Lower Bound Value");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Lower Bound Vector");
    if(m_InputData.fixed_block_ids.size() > 0)
    {
        tmp_node1 = tmp_node.append_child("FixedBlocks");
        for(size_t i=0; i<m_InputData.fixed_block_ids.size(); ++i)
        {
            addChild(tmp_node1, "Index", m_InputData.fixed_block_ids[i].c_str());
        }
    }
    if(m_InputData.fixed_sideset_ids.size() > 0)
    {
        tmp_node1 = tmp_node.append_child("FixedSidesets");
        for(size_t i=0; i<m_InputData.fixed_sideset_ids.size(); ++i)
        {
            addChild(tmp_node1, "Index", m_InputData.fixed_sideset_ids[i].c_str());
        }
    }
    if(m_InputData.fixed_nodeset_ids.size() > 0)
    {
        tmp_node1 = tmp_node.append_child("FixedNodesets");
        for(size_t i=0; i<m_InputData.fixed_nodeset_ids.size(); ++i)
        {
            addChild(tmp_node1, "Index", m_InputData.fixed_nodeset_ids[i].c_str());
        }
    }
}

/******************************************************************************/
void XMLGenerator::addSetUpperBoundsOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "SetUpperBounds");
    addChild(tmp_node, "Discretization", m_InputData.discretization);
    addChild(tmp_node, "Name", "Calculate Upper Bounds");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Upper Bound Value");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Upper Bound Vector");
    if(m_InputData.fixed_block_ids.size() > 0)
    {
        tmp_node1 = tmp_node.append_child("FixedBlocks");
        for(size_t i=0; i<m_InputData.fixed_block_ids.size(); ++i)
        {
            addChild(tmp_node1, "Index", m_InputData.fixed_block_ids[i].c_str());
        }
    }
    if(m_InputData.fixed_sideset_ids.size() > 0)
    {
        tmp_node1 = tmp_node.append_child("FixedSidesets");
        for(size_t i=0; i<m_InputData.fixed_sideset_ids.size(); ++i)
        {
            addChild(tmp_node1, "Index", m_InputData.fixed_sideset_ids[i].c_str());
        }
    }
    if(m_InputData.fixed_nodeset_ids.size() > 0)
    {
        tmp_node1 = tmp_node.append_child("FixedNodesets");
        for(size_t i=0; i<m_InputData.fixed_nodeset_ids.size(); ++i)
        {
            addChild(tmp_node1, "Index", m_InputData.fixed_nodeset_ids[i].c_str());
        }
    }
}

/******************************************************************************/
void XMLGenerator::addEnforceBoundsOperationToFile(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "EnforceBounds");
    addChild(tmp_node, "Name", "EnforceBounds");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Upper Bound Vector");
    tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Lower Bound Vector");
    tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Topology");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Topology");
}

/******************************************************************************/
void XMLGenerator::addEnforceBoundsOperationToStage(pugi::xml_node &aStageNode)
/******************************************************************************/
{
    pugi::xml_node tOperationNode = aStageNode.append_child("Operation");
    addChild(tOperationNode, "Name", "EnforceBounds");
    addChild(tOperationNode, "PerformerName", "PlatoMain");
    pugi::xml_node tInputNode = tOperationNode.append_child("Input");
    addChild(tInputNode, "ArgumentName", "Lower Bound Vector");
    addChild(tInputNode, "SharedDataName", "Lower Bound Vector");
    tInputNode = tOperationNode.append_child("Input");
    addChild(tInputNode, "ArgumentName", "Upper Bound Vector");
    addChild(tInputNode, "SharedDataName", "Upper Bound Vector");
    tInputNode = tOperationNode.append_child("Input");
    addChild(tInputNode, "ArgumentName", "Topology");
    addChild(tInputNode, "SharedDataName", "Topology");
    pugi::xml_node tOutputNode = tOperationNode.append_child("Output");
    addChild(tOutputNode, "ArgumentName", "Topology");
    addChild(tOutputNode, "SharedDataName", "Topology");
}

/******************************************************************************/
void XMLGenerator::addDesignVolumeOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "DesignVolume");
    addChild(tmp_node, "Name", "Design Volume");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Design Volume");
}

/******************************************************************************/
void XMLGenerator::addComputeVolumeOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "ComputeVolume");
    addChild(tmp_node, "Name", "Compute Current Volume");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Topology");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Volume");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Volume Gradient");
    addChild(tmp_node, "PenaltyModel", "SIMP");
    tmp_node1 = tmp_node.append_child("SIMP");
    addChild(tmp_node1, "PenaltyExponent", "1.0");
    addChild(tmp_node1, "MinimumValue", "0.0");
}

/******************************************************************************/
void XMLGenerator::addInitializeFieldOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    // InitializeField
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "InitializeField");
    addChild(tmp_node, "Name", "Initialize Field");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Initialized Field");
    if(m_InputData.initial_guess_filename != "")
    {
        // This handles both restarting from an iteration from a previous
        // run or by just specifying an initial guess filename.
        addChild(tmp_node, "Method", "FromFieldOnInputMesh");
        tmp_node1 = tmp_node.append_child("FromFieldOnInputMesh");
        addChild(tmp_node1, "Name", m_InputData.run_mesh_name);
        addChild(tmp_node1, "VariableName", m_InputData.initial_guess_field_name);
        if(m_InputData.restart_iteration != "")
            addChild(tmp_node1, "Iteration", m_InputData.restart_iteration);
    }
    else
    {
        if(m_InputData.discretization == "density")
        {
            addChild(tmp_node, "Method", "Uniform");
            tmp_node1 = tmp_node.append_child("Uniform");
            addChild(tmp_node1, "Value", m_InputData.initial_density_value);
        }
        else if(m_InputData.discretization == "levelset")
        {
            if(m_InputData.levelset_initialization_method == "primitives")
            {
                addChild(tmp_node, "Method", "PrimitivesLevelSet");
                tmp_node1 = tmp_node.append_child("PrimitivesLevelSet");
                addChild(tmp_node1, "BackgroundMeshName", m_InputData.run_mesh_name);
                if(m_InputData.levelset_material_box_min != "" &&
                   m_InputData.levelset_material_box_max != "")
                {
                    pugi::xml_node tmp_node2 = tmp_node1.append_child("MaterialBox");
                    addChild(tmp_node2, "MinCoords", m_InputData.levelset_material_box_min);
                    addChild(tmp_node2, "MaxCoords", m_InputData.levelset_material_box_max);
                }
            }
            else
            {
                addChild(tmp_node, "Method", "SwissCheeseLevelSet");
                tmp_node1 = tmp_node.append_child("SwissCheeseLevelSet");
                if(m_InputData.create_levelset_spheres == "")
                    m_InputData.create_levelset_spheres = "false";
                addChild(tmp_node1, "CreateSpheres", m_InputData.create_levelset_spheres);
                if(m_InputData.create_levelset_spheres == "true" && m_InputData.levelset_sphere_radius != "")
                    addChild(tmp_node1, "SphereRadius", m_InputData.levelset_sphere_radius);
                if(m_InputData.create_levelset_spheres == "true" && m_InputData.levelset_sphere_packing_factor != "")
                    addChild(tmp_node1, "SpherePackingFactor", m_InputData.levelset_sphere_packing_factor);
                for(size_t r=0; r<m_InputData.levelset_nodesets.size(); ++r)
                {
                    addChild(tmp_node1, "NodeSet", m_InputData.levelset_nodesets[r]);
                }
                addChild(tmp_node1, "BackgroundMeshName", m_InputData.run_mesh_name);
            }
        }
    }
}

/******************************************************************************/
void XMLGenerator::addUpdateGeometryOnChangeOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    // Update Geometry on Change
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "SystemCall");
    addChild(tmp_node, "Name", "Update Geometry on Change");
    addChild(tmp_node, "Command", "python aflr.py");
    addChild(tmp_node, "OnChange", "true");
    addChild(tmp_node, "Argument", m_InputData.csm_filename);
    addChild(tmp_node, "Argument", m_InputData.csm_exodus_filename);
    addChild(tmp_node, "Argument", m_InputData.csm_tesselation_filename);
    addChild(tmp_node, "AppendInput", "true");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Parameters");
}

/******************************************************************************/
void XMLGenerator::addInitializeValuesOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    // InitializeField
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "InitializeValues");
    addChild(tmp_node, "Name", "Initialize Values");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Values");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Lower Bounds");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Upper Bounds");
    if(m_InputData.csm_filename.length() > 0)
    {
        addChild(tmp_node, "Method", "ReadFromCSMFile");
        addChild(tmp_node, "CSMFileName", m_InputData.csm_filename);
    }
    else
    {
        addChild(tmp_node, "Method", "UniformValue");
        addChild(tmp_node, "InitialValue", m_InputData.initial_density_value);
    }
}

/******************************************************************************/
void XMLGenerator::addFilterInfo(pugi::xml_document &aDoc)
/******************************************************************************/
{
    // Filter
    pugi::xml_node tmp_node = aDoc.append_child("Filter");
    if(m_InputData.filter_type == m_filterType_kernelThenHeaviside_generatorName)
    {
        // kernel then heaviside
        addChild(tmp_node, "Name", m_filterType_kernelThenHeaviside_XMLName);
    }
    else if(m_InputData.filter_type == m_filterType_identity_generatorName)
    {
        // identity
        addChild(tmp_node, "Name", m_filterType_identity_XMLName);
    }
    else if(m_InputData.filter_type == m_filterType_kernelThenTANH_generatorName)
    {
        // kernel then tanh
        addChild(tmp_node, "Name", m_filterType_kernelThenTANH_XMLName);
    }
    else
    {
        // kernel is default
        addChild(tmp_node, "Name", m_filterType_kernel_XMLName);
    }
    if(m_InputData.filter_radius_scale != "")
    {
        addChild(tmp_node, "Scale", m_InputData.filter_radius_scale);
    }
    if(m_InputData.filter_radius_absolute != "")
    {
        addChild(tmp_node, "Absolute", m_InputData.filter_radius_absolute);
    }
    if(m_InputData.filter_projection_start_iteration != "")
    {
        addChild(tmp_node, "StartIteration", m_InputData.filter_projection_start_iteration);
    }
    if(m_InputData.filter_projection_update_interval != "")
    {
        addChild(tmp_node, "UpdateInterval", m_InputData.filter_projection_update_interval);
    }
    if(m_InputData.filter_use_additive_continuation != "")
    {
        addChild(tmp_node, "UseAdditiveContinuation", m_InputData.filter_use_additive_continuation);
    }
    if(m_InputData.filter_power != "")
    {
        addChild(tmp_node, "Power", m_InputData.filter_power);
    }
    if(m_InputData.filter_heaviside_min != "")
    {
        addChild(tmp_node, "HeavisideMin", m_InputData.filter_heaviside_min);
    }
    if(m_InputData.filter_heaviside_update != "")
    {
        addChild(tmp_node, "HeavisideUpdate", m_InputData.filter_heaviside_update);
    }
    if(m_InputData.filter_heaviside_max != "")
    {
        addChild(tmp_node, "HeavisideMax", m_InputData.filter_heaviside_max);
    }
}

/******************************************************************************/
void XMLGenerator::addUpdateProblemOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "Update Problem");
    addChild(tmp_node, "Name", "Update Problem");
}

/******************************************************************************/
void XMLGenerator::addPlatoMainOutputOperation(pugi::xml_document &aDoc,
                                               bool &aHasUncertainties,
                                               bool &aRequestedVonMises)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "PlatoMainOutput");
    addChild(tmp_node, "Name", "PlatoMainOutput");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Topology");
    tmp_node1 = tmp_node.append_child("Input");
    if(m_InputData.mPlatoAnalyzePerformerExists)
      addChild(tmp_node1, "ArgumentName", "Objective Gradient");
    else
      addChild(tmp_node1, "ArgumentName", "Internal Energy Gradient");
    tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Optimization DOFs");
    if(m_InputData.constraints.size() > 0)
    {
        if(m_InputData.constraints[0].type == "volume")
        {
            tmp_node1 = tmp_node.append_child("Input");
            addChild(tmp_node1, "ArgumentName", "Volume Gradient");
        }
        else if(m_InputData.constraints[0].type == "surface area")
        {
            tmp_node1 = tmp_node.append_child("Input");
            addChild(tmp_node1, "ArgumentName", "Surface Area Gradient");
        }
    }

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node.append_child("For");
      for_node.append_attribute("var") = "sampleIndex";
      for_node.append_attribute("in") = "Samples";

      tmp_node1 = for_node.append_child("Input");
      addChild(tmp_node1, "ArgumentName", "plato_analyze_{sampleIndex}_vonmises");
      addChild(tmp_node1, "Layout", "Element Field");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        XMLGen::Objective cur_obj = m_InputData.objectives[i];
        if(aHasUncertainties)
        {
          // Only handling vonmises for UQ runs and only one load case
          // per objective/performer.
          tmp_node1 = tmp_node.append_child("Input");
          addChild(tmp_node1, "ArgumentName", cur_obj.performer_name + "_vonmises");
          addChild(tmp_node1, "Layout", "Element Field");
        }
        else if(cur_obj.multi_load_case == "true")
        {
          for(size_t k=0; k<cur_obj.load_case_ids.size(); k++)
          {
            std::string cur_load_string = cur_obj.load_case_ids[k];
            for(size_t j=0; j<cur_obj.output_for_plotting.size(); j++)
            {
              tmp_node1 = tmp_node.append_child("Input");
              addChild(tmp_node1, "ArgumentName", cur_obj.performer_name + "_load" + cur_load_string + "_" + cur_obj.output_for_plotting[j]);
              if(cur_obj.output_for_plotting[j] == "vonmises")
                addChild(tmp_node1, "Layout", "Element Field");
            }
          }
        }
        else
        {
          for(size_t j=0; j<cur_obj.output_for_plotting.size(); j++)
          {
            tmp_node1 = tmp_node.append_child("Input");
            addChild(tmp_node1, "ArgumentName", cur_obj.performer_name + "_" + cur_obj.output_for_plotting[j]);
            if(cur_obj.output_for_plotting[j] == "vonmises" ||
                cur_obj.output_for_plotting[j] == "Vonmises")
              addChild(tmp_node1, "Layout", "Element Field");
          }
        }
      }
    }

    if(aHasUncertainties && aRequestedVonMises)
    {
        tmp_node1 = tmp_node.append_child("Input");
        addChild(tmp_node1, "ArgumentName", "VonMises Mean");
        addChild(tmp_node1, "Layout", "Element Field");
        tmp_node1 = tmp_node.append_child("Input");
        addChild(tmp_node1, "ArgumentName", "VonMises StdDev");
        addChild(tmp_node1, "Layout", "Element Field");
        for(size_t j=0; j<m_InputData.mStandardDeviations.size(); ++j)
        {
            tmp_node1 = tmp_node.append_child("Input");
            std::string tTempString = "VonMises Mean Plus ";
            tTempString += m_InputData.mStandardDeviations[j];
            tTempString += " StdDev";
            addChild(tmp_node1, "ArgumentName", tTempString);
            addChild(tmp_node1, "Layout", "Element Field");
        }
    }

    addChild(tmp_node, "WriteRestart", m_InputData.write_restart_file);
    addChild(tmp_node, "OutputFrequency", m_InputData.output_frequency);
    addChild(tmp_node, "MaxIterations", m_InputData.max_iterations);
    tmp_node1 = tmp_node.append_child("SurfaceExtraction");
    addChild(tmp_node1, "OutputMethod", m_InputData.output_method);
    addChild(tmp_node1, "Discretization", m_InputData.discretization);
    pugi::xml_node tmp_node2 = tmp_node1.append_child("Output");
    addChild(tmp_node2, "Format", "Exodus");

    if(aHasUncertainties)
    {
        addStochasticObjectiveValueOperation(aDoc);
        addStochasticObjectiveGradientOperation(aDoc);
        if(aRequestedVonMises)
            addVonMisesStatisticsOperation(aDoc);
    }
}

/******************************************************************************/
void XMLGenerator::addCSMMeshOutputOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "CSMMeshOutput");
    addChild(tmp_node, "Name", "CSMMeshOutput");
    addChild(tmp_node, "BaseMeshName", m_InputData.csm_exodus_filename);
    addChild(tmp_node, "OutputFrequency", m_InputData.output_frequency);
    addChild(tmp_node, "MaxIterations", m_InputData.max_iterations);
}

/******************************************************************************/
void XMLGenerator::addFilterGradientOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "Filter");
    addChild(tmp_node, "Name", "FilterGradient");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Field");
    tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Gradient");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Filtered Gradient");
    addChild(tmp_node, "Gradient", "True");
}

/******************************************************************************/
void XMLGenerator::addFilterHessianOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "Filter");
    addChild(tmp_node, "Name", "FilterHessian");
    pugi::xml_node tmp_node1 = tmp_node.append_child("Input");
    addChild(tmp_node1, "ArgumentName", "Field");
    tmp_node1 = tmp_node.append_child("Output");
    addChild(tmp_node1, "ArgumentName", "Filtered Field");
    addChild(tmp_node, "Gradient", "False");
}

/******************************************************************************/
void XMLGenerator::addStochasticObjectiveGradientOperation(pugi::xml_document &aDoc)
/******************************************************************************/
{
    pugi::xml_node tmp_node1, tmp_node2;
    pugi::xml_node tmp_node = aDoc.append_child("Operation");
    addChild(tmp_node, "Function", "MeanPlusStdDevGradient");
    addChild(tmp_node, "Name", "Stochastic Objective Gradient");

    tmp_node1 = tmp_node.append_child("CriterionValue");
    addChild(tmp_node1, "Layout", "Global");
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "performerIndex";
      for_node.append_attribute("in") = "Performers";
      for_node = for_node.append_child("For");
      for_node.append_attribute("var") = "PerformerSampleIndex";
      for_node.append_attribute("in") = "PerformerSamples";
      pugi::xml_node input_node = for_node.append_child("Input");
      addChild(input_node, "ArgumentName", "Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
      addChild(input_node, "Probability", "{Probabilities[{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}]}");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        XMLGen::Objective cur_obj = m_InputData.objectives[i];
        pugi::xml_node input_node = tmp_node1.append_child("Input");
        std::string tTmpString = "";
        if(cur_obj.code_name == "plato_analyze")
          tTmpString += "Objective Value ";
        else
          tTmpString += "Internal Energy ";
        tTmpString += Plato::to_string(i+1);
        addChild(input_node, "ArgumentName", tTmpString);
        addChild(input_node, "Probability", Plato::to_string(m_InputData.load_case_probabilities[i]).c_str());
      }
    }
    tmp_node2 = tmp_node1.append_child("Output");
    addChild(tmp_node2, "Statistic", "mean");
    addChild(tmp_node2, "ArgumentName", "objective_mean");
    tmp_node2 = tmp_node1.append_child("Output");
    addChild(tmp_node2, "Statistic", "std_dev");
    addChild(tmp_node2, "ArgumentName", "objective_std_dev");

    tmp_node1 = tmp_node.append_child("CriterionGradient");
    addChild(tmp_node1, "Layout", "Nodal Field");
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = tmp_node1.append_child("For");
      for_node.append_attribute("var") = "performerIndex";
      for_node.append_attribute("in") = "Performers";
      for_node = for_node.append_child("For");
      for_node.append_attribute("var") = "PerformerSampleIndex";
      for_node.append_attribute("in") = "PerformerSamples";
      pugi::xml_node input_node = for_node.append_child("Input");
      addChild(input_node, "ArgumentName", "Objective {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex} Gradient");
      addChild(input_node, "Probability", "{Probabilities[{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}]}");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        XMLGen::Objective cur_obj = m_InputData.objectives[i];
        pugi::xml_node input_node = tmp_node1.append_child("Input");
        std::string tTmpString = "";
        if(cur_obj.code_name == "plato_analyze")
          tTmpString += "Objective ";
        else
          tTmpString += "Internal Energy ";
        tTmpString += Plato::to_string(i+1);
        tTmpString += " Gradient";
        addChild(input_node, "ArgumentName", tTmpString);
        addChild(input_node, "Probability", Plato::to_string(m_InputData.load_case_probabilities[i]).c_str());
      }
    }
    tmp_node2 = tmp_node1.append_child("Output");
    std::string tTmpString = "mean_plus_";
    tTmpString += m_InputData.objective_number_standard_deviations;
    tTmpString += "_std_dev";
    addChild(tmp_node2, "Statistic", tTmpString);
    tTmpString = "Objective Mean Plus ";
    tTmpString += m_InputData.objective_number_standard_deviations;
    tTmpString += " StdDev Gradient";
    addChild(tmp_node2, "ArgumentName", tTmpString);
}

/******************************************************************************/
bool XMLGenerator::generatePlatoMainOperationsXML()
/******************************************************************************/
{
    pugi::xml_document doc;
    pugi::xml_node tmp_node, tmp_node1, tmp_node2;

    // Version entry
    tmp_node = doc.append_child(pugi::node_declaration);
    tmp_node.set_name("xml");
    pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
    tmp_att.set_value("1.0");

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node def_node = doc.append_child("include");
      def_node.append_attribute("filename") = "defines.xml";
    }

    //////////////////////////////////////////////////
    // Operations
    /////////////////////////////////////////////////

    if(m_InputData.optimization_type == "topology")
    {
        addFilterInfo(doc);
        addPlatoMainOutputOperation(doc, m_HasUncertainties, m_RequestedVonMisesOutput);
        addUpdateProblemOperation(doc);
        addFilterControlOperation(doc);
        addFilterGradientOperation(doc);
        if(m_InputData.optimization_algorithm =="ksbc" ||
                m_InputData.optimization_algorithm == "ksal" ||
                m_InputData.optimization_algorithm == "rol ksal" ||
                m_InputData.optimization_algorithm == "rol ksbc")
        {
            addFilterHessianOperation(doc);
        }
        addInitializeFieldOperation(doc);
        if(m_InputData.discretization == "density")
        {
            addDesignVolumeOperation(doc);
            addComputeVolumeOperation(doc);
        }
        else if(m_InputData.discretization == "levelset")
        {
        }
        if(!addAggregateEnergyOperation(doc))
          return false;
        if(!addAggregateGradientOperation(doc))
          return false;
        if(m_InputData.optimization_algorithm =="ksbc" ||
                m_InputData.optimization_algorithm == "ksal" ||
                m_InputData.optimization_algorithm == "rol ksal" ||
                m_InputData.optimization_algorithm == "rol ksbc")
        {
            addAggregateHessianOperation(doc);
        }
        addSetLowerBoundsOperation(doc);
        addSetUpperBoundsOperation(doc);
        if(!m_InputData.mPlatoAnalyzePerformerExists)
            addEnforceBoundsOperationToFile(doc);
    }
    else if(m_InputData.optimization_type == "shape")
    {
        addCSMMeshOutputOperation(doc);
        addInitializeValuesOperation(doc);
        addAggregateValuesOperation(doc);
        addUpdateGeometryOnChangeOperation(doc);
    }

    // Write the file to disk
    doc.save_file("plato_main_operations.xml", "  ");

    return true;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoMainOperationsXMLForShape()
/******************************************************************************/
{
    pugi::xml_document doc;
    pugi::xml_node tmp_node;

    // Version entry
    tmp_node = doc.append_child(pugi::node_declaration);
    tmp_node.set_name("xml");
    pugi::xml_attribute tmp_att = tmp_node.append_attribute("version");
    tmp_att.set_value("1.0");

    // Timers
    tmp_node = doc.append_child("Timers");
    addChild(tmp_node, "time", "true");

    addCSMMeshOutputOperation(doc);
    addInitializeValuesOperation(doc);
    addAggregateValuesOperation(doc);
    addUpdateGeometryOnChangeOperation(doc);

    // Write the file to disk
    doc.save_file("plato_main_operations.xml", "  ");

    return true;
}

/******************************************************************************/
bool XMLGenerator::outputVolumeGradientStage(pugi::xml_document &doc)
/******************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Volume Gradient");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Optimization DOFs");

    // Filter control
    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterControl");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Field");
    addChild(output_node, "SharedDataName", "Topology");

    // Enforce Bounds operation
    if(!m_InputData.mPlatoAnalyzePerformerExists)
        addEnforceBoundsOperationToStage(stage_node);

    // Compute current volume
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Compute Current Volume");
    addChild(op_node, "PerformerName", "PlatoMain");

    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Topology");
    addChild(input_node, "SharedDataName", "Topology");

    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Volume");
    addChild(output_node, "SharedDataName", "Volume");
    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Volume Gradient");
    addChild(output_node, "SharedDataName", "Volume Gradient");

    // Filter gradient
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterGradient");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Gradient");
    addChild(input_node, "SharedDataName", "Volume Gradient");
    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Gradient");
    addChild(output_node, "SharedDataName", "Volume Gradient");

    // stage output
    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Volume Gradient");

    return true;
}

/******************************************************************************/
bool XMLGenerator::outputSurfaceAreaGradientStage(pugi::xml_document &doc)
/******************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Surface Area Gradient");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Optimization DOFs");

    // Filter control
    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterControl");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Field");
    addChild(output_node, "SharedDataName", "Topology");

    // Enforce Bounds operation
    if(!m_InputData.mPlatoAnalyzePerformerExists)
        addEnforceBoundsOperationToStage(stage_node);

    // Compute current surface area gradient
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Compute Surface Area Gradient");
    addChild(op_node, "PerformerName", m_InputData.objectives[0].performer_name.c_str());

    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Topology");
    addChild(input_node, "SharedDataName", "Topology");

    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "SurfaceAreaGradient");
    addChild(output_node, "SharedDataName", "Surface Area Gradient");

    // Filter gradient
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterGradient");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Gradient");
    addChild(input_node, "SharedDataName", "Surface Area Gradient");
    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Gradient");
    addChild(output_node, "SharedDataName", "Surface Area Gradient");

    // stage output
    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Surface Area Gradient");

    return true;
}

/******************************************************************************/
bool XMLGenerator::outputVolumeStage(pugi::xml_document &doc)
/******************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Volume");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Optimization DOFs");

    // Filter control operation
    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterControl");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Field");
    addChild(output_node, "SharedDataName", "Topology");

    // Enforce Bounds operation
    if(!m_InputData.mPlatoAnalyzePerformerExists)
        addEnforceBoundsOperationToStage(stage_node);

    // Compute current volume operation
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Compute Current Volume");
    addChild(op_node, "PerformerName", "PlatoMain");

    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Topology");
    addChild(input_node, "SharedDataName", "Topology");

    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Volume");
    addChild(output_node, "SharedDataName", "Volume");
    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Volume Gradient");
    addChild(output_node, "SharedDataName", "Volume Gradient");

    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Volume");

    return true;
}

/******************************************************************************/
bool XMLGenerator::outputConstraintStage(pugi::xml_document &doc)
/******************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Constraint");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Design Parameters");

    // Update Geometry on Change operation
    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Update Geometry on Change");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Parameters");
    addChild(input_node, "SharedDataName", "Design Parameters");

    // Reinitialize on Change operation
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Reinitialize on Change");
    addChild(op_node, "PerformerName", m_InputData.objectives[0].performer_name);
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Parameters");
    addChild(input_node, "SharedDataName", "Design Parameters");

    // Compute constraint value operation
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Compute Constraint Value");
    addChild(op_node, "PerformerName", m_InputData.objectives[0].performer_name);

    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Constraint Value");
    addChild(output_node, "SharedDataName", "Constraint");

    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Constraint");

    return true;
}

/******************************************************************************/
bool XMLGenerator::outputConstraintGradientStage(pugi::xml_document &doc)
/******************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Constraint Gradient");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Design Parameters");

    // Update Geometry on Change operation
    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Update Geometry on Change");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Parameters");
    addChild(input_node, "SharedDataName", "Design Parameters");

    // Reinitialize on Change operation
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Reinitialize on Change");
    addChild(op_node, "PerformerName", m_InputData.objectives[0].performer_name);
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Parameters");
    addChild(input_node, "SharedDataName", "Design Parameters");

    // Compute constraint gradient value operation
    op_node = stage_node.append_child("Operation");
    pugi::xml_node op_node2 = op_node.append_child("Operation");
    addChild(op_node2, "Name", "Compute Constraint Gradient");
    addChild(op_node2, "PerformerName", m_InputData.objectives[0].performer_name);

    pugi::xml_node tForNode = op_node.append_child("For");
    tForNode.append_attribute("var") = "I";
    tForNode.append_attribute("in") = "Parameters";
    op_node2 = tForNode.append_child("Operation");
    addChild(op_node2, "Name", "Compute Shape Sensitivity On Change");
    addChild(op_node2, "PerformerName", "PlatoESP_{I}");
    pugi::xml_node tmp_node = op_node2.append_child("Parameter");
    addChild(tmp_node, "ArgumentName", "Parameter Index");
    addChild(tmp_node, "ArgumentValue", "{I-1}");
    tmp_node = op_node2.append_child("Input");
    addChild(tmp_node, "ArgumentName", "Parameters");
    addChild(tmp_node, "SharedDataName", "Design Parameters");
    tmp_node = op_node2.append_child("Output");
    addChild(tmp_node, "ArgumentName", "Parameter Sensitivity");
    addChild(tmp_node, "SharedDataName", "Parameter Sensitivity {I}");

    op_node2 = stage_node.append_child("Operation");
    addChild(op_node2, "Name", "Compute Constraint Sensitivity");
    addChild(op_node2, "PerformerName", m_InputData.objectives[0].performer_name);
    tForNode = op_node2.append_child("For");
    tForNode.append_attribute("var") = "I";
    tForNode.append_attribute("in") = "Parameters";
    tmp_node = tForNode.append_child("Input");
    addChild(tmp_node, "ArgumentName", "Parameter Sensitivity {I}");
    addChild(tmp_node, "SharedDataName", "Parameter Sensitivity {I}");
    tmp_node = op_node2.append_child("Output");
    addChild(tmp_node, "ArgumentName", "Constraint Sensitivity");
    addChild(tmp_node, "SharedDataName", "Constraint Gradient");

    pugi::xml_node output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Constraint Gradient");

    return true;
}

/******************************************************************************/
bool XMLGenerator::outputSurfaceAreaStage(pugi::xml_document &doc)
/******************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Surface Area");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Optimization DOFs");

    // Filter control operation
    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterControl");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Field");
    addChild(output_node, "SharedDataName", "Topology");

    // Enforce Bounds operation
    if(!m_InputData.mPlatoAnalyzePerformerExists)
        addEnforceBoundsOperationToStage(stage_node);

    // Compute current surface area operation
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Compute Surface Area");
    addChild(op_node, "PerformerName", m_InputData.objectives[0].performer_name.c_str());

    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Topology");
    addChild(input_node, "SharedDataName", "Topology");

    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "SurfaceArea");
    addChild(output_node, "SharedDataName", "Surface Area");

    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Surface Area");

    return true;
}

/******************************************************************************/
bool XMLGenerator::outputComputeStateStage(pugi::xml_document &doc)
/******************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Compute State");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Optimization DOFs");

    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterControl");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Field");
    addChild(output_node, "SharedDataName", "Topology");

    // Enforce Bounds operation
    if(!m_InputData.mPlatoAnalyzePerformerExists)
        addEnforceBoundsOperationToStage(stage_node);

    pugi::xml_node cur_parent = stage_node;
    if(m_InputData.objectives.size() > 1)
    {
        op_node = stage_node.append_child("Operation");
        cur_parent = op_node;
    }

    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        op_node = cur_parent.append_child("Operation");
        addChild(op_node, "Name", "Compute Displacement");
        addChild(op_node, "PerformerName", m_InputData.objectives[i].performer_name.c_str());

        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Topology");
        addChild(input_node, "SharedDataName", "Topology");
    }

    return true;
}


/******************************************************************************/
bool XMLGenerator::outputInternalEnergyStage(pugi::xml_document &doc,
                                             bool &aHasUncertainties)
/******************************************************************************/
{
    char tmp_buf[200];
    // Internal Energy
    pugi::xml_node stage_node = doc.append_child("Stage");
    pugi::xml_node cur_parent = stage_node;
    if(m_InputData.mPlatoAnalyzePerformerExists)
      addChild(stage_node, "Name", "Objective Value");
    else
      addChild(stage_node, "Name", "Internal Energy");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Optimization DOFs");

    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterControl");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Field");
    addChild(output_node, "SharedDataName", "Topology");

    // Enforce Bounds operation
    if(!m_InputData.mPlatoAnalyzePerformerExists)
        addEnforceBoundsOperationToStage(stage_node);

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      addComputeObjectiveValueOperationForNewUncertaintyWorkflow(stage_node);
    }
    else
    {
      if(m_InputData.objectives.size() > 1)
      {
          op_node = stage_node.append_child("Operation");
          cur_parent = op_node;
      }

      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
          XMLGen::Objective cur_obj = m_InputData.objectives[i];
          op_node = cur_parent.append_child("Operation");
          if(cur_obj.code_name == "plato_analyze")
          {
              addChild(op_node, "Name", "Compute Objective Value");
              addChild(op_node, "PerformerName", cur_obj.performer_name.c_str());
          }
          else
          {
              addChild(op_node, "Name", "Compute Objective");
              addChild(op_node, "PerformerName", cur_obj.performer_name.c_str());
          }

          input_node = op_node.append_child("Input");
          addChild(input_node, "ArgumentName", "Topology");
          addChild(input_node, "SharedDataName", "Topology");

          output_node = op_node.append_child("Output");
          if(m_InputData.mPlatoAnalyzePerformerExists)
            addChild(output_node, "ArgumentName", "Objective Value");
          else
            addChild(output_node, "ArgumentName", "Internal Energy");
          if(cur_obj.code_name == "plato_analyze")
            sprintf(tmp_buf, "Objective Value %d", (int)(i+1));
          else
            sprintf(tmp_buf, "Internal Energy %d", (int)(i+1));

          addChild(output_node, "SharedDataName", tmp_buf);
      }
    }

    // AggregateEnergy
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "AggregateEnergy");
    addChild(op_node, "PerformerName", "PlatoMain");

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
        pugi::xml_node for_node = op_node.append_child("For");
        for_node.append_attribute("var") = "performerIndex";
        for_node.append_attribute("in") = "Performers";
        for_node = for_node.append_child("For");
        for_node.append_attribute("var") = "PerformerSampleIndex";
        for_node.append_attribute("in") = "PerformerSamples";
        input_node = for_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
        addChild(input_node, "SharedDataName", "Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");

        if(m_InputData.mUseNormalizationInAggregator == "true")
        {
          input_node = for_node.append_child("Input");
          addChild(input_node, "ArgumentName", "Normalization Factor {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
          addChild(input_node, "SharedDataName", "Initial Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
        }
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        input_node = op_node.append_child("Input");
        sprintf(tmp_buf, "Value %d", (int)(i+1));
        addChild(input_node, "ArgumentName", tmp_buf);
        if(m_InputData.objectives[i].code_name == "plato_analyze")
          sprintf(tmp_buf, "Objective Value %d", (int)(i+1));
        else
          sprintf(tmp_buf, "Internal Energy %d", (int)(i+1));
        addChild(input_node, "SharedDataName", tmp_buf);
        if(m_InputData.mUseNormalizationInAggregator == "true")
        {
          input_node = op_node.append_child("Input");
          sprintf(tmp_buf, "Normalization Factor %d", (int)(i+1));
          addChild(input_node, "ArgumentName", tmp_buf);
          if(m_InputData.objectives[i].code_name == "plato_analyze")
            sprintf(tmp_buf, "Initial Objective Value %d", (int)(i+1));
          else
            sprintf(tmp_buf, "Initial Internal Energy %d", (int)(i+1));
          addChild(input_node, "SharedDataName", tmp_buf);
        }
      }
    }


    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Value");
    if(m_InputData.mPlatoAnalyzePerformerExists)
      addChild(output_node, "SharedDataName", "Objective Value");
    else
      addChild(output_node, "SharedDataName", "Internal Energy");

    // If there are uncertainties add an operation for
    // the objective mean and std deviation.
    if(aHasUncertainties)
    {
      op_node = stage_node.append_child("Operation");
      addChild(op_node, "Name", "Stochastic Objective Value");
      addChild(op_node, "PerformerName", "PlatoMain");

      if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
      {
        pugi::xml_node for_node = op_node.append_child("For");
        for_node.append_attribute("var") = "performerIndex";
        for_node.append_attribute("in") = "Performers";
        for_node = for_node.append_child("For");
        for_node.append_attribute("var") = "PerformerSampleIndex";
        for_node.append_attribute("in") = "PerformerSamples";
        input_node = for_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
        addChild(input_node, "SharedDataName", "Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
      }
      else
      {
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        {
          XMLGen::Objective cur_obj = m_InputData.objectives[i];
          input_node = op_node.append_child("Input");
          std::string tTmpString = "";
          if(cur_obj.code_name == "plato_analyze")
            tTmpString += "Objective Value ";
          else
            tTmpString += "Internal Energy ";
          tTmpString += Plato::to_string(i+1);
          addChild(input_node, "ArgumentName", tTmpString);
          addChild(input_node, "SharedDataName", tTmpString);
        }
      }

      output_node = op_node.append_child("Output");
      std::string tFieldName = "Objective Mean Plus ";
      tFieldName += m_InputData.objective_number_standard_deviations;
      tFieldName += " StdDev";
      addChild(output_node, "ArgumentName", tFieldName);
      addChild(output_node, "SharedDataName", "Objective Mean Plus StdDev Value");
    }

    output_node = stage_node.append_child("Output");
    if(m_InputData.mPlatoAnalyzePerformerExists)
      addChild(output_node, "SharedDataName", "Objective Value");
    else
      addChild(output_node, "SharedDataName", "Internal Energy");

    return true;
}

/******************************************************************************/
void XMLGenerator::addComputeObjectiveValueOperationForNewUncertaintyWorkflow(pugi::xml_node &aNode)
/******************************************************************************/
{
  pugi::xml_node for_node = aNode.append_child("For");
  for_node.append_attribute("var") = "PerformerSampleIndex";
  for_node.append_attribute("in") = "PerformerSamples";
  pugi::xml_node op_node = for_node.append_child("Operation");
  for_node = op_node.append_child("For");
  for_node.append_attribute("var") = "performerIndex";
  for_node.append_attribute("in") = "Performers";
  op_node = for_node.append_child("Operation");
  addChild(op_node, "PerformerName", "plato_analyze_{performerIndex}");
  addChild(op_node, "Name", "Compute Objective Value");

  for(size_t tRandomLoadIndex = 0; tRandomLoadIndex < m_UncertaintyMetaData.randomVariableIndices.size(); ++tRandomLoadIndex)
  {
    pugi::xml_node tmp_node1 = op_node.append_child("Parameter");
    std::string tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "X";
    addChild(tmp_node1, "ArgumentName", tLoadName);
    std::string tValueName = "{" + tLoadName + "[{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}]}";
    addChild(tmp_node1, "ArgumentValue", tValueName);

    tmp_node1 = op_node.append_child("Parameter");
    tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "Y";
    addChild(tmp_node1, "ArgumentName", tLoadName);
    tValueName = "{" + tLoadName + "[{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}]}";
    addChild(tmp_node1, "ArgumentValue", tValueName);

    tmp_node1 = op_node.append_child("Parameter");
    tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "Z";
    addChild(tmp_node1, "ArgumentName", tLoadName);
    tValueName = "{" + tLoadName + "[{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}]}";
    addChild(tmp_node1, "ArgumentValue", tValueName);
  }

  pugi::xml_node input_node = op_node.append_child("Input");
  addChild(input_node, "ArgumentName", "Topology");
  addChild(input_node, "SharedDataName", "Topology");

  pugi::xml_node output_node = op_node.append_child("Output");
  addChild(output_node, "ArgumentName", "Objective Value");
  addChild(output_node, "SharedDataName", "Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
}

/******************************************************************************/
void XMLGenerator::addComputeObjectiveGradientOperationForNewUncertaintyWorkflow(pugi::xml_node &aNode)
/******************************************************************************/
{
  pugi::xml_node for_node = aNode.append_child("For");
  for_node.append_attribute("var") = "PerformerSampleIndex";
  for_node.append_attribute("in") = "PerformerSamples";
  pugi::xml_node op_node = for_node.append_child("Operation");
  for_node = op_node.append_child("For");
  for_node.append_attribute("var") = "performerIndex";
  for_node.append_attribute("in") = "Performers";
  op_node = for_node.append_child("Operation");
  addChild(op_node, "PerformerName", "plato_analyze_{performerIndex}");
  addChild(op_node, "Name", "Compute Objective Gradient");

  for(size_t tRandomLoadIndex = 0; tRandomLoadIndex < m_UncertaintyMetaData.randomVariableIndices.size(); ++tRandomLoadIndex)
  {
    pugi::xml_node tmp_node1 = op_node.append_child("Parameter");
    std::string tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "X";
    addChild(tmp_node1, "ArgumentName", tLoadName);
    std::string tValueName = "{" + tLoadName + "[{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}]}";
    addChild(tmp_node1, "ArgumentValue", tValueName);

    tmp_node1 = op_node.append_child("Parameter");
    tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "Y";
    addChild(tmp_node1, "ArgumentName", tLoadName);
    tValueName = "{" + tLoadName + "[{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}]}";
    addChild(tmp_node1, "ArgumentValue", tValueName);

    tmp_node1 = op_node.append_child("Parameter");
    tLoadName = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "Z";
    addChild(tmp_node1, "ArgumentName", tLoadName);
    tValueName = "{" + tLoadName + "[{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}]}";
    addChild(tmp_node1, "ArgumentValue", tValueName);
  }

  pugi::xml_node input_node = op_node.append_child("Input");
  addChild(input_node, "ArgumentName", "Topology");
  addChild(input_node, "SharedDataName", "Topology");

  pugi::xml_node output_node = op_node.append_child("Output");
  addChild(output_node, "ArgumentName", "Objective Gradient");
  addChild(output_node, "SharedDataName", "Objective {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex} Gradient");
}

/******************************************************************************/
bool XMLGenerator::outputObjectiveStage(pugi::xml_document &doc)
/******************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Objective");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Design Parameters");

    // Update Geometry on Change operation
    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Update Geometry on Change");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Parameters");
    addChild(input_node, "SharedDataName", "Design Parameters");

    pugi::xml_node cur_parent = stage_node;
    if(m_InputData.objectives.size() > 1)
    {
        op_node = stage_node.append_child("Operation");
        cur_parent = op_node;
    }

    // Reinitialize on Change operation
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        op_node = cur_parent.append_child("Operation");
        addChild(op_node, "Name", "Reinitialize on Change");
        addChild(op_node, "PerformerName", m_InputData.objectives[i].performer_name);
        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Parameters");
        addChild(input_node, "SharedDataName", "Design Parameters");
    }

    cur_parent = stage_node;
    if(m_InputData.objectives.size() > 1)
    {
        op_node = stage_node.append_child("Operation");
        cur_parent = op_node;
    }

    char tmp_buf[200];
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        XMLGen::Objective cur_obj = m_InputData.objectives[i];
        op_node = cur_parent.append_child("Operation");
        addChild(op_node, "Name", "Compute Objective Value");
        addChild(op_node, "PerformerName", cur_obj.performer_name.c_str());


        pugi::xml_node output_node = op_node.append_child("Output");
        addChild(output_node, "ArgumentName", "Objective Value");
        sprintf(tmp_buf, "Objective %d", (int)(i+1));
        addChild(output_node, "SharedDataName", tmp_buf);
    }

    // Aggregate
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "AggregateValues");
    addChild(op_node, "PerformerName", "PlatoMain");

    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        input_node = op_node.append_child("Input");
        sprintf(tmp_buf, "Value %d", (int)(i+1));
        addChild(input_node, "ArgumentName", tmp_buf);
        sprintf(tmp_buf, "Objective %d", (int)(i+1));
        addChild(input_node, "SharedDataName", tmp_buf);
    }

    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Values");
    addChild(output_node, "SharedDataName", "Objective");

    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Objective");

    return true;
}

/******************************************************************************/
bool XMLGenerator::outputObjectiveGradientStage(pugi::xml_document &doc)
/******************************************************************************/
{
    // Internal Energy
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Objective Gradient");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Design Parameters");

    // Update Geometry on Change operation
    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Update Geometry on Change");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Parameters");
    addChild(input_node, "SharedDataName", "Design Parameters");

    pugi::xml_node cur_parent = stage_node;
    if(m_InputData.objectives.size() > 1)
    {
        op_node = stage_node.append_child("Operation");
        cur_parent = op_node;
    }

    // Reinitialize on Change operation
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        op_node = cur_parent.append_child("Operation");
        addChild(op_node, "Name", "Reinitialize on Change");
        addChild(op_node, "PerformerName", m_InputData.objectives[i].performer_name);
        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Parameters");
        addChild(input_node, "SharedDataName", "Design Parameters");
    }

    op_node = stage_node.append_child("Operation");

    pugi::xml_node op_node2;
    cur_parent = op_node;
    if(m_InputData.objectives.size() > 1)
    {
        op_node2 = op_node.append_child("Operation");
        cur_parent = op_node2;
    }

    char tmp_buf[200];
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        XMLGen::Objective cur_obj = m_InputData.objectives[i];
        op_node2 = cur_parent.append_child("Operation");
        addChild(op_node2, "Name", "Compute Objective Gradient");
        addChild(op_node2, "PerformerName", cur_obj.performer_name.c_str());
    }

    pugi::xml_node tForNode = op_node.append_child("For");
    tForNode.append_attribute("var") = "I";
    tForNode.append_attribute("in") = "Parameters";
    op_node2 = tForNode.append_child("Operation");
    addChild(op_node2, "Name", "Compute Parameter Sensitivity On Change");
    addChild(op_node2, "PerformerName", "PlatoESP_{I}");
    pugi::xml_node tmp_node = op_node2.append_child("Parameter");
    addChild(tmp_node, "ArgumentName", "Parameter Index");
    addChild(tmp_node, "ArgumentValue", "{I-1}");
    tmp_node = op_node2.append_child("Input");
    addChild(tmp_node, "ArgumentName", "Parameters");
    addChild(tmp_node, "SharedDataName", "Design Parameters");
    tmp_node = op_node2.append_child("Output");
    addChild(tmp_node, "ArgumentName", "Parameter Sensitivity");
    addChild(tmp_node, "SharedDataName", "Parameter Sensitivity {I}");

    cur_parent = stage_node;
    if(m_InputData.objectives.size() > 1)
    {
        op_node2 = stage_node.append_child("Operation");
        cur_parent = op_node2;
    }
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        op_node2 = cur_parent.append_child("Operation");
        addChild(op_node2, "Name", "Compute Objective Sensitivity");
        addChild(op_node2, "PerformerName", m_InputData.objectives[i].performer_name);
        tForNode = op_node2.append_child("For");
        tForNode.append_attribute("var") = "I";
        tForNode.append_attribute("in") = "Parameters";
        tmp_node = tForNode.append_child("Input");
        addChild(tmp_node, "ArgumentName", "Parameter Sensitivity {I}");
        addChild(tmp_node, "SharedDataName", "Parameter Sensitivity {I}");
        tmp_node = op_node2.append_child("Output");
        addChild(tmp_node, "ArgumentName", "Objective Sensitivity");
        sprintf(tmp_buf, "Objective %d Gradient", int(i+1));
        addChild(tmp_node, "SharedDataName", tmp_buf);
    }

    // Aggregate
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "AggregateValues");
    addChild(op_node, "PerformerName", "PlatoMain");

    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        input_node = op_node.append_child("Input");
        sprintf(tmp_buf, "Value %d", (int)(i+1));
        addChild(input_node, "ArgumentName", tmp_buf);
        sprintf(tmp_buf, "Objective %d Gradient", (int)(i+1));
        addChild(input_node, "SharedDataName", tmp_buf);
    }

    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Values");
    addChild(output_node, "SharedDataName", "Objective Gradient");

    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Objective Gradient");

    return true;
}

/******************************************************************************/
bool XMLGenerator::outputInternalEnergyGradientStage(pugi::xml_document &doc,
                                                     bool &aHasUncertainties)
/******************************************************************************/
{
    char tmp_buf[200];
    // Internal Energy
    pugi::xml_node stage_node = doc.append_child("Stage");
    pugi::xml_node cur_parent = stage_node;
    if(m_InputData.mPlatoAnalyzePerformerExists)
      addChild(stage_node, "Name", "Objective Gradient");
    else
      addChild(stage_node, "Name", "Internal Energy Gradient");

    // fitler control
    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterControl");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Field");
    addChild(output_node, "SharedDataName", "Topology");

    // Enforce Bounds operation
    if(!m_InputData.mPlatoAnalyzePerformerExists)
        addEnforceBoundsOperationToStage(stage_node);

    
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      addComputeObjectiveGradientOperationForNewUncertaintyWorkflow(stage_node);
    }
    else
    {
      if(m_InputData.objectives.size() > 1)
      {
        op_node = stage_node.append_child("Operation");
        cur_parent = op_node;
      }

      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        XMLGen::Objective cur_obj = m_InputData.objectives[i];
        op_node = cur_parent.append_child("Operation");
        if(cur_obj.code_name == "plato_analyze")
        {
          addChild(op_node, "Name", "Compute Objective Gradient");
          addChild(op_node, "PerformerName", cur_obj.performer_name.c_str());
        }
        else
        {
          addChild(op_node, "Name", "Compute Gradient");
          addChild(op_node, "PerformerName", cur_obj.performer_name.c_str());
        }

        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Topology");
        addChild(input_node, "SharedDataName", "Topology");

        output_node = op_node.append_child("Output");
        if(cur_obj.code_name == "plato_analyze")
          addChild(output_node, "ArgumentName", "Objective Gradient");
        else
          addChild(output_node, "ArgumentName", "Internal Energy Gradient");
        if(cur_obj.code_name == "plato_analyze")
          sprintf(tmp_buf, "Objective %d Gradient", (int)(i+1));
        else
          sprintf(tmp_buf, "Internal Energy %d Gradient", (int)(i+1));
        addChild(output_node, "SharedDataName", tmp_buf);
      }
    }
    

    // Aggregate
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "AggregateGradient");
    addChild(op_node, "PerformerName", "PlatoMain");

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = op_node.append_child("For");
      for_node.append_attribute("var") = "performerIndex";
      for_node.append_attribute("in") = "Performers";
      for_node = for_node.append_child("For");
      for_node.append_attribute("var") = "PerformerSampleIndex";
      for_node.append_attribute("in") = "PerformerSamples";

      input_node = for_node.append_child("Input");
      addChild(input_node, "ArgumentName", "Field {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
      addChild(input_node, "SharedDataName", "Objective {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex} Gradient");
      
      if(m_InputData.mUseNormalizationInAggregator == "true")
      {
          input_node = for_node.append_child("Input");
          sprintf(tmp_buf, "Normalization Factor {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
          addChild(input_node, "ArgumentName", "Normalization Factor {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
          addChild(input_node, "SharedDataName", "Initial Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
      }
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        input_node = op_node.append_child("Input");
        sprintf(tmp_buf, "Field %d", (int)(i+1));
        addChild(input_node, "ArgumentName", tmp_buf);
        if(m_InputData.objectives[i].code_name == "plato_analyze")
          sprintf(tmp_buf, "Objective %d Gradient", (int)(i+1));
        else
          sprintf(tmp_buf, "Internal Energy %d Gradient", (int)(i+1));
        addChild(input_node, "SharedDataName", tmp_buf);
        if(m_InputData.mUseNormalizationInAggregator == "true")
        {
          input_node = op_node.append_child("Input");
          sprintf(tmp_buf, "Normalization Factor %d", (int)(i+1));
          addChild(input_node, "ArgumentName", tmp_buf);
          if(m_InputData.objectives[i].code_name == "plato_analyze")
            sprintf(tmp_buf, "Initial Objective Value %d", (int)(i+1));
          else
            sprintf(tmp_buf, "Initial Internal Energy %d", (int)(i+1));
          addChild(input_node, "SharedDataName", tmp_buf);
        }
      }
    }

    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Field");
    if(m_InputData.mPlatoAnalyzePerformerExists)
      addChild(output_node, "SharedDataName", "Objective Gradient");
    else
      addChild(output_node, "SharedDataName", "Internal Energy Gradient");

    // filter gradient
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterGradient");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Gradient");
    if(m_InputData.mPlatoAnalyzePerformerExists)
      addChild(input_node, "SharedDataName", "Objective Gradient");
    else
      addChild(input_node, "SharedDataName", "Internal Energy Gradient");
    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Gradient");
    if(m_InputData.mPlatoAnalyzePerformerExists)
      addChild(output_node, "SharedDataName", "Objective Gradient");
    else
      addChild(output_node, "SharedDataName", "Internal Energy Gradient");

    if(aHasUncertainties)
    {
        op_node = stage_node.append_child("Operation");
        addChild(op_node, "Name", "Stochastic Objective Gradient");
        addChild(op_node, "PerformerName", "PlatoMain");

        if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
        {
          pugi::xml_node for_node = op_node.append_child("For");
          for_node.append_attribute("var") = "performerIndex";
          for_node.append_attribute("in") = "Performers";
          for_node = for_node.append_child("For");
          for_node.append_attribute("var") = "PerformerSampleIndex";
          for_node.append_attribute("in") = "PerformerSamples";

          input_node = for_node.append_child("Input");
          addChild(input_node, "ArgumentName", "Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
          addChild(input_node, "SharedDataName", "Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
          input_node = for_node.append_child("Input");
          addChild(input_node, "ArgumentName", "Objective {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex} Gradient");
          addChild(input_node, "SharedDataName", "Objective {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex} Gradient");
        }
        else
        {
          for(size_t i=0; i<m_InputData.objectives.size(); ++i)
          {
            XMLGen::Objective cur_obj = m_InputData.objectives[i];
            input_node = op_node.append_child("Input");
            std::string tTmpString = "";
            if(cur_obj.code_name == "plato_analyze")
              tTmpString += "Objective Value ";
            else
              tTmpString += "Internal Energy ";
            tTmpString += Plato::to_string(i+1);
            addChild(input_node, "ArgumentName", tTmpString);
            addChild(input_node, "SharedDataName", tTmpString);

            input_node = op_node.append_child("Input");
            tTmpString = "";
            if(cur_obj.code_name == "plato_analyze")
              tTmpString += "Objective ";
            else
              tTmpString += "Internal Energy ";
            tTmpString += Plato::to_string(i+1);
            tTmpString += " Gradient";
            addChild(input_node, "ArgumentName", tTmpString);
            addChild(input_node, "SharedDataName", tTmpString);
          }
        }

        output_node = op_node.append_child("Output");
        std::string tTmpString = "Objective Mean Plus ";
        tTmpString += m_InputData.objective_number_standard_deviations;
        tTmpString += " StdDev Gradient";
        addChild(output_node, "ArgumentName", tTmpString);
        addChild(output_node, "SharedDataName", "Objective Mean Plus StdDev Gradient");
    }

    // stage output
    output_node = stage_node.append_child("Output");
    if(m_InputData.mPlatoAnalyzePerformerExists)
      addChild(output_node, "SharedDataName", "Objective Gradient");
    else
      addChild(output_node, "SharedDataName", "Internal Energy Gradient");

    return true;
}

/******************************************************************************/
bool XMLGenerator::outputInternalEnergyHessianStage(pugi::xml_document &doc)
/******************************************************************************/
{
    char tmp_buf[200];
    // Internal Energy
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Internal Energy Hessian");
    
    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Descent Direction");
    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterControl");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Optimization DOFs");
    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Field");
    addChild(output_node, "SharedDataName", "Topology");
    
    // Enforce Bounds operation
    if(!m_InputData.mPlatoAnalyzePerformerExists)
        addEnforceBoundsOperationToStage(stage_node);

    pugi::xml_node cur_parent = stage_node;
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
        pugi::xml_node for_node = stage_node.append_child("For");
        for_node.append_attribute("var") = "performerIndex";
        for_node.append_attribute("in") = "Performers";
        for_node = for_node.append_child("For");
        for_node.append_attribute("var") = "PerformerSampleIndex";
        for_node.append_attribute("in") = "PerformerSamples";

        op_node = for_node.append_child("Operation");
        addChild(op_node, "Name", "Compute HessianTimesVector");
        addChild(op_node, "PerformerName", "plato_analyze_{performerIndex}");

        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Topology");
        addChild(input_node, "SharedDataName", "Topology");
        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Descent Direction");
        addChild(input_node, "SharedDataName", "Descent Direction");

        output_node = op_node.append_child("Output");
        addChild(output_node, "ArgumentName", "HessianTimesVector");
        addChild(output_node, "SharedDataName", "HessianTimesVector {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
    }
    else
    {
      if(m_InputData.objectives.size() > 1)
      {
        op_node = stage_node.append_child("Operation");
        cur_parent = op_node;
      }

      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        op_node = cur_parent.append_child("Operation");
        addChild(op_node, "Name", "Compute HessianTimesVector");
        addChild(op_node, "PerformerName", m_InputData.objectives[i].performer_name.c_str());

        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Topology");
        addChild(input_node, "SharedDataName", "Topology");
        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Descent Direction");
        addChild(input_node, "SharedDataName", "Descent Direction");

        output_node = op_node.append_child("Output");
        addChild(output_node, "ArgumentName", "HessianTimesVector");
        sprintf(tmp_buf, "HessianTimesVector %d", (int)(i+1));
        addChild(output_node, "SharedDataName", tmp_buf);
      }

    }
    
    // Aggregate
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "AggregateHessian");
    addChild(op_node, "PerformerName", "PlatoMain");
    
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
        pugi::xml_node for_node = op_node.append_child("For");
        for_node.append_attribute("var") = "performerIndex";
        for_node.append_attribute("in") = "Performers";
        for_node = for_node.append_child("For");
        for_node.append_attribute("var") = "PerformerSampleIndex";
        for_node.append_attribute("in") = "PerformerSamples";

        input_node = for_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Field {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
        addChild(input_node, "SharedDataName", "HessianTimesVector {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        input_node = op_node.append_child("Input");
        sprintf(tmp_buf, "Field %d", (int)(i+1));
        addChild(input_node, "ArgumentName", tmp_buf);
        sprintf(tmp_buf, "HessianTimesVector %d", (int)(i+1));
        addChild(input_node, "SharedDataName", tmp_buf);
      }
    }
    
    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Field");
    addChild(output_node, "SharedDataName", "Internal Energy Hessian");
    
    // filter Hessian
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "FilterHessian");
    addChild(op_node, "PerformerName", "PlatoMain");
    input_node = op_node.append_child("Input");
    addChild(input_node, "ArgumentName", "Field");
    addChild(input_node, "SharedDataName", "Internal Energy Hessian");
    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Filtered Field");
    addChild(output_node, "SharedDataName", "Internal Energy Hessian");
    
    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Internal Energy Hessian");
    
    return true;
}

/******************************************************************************/
bool XMLGenerator::outputObjectiveHessianStage(pugi::xml_document &doc)
/******************************************************************************/
{
    char tmp_buf[200];
    // Internal Energy
    pugi::xml_node op_node, output_node;
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Objective Hessian");

    pugi::xml_node input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Design Parameters");
    input_node = stage_node.append_child("Input");
    addChild(input_node, "SharedDataName", "Descent Direction");

    pugi::xml_node cur_parent = stage_node;
    if(m_InputData.objectives.size() > 1)
    {
        op_node = stage_node.append_child("Operation");
        cur_parent = op_node;
    }

    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        op_node = cur_parent.append_child("Operation");
        addChild(op_node, "Name", "Compute HessianTimesVector");
        addChild(op_node, "PerformerName", m_InputData.objectives[i].performer_name.c_str());

        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Design Parameters");
        addChild(input_node, "SharedDataName", "Design Parameters");
        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Descent Direction");
        addChild(input_node, "SharedDataName", "Descent Direction");

        output_node = op_node.append_child("Output");
        addChild(output_node, "ArgumentName", "HessianTimesVector");
        sprintf(tmp_buf, "HessianTimesVector %d", (int)(i+1));
        addChild(output_node, "SharedDataName", tmp_buf);
    }

    // Aggregate
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "AggregateHessian");
    addChild(op_node, "PerformerName", "PlatoMain");

    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        input_node = op_node.append_child("Input");
        sprintf(tmp_buf, "Value %d", (int)(i+1));
        addChild(input_node, "ArgumentName", tmp_buf);
        sprintf(tmp_buf, "HessianTimesVector %d", (int)(i+1));
        addChild(input_node, "SharedDataName", tmp_buf);
    }

    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Value");
    addChild(output_node, "SharedDataName", "Objective Hessian");


    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Objective Hessian");

    return true;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoAnalyzeShapeDefinesXML()
/******************************************************************************/
{
    pugi::xml_document doc;

    pugi::xml_node tTmpNode = doc.append_child("Define");
    tTmpNode.append_attribute("name") = "NumParameters";
    std::string tString = Plato::to_string(m_InputData.num_shape_design_variables);
    tTmpNode.append_attribute("value") = tString.c_str();

    tTmpNode = doc.append_child("Array");
    tTmpNode.append_attribute("name") = "Parameters";
    tTmpNode.append_attribute("type") = "int";
    tTmpNode.append_attribute("from") = "1";
    tTmpNode.append_attribute("to") = "{NumParameters}";

    doc.save_file("defines.xml", "  ");

    return true;
}

/******************************************************************************/
bool XMLGenerator::generateDefinesXML(std::ostringstream *aStringStream)
/******************************************************************************/
{
  if(m_InputData.mPlatoAnalyzePerformerExists && m_InputData.uncertainties.size() > 0)
  {
    pugi::xml_document doc;

    if(!addDefinesToDoc(doc))
      return false;
    
    if(aStringStream)
    {
      std::cout << "stringstream" << std::endl;
      doc.save(*aStringStream, "\t", pugi::format_default & ~pugi::format_indent);
    }
    else
      doc.save_file("defines.xml", "  ");
  }

  return true;
}

/******************************************************************************/
void XMLGenerator::addVersionEntryToDoc(pugi::xml_document& doc)
/******************************************************************************/
{
    // Version entry
    pugi::xml_node tTmpNode = doc.append_child(pugi::node_declaration);
    tTmpNode.set_name("xml");
    pugi::xml_attribute tmp_att = tTmpNode.append_attribute("version");
    tmp_att.set_value("1.0");
}

/******************************************************************************/
bool XMLGenerator::addDefinesToDoc(pugi::xml_document& doc)
/******************************************************************************/
{
  pugi::xml_node tTmpNode = doc.append_child("Define");
  tTmpNode.append_attribute("name") = "NumSamples";
  tTmpNode.append_attribute("type") = "int";
  std::string tNumSamplesString = Plato::to_string(m_UncertaintyMetaData.numSamples);
  tTmpNode.append_attribute("value") = tNumSamplesString.c_str();

  tTmpNode = doc.append_child("Define");
  tTmpNode.append_attribute("name") = "NumPerformers";
  tTmpNode.append_attribute("type") = "int";
  size_t tNumPerformers = m_UncertaintyMetaData.numPeformers;
  size_t tNumSamples = stringToSizeT(tNumSamplesString);

  if(tNumPerformers == 0)
  {
    std::cout << "ERROR: Cannot assign zero MPI processes" << std::endl;
    return false;
  }

  tNumPerformers = getGreatestDivisor(tNumSamples,tNumPerformers);

  std::string tNumPerformersString = Plato::to_string(tNumPerformers);
  tTmpNode.append_attribute("value") = tNumPerformersString.c_str();

  tTmpNode = doc.append_child("Define");
  tTmpNode.append_attribute("name") = "NumSamplesPerPerformer";
  tTmpNode.append_attribute("type") = "int";
  tTmpNode.append_attribute("value") = "{NumSamples/NumPerformers}";

  tTmpNode = doc.append_child("Array");
  tTmpNode.append_attribute("name") = "Samples";
  tTmpNode.append_attribute("type") = "int";
  tTmpNode.append_attribute("from") = "0";
  tTmpNode.append_attribute("to") = "{NumSamples-1}";

  tTmpNode = doc.append_child("Array");
  tTmpNode.append_attribute("name") = "Performers";
  tTmpNode.append_attribute("type") = "int";
  tTmpNode.append_attribute("from") = "0";
  tTmpNode.append_attribute("to") = "{NumPerformers-1}";

  tTmpNode = doc.append_child("Array");
  tTmpNode.append_attribute("name") = "PerformerSamples";
  tTmpNode.append_attribute("type") = "int";
  tTmpNode.append_attribute("from") = "0";
  tTmpNode.append_attribute("to") = "{NumSamplesPerPerformer-1}";

  std::vector<XMLGen::LoadCase> tLoadCases = m_InputData.load_cases;

  std::vector<std::string> tProbabilitiesForLoadCaseIndex;

  for(size_t tLoadCaseIndex = 0; tLoadCaseIndex < tLoadCases.size(); ++tLoadCaseIndex)
  {
    XMLGen::LoadCase load_case = tLoadCases[tLoadCaseIndex];
    char buf[200];
    sprintf(buf, "%.17e", m_InputData.load_case_probabilities[tLoadCaseIndex]);
    tProbabilitiesForLoadCaseIndex.push_back(buf);
  }

  std::string tProbabilitiesString = makeValuesString(tProbabilitiesForLoadCaseIndex);

  for(size_t tRandomLoadIndex = 0; tRandomLoadIndex < m_UncertaintyMetaData.randomVariableIndices.size(); ++tRandomLoadIndex)
  {
    std::vector<std::string> tXValuesForRandomLoadIndex;
    std::vector<std::string> tYValuesForRandomLoadIndex;
    std::vector<std::string> tZValuesForRandomLoadIndex;

    for(size_t tLoadCaseIndex = 0; tLoadCaseIndex < tLoadCases.size(); ++tLoadCaseIndex)
    {
      XMLGen::LoadCase load_case = tLoadCases[tLoadCaseIndex];
      if(load_case.loads.size() != m_UncertaintyMetaData.numVariables)
      {
        std::cerr << "Error: Load case size does not match number of loads in uncertainty metadata" << std::endl;
        return false;
      }

      XMLGen::Load tRandomLoad = load_case.loads[tRandomLoadIndex];
      tXValuesForRandomLoadIndex.push_back(tRandomLoad.values[0]);
      tYValuesForRandomLoadIndex.push_back(tRandomLoad.values[1]);
      tZValuesForRandomLoadIndex.push_back(tRandomLoad.values[2]);
      tProbabilitiesForLoadCaseIndex.push_back(Plato::to_string(m_InputData.load_case_probabilities[tLoadCaseIndex]));

    }

    std::string tXValuesString = makeValuesString(tXValuesForRandomLoadIndex);
    std::string tYValuesString = makeValuesString(tYValuesForRandomLoadIndex);
    std::string tZValuesString = makeValuesString(tZValuesForRandomLoadIndex);

    tTmpNode = doc.append_child("Array");
    std::string tRandomLoadString = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "X";
    tTmpNode.append_attribute("name") = tRandomLoadString.c_str();
    tTmpNode.append_attribute("type") = "real";
    tTmpNode.append_attribute("values") = tXValuesString.c_str();

    tTmpNode = doc.append_child("Array");
    tRandomLoadString = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "Y";
    tTmpNode.append_attribute("name") = tRandomLoadString.c_str();
    tTmpNode.append_attribute("type") = "real";
    tTmpNode.append_attribute("values") = tYValuesString.c_str();

    tTmpNode = doc.append_child("Array");
    tRandomLoadString = "RandomLoad" + Plato::to_string(tRandomLoadIndex) + "Z";
    tTmpNode.append_attribute("name") = tRandomLoadString.c_str();
    tTmpNode.append_attribute("type") = "real";
    tTmpNode.append_attribute("values") = tZValuesString.c_str();

  }

  tTmpNode = doc.append_child("Array");
  tTmpNode.append_attribute("name") = "Probabilities";
  tTmpNode.append_attribute("type") = "real";
  tTmpNode.append_attribute("values") = tProbabilitiesString.c_str();

  return true;
}

/******************************************************************************/
std::string XMLGenerator::makeValuesString(const std::vector<std::string>& aValues)
/******************************************************************************/
{
  std::string tValuesString;
  for(size_t i = 0; i < aValues.size(); ++i)
  {
    tValuesString += aValues[i];
    if(i < aValues.size()-1)
      tValuesString += ", ";
  }
  return tValuesString;
}

/******************************************************************************/
size_t XMLGenerator::getGreatestDivisor(const size_t& aDividend, size_t aDivisor)
/******************************************************************************/
{
  if(aDivisor == 0)
    throw std::invalid_argument( "Error: divide by zero" );
  while(aDividend % aDivisor != 0)
  {
    --aDivisor;
  }
  return aDivisor;
}

/******************************************************************************/
size_t XMLGenerator::stringToSizeT(const std::string& aString)
/******************************************************************************/
{
  std::stringstream sstream(aString);
  size_t result;
  sstream >> result;
  return result;
}

/******************************************************************************/
bool XMLGenerator::generateInterfaceXML()
/******************************************************************************/
{
    pugi::xml_document doc;

/*
    InterfaceXMLWriter tInterfaceXMLWriter;
    tInterfaceXMLWriter.generateInterfaceXML(doc, m_InputData, m_UseNewPlatoAnalyzeUncertaintyWorkflow,
                                             m_HasUncertainties, m_RequestedVonMisesOutput);
*/

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node def_node = doc.append_child("include");
      def_node.append_attribute("filename") = "defines.xml";
    }

    // // Version entry
    pugi::xml_node tTmpNode;
    // pugi::xml_node tTmpNode = doc.append_child(pugi::node_declaration);
    // tTmpNode.set_name("xml");
    // pugi::xml_attribute tmp_att = tTmpNode.append_attribute("version");
    // tmp_att.set_value("1.0");
    

    // Console output control
    tTmpNode = doc.append_child("Console");
    addChild(tTmpNode, "Verbose", "true");

    //////////////////////////////////////////////////
    // Performers
    /////////////////////////////////////////////////

    // PlatoMain performer entry.
    pugi::xml_node performer_node = doc.append_child("Performer");
    addChild(performer_node, "Name", "PlatoMain");
    addChild(performer_node, "Code", "PlatoMain");
    addChild(performer_node, "PerformerID", "0");

    pugi::xml_node sd_node;
    char tmp_buf[200];

    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
        performer_node = doc.append_child("Performer");
        pugi::xml_node for_node = performer_node.append_child("For");
        for_node.append_attribute("var") = "performerIndex";
        for_node.append_attribute("in") = "Performers";
        addChild(for_node, "Name", "plato_analyze_{performerIndex}");
        addChild(for_node, "Code", "plato_analyze");
        addChild(performer_node, "PerformerID", "1");
    }
    else
    {
      // Other performers (one for each objective)
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        performer_node = doc.append_child("Performer");
        addChild(performer_node, "Name", m_InputData.objectives[i].performer_name);
        addChild(performer_node, "Code", m_InputData.objectives[i].code_name);
        sprintf(tmp_buf, "%d", (int)(i+1));
        addChild(performer_node, "PerformerID", tmp_buf);
      }
    }


    //////////////////////////////////////////////////
    // Shared Data
    /////////////////////////////////////////////////

    // Internal Energy XXX shared data
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = doc.append_child("For");
      for_node.append_attribute("var") = "performerIndex";
      for_node.append_attribute("in") = "Performers";
      for_node = for_node.append_child("For");
      for_node.append_attribute("var") = "PerformerSampleIndex";
      for_node.append_attribute("in") = "PerformerSamples";
      std::string tSharedDataName = "Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}";
      createSingleUserGlobalSharedData(for_node, tSharedDataName.c_str(), "Scalar", "1", "plato_analyze_{performerIndex}", "PlatoMain");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        if(m_InputData.optimization_type == "topology")
        {
          // create shared data for objectives
          if(m_InputData.objectives[i].code_name == "plato_analyze")
            sprintf(tmp_buf, "Objective Value %d", (int)(i+1));
          else
            sprintf(tmp_buf, "Internal Energy %d", (int)(i+1));
          createSingleUserGlobalSharedData(doc, tmp_buf, "Scalar", "1", m_InputData.objectives[i].performer_name, "PlatoMain");
        }
        else if(m_InputData.optimization_type == "shape")
        {
          // create shared data for objectives
          sprintf(tmp_buf, "Objective %d", (int)(i+1));
          createSingleUserGlobalSharedData(doc, tmp_buf, "Scalar", "1", m_InputData.objectives[i].performer_name, "PlatoMain");
        }

        if(m_InputData.mUseNormalizationInAggregator == "true")
        {
          if(m_InputData.optimization_type == "topology")
          {
            if(m_InputData.objectives[i].code_name == "plato_analyze")
              sprintf(tmp_buf, "Initial Objective Value %d", (int)(i+1));
            else
              sprintf(tmp_buf, "Initial Internal Energy %d", (int)(i+1));
            createSingleUserGlobalSharedData(doc, tmp_buf, "Scalar", "1", m_InputData.objectives[i].performer_name, "PlatoMain");
          }
        }
      }
    }
    // Internal Energy XXX Gradient shared data
    if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
    {
      pugi::xml_node for_node = doc.append_child("For");
      for_node.append_attribute("var") = "performerIndex";
      for_node.append_attribute("in") = "Performers";
      for_node = for_node.append_child("For");
      for_node.append_attribute("var") = "PerformerSampleIndex";
      for_node.append_attribute("in") = "PerformerSamples";
      std::string tSharedDataName = "Objective {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex} Gradient";
      createSingleUserNodalSharedData(for_node, tSharedDataName.c_str(), "Scalar", "plato_analyze_{performerIndex}", "PlatoMain");
    }
    else
    {
      for(size_t i=0; i<m_InputData.objectives.size(); ++i)
      {
        if(m_InputData.optimization_type == "topology")
        {
          if(m_InputData.objectives[i].code_name == "plato_analyze")
            sprintf(tmp_buf, "Objective %d Gradient", (int)(i+1));
          else
            sprintf(tmp_buf, "Internal Energy %d Gradient", (int)(i+1));
          createSingleUserNodalSharedData(doc, tmp_buf, "Scalar", m_InputData.objectives[i].performer_name, "PlatoMain");
        }
        else if(m_InputData.optimization_type == "shape" && m_InputData.num_shape_design_variables > 0)
        {
          char tTempBuffer[100];
          sprintf(tTempBuffer, "%d", m_InputData.num_shape_design_variables);
          sprintf(tmp_buf, "Objective %d Gradient", (int)(i+1));
          createSingleUserGlobalSharedData(doc, tmp_buf, "Scalar", tTempBuffer, m_InputData.objectives[i].performer_name, "PlatoMain");
        }
      }
    }

    if(m_InputData.optimization_type == "topology")
    {
      if(m_HasUncertainties)
      {
        // Objective statistics
        createSingleUserGlobalSharedData(doc, "Objective Mean Plus StdDev Value", "Scalar", "1", "PlatoMain", "PlatoMain");
        createSingleUserNodalSharedData(doc, "Objective Mean Plus StdDev Gradient", "Scalar", "PlatoMain", "PlatoMain");

        if(m_RequestedVonMisesOutput)
        {
          // VonMises statistics
          createSingleUserElementSharedData(doc, "VonMises Mean", "Scalar", "PlatoMain", "PlatoMain");
          createSingleUserElementSharedData(doc, "VonMises StdDev", "Scalar", "PlatoMain", "PlatoMain");
          for(size_t i=0; i<m_InputData.mStandardDeviations.size(); ++i)
          {
            std::string tFieldName = "VonMises Mean Plus ";
            tFieldName += m_InputData.mStandardDeviations[i];
            tFieldName += " StdDev";
            createSingleUserElementSharedData(doc, tFieldName, "Scalar", "PlatoMain", "PlatoMain");
          }
        }
      }


      // QOI shared data
      if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
      {
        pugi::xml_node for_node = doc.append_child("For");
        for_node.append_attribute("var") = "performerIndex";
        for_node.append_attribute("in") = "Performers";
        for_node = for_node.append_child("For");
        for_node.append_attribute("var") = "PerformerSampleIndex";
        for_node.append_attribute("in") = "PerformerSamples";
        createSingleUserElementSharedData(for_node, "plato_analyze_{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}_vonmises", "Scalar", "plato_analyze_{performerIndex}", "PlatoMain");
      }
      else
      {
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        {
          XMLGen::Objective cur_obj = m_InputData.objectives[i];
          // If this is a UQ run we are assuming only one load case per objective
          // and we are only supporting vonmises stress right now.
          if(m_HasUncertainties)
          {
            // create shared data for vonmises
            sprintf(tmp_buf, "%s_%s", cur_obj.performer_name.c_str(), "vonmises");
            createSingleUserElementSharedData(doc, tmp_buf, "Scalar", cur_obj.performer_name, "PlatoMain");
          }
          else if(cur_obj.multi_load_case == "true")
          {
            for(size_t k=0; k<cur_obj.load_case_ids.size(); ++k)
            {
              std::string cur_load_string = cur_obj.load_case_ids[k];
              for(size_t j=0; j<cur_obj.output_for_plotting.size(); ++j)
              {
                // create shared data for objectives
                sprintf(tmp_buf, "%s_load%s_%s", cur_obj.performer_name.c_str(), cur_load_string.c_str(), cur_obj.output_for_plotting[j].c_str());
                if(cur_obj.output_for_plotting[j] == "vonmises")
                  createSingleUserElementSharedData(doc, tmp_buf, "Scalar", cur_obj.performer_name, "PlatoMain");
                else
                  createSingleUserNodalSharedData(doc, tmp_buf, "Scalar", cur_obj.performer_name, "PlatoMain");
              }
            }
          }
          else
          {
            for(size_t j=0; j<cur_obj.output_for_plotting.size(); ++j)
            {
              // create shared data for objectives
              sprintf(tmp_buf, "%s_%s", cur_obj.performer_name.c_str(), cur_obj.output_for_plotting[j].c_str());
              if(cur_obj.output_for_plotting[j] == "vonmises")
                createSingleUserElementSharedData(doc, tmp_buf, "Scalar", cur_obj.performer_name, "PlatoMain");
              else
                createSingleUserNodalSharedData(doc, tmp_buf, "Scalar", cur_obj.performer_name, "PlatoMain");
            }
          }
        }
      }
    }

    // Hessian shared data
    if(m_InputData.optimization_algorithm =="ksbc" ||
            m_InputData.optimization_algorithm == "ksal" ||
            m_InputData.optimization_algorithm == "rol ksal" ||
            m_InputData.optimization_algorithm == "rol ksbc")
    {
      if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
      {
        pugi::xml_node for_node = doc.append_child("For");
        for_node.append_attribute("var") = "performerIndex";
        for_node.append_attribute("in") = "Performers";
        for_node = for_node.append_child("For");
        for_node.append_attribute("var") = "PerformerSampleIndex";
        for_node.append_attribute("in") = "PerformerSamples";
        std::string tSharedDataName = "HessianTimesVector {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}";
        createSingleUserNodalSharedData(doc, tSharedDataName.c_str(), "Scalar", "plato_analyze_{performerIndex}", "PlatoMain");
      }
      else
      {
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        {
          sprintf(tmp_buf, "HessianTimesVector %d", (int)(i+1));
          createSingleUserNodalSharedData(doc, tmp_buf, "Scalar", m_InputData.objectives[i].performer_name, "PlatoMain");
        }
      }
    }

    if(m_InputData.optimization_type == "topology")
    {
        // Internal Energy
        if(m_InputData.mPlatoAnalyzePerformerExists)
          createSingleUserGlobalSharedData(doc, "Objective Value", "Scalar", "1", "PlatoMain", "PlatoMain");
        else
          createSingleUserGlobalSharedData(doc, "Internal Energy", "Scalar", "1", "PlatoMain", "PlatoMain");

        // Internal Energy Gradient
        if(m_InputData.mPlatoAnalyzePerformerExists)
          createSingleUserNodalSharedData(doc, "Objective Gradient", "Scalar", "PlatoMain", "PlatoMain");
        else
          createSingleUserNodalSharedData(doc, "Internal Energy Gradient", "Scalar", "PlatoMain", "PlatoMain");
    }
    else if(m_InputData.optimization_type == "shape" && m_InputData.num_shape_design_variables > 0)
    {
        createSingleUserGlobalSharedData(doc, "Objective", "Scalar", "1", "PlatoMain", "PlatoMain");

        char tTempBuffer[100];
        sprintf(tTempBuffer, "%d", m_InputData.num_shape_design_variables);
        createSingleUserGlobalSharedData(doc, "Objective Gradient", "Scalar", tTempBuffer, "PlatoMain", "PlatoMain");
    }


    // Internal Energy Hessian and Descent Direction
    if(m_InputData.optimization_algorithm =="ksbc" ||
       m_InputData.optimization_algorithm == "ksal" ||
       m_InputData.optimization_algorithm == "rol ksal" ||
       m_InputData.optimization_algorithm == "rol ksbc")
    {
      if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
      {
        createSingleUserNodalSharedData(doc, "Internal Energy Hessian", "Scalar", "PlatoMain", "PlatoMain");
        sd_node = createSingleUserNodalSharedData(doc, "Descent Direction", "Scalar", "PlatoMain", "PlatoMain");
        pugi::xml_node for_node = sd_node.append_child("For");
        for_node.append_attribute("var") = "performerIndex";
        for_node.append_attribute("in") = "Performers";
        addChild(for_node, "UserName", "plato_analyze_{performerIndex}");
      }
      else
      {
        if(m_InputData.optimization_type == "topology")
        {
          createSingleUserNodalSharedData(doc, "Internal Energy Hessian", "Scalar", "PlatoMain", "PlatoMain");
          sd_node = createSingleUserNodalSharedData(doc, "Descent Direction", "Scalar", "PlatoMain", "PlatoMain");
          for(size_t i=0; i<m_InputData.objectives.size(); ++i)
            addChild(sd_node, "UserName", m_InputData.objectives[i].performer_name);
        }
        else if(m_InputData.optimization_type == "shape")
        {
          createSingleUserNodalSharedData(doc, "Objective Hessian", "Scalar", "PlatoMain", "PlatoMain");
          sd_node = createSingleUserNodalSharedData(doc, "Descent Direction", "Scalar", "PlatoMain", "PlatoMain");
          for(size_t i=0; i<m_InputData.objectives.size(); ++i)
            addChild(sd_node, "UserName", m_InputData.objectives[i].performer_name);
        }
      }
    }
    
    if(m_InputData.optimization_type == "shape" && m_InputData.num_shape_design_variables > 0)
    {
        char tTempBuffer[100];
        sprintf(tTempBuffer, "%d", m_InputData.num_shape_design_variables);
        std::vector<std::string> tUserNames;
        tUserNames.push_back("PlatoMain");
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
            tUserNames.push_back(m_InputData.objectives[i].performer_name);
        createMultiUserGlobalSharedData(doc, "Design Parameters", "Scalar", tTempBuffer, "PlatoMain", tUserNames);
    }
    else if(m_InputData.optimization_type == "topology")
    {
      if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
      {
        sd_node = createSingleUserNodalSharedData(doc, "Optimization DOFs", "Scalar", "PlatoMain", "PlatoMain");
        pugi::xml_node for_node = sd_node.append_child("For");
        for_node.append_attribute("var") = "performerIndex";
        for_node.append_attribute("in") = "Performers";
        addChild(for_node, "UserName", "plato_analyze_{performerIndex}");

        sd_node = createSingleUserNodalSharedData(doc, "Topology", "Scalar", "PlatoMain", "PlatoMain");
        for_node = sd_node.append_child("For");
        for_node.append_attribute("var") = "performerIndex";
        for_node.append_attribute("in") = "Performers";
        addChild(for_node, "UserName", "plato_analyze_{performerIndex}");
      }
      else
      {
        // Optimization DOFs
        sd_node = createSingleUserNodalSharedData(doc, "Optimization DOFs", "Scalar", "PlatoMain", "PlatoMain");
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
          addChild(sd_node, "UserName", m_InputData.objectives[i].performer_name);

        // Topology
        sd_node = createSingleUserNodalSharedData(doc, "Topology", "Scalar", "PlatoMain", "PlatoMain");
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
          addChild(sd_node, "UserName", m_InputData.objectives[i].performer_name);
      }
    }

    if(m_InputData.optimization_type == "topology")
    {
        if(m_InputData.discretization == "density")
        {
            // Volume
            createSingleUserGlobalSharedData(doc, "Volume", "Scalar", "1", "PlatoMain", "PlatoMain");

            // Volume Gradient
            createSingleUserNodalSharedData(doc, "Volume Gradient", "Scalar", "PlatoMain", "PlatoMain");

            // Design Volume
            createSingleUserGlobalSharedData(doc, "Design Volume", "Scalar", "1", "PlatoMain", "PlatoMain");
        }
        else if(m_InputData.discretization == "levelset")
        {
            // For now we will assume there is only one type of objective
            // if doing levelsets and we will just grab the first performer
            // as the one that will calculate the surface area.
            // Surface Area
            createSingleUserGlobalSharedData(doc, "Surface Area", "Scalar", "1", m_InputData.objectives[0].performer_name, "PlatoMain");

            // Surface Area Gradient
            createSingleUserNodalSharedData(doc, "Surface Area Gradient", "Scalar", m_InputData.objectives[0].performer_name, "PlatoMain");
        }
    }
    else if(m_InputData.optimization_type == "shape")
    {
        createSingleUserGlobalSharedData(doc, "Constraint", "Scalar", "1", m_InputData.objectives[0].performer_name, "PlatoMain");
        char tTempBuffer[100];
        sprintf(tTempBuffer, "%d", m_InputData.num_shape_design_variables);
        createSingleUserGlobalSharedData(doc, "Constraint Gradient", "Scalar", tTempBuffer, m_InputData.objectives[0].performer_name, "PlatoMain");
    }

    // Lower Bound Value
    createSingleUserGlobalSharedData(doc, "Lower Bound Value", "Scalar", "1", "PlatoMain", "PlatoMain");

    // Upper Bound Value
    createSingleUserGlobalSharedData(doc, "Upper Bound Value", "Scalar", "1", "PlatoMain", "PlatoMain");

    if(m_InputData.optimization_type == "topology")
    {
        // Lower Bound Vector
        createSingleUserNodalSharedData(doc, "Lower Bound Vector", "Scalar", "PlatoMain", "PlatoMain");

        // Upper Bound Vector
        createSingleUserNodalSharedData(doc, "Upper Bound Vector", "Scalar", "PlatoMain", "PlatoMain");
    }
    else if(m_InputData.optimization_type == "shape" && m_InputData.num_shape_design_variables > 0)
    {
        char tTempBuffer[100];
        sprintf(tTempBuffer, "%d", m_InputData.num_shape_design_variables);
        createSingleUserGlobalSharedData(doc, "Lower Bound Vector", "Scalar", tTempBuffer, "PlatoMain", "PlatoMain");
        createSingleUserGlobalSharedData(doc, "Upper Bound Vector", "Scalar", tTempBuffer, "PlatoMain", "PlatoMain");
    }

    
    //////////////////////////////////////////////////
    // Stages
    /////////////////////////////////////////////////

    // Output To File
    outputOutputToFileStage(doc, m_HasUncertainties, m_RequestedVonMisesOutput);

    // Initialize Optimization
    outputInitializeOptimizationStage(doc);

    if(m_InputData.optimization_type == "topology")
    {
        // Update Problem
        outputUpdateProblemStage(doc);
    }

    // Cache State Stage
    outputCacheStateStage(doc, m_HasUncertainties);

    // Set Lower Bounds Stage
    outputSetLowerBoundsStage(doc);

    // Set Upper Bounds
    outputSetUpperBoundsStage(doc);

    if(m_InputData.optimization_type == "topology")
    {
        if(m_InputData.discretization == "density")
        {
            // Design Volume
            outputDesignVolumeStage(doc);

            // Volume
            outputVolumeStage(doc);

            // Volume Gradient
            outputVolumeGradientStage(doc);
        }
        else if(m_InputData.discretization == "levelset")
        {
            // Surface Area
            outputSurfaceAreaStage(doc);

            // Surface Area Gradient
            outputSurfaceAreaGradientStage(doc);
        }
    }
    else if(m_InputData.optimization_type == "shape")
    {
        outputConstraintStage(doc);
        outputConstraintGradientStage(doc);
    }

    // Compute State
//    outputComputeStateStage(doc);

    if(m_InputData.optimization_type == "topology")
    {
        // Internal Energy
        outputInternalEnergyStage(doc, m_HasUncertainties);

        // Internal Energy Gradient
        outputInternalEnergyGradientStage(doc, m_HasUncertainties);
    }
    else if(m_InputData.optimization_type == "shape")
    {
        // Internal Energy
        outputObjectiveStage(doc);

        // Internal Energy Gradient
        outputObjectiveGradientStage(doc);
    }

    // Internal Energy Hessian
    if(m_InputData.optimization_algorithm =="ksbc" ||
       m_InputData.optimization_algorithm == "ksal" ||
       m_InputData.optimization_algorithm == "rol ksal" ||
       m_InputData.optimization_algorithm == "rol ksbc")
    {
        if(m_InputData.optimization_type == "topology")
        {
            outputInternalEnergyHessianStage(doc);
        }
        else if(m_InputData.optimization_type == "shape")
        {
            outputObjectiveHessianStage(doc);
        }
    }

    /////////////////////////////////////////////////
    // Misc.
    ////////////////////////////////////////////////

    pugi::xml_node tMiscNode = doc.append_child("Optimizer");
    this->setOptimizerMethod(tMiscNode);
    this->setOptimalityCriteriaOptions(tMiscNode);
    tTmpNode = tMiscNode.append_child("Options");
    this->setGCMMAoptions(tTmpNode);
    this->setMMAoptions(tTmpNode);
    this->setAugmentedLagrangianOptions(tTmpNode);
    this->setTrustRegionAlgorithmOptions(tTmpNode);
    this->setKelleySachsAlgorithmOptions(tTmpNode);

    if(m_InputData.optimization_algorithm == "rol ksal" ||
       m_InputData.optimization_algorithm == "rol ksbc")
    {
        addChild(tTmpNode, "InputFileName", "rol_inputs.xml");
        addChild(tTmpNode, "OutputDiagnosticsToFile", "true");
    }

    tTmpNode = tMiscNode.append_child("Output");
    addChild(tTmpNode, "OutputStage", "Output To File");

    tTmpNode = tMiscNode.append_child("CacheStage");
    addChild(tTmpNode, "Name", "Cache State");

    if(m_InputData.optimization_type == "topology")
    {
        tTmpNode = tMiscNode.append_child("UpdateProblemStage");
        addChild(tTmpNode, "Name", "Update Problem");
    }

    tTmpNode = tMiscNode.append_child("OptimizationVariables");
    if(m_InputData.optimization_type == "topology")
    {
        addChild(tTmpNode, "ValueName", "Optimization DOFs");
        addChild(tTmpNode, "InitializationStage", "Initialize Optimization");
        addChild(tTmpNode, "FilteredName", "Topology");
        addChild(tTmpNode, "LowerBoundValueName", "Lower Bound Value");
        addChild(tTmpNode, "LowerBoundVectorName", "Lower Bound Vector");
        addChild(tTmpNode, "UpperBoundValueName", "Upper Bound Value");
        addChild(tTmpNode, "UpperBoundVectorName", "Upper Bound Vector");
        addChild(tTmpNode, "SetLowerBoundsStage", "Set Lower Bounds");
        addChild(tTmpNode, "SetUpperBoundsStage", "Set Upper Bounds");
        if(m_InputData.optimization_algorithm =="ksbc" ||
                m_InputData.optimization_algorithm == "ksal" ||
                m_InputData.optimization_algorithm == "rol ksal" ||
                m_InputData.optimization_algorithm == "rol ksbc")
        {
            addChild(tTmpNode, "DescentDirectionName", "Descent Direction");
        }
    }
    else if(m_InputData.optimization_type == "shape")
    {
        addChild(tTmpNode, "ValueName", "Design Parameters");
        addChild(tTmpNode, "InitializationStage", "Initialize Design Parameters");
        addChild(tTmpNode, "FilteredName", "Topology");
        addChild(tTmpNode, "LowerBoundValueName", "Lower Bound Value");
        addChild(tTmpNode, "LowerBoundVectorName", "Lower Bound Vector");
        addChild(tTmpNode, "UpperBoundValueName", "Upper Bound Value");
        addChild(tTmpNode, "UpperBoundVectorName", "Upper Bound Vector");
        addChild(tTmpNode, "SetLowerBoundsStage", "Set Lower Bounds");
        addChild(tTmpNode, "SetUpperBoundsStage", "Set Upper Bounds");
        if(m_InputData.optimization_algorithm =="ksbc" ||
                m_InputData.optimization_algorithm == "ksal" ||
                m_InputData.optimization_algorithm == "rol ksal" ||
                m_InputData.optimization_algorithm == "rol ksbc")
        {
            addChild(tTmpNode, "DescentDirectionName", "Descent Direction");
        }
    }

    if(m_InputData.optimization_type == "topology")
    {
        tTmpNode = tMiscNode.append_child("Objective");
        if(m_InputData.mPlatoAnalyzePerformerExists)
          addChild(tTmpNode, "ValueName", "Objective Value");
        else
          addChild(tTmpNode, "ValueName", "Internal Energy");
        if(m_InputData.mPlatoAnalyzePerformerExists)
          addChild(tTmpNode, "ValueStageName", "Objective Value");
        else
          addChild(tTmpNode, "ValueStageName", "Internal Energy");
        if(m_InputData.mPlatoAnalyzePerformerExists)
          addChild(tTmpNode, "GradientName", "Objective Gradient");
        else
          addChild(tTmpNode, "GradientName", "Internal Energy Gradient");
        if(m_InputData.mPlatoAnalyzePerformerExists)
          addChild(tTmpNode, "GradientStageName", "Objective Gradient");
        else
          addChild(tTmpNode, "GradientStageName", "Internal Energy Gradient");
        if(m_InputData.optimization_algorithm =="ksbc" ||
                m_InputData.optimization_algorithm == "ksal" ||
                m_InputData.optimization_algorithm == "rol ksal" ||
                m_InputData.optimization_algorithm == "rol ksbc")
        {
            addChild(tTmpNode, "HessianName", "Internal Energy Hessian");
        }
    }
    else if(m_InputData.optimization_type == "shape")
    {
        tTmpNode = tMiscNode.append_child("Objective");
        addChild(tTmpNode, "ValueName", "Objective");
        addChild(tTmpNode, "ValueStageName", "Objective");
        addChild(tTmpNode, "GradientName", "Objective Gradient");
        addChild(tTmpNode, "GradientStageName", "Objective Gradient");
        if(m_InputData.optimization_algorithm =="ksbc" ||
                m_InputData.optimization_algorithm == "ksal" ||
                m_InputData.optimization_algorithm == "rol ksal" ||
                m_InputData.optimization_algorithm == "rol ksbc")
        {
            addChild(tTmpNode, "HessianName", "Internal Energy Hessian");
        }
    }

    tTmpNode = tMiscNode.append_child("BoundConstraint");
    if(m_InputData.discretization == "density")
    {
        addChild(tTmpNode, "Upper", "1.0");
        addChild(tTmpNode, "Lower", "0.0");
    }
    else
    {
        addChild(tTmpNode, "Upper", "10.0");
        addChild(tTmpNode, "Lower", "-10.0");
    }

    if(m_InputData.optimization_type == "topology")
    {
        for(size_t b=0; b<m_InputData.constraints.size(); ++b)
        {
            tTmpNode = tMiscNode.append_child("Constraint");
            if(m_InputData.constraints[b].type == "volume")
            {
                addChild(tTmpNode, "Equality", "True");
                addChild(tTmpNode, "Linear", "True");
                addChild(tTmpNode, "ValueName", "Volume");
                addChild(tTmpNode, "ReferenceValueName", "Design Volume");
                addChild(tTmpNode, "GradientName", "Volume Gradient");
                addChild(tTmpNode, "ValueStageName", "Volume");
                addChild(tTmpNode, "GradientStageName", "Volume Gradient");
                if(m_InputData.constraints[b].volume_fraction != "")
                    addChild(tTmpNode, "NormalizedTargetValue", m_InputData.constraints[b].volume_fraction);
                if(m_InputData.constraints[b].volume_absolute != "")
                    addChild(tTmpNode, "AbsoluteTargetValue", m_InputData.constraints[b].volume_absolute);
                addChild(tTmpNode, "Tolerance", "1e-3");
            }
            else if(m_InputData.constraints[b].type == "surface area")
            {
                addChild(tTmpNode, "Equality", "True");
                addChild(tTmpNode, "Linear", "True");
                addChild(tTmpNode, "ValueName", "Surface Area");
                addChild(tTmpNode, "ReferenceValue", m_InputData.constraints[b].surface_area);
                addChild(tTmpNode, "GradientName", "Surface Area Gradient");
                addChild(tTmpNode, "NormalizedTargetValue", "1.0");
                addChild(tTmpNode, "Tolerance", "1e-3");
                addChild(tTmpNode, "ValueStageName", "Surface Area");
                addChild(tTmpNode, "GradientStageName", "Surface Area Gradient");
            }
        }
    }
    else if(m_InputData.optimization_type == "shape")
    {
        tTmpNode = tMiscNode.append_child("Constraint");
        addChild(tTmpNode, "ValueName", "Constraint");
        addChild(tTmpNode, "ValueStageName", "Constraint");
        addChild(tTmpNode, "GradientName", "Constraint Gradient");
        addChild(tTmpNode, "GradientStageName", "Constraint Gradient");
        if(m_InputData.constraints[0].volume_absolute != "")
            addChild(tTmpNode, "AbsoluteTargetValue", m_InputData.constraints[0].volume_absolute);
    }

    if(m_InputData.optimization_algorithm != "mma")
    {
        tTmpNode = tMiscNode.append_child("Convergence");
        addChild(tTmpNode, "MaxIterations", m_InputData.max_iterations);
    }

    if(m_InputData.optimization_type == "topology")
    {
        // mesh
        pugi::xml_node mesh_node = doc.append_child("mesh");
        addChild(mesh_node, "type", "unstructured");
        addChild(mesh_node, "format", "exodus");
        addChild(mesh_node, "mesh", m_InputData.run_mesh_name);
        for(size_t n=0; n<m_InputData.blocks.size(); ++n)
        {
            tTmpNode = mesh_node.append_child("block");
            addChild(tTmpNode, "index", m_InputData.blocks[n].block_id);
            pugi::xml_node tmp_node1 = tTmpNode.append_child("integration");
            addChild(tmp_node1, "type", "gauss");
            addChild(tmp_node1, "order", "2");
            addChild(tTmpNode, "material", m_InputData.blocks[n].material_id);
        }

        // output
        tTmpNode = doc.append_child("output");
        addChild(tTmpNode, "file", "plato");
        addChild(tTmpNode, "format", "exodus");
    }

    // Write the file to disk
    doc.save_file("interface.xml", "  ");

    return true;
}

/******************************************************************************/
bool XMLGenerator::generatePlatoAnalyzeShapeInterfaceXML()
/******************************************************************************/
{
    pugi::xml_document tDoc;
    pugi::xml_node tTmpNode, tForNode;
    char tmp_buf[200];

    // Version entry
    tTmpNode = tDoc.append_child(pugi::node_declaration);
    tTmpNode.set_name("xml");
    pugi::xml_attribute tTmpAtt = tTmpNode.append_attribute("version");
    tTmpAtt.set_value("1.0");
    
    // Console output control
    tTmpNode = tDoc.append_child("Console");
    addChild(tTmpNode, "Verbose", "true");
    addChild(tTmpNode, "Enabled", "true");

    // defines.xml
    tTmpNode = tDoc.append_child("include");
    tTmpNode.append_attribute("filename") = "defines.xml";

    //////////////////////////////////////////////////
    // Performers
    /////////////////////////////////////////////////

    // PlatoMain performer entry.
    tTmpNode = tDoc.append_child("Performer");
    addChild(tTmpNode, "Name", "PlatoMain");
    addChild(tTmpNode, "Code", "PlatoMain");
    addChild(tTmpNode, "PerformerID", "0");

    // ESP performers
    tTmpNode = tDoc.append_child("Performer");
    tForNode = tTmpNode.append_child("For");
    tForNode.append_attribute("var") = "I";
    tForNode.append_attribute("in") = "Parameters";
    addChild(tForNode, "Name", "PlatoESP_{I}");
    addChild(tTmpNode, "PerformerID", "1");

    // Other PA perfromers
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        tTmpNode = tDoc.append_child("Performer");
        addChild(tTmpNode, "Name", m_InputData.objectives[i].performer_name);
        sprintf(tmp_buf, "%d", (int)(i+2));
        addChild(tTmpNode, "PerformerID", tmp_buf);
    }

    //////////////////////////////////////////////////
    // Shared Data
    /////////////////////////////////////////////////

    // Performer Objectives
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        sprintf(tmp_buf, "Objective %d", (int)(i+1));
        createSingleUserGlobalSharedData(tDoc, tmp_buf, "Scalar", "1", m_InputData.objectives[i].performer_name, "PlatoMain");
    }

    // Aggregated Objective
    createSingleUserGlobalSharedData(tDoc, "Objective", "Scalar", "1", "PlatoMain", "PlatoMain");

    // Performer Objective Gradients
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        char tTempBuffer[100];
        sprintf(tTempBuffer, "%d", m_InputData.num_shape_design_variables);
        sprintf(tmp_buf, "Objective %d Gradient", (int)(i+1));
        createSingleUserGlobalSharedData(tDoc, tmp_buf, "Scalar", tTempBuffer, m_InputData.objectives[i].performer_name, "PlatoMain");
    }

    // Aggregated Objective Gradient
    sprintf(tmp_buf, "%d", m_InputData.num_shape_design_variables);
    createSingleUserGlobalSharedData(tDoc, "Objective Gradient", "Scalar", tmp_buf, "PlatoMain", "PlatoMain");

    // Design Parameters
    sprintf(tmp_buf, "%d", m_InputData.num_shape_design_variables);
    std::vector<std::string> tUserNames;
    tUserNames.push_back("PlatoMain");
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        tUserNames.push_back(m_InputData.objectives[i].performer_name);
    createMultiUserGlobalSharedData(tDoc, "Design Parameters", "Scalar", tmp_buf, "PlatoMain", tUserNames);

    // Design Parameter Sensitivities
    tForNode = tDoc.append_child("For");
    tForNode.append_attribute("var") = "I";
    tForNode.append_attribute("in") = "Parameters";
    tTmpNode = tForNode.append_child("SharedData");
    addChild(tTmpNode, "Name", "Parameter Sensitivity {I}");
    addChild(tTmpNode, "Type", "Scalar");
    addChild(tTmpNode, "Layout", "Global");
    addChild(tTmpNode, "Dynamic", "true");
    addChild(tTmpNode, "OwnerName", "PlatoESP_{I}");
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        addChild(tTmpNode, "UserName", m_InputData.objectives[i].performer_name);

    // Constraint
    createSingleUserGlobalSharedData(tDoc, "Constraint", "Scalar", "1", m_InputData.objectives[0].performer_name, "PlatoMain");

    // Constraint Gradient
    sprintf(tmp_buf, "%d", m_InputData.num_shape_design_variables);
    createSingleUserGlobalSharedData(tDoc, "Constraint Gradient", "Scalar", tmp_buf, m_InputData.objectives[0].performer_name, "PlatoMain");

    // Lower/Upper Bound Vectors
    sprintf(tmp_buf, "%d", m_InputData.num_shape_design_variables);
    createSingleUserGlobalSharedData(tDoc, "Lower Bound Vector", "Scalar", tmp_buf, "PlatoMain", "PlatoMain");
    createSingleUserGlobalSharedData(tDoc, "Upper Bound Vector", "Scalar", tmp_buf, "PlatoMain", "PlatoMain");

    //////////////////////////////////////////////////
    // Stages
    /////////////////////////////////////////////////

    // Output To File
    outputOutputToFileStage(tDoc, m_HasUncertainties, m_RequestedVonMisesOutput);

    // Initialize Optimization
    outputInitializeOptimizationStage(tDoc);

    // Cache State Stage
    outputCacheStateStage(tDoc, m_HasUncertainties);

    // Set Lower Bounds Stage
    outputSetLowerBoundsStage(tDoc);

    // Set Upper Bounds
    outputSetUpperBoundsStage(tDoc);

    // Constraint
    outputConstraintStage(tDoc);
  
    // Constraint Gradient
    outputConstraintGradientStage(tDoc);

    // Objective
    outputObjectiveStage(tDoc);

    // Objective Gradient
    outputObjectiveGradientStage(tDoc);

    /////////////////////////////////////////////////
    // Misc.
    ////////////////////////////////////////////////

    pugi::xml_node tMiscNode = tDoc.append_child("Optimizer");
    this->setOptimizerMethod(tMiscNode);
    this->setOptimalityCriteriaOptions(tMiscNode);
    tTmpNode = tMiscNode.append_child("Options");
    this->setGCMMAoptions(tTmpNode);
    this->setMMAoptions(tTmpNode);
    this->setAugmentedLagrangianOptions(tTmpNode);
    this->setTrustRegionAlgorithmOptions(tTmpNode);
    this->setKelleySachsAlgorithmOptions(tTmpNode);

    if(m_InputData.optimization_algorithm == "rol ksal" ||
       m_InputData.optimization_algorithm == "rol ksbc")
    {
        addChild(tTmpNode, "InputFileName", "rol_inputs.xml");
        addChild(tTmpNode, "OutputDiagnosticsToFile", "true");
    }

    tTmpNode = tMiscNode.append_child("Output");
    addChild(tTmpNode, "OutputStage", "Output To File");

    tTmpNode = tMiscNode.append_child("CacheStage");
    addChild(tTmpNode, "Name", "Cache State");

    if(m_InputData.optimization_type == "topology")
    {
        tTmpNode = tMiscNode.append_child("UpdateProblemStage");
        addChild(tTmpNode, "Name", "Update Problem");
    }

    tTmpNode = tMiscNode.append_child("OptimizationVariables");
    addChild(tTmpNode, "ValueName", "Design Parameters");
    addChild(tTmpNode, "InitializationStage", "Initialize Design Parameters");
    addChild(tTmpNode, "FilteredName", "Topology");
    addChild(tTmpNode, "LowerBoundValueName", "Lower Bound Value");
    addChild(tTmpNode, "LowerBoundVectorName", "Lower Bound Vector");
    addChild(tTmpNode, "UpperBoundValueName", "Upper Bound Value");
    addChild(tTmpNode, "UpperBoundVectorName", "Upper Bound Vector");
    addChild(tTmpNode, "SetLowerBoundsStage", "Set Lower Bounds");
    addChild(tTmpNode, "SetUpperBoundsStage", "Set Upper Bounds");
    if(m_InputData.optimization_algorithm =="ksbc" ||
            m_InputData.optimization_algorithm == "ksal" ||
            m_InputData.optimization_algorithm == "rol ksal" ||
            m_InputData.optimization_algorithm == "rol ksbc")
    {
        addChild(tTmpNode, "DescentDirectionName", "Descent Direction");
    }

    tTmpNode = tMiscNode.append_child("Objective");
    addChild(tTmpNode, "ValueName", "Objective");
    addChild(tTmpNode, "ValueStageName", "Objective");
    addChild(tTmpNode, "GradientName", "Objective Gradient");
    addChild(tTmpNode, "GradientStageName", "Objective Gradient");
    if(m_InputData.optimization_algorithm =="ksbc" ||
            m_InputData.optimization_algorithm == "ksal" ||
            m_InputData.optimization_algorithm == "rol ksal" ||
            m_InputData.optimization_algorithm == "rol ksbc")
    {
        addChild(tTmpNode, "HessianName", "Internal Energy Hessian");
    }

    tTmpNode = tMiscNode.append_child("BoundConstraint");
    if(m_InputData.discretization == "density")
    {
        addChild(tTmpNode, "Upper", "1.0");
        addChild(tTmpNode, "Lower", "0.0");
    }
    else
    {
        addChild(tTmpNode, "Upper", "10.0");
        addChild(tTmpNode, "Lower", "-10.0");
    }

    tTmpNode = tMiscNode.append_child("Constraint");
    addChild(tTmpNode, "ValueName", "Constraint");
    addChild(tTmpNode, "ValueStageName", "Constraint");
    addChild(tTmpNode, "GradientName", "Constraint Gradient");
    addChild(tTmpNode, "GradientStageName", "Constraint Gradient");
    if(m_InputData.constraints[0].volume_absolute != "")
        addChild(tTmpNode, "AbsoluteTargetValue", m_InputData.constraints[0].volume_absolute);

    if(m_InputData.optimization_algorithm != "mma")
    {
        tTmpNode = tMiscNode.append_child("Convergence");
        addChild(tTmpNode, "MaxIterations", m_InputData.max_iterations);
    }

    // Write the file to disk
    tDoc.save_file("interface.xml", "  ");

    return true;
}

/**********************************************************************************/
void XMLGenerator::outputCacheStateStage(pugi::xml_document &doc,
                                         bool &aHasUncertainties)
/**********************************************************************************/
{
    pugi::xml_node op_node, output_node;
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Cache State");

    size_t tNum_objectives = m_InputData.objectives.size();
    size_t tNum_to_write = 0;
    bool tHandles_cache_state;

    for(size_t i = 0; i < tNum_objectives; i++)
    {
      XMLGen::Objective cur_obj = m_InputData.objectives[i];
      tHandles_cache_state = cur_obj.code_name.compare("albany") && cur_obj.code_name.compare("plato_analyze") &&
              cur_obj.code_name.compare("lightmp"); // Albany, analyze, and lightmp don't handle Cache State correctly yet

      if(tHandles_cache_state)
        ++tNum_to_write;
    }

    pugi::xml_node cur_parent = stage_node;
    if(tNum_to_write > 1)
    {
        op_node = stage_node.append_child("Operation");
        cur_parent = op_node;
    }
    for(size_t i=0; i<tNum_objectives; ++i)
    {

        XMLGen::Objective cur_obj = m_InputData.objectives[i];

        tHandles_cache_state = cur_obj.code_name.compare("albany") && cur_obj.code_name.compare("plato_analyze") &&
                  cur_obj.code_name.compare("lightmp"); // Albany, analyze, and lightmp don't handle Cache State correctly yet

        if(tHandles_cache_state)
        {
            op_node = cur_parent.append_child("Operation");
            addChild(op_node, "Name", "Cache State");
            addChild(op_node, "PerformerName", cur_obj.performer_name);
            if(aHasUncertainties)
            {
                output_node = op_node.append_child("Output");
                addChild(output_node, "ArgumentName", "vonmises0");
                addChild(output_node, "SharedDataName", cur_obj.performer_name + "_" + "vonmises");
            }
            else if(cur_obj.multi_load_case == "true")
            {
                for(size_t k=0; k<cur_obj.load_case_ids.size(); k++)
                {
                    char buffer[100];
                    sprintf(buffer, "%lu", k);
                    std::string cur_load_string = cur_obj.load_case_ids[k];
                    for(size_t j=0; j<cur_obj.output_for_plotting.size(); j++)
                    {
                        output_node = op_node.append_child("Output");
                        addChild(output_node, "ArgumentName", cur_obj.output_for_plotting[j] + buffer);
                        addChild(output_node, "SharedDataName", cur_obj.performer_name + "_" + "load" + cur_load_string + "_" + cur_obj.output_for_plotting[j]);
                    }
                }
            }
            else
            {
                for(size_t j=0; j<cur_obj.output_for_plotting.size(); j++)
                {
                    output_node = op_node.append_child("Output");
                    addChild(output_node, "ArgumentName", cur_obj.output_for_plotting[j] + "0");
                    addChild(output_node, "SharedDataName", cur_obj.performer_name + "_" + cur_obj.output_for_plotting[j]);
                }
            }
        }
    }
}

/**********************************************************************************/
void XMLGenerator::outputSetLowerBoundsStage(pugi::xml_document &doc)
/**********************************************************************************/
{
    pugi::xml_node stage_node, input_node, op_node, output_node;
    stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Set Lower Bounds");
    if(m_InputData.optimization_type == "topology")
    {
        input_node = stage_node.append_child("Input");
        addChild(input_node, "SharedDataName", "Lower Bound Value");

        op_node = stage_node.append_child("Operation");
        addChild(op_node, "Name", "Calculate Lower Bounds");
        addChild(op_node, "PerformerName", "PlatoMain");

        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Lower Bound Value");
        addChild(input_node, "SharedDataName", "Lower Bound Value");

        output_node = op_node.append_child("Output");
        addChild(output_node, "ArgumentName", "Lower Bound Vector");
        addChild(output_node, "SharedDataName", "Lower Bound Vector");
    }

    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Lower Bound Vector");
}

/**********************************************************************************/
void XMLGenerator::outputSetUpperBoundsStage(pugi::xml_document &doc)
/**********************************************************************************/
{
    pugi::xml_node stage_node, input_node, op_node, output_node;
    stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Set Upper Bounds");
    if(m_InputData.optimization_type == "topology")
    {
        input_node = stage_node.append_child("Input");
        addChild(input_node, "SharedDataName", "Upper Bound Value");

        op_node = stage_node.append_child("Operation");
        addChild(op_node, "Name", "Calculate Upper Bounds");
        addChild(op_node, "PerformerName", "PlatoMain");

        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Upper Bound Value");
        addChild(input_node, "SharedDataName", "Upper Bound Value");

        output_node = op_node.append_child("Output");
        addChild(output_node, "ArgumentName", "Upper Bound Vector");
        addChild(output_node, "SharedDataName", "Upper Bound Vector");
    }

    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Upper Bound Vector");
}

/**********************************************************************************/
void XMLGenerator::outputUpdateProblemStage(pugi::xml_document &doc)
/**********************************************************************************/
{
    pugi::xml_node op_node;
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Update Problem");
    for(size_t i=0; i<m_InputData.objectives.size(); ++i)
    {
        // Hack
        if(m_InputData.objectives[i].code_name != "plato_analyze")
        {
            op_node = stage_node.append_child("Operation");
            addChild(op_node, "Name", "Update Problem");
            XMLGen::Objective cur_obj = m_InputData.objectives[i];
            addChild(op_node, "PerformerName", cur_obj.performer_name);
        }
    }
    op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Update Problem");
    addChild(op_node, "PerformerName", "PlatoMain");
}

/**********************************************************************************/
void XMLGenerator::outputInitializeOptimizationStage(pugi::xml_document &doc)
/**********************************************************************************/
{
    if(m_InputData.optimization_type == "topology")
        outputInitializeOptimizationStageForTO(doc);
    else if(m_InputData.optimization_type == "shape")
        outputInitializeOptimizationStageForSO(doc);

}

/**********************************************************************************/
void XMLGenerator::outputInitializeOptimizationStageForSO(pugi::xml_document &doc)
/**********************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Initialize Design Parameters");

    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Initialize Values");
    addChild(op_node, "PerformerName", "PlatoMain");

    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Values");
    addChild(output_node, "SharedDataName", "Design Parameters");
    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Lower Bounds");
    addChild(output_node, "SharedDataName", "Lower Bound Vector");
    output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Upper Bounds");
    addChild(output_node, "SharedDataName", "Upper Bound Vector");

    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Design Parameters");
    /*
    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Lower Bound Vector");
    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Upper Bound Vector");
    */
}

/**********************************************************************************/
void XMLGenerator::outputInitializeOptimizationStageForTO(pugi::xml_document &doc)
/**********************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Initialize Optimization");

    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Initialize Field");
    addChild(op_node, "PerformerName", "PlatoMain");

    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Initialized Field");
    addChild(output_node, "SharedDataName", "Optimization DOFs");

    if(m_InputData.mUseNormalizationInAggregator == "true")
    {
        // Filter control
        op_node = stage_node.append_child("Operation");
        addChild(op_node, "Name", "FilterControl");
        addChild(op_node, "PerformerName", "PlatoMain");
        pugi::xml_node input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Field");
        addChild(input_node, "SharedDataName", "Optimization DOFs");
        output_node = op_node.append_child("Output");
        addChild(output_node, "ArgumentName", "Filtered Field");
        addChild(output_node, "SharedDataName", "Topology");

        if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
        {
            pugi::xml_node for_node = stage_node.append_child("For");
            for_node.append_attribute("var") = "performerIndex";
            for_node.append_attribute("in") = "Performers";
            for_node = for_node.append_child("For");
            for_node.append_attribute("var") = "PerformerSampleIndex";
            for_node.append_attribute("in") = "PerformerSamples";
            op_node = for_node.append_child("Operation");
            addChild(op_node, "Name", "Compute Objective Value");
            addChild(op_node, "PerformerName", "plato_analyze_{performerIndex}");
            input_node = op_node.append_child("Input");
            addChild(input_node, "ArgumentName", "Topology");
            addChild(input_node, "SharedDataName", "Topology");
            output_node = op_node.append_child("Output");
            addChild(output_node, "ArgumentName", "Objective Value");
            addChild(output_node, "SharedDataName", "Initial Objective Value {performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}");
        }
        else
        {
          for(size_t i=0; i<m_InputData.objectives.size(); ++i)
          {
            XMLGen::Objective cur_obj = m_InputData.objectives[i];
            op_node = stage_node.append_child("Operation");
            if(cur_obj.code_name == "plato_analyze")
            {
              addChild(op_node, "Name", "Compute Objective Value");
              addChild(op_node, "PerformerName", cur_obj.performer_name.c_str());
            }
            else
            {
              addChild(op_node, "Name", "Compute Objective");
              addChild(op_node, "PerformerName", cur_obj.performer_name.c_str());
            }

            input_node = op_node.append_child("Input");
            addChild(input_node, "ArgumentName", "Topology");
            addChild(input_node, "SharedDataName", "Topology");

            output_node = op_node.append_child("Output");
            if(m_InputData.objectives[i].code_name == "plato_analyze")
              addChild(output_node, "ArgumentName", "Objective Value");
            else
              addChild(output_node, "ArgumentName", "Internal Energy");
            char tBuffer[800];
            if(m_InputData.objectives[i].code_name == "plato_analyze")
              sprintf(tBuffer, "Initial Objective Value %d", (int)(i+1));
            else
              sprintf(tBuffer, "Initial Internal Energy %d", (int)(i+1));
            addChild(output_node, "SharedDataName", tBuffer);
          }
        }
    }

    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Optimization DOFs");
}

/**********************************************************************************/
void XMLGenerator::outputOutputToFileStage(pugi::xml_document &doc,
                                           bool &aHasUncertainties,
                                           bool &aRequestedVonMises)
/**********************************************************************************/
{
    char tmp_buf[1000];
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Output To File");
    pugi::xml_node op_node, output_node, input_node;

    if(m_InputData.optimization_type == "topology")
    {
        // *********************************************************
        // This code is a hack and needs to be fixed!!!!!!!!!!!!!
        // This Alexa specific output information should probably
        // go in the Alexa input deck rather than in interface.xml.
        // We can't have it in hear because other performers won't
        // know how to execute this Alexa-specific operation.
        // *********************************************************
        bool tFirstTime = true;
        for(size_t i=0; i<m_InputData.objectives.size(); ++i)
        {
            XMLGen::Objective cur_obj = m_InputData.objectives[i];
            if(cur_obj.code_name == "plato_analyze")
            {
                for(size_t j=0; j<cur_obj.output_for_plotting.size(); ++j)
                {
                    if(tFirstTime)
                    {
                        op_node = stage_node.append_child("Operation");
                        addChild(op_node, "Name", "Write Output");
                        addChild(op_node, "PerformerName", cur_obj.performer_name);
                        tFirstTime = false;
                    }
                    sprintf(tmp_buf, "%s_%s", cur_obj.performer_name.c_str(), cur_obj.output_for_plotting[j].c_str());
                    output_node = op_node.append_child("Output");
                    addChild(output_node, "SharedDataName", tmp_buf);
                    if(cur_obj.output_for_plotting[j] == "dispx")
                        addChild(output_node, "ArgumentName", "Solution X");
                    else if(cur_obj.output_for_plotting[j] == "dispy")
                        addChild(output_node, "ArgumentName", "Solution Y");
                    else if(cur_obj.output_for_plotting[j] == "dispz")
                        addChild(output_node, "ArgumentName", "Solution Z");
                    else if(cur_obj.output_for_plotting[j] == "temperature")
                        addChild(output_node, "ArgumentName", "Solution");
                }
            }
        }

        // VonMises Statistics
        if(aHasUncertainties && aRequestedVonMises)
        {
            op_node = stage_node.append_child("Operation");
            addChild(op_node, "Name", "VonMises Statistics");
            addChild(op_node, "PerformerName", "PlatoMain");
            // We are assuming only one load case per objective/performer which
            // means we will only have one vonmises per performer.
            if(m_UseNewPlatoAnalyzeUncertaintyWorkflow)
            {
              pugi::xml_node for_node = op_node.append_child("For");
              for_node.append_attribute("var") = "performerIndex";
              for_node.append_attribute("in") = "Performers";
              for_node = for_node.append_child("For");
              for_node.append_attribute("var") = "PerformerSampleIndex";
              for_node.append_attribute("in") = "PerformerSamples";
              input_node = for_node.append_child("Input");
              addChild(input_node, "ArgumentName", "plato_analyze_{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}_vonmises");
              addChild(input_node, "SharedDataName", "plato_analyze_{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}_vonmises");
            }
            else
            {
              for(size_t i=0; i<m_InputData.objectives.size(); ++i)
              {
                XMLGen::Objective cur_obj = m_InputData.objectives[i];
                input_node = op_node.append_child("Input");
                addChild(input_node, "ArgumentName", cur_obj.performer_name + "_vonmises");
                addChild(input_node, "SharedDataName", cur_obj.performer_name + "_vonmises");
              }
            }
            output_node = op_node.append_child("Output");
            addChild(output_node, "ArgumentName", "von_mises_mean");
            addChild(output_node, "SharedDataName", "VonMises Mean");
            output_node = op_node.append_child("Output");
            addChild(output_node, "ArgumentName", "von_mises_std_dev");
            addChild(output_node, "SharedDataName", "VonMises StdDev");
            for(size_t j=0; j<m_InputData.mStandardDeviations.size(); ++j)
            {
                output_node = op_node.append_child("Output");
                std::string tTmpString = "VonMises Mean Plus ";
                tTmpString += m_InputData.mStandardDeviations[j];
                tTmpString += " StdDev";
                addChild(output_node, "ArgumentName", tTmpString);
                addChild(output_node, "SharedDataName", tTmpString);
            }
        }

        op_node = stage_node.append_child("Operation");
        addChild(op_node, "Name", "PlatoMainOutput");
        addChild(op_node, "PerformerName", "PlatoMain");

        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Topology");
        addChild(input_node, "SharedDataName", "Topology");
        input_node = op_node.append_child("Input");
        if(m_InputData.mPlatoAnalyzePerformerExists)
          addChild(input_node, "ArgumentName", "Objective Gradient");
        else
          addChild(input_node, "ArgumentName", "Internal Energy Gradient");
        if(m_InputData.mPlatoAnalyzePerformerExists)
          addChild(input_node, "SharedDataName", "Objective Gradient");
        else
          addChild(input_node, "SharedDataName", "Internal Energy Gradient");
        input_node = op_node.append_child("Input");
        addChild(input_node, "ArgumentName", "Optimization DOFs");
        addChild(input_node, "SharedDataName", "Optimization DOFs");

        if(m_InputData.constraints.size() > 0)
        {
            if(m_InputData.constraints[0].type == "volume")
            {
                input_node = op_node.append_child("Input");
                addChild(input_node, "ArgumentName", "Volume Gradient");
                addChild(input_node, "SharedDataName", "Volume Gradient");
            }
            else if(m_InputData.constraints[0].type == "surface area")
            {
                input_node = op_node.append_child("Input");
                addChild(input_node, "ArgumentName", "Surface Area Gradient");
                addChild(input_node, "SharedDataName", "Surface Area Gradient");
            }
        }
        if(m_UseNewPlatoAnalyzeUncertaintyWorkflow && m_HasUncertainties)
        {
          pugi::xml_node for_node = op_node.append_child("For");
          for_node.append_attribute("var") = "performerIndex";
          for_node.append_attribute("in") = "Performers";
          for_node = for_node.append_child("For");
          for_node.append_attribute("var") = "PerformerSampleIndex";
          for_node.append_attribute("in") = "PerformerSamples";
          input_node = for_node.append_child("Input");
          addChild(input_node, "ArgumentName", "plato_analyze_{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}_vonmises");
          addChild(input_node, "SharedDataName", "plato_analyze_{performerIndex*NumSamplesPerPerformer+PerformerSampleIndex}_vonmises");
        }
        else
        {
          for(size_t i=0; i<m_InputData.objectives.size(); ++i)
          {
            XMLGen::Objective cur_obj = m_InputData.objectives[i];
            if(aHasUncertainties)
            {
              input_node = op_node.append_child("Input");
              sprintf(tmp_buf, "%s_%s", cur_obj.performer_name.c_str(), "vonmises");
              addChild(input_node, "ArgumentName", tmp_buf);
              addChild(input_node, "SharedDataName", tmp_buf);
            }
            else if(cur_obj.multi_load_case == "true")
            {
              for(size_t k=0; k<cur_obj.load_case_ids.size(); k++)
              {
                std::string cur_load_string = cur_obj.load_case_ids[k];
                for(size_t j=0; j<cur_obj.output_for_plotting.size(); j++)
                {
                  input_node = op_node.append_child("Input");
                  sprintf(tmp_buf, "%s_load%s_%s", cur_obj.performer_name.c_str(), cur_load_string.c_str(), cur_obj.output_for_plotting[j].c_str());
                  addChild(input_node, "ArgumentName", tmp_buf);
                  addChild(input_node, "SharedDataName", tmp_buf);
                }
              }
            }
            else
            {
              for(size_t j=0; j<cur_obj.output_for_plotting.size(); j++)
              {
                input_node = op_node.append_child("Input");
                sprintf(tmp_buf, "%s_%s", cur_obj.performer_name.c_str(), cur_obj.output_for_plotting[j].c_str());
                addChild(input_node, "ArgumentName", tmp_buf);
                addChild(input_node, "SharedDataName", tmp_buf);
              }
            }
          }
        }

        if(aHasUncertainties && aRequestedVonMises)
        {
            input_node = op_node.append_child("Input");
            addChild(input_node, "ArgumentName", "VonMises Mean");
            addChild(input_node, "SharedDataName", "VonMises Mean");
            input_node = op_node.append_child("Input");
            addChild(input_node, "ArgumentName", "VonMises StdDev");
            addChild(input_node, "SharedDataName", "VonMises StdDev");
            for(size_t j=0; j<m_InputData.mStandardDeviations.size(); ++j)
            {
                input_node = op_node.append_child("Input");
                std::string tTmpString = "VonMises Mean Plus ";
                tTmpString += m_InputData.mStandardDeviations[j];
                tTmpString += " StdDev";
                addChild(input_node, "ArgumentName", tTmpString);
                addChild(input_node, "SharedDataName", tTmpString);
            }
        }
    }
    else if(m_InputData.optimization_type == "shape")
    {
        op_node = stage_node.append_child("Operation");
        addChild(op_node, "Name", "CSMMeshOutput");
        addChild(op_node, "PerformerName", "PlatoMain");
    }
}

/**********************************************************************************/
void XMLGenerator::outputDesignVolumeStage(pugi::xml_document &doc)
/**********************************************************************************/
{
    pugi::xml_node stage_node = doc.append_child("Stage");
    addChild(stage_node, "Name", "Design Volume");

    pugi::xml_node op_node = stage_node.append_child("Operation");
    addChild(op_node, "Name", "Design Volume");
    addChild(op_node, "PerformerName", "PlatoMain");

    pugi::xml_node output_node = op_node.append_child("Output");
    addChild(output_node, "ArgumentName", "Design Volume");
    addChild(output_node, "SharedDataName", "Design Volume");

    output_node = stage_node.append_child("Output");
    addChild(output_node, "SharedDataName", "Design Volume");
}

/**********************************************************************************/
bool XMLGenerator::setOptimizerMethod(pugi::xml_node & aXMLnode)
{
    if(m_InputData.optimization_algorithm.compare("oc") == 0)
    {
        addChild(aXMLnode, "Package", "OC");
    }
    else if(m_InputData.optimization_algorithm.compare("gcmma") == 0)
    {
        addChild(aXMLnode, "Package", "GCMMA");
    }
    else if(m_InputData.optimization_algorithm.compare("mma") == 0)
    {
        addChild(aXMLnode, "Package", "MMA");
    }
    else if(m_InputData.optimization_algorithm.compare("ksbc") == 0)
    {
        addChild(aXMLnode, "Package", "KSBC");
    }
    else if(m_InputData.optimization_algorithm.compare("ksal") == 0)
    {
        addChild(aXMLnode, "Package", "KSAL");
    }
    else if(m_InputData.optimization_algorithm.compare("rol ksbc") == 0)
    {
        addChild(aXMLnode, "Package", "ROL KSBC");
    }
    else if(m_InputData.optimization_algorithm.compare("rol ksal") == 0)
    {
        addChild(aXMLnode, "Package", "ROL KSAL");
    }
    else if(m_InputData.optimization_algorithm.compare("derivativechecker") == 0)
    {
        addChild(aXMLnode, "Package", "DerivativeChecker");
        addChild(aXMLnode, "CheckGradient", m_InputData.check_gradient);
        addChild(aXMLnode, "CheckHessian", m_InputData.check_hessian);
        addChild(aXMLnode, "UseUserInitialGuess", "True");
        pugi::xml_node tTmpNode = aXMLnode.append_child("Options");
        addChild(tTmpNode, "DerivativeCheckerInitialSuperscript", "1");
        addChild(tTmpNode, "DerivativeCheckerFinalSuperscript", "8");
    }

    return true;
}

/**********************************************************************************/
bool XMLGenerator::setOptimalityCriteriaOptions(pugi::xml_node & aXMLnode)
{
    if(m_InputData.optimization_algorithm.compare("oc") == 0)
    {
//        addChild(aXMLnode, "Package", "OC");
        pugi::xml_node tTmpNode = aXMLnode.append_child("OC");
        addChild(tTmpNode, "MoveLimiter", "1.0");
        addChild(tTmpNode, "StabilizationParameter", "0.5");
        addChild(tTmpNode, "UseNewtonSearch", "True");
        addChild(tTmpNode, "ConstraintMaxIterations", "25");
    }

    return true;
}

/**********************************************************************************/
bool XMLGenerator::setTrustRegionAlgorithmOptions(const pugi::xml_node & aXMLnode)
{
    if(m_InputData.mMaxTrustRegionRadius.size() > 0)
    {
        addChild(aXMLnode, "MaxTrustRegionRadius", m_InputData.mMaxTrustRegionRadius);
    }
    if(m_InputData.mMinTrustRegionRadius.size() > 0)
    {
        addChild(aXMLnode, "MinTrustRegionRadius", m_InputData.mMinTrustRegionRadius);
    }
    if(m_InputData.mTrustRegionExpansionFactor.size() > 0)
    {
        addChild(aXMLnode, "KSTrustRegionExpansionFactor", m_InputData.mTrustRegionExpansionFactor);
    }
    if(m_InputData.mTrustRegionContractionFactor.size() > 0)
    {
        addChild(aXMLnode, "KSTrustRegionContractionFactor", m_InputData.mTrustRegionContractionFactor);
    }
    if(m_InputData.mMaxTrustRegionIterations.size() > 0)
    {
        addChild(aXMLnode, "KSMaxTrustRegionIterations", m_InputData.mMaxTrustRegionIterations);
    }
    if(m_InputData.mInitialRadiusScale.size() > 0)
    {
        addChild(aXMLnode, "KSInitialRadiusScale", m_InputData.mInitialRadiusScale);
    }
    if(m_InputData.mMaxRadiusScale.size() > 0)
    {
        addChild(aXMLnode, "KSMaxRadiusScale", m_InputData.mMaxRadiusScale);
    }

    return (true);
}

/**********************************************************************************/
bool XMLGenerator::setAugmentedLagrangianOptions(const pugi::xml_node & aXMLnode)
{
    if(m_InputData.mUseMeanNorm.size() > 0)
    {
        addChild(aXMLnode, "UseMeanNorm", m_InputData.mUseMeanNorm);
    }
    if(m_InputData.mAugLagPenaltyParam.size() > 0)
    {
        addChild(aXMLnode, "AugLagPenaltyParam", m_InputData.mAugLagPenaltyParam);
    }
    if(m_InputData.mAugLagPenaltyParamScale.size() > 0)
    {
        addChild(aXMLnode, "AugLagPenaltyParamScaleFactor", m_InputData.mAugLagPenaltyParamScale);
    }
    if(m_InputData.mMaxNumAugLagSubProbIter.size() > 0)
    {
        addChild(aXMLnode, "MaxNumAugLagSubProbIter", m_InputData.mMaxNumAugLagSubProbIter);
    }
    if(m_InputData.mFeasibilityTolerance.size() > 0)
    {
        addChild(aXMLnode, "FeasibilityTolerance", m_InputData.mFeasibilityTolerance);
    }

    return (true);
}

/**********************************************************************************/
bool XMLGenerator::setKelleySachsAlgorithmOptions(const pugi::xml_node & aXMLnode)
{
    if(m_InputData.mHessianType.size() > 0)
    {
        addChild(aXMLnode, "HessianType", m_InputData.mHessianType);
    }
    if(m_InputData.mLimitedMemoryStorage.size() > 0)
    {
        addChild(aXMLnode, "LimitedMemoryStorage", m_InputData.mLimitedMemoryStorage);
    }
    if(m_InputData.mOuterGradientToleranceKS.size() > 0)
    {
        addChild(aXMLnode, "KSOuterGradientTolerance", m_InputData.mOuterGradientToleranceKS);
    }
    if(m_InputData.mOuterStationarityToleranceKS.size() > 0)
    {
        addChild(aXMLnode, "KSOuterStationarityTolerance", m_InputData.mOuterStationarityToleranceKS);
    }
    if(m_InputData.mOuterStagnationToleranceKS.size() > 0)
    {
        addChild(aXMLnode, "KSOuterStagnationTolerance", m_InputData.mOuterStagnationToleranceKS);
    }
    if(m_InputData.mOuterControlStagnationToleranceKS.size() > 0)
    {
        addChild(aXMLnode, "KSOuterControlStagnationTolerance", m_InputData.mOuterControlStagnationToleranceKS);
    }
    if(m_InputData.mOuterActualReductionToleranceKS.size() > 0)
    {
        addChild(aXMLnode, "KSOuterActualReductionTolerance", m_InputData.mOuterActualReductionToleranceKS);
    }
    if(m_InputData.mProblemUpdateFrequency.size() > 0)
    {
        addChild(aXMLnode, "ProblemUpdateFrequency", m_InputData.mProblemUpdateFrequency);
    }
    if(m_InputData.mDisablePostSmoothingKS.size() > 0)
    {
        addChild(aXMLnode, "DisablePostSmoothing", m_InputData.mDisablePostSmoothingKS);
    }
    if(m_InputData.mTrustRegionRatioLowKS.size() > 0)
    {
        addChild(aXMLnode, "KSTrustRegionRatioLow", m_InputData.mTrustRegionRatioLowKS);
    }
    if(m_InputData.mTrustRegionRatioMidKS.size() > 0)
    {
        addChild(aXMLnode, "KSTrustRegionRatioMid", m_InputData.mTrustRegionRatioMidKS);
    }
    if(m_InputData.mTrustRegionRatioUpperKS.size() > 0)
    {
        addChild(aXMLnode, "KSTrustRegionRatioUpper", m_InputData.mTrustRegionRatioUpperKS);
    }

    return (true);
}

/**********************************************************************************/
bool XMLGenerator::setGCMMAoptions(const pugi::xml_node & aXMLnode)
{
    if(m_InputData.mInnerKKTtoleranceGCMMA.size() > 0)
    {
        addChild(aXMLnode, "GCMMAInnerKKTTolerance", m_InputData.mInnerKKTtoleranceGCMMA);
    }
    if(m_InputData.mOuterKKTtoleranceGCMMA.size() > 0)
    {
        addChild(aXMLnode, "GCMMAOuterKKTTolerance", m_InputData.mOuterKKTtoleranceGCMMA);
    }
    if(m_InputData.mInnerControlStagnationToleranceGCMMA.size() > 0)
    {
        addChild(aXMLnode, "GCMMAInnerControlStagnationTolerance", m_InputData.mInnerControlStagnationToleranceGCMMA);
    }
    if(m_InputData.mOuterControlStagnationToleranceGCMMA.size() > 0)
    {
        addChild(aXMLnode, "GCMMAOuterControlStagnationTolerance", m_InputData.mOuterControlStagnationToleranceGCMMA);
    }
    if(m_InputData.mOuterObjectiveStagnationToleranceGCMMA.size() > 0)
    {
        addChild(aXMLnode, "GCMMAOuterObjectiveStagnationTolerance", m_InputData.mOuterObjectiveStagnationToleranceGCMMA);
    }
    if(m_InputData.mMaxInnerIterationsGCMMA.size() > 0)
    {
        addChild(aXMLnode, "GCMMAMaxInnerIterations", m_InputData.mMaxInnerIterationsGCMMA);
    }
    if(m_InputData.mOuterStationarityToleranceGCMMA.size() > 0)
    {
        addChild(aXMLnode, "GCMMAOuterStationarityTolerance", m_InputData.mOuterStationarityToleranceGCMMA);
    }
    if(m_InputData.mInitialMovingAsymptotesScaleFactorGCMMA.size() > 0)
    {
        addChild(aXMLnode, "GCMMAInitialMovingAsymptoteScaleFactor", m_InputData.mInitialMovingAsymptotesScaleFactorGCMMA);
    }

    return (true);
}

/**********************************************************************************/
bool XMLGenerator::setMMAoptions(const pugi::xml_node & aXMLnode)
{
    if(m_InputData.max_iterations.size() > 0)
    {
        addChild(aXMLnode, "MaxNumOuterIterations", m_InputData.max_iterations);
    }
    if(m_InputData.mMMAMoveLimit.size() > 0)
    {
        addChild(aXMLnode, "MoveLimit", m_InputData.mMMAMoveLimit);
    }
    if(m_InputData.mMMAAsymptoteExpansion.size() > 0)
    {
        addChild(aXMLnode, "AsymptoteExpansion", m_InputData.mMMAAsymptoteExpansion);
    }
    if(m_InputData.mMMAAsymptoteContraction.size() > 0)
    {
        addChild(aXMLnode, "AsymptoteContraction", m_InputData.mMMAAsymptoteContraction);
    }
    if(m_InputData.mMMAMaxNumSubProblemIterations.size() > 0)
    {
        addChild(aXMLnode, "MaxNumSubProblemIter", m_InputData.mMMAMaxNumSubProblemIterations);
    }
    if(m_InputData.mMMAMaxTrustRegionIterations.size() > 0)
    {
        addChild(aXMLnode, "MaxNumTrustRegionIter", m_InputData.mMMAMaxTrustRegionIterations);
    }
    if(m_InputData.mMMAControlStagnationTolerance.size() > 0)
    {
        addChild(aXMLnode, "ControlStagnationTolerance", m_InputData.mMMAControlStagnationTolerance);
    }
    if(m_InputData.mMMAObjectiveStagnationTolerance.size() > 0)
    {
        addChild(aXMLnode, "ObjectiveStagnationTolerance", m_InputData.mMMAObjectiveStagnationTolerance);
    }

    return (true);
}

}


