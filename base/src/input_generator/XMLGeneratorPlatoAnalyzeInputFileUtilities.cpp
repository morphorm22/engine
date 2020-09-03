/*
 * XMLGeneratorPlatoAnalyzeInputFileUtilities.cpp
 *
 *  Created on: Jun 15, 2020
 */

#include "XMLGeneratorUtilities.hpp"
#include "XMLGeneratorValidInputKeys.hpp"
#include "XMLGeneratorPlatoAnalyzeUtilities.hpp"
#include "XMLGeneratorPlatoAnalyzeInputFileUtilities.hpp"
#include "XMLGeneratorAnalyzePhysicsFunctionInterface.hpp"
#include "XMLGeneratorAnalyzeNaturalBCFunctionInterface.hpp"
#include "XMLGeneratorAnalyzeNaturalBCTagFunctionInterface.hpp"
#include "XMLGeneratorAnalyzeMaterialModelFunctionInterface.hpp"
#include "XMLGeneratorAnalyzeAppendCriterionFunctionInterface.hpp"
#include "XMLGeneratorAnalyzeEssentialBCFunctionInterface.hpp"
#include "XMLGeneratorAnalyzeEssentialBCTagFunctionInterface.hpp"

namespace XMLGen
{

/**********************************************************************************/
void check_input_mesh_file_keyword
(const XMLGen::InputData& aXMLMetaData)
{
    if (aXMLMetaData.run_mesh_name.empty())
    {
        THROWERR("Check Input Mesh File Keyword: Input mesh filename is empty.")
    }
}
// function check_input_mesh_file_keyword
/**********************************************************************************/

/**********************************************************************************/
void is_objective_container_empty
(const XMLGen::InputData& aXMLMetaData)
{
    if(aXMLMetaData.objectives.empty())
    {
        THROWERR("Check Objective is Defined: Objective container is empty.")
    }
}
// function is_objective_container_empty
/**********************************************************************************/

/**********************************************************************************/
std::vector<std::string> return_list_of_objective_functions
(const XMLGen::InputData& aXMLMetaData)
{
    std::vector<std::string> tTokens;
    for(auto& tObjective : aXMLMetaData.objectives)
    {
        if (tObjective.code().compare("plato_analyze") == 0)
        {
            auto tToken = std::string("my ") + Plato::tolower(tObjective.category());
            tTokens.push_back(tToken);
        }
    }
    return tTokens;
}
// function return_list_of_objective_functions
/**********************************************************************************/

/**********************************************************************************/
std::vector<std::string> return_list_of_objective_weights
(const XMLGen::InputData& aXMLMetaData)
{
    std::vector<std::string> tTokens;
    for(auto& tObjective : aXMLMetaData.objectives)
    {
        if (tObjective.code().compare("plato_analyze") == 0)
        {
            auto tWeight = tObjective.weight.empty() ? "1.0" : tObjective.weight;
            tTokens.push_back(tWeight);
        }
    }
    return tTokens;
}
// function return_list_of_objective_weights
/**********************************************************************************/

/**********************************************************************************/
std::vector<std::string> return_list_of_constraint_functions
(const XMLGen::InputData& aXMLMetaData)
{
    std::vector<std::string> tTokens;
    for(auto& tConstraint : aXMLMetaData.constraints)
    {
        if (tConstraint.code().compare("plato_analyze") == 0)
        {
            auto tToken = std::string("my ") + Plato::tolower(tConstraint.category());
            tTokens.push_back(tToken);
        }
    }
    return tTokens;
}
// function return_list_of_constraint_functions
/**********************************************************************************/

/**********************************************************************************/
std::vector<std::string> return_list_of_constraint_weights
(const XMLGen::InputData& aXMLMetaData)
{
    std::vector<std::string> tTokens;
    for(auto& tConstraint : aXMLMetaData.constraints)
    {
        if (tConstraint.code().compare("plato_analyze") == 0)
        {
            auto tCurrentWeight = tConstraint.weight();
            auto tProposedWeight = tCurrentWeight.empty() ? "1.0" : tCurrentWeight;
            tTokens.push_back(tProposedWeight);
        }
    }
    return tTokens;
}
// function return_list_of_constraint_weights
/**********************************************************************************/

/**********************************************************************************/
std::string transform_tokens_for_plato_analyze_input_deck
(const std::vector<std::string> &aTokens)
{
    if(aTokens.empty())
    {
        THROWERR("Transform Tokens for Plato Analyze Input Deck: Input list of tokens is empty.")
    }

    std::string tOutput("{");
    auto tEndIndex = aTokens.size() - 1u;
    auto tEndIterator = std::next(aTokens.begin(), tEndIndex);
    for(auto tItr = aTokens.begin(); tItr != tEndIterator; ++tItr)
    {
        auto tIndex = std::distance(aTokens.begin(), tItr);
        tOutput += aTokens[tIndex] + ", ";
    }
    tOutput += aTokens[tEndIndex] + "}";
    return tOutput;
}
// function transform_tokens_for_plato_analyze_input_deck
/**********************************************************************************/

/**********************************************************************************/
void append_problem_description_to_plato_analyze_input_deck
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    XMLGen::ValidSpatialDimsKeys tValidKeys;
    auto tSpatialDim = std::find(tValidKeys.mKeys.begin(), tValidKeys.mKeys.end(), aXMLMetaData.service(0u).dimensions());
    if (tSpatialDim == tValidKeys.mKeys.end())
    {
        THROWERR(std::string("Append Problem Description to Plato Analyze Input Deck: Invalid spatial dimensions '")
            + aXMLMetaData.service(0u).dimensions() + "'.  Only three and two dimensional problems are supported in Plato Analyze.")
    }
    XMLGen::check_input_mesh_file_keyword(aXMLMetaData);

