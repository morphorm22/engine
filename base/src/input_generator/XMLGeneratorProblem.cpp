/*
 * XMLGeneratorProblem.cpp
 *
 *  Created on: March 24, 2022
 */

#include <string>
#include <vector>

#include "XMLGeneratorDakotaProblem.hpp"

#include "XMLGeneratorProblem.hpp"
#include "XMLGeneratorUtilities.hpp"
#include "XMLGeneratorDataStruct.hpp"
#include "XMLGeneratorServiceUtilities.hpp"
#include "XMLGeneratorPlatoXTKInputFile.hpp"
#include "XMLGeneratorDefinesFileUtilities.hpp"
#include "XMLGeneratorPlatoXTKOperationFile.hpp"
#include "XMLGeneratorLaunchScriptUtilities.hpp"
#include "XMLGeneratorInterfaceFileUtilities.hpp"
#include "XMLGeneratorPlatoESPInputFileUtilities.hpp"
#include "XMLGeneratorPlatoMainInputFileUtilities.hpp"
#include "XMLGeneratorPlatoAnalyzeInputFileUtilities.hpp"
#include "XMLGeneratorPlatoMainOperationFileUtilities.hpp"
#include "XMLGeneratorPlatoESPOperationsFileUtilities.hpp"
#include "XMLGeneratorPlatoAnalyzeOperationsFileUtilities.hpp"

namespace XMLGen
{

namespace Problem
{

void write_performer_operation_xml_file_gradient_based_problem
(const XMLGen::InputData& aMetaData)
{
    if(aMetaData.services().size() > 0)
    {
        if(aMetaData.services()[0].id() == "helmholtz")
        {
            XMLGen::write_plato_analyze_helmholtz_operation_xml_file(aMetaData);
        }
        else
        {
            XMLGen::write_plato_analyze_operation_xml_file(aMetaData);
        }
    }
}
// function write_performer_operation_xml_file_gradient_based_problem

void write_performer_input_deck_file_gradient_based_problem
(const XMLGen::InputData& aMetaData)
{
    if(aMetaData.services().size() > 0)
    {
        if(aMetaData.services()[0].id() == "helmholtz")
        {
            XMLGen::write_plato_analyze_helmholtz_input_deck_file(aMetaData);
        }
        else
        {
            XMLGen::write_plato_analyze_input_deck_file(aMetaData);
        }
    }
}
// function write_performer_input_deck_file_gradient_based_problem

void write_optimization_problem
(const XMLGen::InputData& aMetaData,
 const std::vector<XMLGen::InputData>& aPreProcessedMetaData)
{
    XMLGen::write_define_xml_file(aMetaData);
    XMLGen::write_interface_xml_file(aMetaData);
    XMLGen::generate_launch_script(aMetaData);
    XMLGen::write_plato_main_operations_xml_file(aMetaData);
    XMLGen::write_plato_main_input_deck_file(aMetaData);
    XMLGen::write_amgx_input_file(aMetaData);
    XMLGen::write_plato_esp_input_deck_file(aMetaData);
    XMLGen::write_plato_esp_operations_file(aMetaData);
    XMLGen::write_xtk_input_deck_file(aMetaData);
    XMLGen::write_xtk_operations_file(aMetaData);

    for(auto tCurMetaData : aPreProcessedMetaData)
    {
        XMLGen::Problem::write_performer_operation_xml_file_gradient_based_problem(tCurMetaData);
        XMLGen::Problem::write_performer_input_deck_file_gradient_based_problem(tCurMetaData);
    }

}
// write_optimization_problem

void write_dakota_problem
(XMLGen::InputData& aMetaData,
 const std::vector<XMLGen::InputData>& aPreProcessedMetaData)
{
    XMLGen::dakota::write_problem(aMetaData, aPreProcessedMetaData);
}
// function write_dakota_problem

}
// namespace Problem

}
// namespace XMLGen