    auto tProblem = aDocument.append_child("ParameterList");
    XMLGen::append_attributes({"name"}, {"Problem"}, tProblem);
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Physics", "string", "Plato Driver"};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, tProblem);
    tValues = {"Spatial Dimension", "int", tSpatialDim->c_str()};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, tProblem);
    tValues = {"Input Mesh", "string", aXMLMetaData.run_mesh_name};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, tProblem);
}
// function append_problem_description_to_plato_analyze_input_deck
/**********************************************************************************/

/**********************************************************************************/
void append_physics_parameter_to_plato_problem
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::ValidAnalyzePhysicsKeys tValidKeys;
    auto tPhysicsTag = tValidKeys.physics(aXMLMetaData.service(0u).physics());
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Physics", "string", tPhysicsTag};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, aParentNode);
}
// function append_physics_parameter_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_pde_constraint_parameter_to_plato_problem
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::ValidAnalyzePhysicsKeys tValidKeys;
    auto tPDE = tValidKeys.pde(aXMLMetaData.service(0u).physics());
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"PDE Constraint", "string", tPDE};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, aParentNode);
}
// function append_physics_parameter_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_self_adjoint_parameter_to_plato_problem
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::is_objective_container_empty(aXMLMetaData);

    std::string tIsSelfAdjoint = "false";
    if(aXMLMetaData.objectives.size() == 1u)
    {
        XMLGen::ValidAnalyzeCriteriaKeys tValidKeys;
        auto tLowerCriterion = Plato::tolower(aXMLMetaData.objectives.begin()->type);
        auto tItr = tValidKeys.mKeys.find(tLowerCriterion);
        if (tItr == tValidKeys.mKeys.end())
        {
            THROWERR(std::string("Append Self Adjoint Parameter to Plato Analyze Input Deck: Criterion '")
                + tLowerCriterion + "' is not supported.")
        }
        tIsSelfAdjoint = tItr->second.second ? "true" : "false";
    }
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Self-Adjoint", "bool", tIsSelfAdjoint};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, aParentNode);
}
// function append_self_adjoint_parameter_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_plato_problem_description_to_plato_analyze_input_deck
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    auto tProblem = aDocument.child("ParameterList");
    if(tProblem.empty())
    {
        THROWERR("Append Plato Problem To Plato Analyze Input Deck: Parameter List with name 'Problem' is empty.")
    }
    auto tPlatoProblem = tProblem.append_child("ParameterList");
    XMLGen::append_attributes({"name"}, {"Plato Problem"}, tPlatoProblem);
    XMLGen::append_physics_parameter_to_plato_problem(aXMLMetaData, tPlatoProblem);
    XMLGen::append_pde_constraint_parameter_to_plato_problem(aXMLMetaData, tPlatoProblem);
    XMLGen::append_self_adjoint_parameter_to_plato_problem(aXMLMetaData, tPlatoProblem);
}
// function append_plato_problem_description_to_plato_analyze_input_deck
/**********************************************************************************/

/**********************************************************************************/
void append_plato_problem_to_plato_analyze_input_deck
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    auto tProblem = aDocument.child("ParameterList");
    if(tProblem.empty())
    {
        THROWERR("Append Plato Problem To Plato Analyze Input Deck: Parameter List with name 'Problem' is empty.")
    }
    auto tPlatoProblem = tProblem.child("ParameterList");
    if(tPlatoProblem.empty())
    {
        THROWERR("Append Plato Problem To Plato Analyze Input Deck: Parameter List with name 'Plato Problem' is empty.")
    }
    XMLGen::append_criteria_list_to_plato_analyze_input_deck(aXMLMetaData, tPlatoProblem);
    XMLGen::append_physics_to_plato_analyze_input_deck(aXMLMetaData, tPlatoProblem);

    XMLGen::append_spatial_model_to_plato_analyze_input_deck(aXMLMetaData, tPlatoProblem);
    XMLGen::append_material_models_to_plato_analyze_input_deck(aXMLMetaData, tPlatoProblem);

    XMLGen::append_natural_boundary_conditions_to_plato_analyze_input_deck(aXMLMetaData, tPlatoProblem);
    XMLGen::append_essential_boundary_conditions_to_plato_analyze_input_deck(aXMLMetaData, tPlatoProblem);
}
// function append_plato_problem_to_plato_analyze_input_deck
/**********************************************************************************/
void append_criteria_list_to_plato_analyze_input_deck
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tCriteriaList = aParentNode.append_child("ParameterList");
    XMLGen::append_attributes({"name"}, {"Criteria"}, tCriteriaList);
    XMLGen::append_objective_criteria_to_criteria_list(aXMLMetaData, tCriteriaList);
    XMLGen::append_constraint_criteria_to_criteria_list(aXMLMetaData, tCriteriaList);
}

/**********************************************************************************/
void append_weighted_sum_objective_to_plato_problem
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::append_attributes({"name"}, {"My Objective"}, aParentNode);
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Type", "string", "Weighted Sum"};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, aParentNode);
}
// function append_weighted_sum_objective_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_functions_to_weighted_sum_objective
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tTokens = XMLGen::return_list_of_objective_functions(aXMLMetaData);
    auto tFunctions = XMLGen::transform_tokens_for_plato_analyze_input_deck(tTokens);
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Functions", "Array(string)", tFunctions};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, aParentNode);
}
// function append_functions_to_weighted_sum_objective
/**********************************************************************************/

/**********************************************************************************/
void append_weights_to_weighted_sum_objective
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tTokens = XMLGen::return_list_of_objective_weights(aXMLMetaData);
    auto tWeights = XMLGen::transform_tokens_for_plato_analyze_input_deck(tTokens);
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Weights", "Array(double)", tWeights};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, aParentNode);
}
// function append_weights_to_weighted_sum_objective
/**********************************************************************************/

/**********************************************************************************/
void append_objective_criteria_to_plato_problem
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::AppendCriterionParameters<XMLGen::Objective> tFunctionInterface;
    for(auto& tObjective : aXMLMetaData.objectives)
    {
        if(tObjective.code().compare("plato_analyze") == 0)
        {
            tFunctionInterface.call(tObjective, aParentNode);
        }
    }
}
// function append_objective_criteria_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_objective_criteria_to_criteria_list
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    if(XMLGen::is_any_objective_computed_by_plato_analyze(aXMLMetaData) == false)
    {
        return;
    }

    auto tObjective = aParentNode.append_child("ParameterList");
    XMLGen::append_weighted_sum_objective_to_plato_problem(aXMLMetaData, tObjective);
    XMLGen::append_functions_to_weighted_sum_objective(aXMLMetaData, tObjective);
    XMLGen::append_weights_to_weighted_sum_objective(aXMLMetaData, tObjective);
    XMLGen::append_objective_criteria_to_plato_problem(aXMLMetaData, aParentNode);
}
// function append_objective_criteria_to_criteria_list
/**********************************************************************************/

/**********************************************************************************/
void append_constraint_criteria_to_plato_problem
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::AppendCriterionParameters<XMLGen::Constraint> tFunctionInterface;
    for(auto& tConstraint : aXMLMetaData.constraints)
    {
        tFunctionInterface.call(tConstraint, aParentNode);
    }
}
// function append_constraint_criteria_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_weighted_sum_constraint_to_plato_problem
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::append_attributes({"name"}, {"My Constraint"}, aParentNode);
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Type", "string", "Weighted Sum"};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, aParentNode);
}
// function append_weighted_sum_constraint_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_functions_to_weighted_sum_constraint
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tTokens = XMLGen::return_list_of_constraint_functions(aXMLMetaData);
    auto tFunctions = XMLGen::transform_tokens_for_plato_analyze_input_deck(tTokens);
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Functions", "Array(string)", tFunctions};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, aParentNode);
}
// function append_functions_to_weighted_sum_constraint
/**********************************************************************************/

/**********************************************************************************/
void append_weights_to_weighted_sum_constraint
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tTokens = XMLGen::return_list_of_constraint_weights(aXMLMetaData);
    auto tWeights = XMLGen::transform_tokens_for_plato_analyze_input_deck(tTokens);
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Weights", "Array(double)", tWeights};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, aParentNode);
}
// function append_weights_to_weighted_sum_constraint
/**********************************************************************************/

/**********************************************************************************/
void append_constraint_criteria_to_criteria_list
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    if(XMLGen::is_any_constraint_computed_by_plato_analyze(aXMLMetaData) == false)
    {
        return;
    }
    auto tConstraint = aParentNode.append_child("ParameterList");
    XMLGen::append_weighted_sum_constraint_to_plato_problem(aXMLMetaData, tConstraint);
    XMLGen::append_functions_to_weighted_sum_constraint(aXMLMetaData, tConstraint);
    XMLGen::append_weights_to_weighted_sum_constraint(aXMLMetaData, tConstraint);
    XMLGen::append_constraint_criteria_to_plato_problem(aXMLMetaData, aParentNode);
}
// function append_constraint_criteria_to_criteria_list
/**********************************************************************************/

/**********************************************************************************/
void append_physics_to_plato_analyze_input_deck
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::AnalyzePhysicsFunctionInterface tPhysicsInterface;
    tPhysicsInterface.call(aXMLMetaData.service(0u), aXMLMetaData.mOutputMetaData, aParentNode);
}
// function append_physics_to_plato_analyze_input_deck
/**********************************************************************************/

/**********************************************************************************/
void append_material_models_to_plato_problem
(const std::vector<XMLGen::Material>& aMaterials,
 pugi::xml_node& aParentNode)
{
    if(aMaterials.empty())
    {
        THROWERR("Append Material Model to Plato Problem: Material container is empty.")
    }

    auto tMaterialModels = aParentNode.append_child("ParameterList");
    XMLGen::append_attributes({"name"}, {"Material Models"}, tMaterialModels);

    XMLGen::AppendMaterialModelParameters tMaterialInterface;
    for(auto& tMaterial : aMaterials)
    {
        tMaterialInterface.call(tMaterial, tMaterialModels);
    }
}
// function append_material_model_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_material_models_to_plato_analyze_input_deck
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::append_material_models_to_plato_problem(aXMLMetaData.materials, aParentNode);
}
// function append_material_model_to_plato_analyze_input_deck
/**********************************************************************************/

/**********************************************************************************/
void append_spatial_model_to_plato_analyze_input_deck
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tBlocks = aXMLMetaData.blocks;
    if(tBlocks.empty())
    {
        THROWERR("Append Spatial Model to Plato Analyze Input Deck: Block container is empty.")
    }

    auto tSpatialModel = aParentNode.append_child("ParameterList");
    XMLGen::append_attributes({"name"}, {"Spatial Model"}, tSpatialModel);
    auto tDomains = tSpatialModel.append_child("ParameterList");
    XMLGen::append_attributes({"name"}, {"Domains"}, tDomains);

    for(auto& tBlock : tBlocks)
    {
        auto tDomain = tDomains.append_child("ParameterList");
        XMLGen::append_attributes({"name"}, {"Volume " + tBlock.name}, tDomain);

        std::vector<std::string> tKeys = {"name", "type", "value"};
        std::vector<std::string> tValues = {"Element Block", "string", tBlock.name};
        XMLGen::append_parameter_plus_attributes(tKeys, tValues, tDomain);

        auto tMaterials = aXMLMetaData.materials;
        std::vector<std::string> tMaterialIDs;
        bool tMaterialFound = false;
        XMLGen::Material tMaterial;

        for(auto tMat : tMaterials)
        {
            if(tMat.id() == tBlock.material_id)
            {
                tMaterial = tMat;
                tMaterialFound = true;
                break;
            }

        }
        if(!tMaterialFound)
        {
            THROWERR("Append Spatial Model to Plato Analyze Input Deck: Block " + tBlock.block_id + 
                    " lists material with material_id " + tBlock.material_id + " but no material with ID " + tBlock.material_id + " exists")
        }

        tValues = {"Material Model", "string", tMaterial.name()};
        XMLGen::append_parameter_plus_attributes(tKeys, tValues, tDomain);
    }
}
// function append_material_model_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_natural_boundary_conditions_to_plato_problem
(const XMLGen::LoadCase& aLoadCase,
 pugi::xml_node& aParentNode)
{
    auto tLowerPerformer = Plato::tolower(aLoadCase.mCode);
    if (tLowerPerformer.compare("plato_analyze") != 0)
    {
        return;
    }

    auto tNaturalBC = aParentNode.append_child("ParameterList");
    XMLGen::append_attributes({"name"}, {"Natural Boundary Conditions"}, tNaturalBC);

    XMLGen::AppendNaturalBoundaryCondition tNaturalBCFuncInterface;
    XMLGen::NaturalBoundaryConditionTag tNaturalBCNameFuncInterface;
    for(auto& tLoad : aLoadCase.loads)
    {
        auto tName = tNaturalBCNameFuncInterface.call(tLoad);
        tNaturalBCFuncInterface.call(tName, tLoad, tNaturalBC);
    }
}
// function append_natural_boundary_conditions_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_deterministic_natural_boundary_conditions_to_plato_problem
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    for (auto &tLoadCase : aXMLMetaData.load_cases)
    {
        XMLGen::append_natural_boundary_conditions_to_plato_problem(tLoadCase, aParentNode);
    }
}
// function append_deterministic_natural_boundary_conditions_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_random_natural_boundary_conditions_to_plato_problem
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    if(aXMLMetaData.mRandomMetaData.loadSamplesDrawn())
    {
        auto tRandomLoads = aXMLMetaData.mRandomMetaData.loadcase();
        XMLGen::append_natural_boundary_conditions_to_plato_problem(tRandomLoads, aParentNode);
    }
    else
    {
        XMLGen::append_deterministic_natural_boundary_conditions_to_plato_problem(aXMLMetaData, aParentNode);
    }
}
// function append_random_natural_boundary_conditions_to_plato_problem
/**********************************************************************************/

/**********************************************************************************/
void append_natural_boundary_conditions_to_plato_analyze_input_deck
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    if(aXMLMetaData.mRandomMetaData.samplesDrawn())
    {
        XMLGen::append_random_natural_boundary_conditions_to_plato_problem(aXMLMetaData, aParentNode);
    }
    else
    {
        XMLGen::append_deterministic_natural_boundary_conditions_to_plato_problem(aXMLMetaData, aParentNode);
    }
}
// function append_natural_boundary_conditions_to_plato_analyze_input_deck
/**********************************************************************************/

/**********************************************************************************/
void append_essential_boundary_conditions_to_plato_analyze_input_deck
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tEssentialBC = aParentNode.append_child("ParameterList");
    XMLGen::append_attributes({"name"}, {"Essential Boundary Conditions"}, tEssentialBC);

    XMLGen::EssentialBoundaryConditionTag tTagInterface;
    XMLGen::AppendEssentialBoundaryCondition tFuncInterface;
    for (auto &tBC : aXMLMetaData.bcs)
    {
        auto tName = tTagInterface.call(tBC);
        tFuncInterface.call(tName, tBC, tEssentialBC);
    }
}
// function append_essential_boundary_conditions_to_plato_analyze_input_deck
/**********************************************************************************/

/**********************************************************************************/
void write_plato_analyze_input_deck_file
(const XMLGen::InputData& aXMLMetaData)
{
    pugi::xml_document tDocument;

    XMLGen::append_problem_description_to_plato_analyze_input_deck(aXMLMetaData, tDocument);
    XMLGen::append_plato_problem_description_to_plato_analyze_input_deck(aXMLMetaData, tDocument);
    XMLGen::append_plato_problem_to_plato_analyze_input_deck(aXMLMetaData, tDocument);

    tDocument.save_file("plato_analyze_input_deck.xml", "  ");
}
// function write_plato_analyze_input_deck_file
/**********************************************************************************/

}
// namespace XMLGen
