/*
 * XMLGeneratorInterfaceFileUtilities.cpp
 *
 *  Created on: May 26, 2020
 */

#include "XMLGeneratorUtilities.hpp"
#include "XMLGeneratorValidInputKeys.hpp"
#include "XMLGeneratorParserUtilities.hpp"
#include "XMLGeneratorPlatoAnalyzeProblem.hpp"
#include "XMLGeneratorInterfaceFileUtilities.hpp"
#include "XMLGeneratorPlatoMainConstraintValueOperationInterface.hpp"
#include "XMLGeneratorPlatoMainConstraintGradientOperationInterface.hpp"

namespace XMLGen
{

/******************************************************************************/
void write_interface_xml_file
(const XMLGen::InputData& aMetaData)
{
    if (aMetaData.objectives.empty())
    {
        THROWERR("Write Interface XML File for a Nondeterministic Optimization Use Case: Objective block was not defined.")
    }
    if (aMetaData.objectives.size() > 1u)
    {
        THROWERR(std::string("Write Interface XML File for a Nondeterministic Optimization Use Case: Only one objective, ")
            + "i.e. objective block, is expected to be defined for a nondeterministic optimization use case.")
    }

    pugi::xml_document tDocument;

    if(XMLGen::Analyze::is_robust_optimization_problem(aMetaData))
        XMLGen::append_attributes("include", {"filename"}, {"defines.xml"}, tDocument);

    auto tNode = tDocument.append_child("Console");
    XMLGen::append_children({"Verbose"}, {aMetaData.mVerbose}, tNode);
    XMLGen::append_plato_main_performer(tDocument);

    // note: multiperformer use case currently only works with Plato Analyze, and is only used currently with the robust optimization workflow
    if(XMLGen::Analyze::is_robust_optimization_problem(aMetaData))
        XMLGen::append_physics_performers_multiperformer_usecase(aMetaData, tDocument);
    else
        XMLGen::append_physics_performers(aMetaData, tDocument);

    XMLGen::append_shared_data(aMetaData, tDocument);
    XMLGen::append_stages(aMetaData, tDocument);
    XMLGen::append_optimizer_options(aMetaData, tDocument);

    tDocument.save_file("interface.xml", "  ");
}
/******************************************************************************/

/******************************************************************************/
void append_compute_qoi_statistics_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tQoIIDs = aXMLMetaData.mOutputMetaData.randomIDs();
    for (auto &tQoIID : tQoIIDs)
    {
        auto tOperationNode = aParentNode.append_child("Operation");
        auto tOperationName = "compute " + tQoIID + " statistics";
        XMLGen::append_children({"Name", "PerformerName"}, {tOperationName, "platomain"}, tOperationNode);

        auto tForNode = tOperationNode.append_child("For");
        XMLGen::append_attributes({"var", "in"}, {"PerformerIndex", "Performers"}, tForNode);
        tForNode = tForNode.append_child("For");
        XMLGen::append_attributes({"var", "in"}, {"PerformerSampleIndex", "PerformerSamples"}, tForNode);

        auto tInputNode = tForNode.append_child("Input");
        auto tDataName = tQoIID + " {PerformerIndex*NumSamplesPerPerformer+PerformerSampleIndex}";
        XMLGen::append_children({ "ArgumentName", "SharedDataName" }, { tDataName, tDataName }, tInputNode);

        auto tMeanName = tQoIID + " mean";
        auto tOutputNode = tOperationNode.append_child("Output");
        XMLGen::append_children({ "ArgumentName", "SharedDataName" }, { tMeanName, tMeanName }, tOutputNode);

        auto tStdDevName = tQoIID + " standard deviation";
        tOutputNode = tOperationNode.append_child("Output");
        XMLGen::append_children({ "ArgumentName", "SharedDataName" }, { tStdDevName, tStdDevName }, tOutputNode);
    }
}
/******************************************************************************/

/******************************************************************************/
void append_physics_performers
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    if(aXMLMetaData.services().empty())
    {
        THROWERR("Append Physics Performer: Services list is empty.")
    }

    std::vector<std::string> tKeywords = { "Name", "Code", "PerformerID" };
    for(auto& tService : aXMLMetaData.services())
    {
        const int tID = (&tService - &aXMLMetaData.services()[0]) + 1;
        auto tPerformerNode = aParentNode.append_child("Performer");
        std::vector<std::string> tValues = { tService.performer(), tService.code(), std::to_string(tID) };
        XMLGen::append_children( tKeywords, tValues, tPerformerNode);
    }
}
/******************************************************************************/

/******************************************************************************/
void append_qoi_shared_data
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aParentNode)
{
    if(aXMLMetaData.services().empty())
    {
        THROWERR("Append QoI Shared Data: list of 'services' is empty.")
    }

    auto tOutputIDs = aXMLMetaData.mOutputMetaData.deterministicIDs();
    auto tServiceID = aXMLMetaData.mOutputMetaData.serviceID();
    for(auto& tID : tOutputIDs)
    {
        auto tLayout = aXMLMetaData.mOutputMetaData.deterministicLayout(tID);
        auto tSharedDataName = aXMLMetaData.mOutputMetaData.deterministicSharedDataName(tID);
        auto tOwnerName = aXMLMetaData.service(tServiceID).performer();
        std::vector<std::string> tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName"};
        std::vector<std::string> tValues = {tSharedDataName, "Scalar", tLayout, "IGNORE", tOwnerName, "platomain"};
        auto tSharedDataNode = aParentNode.append_child("SharedData");
        XMLGen::append_children(tKeys, tValues, tSharedDataNode);
    }
}
/******************************************************************************/

/******************************************************************************/
void append_topology_shared_data
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    if(aXMLMetaData.services().empty())
    {
        THROWERR("Append Topology Shared Data: Services list is empty.")
    }

    auto tSharedData = aDocument.append_child("SharedData");
    std::vector<std::string> tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName", "UserName"};
    std::vector<std::string> tValues = {"Topology", "Scalar", "Nodal Field", "IGNORE", "platomain", "platomain"};
    for(auto& tService : aXMLMetaData.services())
    {
        std::vector<std::string> tValues =
            {"Topology", "Scalar", "Nodal Field", "IGNORE", "platomain", "platomain", tService.performer()};
        XMLGen::append_children(tKeys, tValues, tSharedData);
    }
}
/******************************************************************************/

/******************************************************************************/
void append_shared_data
(const XMLGen::InputData& aMetaData,
 pugi::xml_document& aDocument)
{
    XMLGen::append_control_shared_data(aDocument);
    XMLGen::append_lower_bounds_shared_data(aDocument);
    XMLGen::append_upper_bounds_shared_data(aDocument);
    XMLGen::append_design_volume_shared_data(aDocument);
    XMLGen::append_constraint_shared_data(aMetaData, aDocument);
    XMLGen::append_objective_shared_data(aMetaData, aDocument);

    if(XMLGen::Analyze::is_robust_optimization_problem(aMetaData))
    {
        XMLGen::append_qoi_statistics_shared_data(aMetaData, aDocument);
        XMLGen::append_multiperformer_qoi_shared_data(aMetaData, aDocument);
        XMLGen::append_multiperformer_topology_shared_data(aMetaData, aDocument);
        XMLGen::append_multiperformer_criterion_shared_data("Objective", aMetaData, aDocument);
    }
    else
    {
        XMLGen::append_qoi_shared_data(aMetaData, aDocument);
        XMLGen::append_topology_shared_data(aMetaData, aDocument);
    }
}
/******************************************************************************/

/******************************************************************************/
void append_stages
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    // deterministic stages
    XMLGen::append_design_volume_stage(aDocument);
    XMLGen::append_initial_guess_stage(aDocument);
    XMLGen::append_lower_bound_stage(aXMLMetaData, aDocument);
    XMLGen::append_upper_bound_stage(aXMLMetaData, aDocument);
    XMLGen::append_plato_main_output_stage(aXMLMetaData, aDocument);

    // nondeterministic stages
    if(XMLGen::Analyze::is_robust_optimization_problem(aXMLMetaData))
    {
        XMLGen::append_cache_state_stage_for_nondeterministic_usecase(aXMLMetaData, aDocument);
        XMLGen::append_update_problem_stage_for_nondeterministic_usecase(aXMLMetaData, aDocument);
    }

    // criteria stages
    XMLGen::append_constraint_value_stage(aXMLMetaData, aDocument);
    XMLGen::append_constraint_gradient_stage(aXMLMetaData, aDocument);
    XMLGen::append_objective_value_stage(aXMLMetaData, aDocument);
    XMLGen::append_objective_gradient_stage(aXMLMetaData, aDocument);
}
/******************************************************************************/

/******************************************************************************/
void append_cache_state_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    for (auto &tService : aXMLMetaData.services())
    {
        if (!tService.cacheState())
        {
            continue;
        }
        auto tStageNode = aDocument.append_child("Stage");
        XMLGen::append_children( { "Name" }, { "Cache State" }, tStageNode);
        auto tOperationNode = tStageNode.append_child("Operation");
        std::vector<std::string> tKeys = { "Name", "PerformerName" };
        std::vector<std::string> tValues = { "Cache State", tService.performer() };
        XMLGen::append_children(tKeys, tValues, tOperationNode);
    }
}
/******************************************************************************/

/******************************************************************************/
void append_update_problem_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    for (auto &tService : aXMLMetaData.services())
    {
        if (!tService.updateProblem())
        {
            continue;
        }
        auto tStageNode = aDocument.append_child("Stage");
        XMLGen::append_children( { "Name" }, { "Update Problem" }, tStageNode);
        auto tOperationNode = tStageNode.append_child("Operation");
        std::vector<std::string> tKeys = { "Name", "PerformerName" };
        std::vector<std::string> tValues = { "Update Problem", tService.performer() };
        XMLGen::append_children(tKeys, tValues, tOperationNode);
    }
}
/******************************************************************************/

/******************************************************************************/
void append_objective_value_operation
(const XMLGen::Objective& aObjective,
 pugi::xml_node &aParentNode)
{
    auto tOperationNode = aParentNode.append_child("Operation");
    XMLGen::append_children({"Name", "PerformerName"}, {"Compute Objective Value", aObjective.performer()}, tOperationNode);
    auto tOperationInput = tOperationNode.append_child("Input");
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"Topology", "Topology"}, tOperationInput);
    auto tOperationOutput = tOperationNode.append_child("Output");
    auto tOutputSharedData = std::string("Objective Value ID-") + aObjective.name;
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"Objective Value", tOutputSharedData}, tOperationOutput);
}
/******************************************************************************/

/******************************************************************************/
void append_aggregate_operation
(const std::string& aArgumentName,
 const std::string& aSharedDataName,
 const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node &aParentNode)
{
    auto tOperationNode = aParentNode.append_child("Operation");
    XMLGen::append_children({"Name", "PerformerName"}, {"AggregateEnergy", "platomain"}, tOperationNode);

    for(auto& tObjective : aXMLMetaData.objectives)
    {
        auto tOperationInput = tOperationNode.append_child("Input");
        auto tArgumentName = aArgumentName + " " + tObjective.name;
        auto tSharedDataName = aSharedDataName + " ID-" + tObjective.name;
        XMLGen::append_children({"ArgumentName", "SharedDataName"}, {tArgumentName, tSharedDataName}, tOperationInput);
    }

    auto tOperationOutput = tOperationNode.append_child("Output");
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {aArgumentName, aSharedDataName}, tOperationOutput);
}
/******************************************************************************/

/******************************************************************************/
void append_objective_value_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    // append objective value stage metadata
    auto tStageNode = aDocument.append_child("Stage");
    XMLGen::append_children( { "Name"}, { "Compute Objective Value" }, tStageNode);
    auto tStageInputNode = tStageNode.append_child("Input");
    XMLGen::append_children( { "SharedDataName" }, { "Control" }, tStageInputNode);

    XMLGen::append_filter_control_operation(tStageNode);

    //append criteria evaluations
    if(XMLGen::Analyze::is_robust_optimization_problem(aXMLMetaData))
        XMLGen::append_sample_objective_value_operation(aXMLMetaData.objectives[0].mPerformerName, aXMLMetaData, tStageNode);
    else
    {
        for(auto& tObjective : aXMLMetaData.objectives)
            XMLGen::append_objective_value_operation(tObjective, tStageNode);
        
        if(aXMLMetaData.objectives.size() > 1)
            XMLGen::append_aggregate_operation("Value", "Objective Value", aXMLMetaData, tStageNode);
    }

    if(XMLGen::Analyze::is_robust_optimization_problem(aXMLMetaData))
        XMLGen::append_evaluate_nondeterministic_objective_value_operation("Objective Value", aXMLMetaData, tStageNode);

    // append objective value stage output metadata
    //
    std::string tSharedDataName;
    if(aXMLMetaData.objectives.size() > 1 || XMLGen::Analyze::is_robust_optimization_problem(aXMLMetaData))
        tSharedDataName = std::string("Objective Value");
    else
        tSharedDataName = std::string("Objective Value ID-") + aXMLMetaData.objectives[0].name;

    auto tStageOutputNode = tStageNode.append_child("Output");
    XMLGen::append_children( { "SharedDataName" }, { tSharedDataName }, tStageOutputNode);
}
/******************************************************************************/

/******************************************************************************/
void append_objective_gradient_operation
(const XMLGen::Objective& aObjective,
 pugi::xml_node &aParentNode)
{
    auto tOperationNode = aParentNode.append_child("Operation");
    XMLGen::append_children({"Name", "PerformerName"}, {"Compute Objective Gradient", aObjective.performer()}, tOperationNode);
    auto tOperationInput = tOperationNode.append_child("Input");
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"Topology", "Topology"}, tOperationInput);
    auto tOperationOutput = tOperationNode.append_child("Output");
    auto tOutputSharedData = std::string("Objective Gradient ID-") + aObjective.name;
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"Objective Gradient", tOutputSharedData}, tOperationOutput);
}
/******************************************************************************/

/******************************************************************************/
void append_objective_gradient_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    // append objective gradient stage metadata
    auto tStageNode = aDocument.append_child("Stage");
    XMLGen::append_children({"Name"}, {"Compute Objective Gradient"}, tStageNode);
    auto tStageInputNode = tStageNode.append_child("Input");
    XMLGen::append_children({"SharedDataName"}, {"Control"}, tStageInputNode);

    XMLGen::append_filter_control_operation(tStageNode);

    if(XMLGen::Analyze::is_robust_optimization_problem(aXMLMetaData))
        XMLGen::append_sample_objective_gradient_operation(aXMLMetaData, tStageNode);
    else
    {
        for(auto& tObjective : aXMLMetaData.objectives)
            XMLGen::append_objective_gradient_operation(tObjective, tStageNode);
        if(aXMLMetaData.objectives.size() > 1)
            XMLGen::append_aggregate_operation("Field", "Objective Gradient", aXMLMetaData, tStageNode);
    }

    auto tSharedDataName = std::string("Objective Gradient");

    if(XMLGen::Analyze::is_robust_optimization_problem(aXMLMetaData))
        XMLGen::append_evaluate_nondeterministic_objective_gradient_operation(tSharedDataName, aXMLMetaData, tStageNode);

    // append filter gradient operation
    if(aXMLMetaData.objectives.size() > 1 || XMLGen::Analyze::is_robust_optimization_problem(aXMLMetaData))
        XMLGen::append_filter_criterion_gradient_operation(tSharedDataName, tSharedDataName, tStageNode);
    else
    {
        auto tInputSharedDataName = std::string("Objective Gradient ID-") + aXMLMetaData.objectives[0].name;
        XMLGen::append_filter_criterion_gradient_operation(tInputSharedDataName, tSharedDataName, tStageNode);
    }

    // append objective value stage output metadata
    auto tStageOutputNode = tStageNode.append_child("Output");
    XMLGen::append_children({"SharedDataName"}, {tSharedDataName}, tStageOutputNode);
}
/******************************************************************************/

/******************************************************************************/
void append_objective_gradient_to_plato_main_output_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    for(auto& tObjective : aXMLMetaData.objectives)
    {
        auto tInput = aParentNode.append_child("Input");
        auto tSharedDataName = std::string("Objective Gradient ID-") + tObjective.name;
        auto tArgumentName  = XMLGen::to_lower(tSharedDataName);
        XMLGen::append_children({"ArgumentName", "SharedDataName"}, {tArgumentName, tSharedDataName}, tInput);
    }
}
// function append_objective_gradient_to_plato_main_output_stage
/******************************************************************************/

/******************************************************************************/
void append_constraint_gradient_to_plato_main_output_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    for(auto& tConstraint : aXMLMetaData.constraints)
    {
        auto tInput = aParentNode.append_child("Input");
        auto tSharedDataName = std::string("Constraint Gradient ID-") + tConstraint.name();
        auto tArgumentName  = XMLGen::to_lower(tSharedDataName);
        XMLGen::append_children({"ArgumentName", "SharedDataName"}, {tArgumentName, tSharedDataName}, tInput);
    }
}
// function append_constraint_gradient_to_plato_main_output_stage
/******************************************************************************/

/******************************************************************************/
inline void append_qoi_to_random_write_output_operation
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::ValidPerformerOutputKeys tValidKeys;
    auto tServiceID = aMetaData.mOutputMetaData.serviceID();
    auto tCodeName = aMetaData.service(tServiceID).code();
    auto tOutputQoIs = aMetaData.mOutputMetaData.randomIDs();
    for(auto& tQoI : tOutputQoIs)
    {
        auto tOutput = aParentNode.append_child("Output");
        auto tArgumentName = tValidKeys.argument(tCodeName, tQoI);
        auto tSharedDataName = aMetaData.mOutputMetaData.randomSharedDataName(tQoI);
        XMLGen::append_children({"ArgumentName", "SharedDataName"}, {tArgumentName, tSharedDataName}, tOutput);
    }
}
// function append_random_write_output_operation
/******************************************************************************/

/******************************************************************************/
inline void append_random_write_output_operation
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    if(aMetaData.mOutputMetaData.randomIDs().empty())
    {
        return;
    }

    auto tForNode = aParentNode.append_child("For");
    XMLGen::append_attributes( { "var", "in" }, { "PerformerSampleIndex", "PerformerSamples" }, tForNode);
    auto tOperationNode = tForNode.append_child("Operation");
    tForNode = tOperationNode.append_child("For");
    XMLGen::append_attributes( { "var", "in" }, { "PerformerIndex", "Performers" }, tForNode);

    tOperationNode = tForNode.append_child("Operation");
    auto tServiceID = aMetaData.mOutputMetaData.serviceID();
    auto tBasePerformerName = aMetaData.service(tServiceID).performer();
    auto tPerformerName = tBasePerformerName + "_{PerformerIndex}";
    XMLGen::append_children( { "Name", "PerformerName" }, { "Write Output", tPerformerName }, tOperationNode);
    XMLGen::append_qoi_to_random_write_output_operation(aMetaData, tOperationNode);
}
// function append_random_qoi_outputs
/******************************************************************************/

/******************************************************************************/
inline void append_qoi_to_deterministic_write_output_operation
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    XMLGen::ValidPerformerOutputKeys tValidKeys;
    auto tServiceID = aMetaData.mOutputMetaData.serviceID();
    auto tCodeName = aMetaData.service(tServiceID).code();
    auto tOutputQoIs = aMetaData.mOutputMetaData.deterministicIDs();
    for(auto& tQoI : tOutputQoIs)
    {
        auto tOutput = aParentNode.append_child("Output");
        auto tArgumentName = tValidKeys.argument(tCodeName, tQoI);
        auto tSharedDataName = aMetaData.mOutputMetaData.deterministicSharedDataName(tQoI);
        XMLGen::append_children({"ArgumentName", "SharedDataName"}, {tArgumentName, tSharedDataName}, tOutput);
    }
}
// function append_deterministic_qoi_outputs

/******************************************************************************/
inline void append_deterministic_write_output_operation
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    if(aMetaData.mOutputMetaData.deterministicIDs().empty())
    {
        return;
    }

    auto tOperationNode = aParentNode.append_child("Operation");
    auto tServiceID = aMetaData.mOutputMetaData.serviceID();
    auto tPerformerName = aMetaData.service(tServiceID).performer();
    XMLGen::append_children( { "Name", "PerformerName" }, { "Write Output", tPerformerName }, tOperationNode);
    XMLGen::append_qoi_to_deterministic_write_output_operation(aMetaData, tOperationNode);
}
// function append_random_qoi_outputs
/******************************************************************************/

/******************************************************************************/
void append_write_ouput_operation
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    if(aMetaData.mOutputMetaData.isOutputDisabled())
    {
        return;
    }
    XMLGen::append_random_write_output_operation(aMetaData, aParentNode);
    XMLGen::append_deterministic_write_output_operation(aMetaData, aParentNode);
}
// function append_write_ouput_operation
/******************************************************************************/

/******************************************************************************/
inline void append_default_qoi_to_plato_main_output_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tInput = aParentNode.append_child("Input");
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"topology", "Topology"}, tInput);
    tInput = aParentNode.append_child("Input");
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"control", "Control"}, tInput);
    XMLGen::append_objective_gradient_to_plato_main_output_stage(aXMLMetaData, aParentNode);
    XMLGen::append_constraint_gradient_to_plato_main_output_stage(aXMLMetaData, aParentNode);
}
// function append_default_qoi_to_plato_main_output_stage
/******************************************************************************/

/******************************************************************************/
inline void append_random_qoi_samples_to_plato_main_output_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    if(aXMLMetaData.mOutputMetaData.outputSamples() == false)
    {
        return;
    }

    auto tQoIIDs = aXMLMetaData.mOutputMetaData.randomIDs();
    for(auto& tID : tQoIIDs)
    {
        auto tOuterFor = aParentNode.append_child("For");
        XMLGen::append_attributes({"var", "in"}, {"PerformerIndex", "Performers"}, tOuterFor);
        auto tInnerFor = tOuterFor.append_child("For");
        XMLGen::append_attributes({"var", "in"}, {"PerformerSampleIndex", "PerformerSamples"}, tInnerFor);
        auto tInput = tInnerFor.append_child("Input");
        auto tArgumentName = aXMLMetaData.mOutputMetaData.randomArgumentName(tID);
        auto tSharedDataName = aXMLMetaData.mOutputMetaData.randomSharedDataName(tID);
        XMLGen::append_children({"ArgumentName", "SharedDataName"}, {tArgumentName, tSharedDataName}, tInput);
    }
}
// function append_random_qoi_samples_to_plato_main_output_stage
/******************************************************************************/

/******************************************************************************/
inline void append_random_qoi_statistics_to_plato_main_output_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tQoIIDs = aXMLMetaData.mOutputMetaData.randomIDs();
    for (auto &tID : tQoIIDs)
    {
        auto tMeanName = tID + " mean";
        auto tInput = aParentNode.append_child("Input");
        XMLGen::append_children( { "ArgumentName", "SharedDataName" }, { tMeanName, tMeanName }, tInput);

        auto tStdDevName = tID + " standard deviation";
        tInput = aParentNode.append_child("Input");
        XMLGen::append_children( { "ArgumentName", "SharedDataName" }, { tStdDevName, tStdDevName }, tInput);
    }
}
// function append_random_qoi_statistics_to_plato_main_output_stage
/******************************************************************************/

/******************************************************************************/
inline void append_deterministic_qoi_to_plato_main_output_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tQoIIDs = aXMLMetaData.mOutputMetaData.deterministicIDs();
    for(auto& tID : tQoIIDs)
    {
        auto tInput = aParentNode.append_child("Input");
        auto tArgumentName = aXMLMetaData.mOutputMetaData.deterministicArgumentName(tID);
        auto tSharedDataName = aXMLMetaData.mOutputMetaData.deterministicSharedDataName(tID);
        XMLGen::append_children({"ArgumentName", "SharedDataName"}, {tArgumentName, tSharedDataName}, tInput);
    }
}
// function append_deterministic_qoi_to_plato_main_output_stage
/******************************************************************************/

/******************************************************************************/
inline void append_platomain_output_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tOperation = aParentNode.append_child("Operation");
    XMLGen::append_children({"Name", "PerformerName"},{"PlatoMainOutput", "platomain"}, tOperation);
    XMLGen::append_default_qoi_to_plato_main_output_stage(aXMLMetaData, tOperation);
    XMLGen::append_deterministic_qoi_to_plato_main_output_stage(aXMLMetaData, tOperation);
    XMLGen::append_random_qoi_samples_to_plato_main_output_stage(aXMLMetaData, tOperation);
    XMLGen::append_random_qoi_statistics_to_plato_main_output_stage(aXMLMetaData, tOperation);
}
/******************************************************************************/

/******************************************************************************/
void append_plato_main_output_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    if(aXMLMetaData.mOutputMetaData.isOutputDisabled())
    {
        return;
    }
    auto tOutputStage = aDocument.append_child("Stage");
    XMLGen::append_children({"Name"}, {"Output To File"}, tOutputStage);
    XMLGen::append_write_ouput_operation(aXMLMetaData, tOutputStage);
    XMLGen::append_compute_qoi_statistics_operation(aXMLMetaData, tOutputStage);
    XMLGen::append_platomain_output_operation(aXMLMetaData, tOutputStage);
}
// function append_plato_main_output_stage
/******************************************************************************/

/******************************************************************************/
void append_lower_bounds_shared_data
(pugi::xml_document& aDocument)
{
    // shared data - lower bound value
    std::vector<std::string> tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName"};
    std::vector<std::string> tValues = {"Lower Bound Value", "Scalar", "Global", "1", "platomain", "platomain"};
    auto tSharedDataNode = aDocument.append_child("SharedData");
    XMLGen::append_children(tKeys, tValues, tSharedDataNode);

    // shared data - lower bound vector
    tValues = {"Lower Bound Vector", "Scalar", "Nodal Field", "IGNORE", "platomain", "platomain"};
    tSharedDataNode = aDocument.append_child("SharedData");
    XMLGen::append_children(tKeys, tValues, tSharedDataNode);
}
// function append_lower_bounds_shared_data
/******************************************************************************/

/******************************************************************************/
void append_upper_bounds_shared_data
(pugi::xml_document& aDocument)
{
    // shared data - upper bound value
    std::vector<std::string> tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName"};
    std::vector<std::string> tValues = {"Upper Bound Value", "Scalar", "Global", "1", "platomain", "platomain"};
    auto tSharedDataNode = aDocument.append_child("SharedData");
    XMLGen::append_children(tKeys, tValues, tSharedDataNode);

    // shared data - upper bound vector
    tValues = {"Upper Bound Vector", "Scalar", "Nodal Field", "IGNORE", "platomain", "platomain"};
    tSharedDataNode = aDocument.append_child("SharedData");
    XMLGen::append_children(tKeys, tValues, tSharedDataNode);
}
// function append_upper_bounds_shared_data
/******************************************************************************/

/******************************************************************************/
void append_design_volume_shared_data
(pugi::xml_document& aDocument)
{
    std::vector<std::string> tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName"};
    std::vector<std::string> tValues = {"Design Volume", "Scalar", "Global", "1", "platomain", "platomain"};
    auto tSharedDataNode = aDocument.append_child("SharedData");
    XMLGen::append_children(tKeys, tValues, tSharedDataNode);
}
// function append_design_volume_shared_data
/******************************************************************************/

/******************************************************************************/
void append_objective_shared_data
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument,
 std::string aOwnerName)
{
    if(aXMLMetaData.objectives.empty())
    {
        THROWERR("Append Criterion Shared Data: Objective function list is empty. "
            + "Plato optimization problems must have at least one objective function defined.")
    }

    if(!XMLGen::Analyze::is_robust_optimization_problem(aXMLMetaData))
    {
        for(auto& tObjective : aXMLMetaData.objectives)
        {
            // shared data - deterministic criterion value
            auto tTag = std::string("Objective Value ID-") + tObjective.name;
            auto tOwnerName = aOwnerName.empty() ? tObjective.mPerformerName : aOwnerName;
            std::vector<std::string> tKeys = { "Name", "Type", "Layout", "Size", "OwnerName", "UserName" };
            std::vector<std::string> tValues = { tTag, "Scalar", "Global", "1", tOwnerName, "platomain" };
            auto tSharedDataNode = aDocument.append_child("SharedData");
            XMLGen::append_children(tKeys, tValues, tSharedDataNode);

            // shared data - deterministic criterion gradient
            tTag = std::string("Objective Gradient ID-") + tObjective.name;
            tValues = { tTag, "Scalar", "Nodal Field", "IGNORE", tOwnerName, "platomain" };
            tSharedDataNode = aDocument.append_child("SharedData");
            XMLGen::append_children(tKeys, tValues, tSharedDataNode);
        }
    }

    // shared data for aggregated/filtered values and gradients
    if(aXMLMetaData.objectives.size() > 1 || XMLGen::Analyze::is_robust_optimization_problem(aXMLMetaData))
    {
        std::vector<std::string> tKeys = { "Name", "Type", "Layout", "Size", "OwnerName", "UserName" };
        std::vector<std::string> tValues = {"Objective Value", "Scalar", "Global", "1", "platomain", "platomain" };
        auto tSharedDataNode = aDocument.append_child("SharedData");
        XMLGen::append_children(tKeys, tValues, tSharedDataNode);
    }

    std::vector<std::string> tKeys = { "Name", "Type", "Layout", "OwnerName", "UserName" };
    std::vector<std::string> tValues = { "Objective Gradient", "Scalar", "Nodal Field", "platomain", "platomain" };
    auto tSharedDataNode = aDocument.append_child("SharedData");
    XMLGen::append_children(tKeys, tValues, tSharedDataNode);
}
// function append_objective_shared_data
/******************************************************************************/

/******************************************************************************/
void append_constraint_shared_data
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument,
 std::string aOwnerName)
{
    for(auto& tConstraint : aXMLMetaData.constraints)
    {
        // shared data - deterministic criterion value
        auto tTag = std::string("Constraint Value ID-") + tConstraint.name();
        auto tOwnerName = aOwnerName.empty() ? tConstraint.performer() : aOwnerName;
        std::vector<std::string> tKeys = { "Name", "Type", "Layout", "Size", "OwnerName", "UserName" };
        std::vector<std::string> tValues = { tTag, "Scalar", "Global", "1", tOwnerName, "platomain" };
        auto tSharedDataNode = aDocument.append_child("SharedData");
        XMLGen::append_children(tKeys, tValues, tSharedDataNode);

        // shared data - deterministic criterion gradient
        tTag = std::string("Constraint Gradient ID-") + tConstraint.name();
        tValues = { tTag, "Scalar", "Nodal Field", "IGNORE", tOwnerName, "platomain" };
        tSharedDataNode = aDocument.append_child("SharedData");
        XMLGen::append_children(tKeys, tValues, tSharedDataNode);
    }
}
// function append_constraint_shared_data
/******************************************************************************/

/******************************************************************************/
void append_control_shared_data
(pugi::xml_document& aDocument)
{
    auto tSharedData = aDocument.append_child("SharedData");
    std::vector<std::string> tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName"};
    std::vector<std::string> tValues = {"Control", "Scalar", "Nodal Field", "IGNORE", "platomain", "platomain"};
    XMLGen::append_children(tKeys, tValues, tSharedData);
}
// function append_control_shared_data
/******************************************************************************/

/******************************************************************************/
void append_plato_main_performer
(pugi::xml_document& aDocument)
{
    auto tPerformerNode = aDocument.append_child("Performer");
    XMLGen::append_children( {"Name", "Code", "PerformerID"}, {"platomain", "platomain", "0"}, tPerformerNode);
}
// function append_plato_main_performer
/******************************************************************************/

/******************************************************************************/
void append_filter_control_operation
(pugi::xml_node& aParentNode)
{
    auto tOperationNode = aParentNode.append_child("Operation");
    XMLGen::append_children({"Name", "PerformerName"},{"Filter Control", "platomain"}, tOperationNode);
    auto tInputNode = tOperationNode.append_child("Input");
    XMLGen::append_children({"ArgumentName", "SharedDataName"},{"Field", "Control"}, tInputNode);
    auto tOutputNode = tOperationNode.append_child("Output");
    XMLGen::append_children({"ArgumentName", "SharedDataName"},{"Filtered Field", "Topology"}, tOutputNode);
}
// function append_filter_control_operation
/******************************************************************************/

/******************************************************************************/
void append_filter_criterion_gradient_operation
(const std::string& aInputSharedDataName,
 const std::string& aOutputSharedDataName,
 pugi::xml_node& aParentNode)
{
    auto tOperationNode = aParentNode.append_child("Operation");
    XMLGen::append_children({"Name", "PerformerName"},{"Filter Gradient", "platomain"}, tOperationNode);
    auto tInputNode = tOperationNode.append_child("Input");
    XMLGen::append_children({"ArgumentName", "SharedDataName"},{"Field", "Control"}, tInputNode);
    tInputNode = tOperationNode.append_child("Input");
    XMLGen::append_children({"ArgumentName", "SharedDataName"},{"Gradient", aInputSharedDataName}, tInputNode);
    auto tOutputNode = tOperationNode.append_child("Output");
    XMLGen::append_children({"ArgumentName", "SharedDataName"},{"Filtered Gradient", aOutputSharedDataName}, tOutputNode);
}
// function append_filter_criterion_gradient_operation
/******************************************************************************/

/******************************************************************************/
void append_initial_field_operation
(pugi::xml_node& aParentNode)
{
    auto tOperationNode = aParentNode.append_child("Operation");
    XMLGen::append_children({"Name", "PerformerName"},{"Initialize Field", "platomain"}, tOperationNode);
    auto tOutputNode = tOperationNode.append_child("Output");
    XMLGen::append_children({"ArgumentName", "SharedDataName"},{"Initialized Field", "Control"}, tOutputNode);
}
// function append_initial_field_operation
/******************************************************************************/

/******************************************************************************/
void append_initial_guess_stage
(pugi::xml_document& aDocument)
{
    auto tStageNode = aDocument.append_child("Stage");
    XMLGen::append_children({"Name"},{"Initial Guess"}, tStageNode);
    XMLGen::append_initial_field_operation(tStageNode);
    auto tOutputNode = tStageNode.append_child("Output");
    XMLGen::append_children({"SharedDataName"},{"Control"}, tOutputNode);
}
// function append_initial_guess_stage
/******************************************************************************/

/******************************************************************************/
void append_lower_bound_operation
(pugi::xml_node& aParentNode)
{
    auto tOperationNode = aParentNode.append_child("Operation");
    XMLGen::append_children({"Name", "PerformerName"}, {"Compute Lower Bounds", "platomain"}, tOperationNode);
    auto tInputNode = tOperationNode.append_child("Input");
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"Lower Bound Value", "Lower Bound Value"}, tInputNode);
    auto tOutputNode = tOperationNode.append_child("Output");
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"Lower Bound Vector", "Lower Bound Vector"}, tOutputNode);
}
// function append_lower_bound_operation
/******************************************************************************/

/******************************************************************************/
void append_lower_bound_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    auto tStageNode = aDocument.append_child("Stage");
    XMLGen::append_children({"Name"}, {"Set Lower Bounds"}, tStageNode);
    auto tLower = Plato::tolower(aXMLMetaData.optimization_type);
    if(tLower.compare("topology") == 0)
    {
        auto tInputNode = tStageNode.append_child("Input");
        XMLGen::append_children({"SharedDataName"}, {"Lower Bound Value"}, tInputNode);
        XMLGen::append_lower_bound_operation(tStageNode);
    }
    auto tOutputNode = tStageNode.append_child("Output");
    XMLGen::append_children({"SharedDataName"}, {"Lower Bound Vector"}, tOutputNode);
}
// function append_lower_bound_stage
/******************************************************************************/

/******************************************************************************/
void append_upper_bound_operation
(pugi::xml_node& aParentNode)
{
    auto tOperationNode = aParentNode.append_child("Operation");
    XMLGen::append_children({"Name", "PerformerName"}, {"Compute Upper Bounds", "platomain"}, tOperationNode);
    auto tInputNode = tOperationNode.append_child("Input");
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"Upper Bound Value", "Upper Bound Value"}, tInputNode);
    auto tOutputNode = tOperationNode.append_child("Output");
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"Upper Bound Vector", "Upper Bound Vector"}, tOutputNode);
}
// function append_upper_bound_operation
/******************************************************************************/

/******************************************************************************/
void append_upper_bound_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    auto tStageNode = aDocument.append_child("Stage");
    XMLGen::append_children({"Name"}, {"Set Upper Bounds"}, tStageNode);
    auto tLower = Plato::tolower(aXMLMetaData.optimization_type);
    if(tLower.compare("topology") == 0)
    {
        auto tInputNode = tStageNode.append_child("Input");
        XMLGen::append_children({"SharedDataName"}, {"Upper Bound Value"}, tInputNode);
        XMLGen::append_upper_bound_operation(tStageNode);
    }
    auto tOutputNode = tStageNode.append_child("Output");
    XMLGen::append_children({"SharedDataName"}, {"Upper Bound Vector"}, tOutputNode);
}
// function append_upper_bound_stage
/******************************************************************************/

/******************************************************************************/
void append_design_volume_operation
(pugi::xml_node& aParentNode)
{
    auto tOperationNode = aParentNode.append_child("Operation");
    XMLGen::append_children({"Name", "PerformerName"}, {"Design Volume", "platomain"}, tOperationNode);
    auto tOutputNode = tOperationNode.append_child("Output");
    XMLGen::append_children({"ArgumentName", "SharedDataName"}, {"Design Volume", "Design Volume"}, tOutputNode);
}
// function append_design_volume_stage
/******************************************************************************/

/******************************************************************************/
void append_design_volume_stage
(pugi::xml_document& aDocument)
{
    auto tStageNode = aDocument.append_child("Stage");
    XMLGen::append_children({"Name"}, {"Design Volume"}, tStageNode);
    XMLGen::append_design_volume_operation(tStageNode);
    auto tOutputNode = tStageNode.append_child("Output");
    XMLGen::append_children({"SharedDataName"}, {"Design Volume"}, tOutputNode);
}
// function append_design_volume_stage
/******************************************************************************/

/******************************************************************************/
void append_constraint_value_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    XMLGen::ConstraintValueOperation tValueOperationInterface;
    for(auto& tConstraint : aXMLMetaData.constraints)
    {
        auto tStageNode = aDocument.append_child("Stage");
        auto tStageName = std::string("Compute Constraint Value ID-") + tConstraint.name();
        XMLGen::append_children({"Name", "Type"}, {tStageName, tConstraint.category()}, tStageNode);
        auto tInputNode = tStageNode.append_child("Input");
        XMLGen::append_children({"SharedDataName"}, {"Control"}, tInputNode);

        XMLGen::append_filter_control_operation(tStageNode);
        tValueOperationInterface.call(tConstraint, tStageNode);

        auto tOutputNode = tStageNode.append_child("Output");
        auto tSharedDataName = std::string("Constraint Value ID-") + tConstraint.name();
        XMLGen::append_children({"SharedDataName"}, {tSharedDataName}, tOutputNode);
    }
}
// function append_constraint_value_stage
/******************************************************************************/

/******************************************************************************/
void append_constraint_gradient_stage
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument)
{
    XMLGen::ConstraintGradientOperation tGradOperationInterface;
    for(auto& tConstraint : aXMLMetaData.constraints)
    {
        auto tStageNode = aDocument.append_child("Stage");
        auto tStageName = std::string("Compute Constraint Gradient ID-") + tConstraint.name();
        XMLGen::append_children({"Name", "Type"}, {tStageName, tConstraint.category()}, tStageNode);
        auto tInputNode = tStageNode.append_child("Input");
        XMLGen::append_children({"SharedDataName"}, {"Control"}, tInputNode);

        XMLGen::append_filter_control_operation(tStageNode);
        tGradOperationInterface.call(tConstraint, tStageNode);
        auto tSharedDataName = std::string("Constraint Gradient ID-") + tConstraint.name();
        XMLGen::append_filter_criterion_gradient_operation(tSharedDataName, tSharedDataName, tStageNode);

        auto tOutputNode = tStageNode.append_child("Output");
        XMLGen::append_children({"SharedDataName"}, {tSharedDataName}, tOutputNode);
    }
}
// function append_constraint_gradient_stage
/******************************************************************************/

/******************************************************************************/
void append_derivative_checker_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tCheckGradient = aXMLMetaData.check_gradient.empty() ? std::string("true") : aXMLMetaData.check_gradient;
    std::vector<std::string> tKeys = {"CheckGradient", "CheckHessian", "UseUserInitialGuess"};
    std::vector<std::string> tValues = {tCheckGradient, aXMLMetaData.check_hessian, "True"};
    XMLGen::append_children(tKeys, tValues, aParentNode);

    auto tOptionsNode = aParentNode.append_child("Options");
    tKeys = {"DerivativeCheckerInitialSuperscript", "DerivativeCheckerFinalSuperscript"};
    tValues = {aXMLMetaData.mDerivativeCheckerInitialSuperscript, aXMLMetaData.mDerivativeCheckerFinalSuperscript};
    XMLGen::append_children(tKeys, tValues, tOptionsNode);
}
// function append_derivative_checker_options
/******************************************************************************/

/******************************************************************************/
void append_optimality_criteria_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tConvergenceNode = aParentNode.append_child("Convergence");
    XMLGen::append_children({"MaxIterations"}, {aXMLMetaData.max_iterations}, tConvergenceNode);
}
// function append_optimality_criteria_options
/******************************************************************************/

/******************************************************************************/
void append_method_moving_asymptotes_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys = {"MaxNumOuterIterations", "MoveLimit", "AsymptoteExpansion", "AsymptoteContraction",
         "MaxNumSubProblemIter", "ControlStagnationTolerance", "ObjectiveStagnationTolerance"};
    std::vector<std::string> tValues = {aXMLMetaData.max_iterations, aXMLMetaData.mMMAMoveLimit, aXMLMetaData.mMMAAsymptoteExpansion,
        aXMLMetaData.mMMAAsymptoteContraction, aXMLMetaData.mMMAMaxNumSubProblemIterations, aXMLMetaData.mMMAControlStagnationTolerance,
        aXMLMetaData.mMMAObjectiveStagnationTolerance};
    XMLGen::set_value_keyword_to_ignore_if_empty(tValues);
    auto tOptionsNode = aParentNode.append_child("Options");
    XMLGen::append_children(tKeys, tValues, tOptionsNode);
}
// function append_method_moving_asymptotes_options
/******************************************************************************/

/******************************************************************************/
void append_trust_region_kelley_sachs_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys = {"KSTrustRegionExpansionFactor", "KSTrustRegionContractionFactor", "KSMaxTrustRegionIterations", "KSInitialRadiusScale",
         "KSMaxRadiusScale", "HessianType", "MinTrustRegionRadius", "LimitedMemoryStorage", "KSOuterGradientTolerance", "KSOuterStationarityTolerance",
         "KSOuterStagnationTolerance", "KSOuterControlStagnationTolerance", "KSOuterActualReductionTolerance", "ProblemUpdateFrequency", "DisablePostSmoothing",
         "KSTrustRegionRatioLow", "KSTrustRegionRatioMid", "KSTrustRegionRatioUpper"};
    std::vector<std::string> tValues = {aXMLMetaData.mTrustRegionExpansionFactor, aXMLMetaData.mTrustRegionContractionFactor, aXMLMetaData.mMaxTrustRegionIterations,
        aXMLMetaData.mInitialRadiusScale, aXMLMetaData.mMaxRadiusScale, aXMLMetaData.mHessianType, aXMLMetaData.mMinTrustRegionRadius, aXMLMetaData.mLimitedMemoryStorage,
        aXMLMetaData.mOuterGradientToleranceKS, aXMLMetaData.mOuterStationarityToleranceKS, aXMLMetaData.mOuterStagnationToleranceKS, aXMLMetaData.mOuterControlStagnationToleranceKS,
        aXMLMetaData.mOuterActualReductionToleranceKS, aXMLMetaData.mProblemUpdateFrequency, aXMLMetaData.mDisablePostSmoothingKS, aXMLMetaData.mTrustRegionRatioLowKS,
        aXMLMetaData.mTrustRegionRatioMidKS, aXMLMetaData.mTrustRegionRatioUpperKS};
    XMLGen::set_value_keyword_to_ignore_if_empty(tValues);
    auto tOptionsNode = aParentNode.append_child("Options");
    XMLGen::append_children(tKeys, tValues, tOptionsNode);
}
// function append_trust_region_kelley_sachs_options
/******************************************************************************/

/******************************************************************************/
void append_optimization_algorithm_parameters_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tLower = XMLGen::to_lower(aXMLMetaData.optimization_algorithm);
    if(tLower.compare("oc") == 0)
    {
        XMLGen::append_optimality_criteria_options(aXMLMetaData, aParentNode);
    }
    else if(tLower.compare("mma") == 0)
    {
        XMLGen::append_method_moving_asymptotes_options(aXMLMetaData, aParentNode);
    }
    else if(tLower.compare("ksbc") == 0)
    {
        XMLGen::append_trust_region_kelley_sachs_options(aXMLMetaData, aParentNode);
    }
    else if(tLower.compare("ksal") == 0)
    {
        XMLGen::append_trust_region_kelley_sachs_options(aXMLMetaData, aParentNode);
    }
    else if(tLower.compare("derivativechecker") == 0)
    {
        XMLGen::append_derivative_checker_options(aXMLMetaData, aParentNode);
    }
    else
    {
        THROWERR(std::string("Append Optimization Algorithm Options: ") + "Optimization algorithm '"
            + aXMLMetaData.optimization_algorithm + "' is not supported.")
    }
}
// function append_optimization_algorithm_parameters_options
/******************************************************************************/

/******************************************************************************/
void append_optimization_algorithm_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    std::unordered_map<std::string, std::string> tValidOptimizers =
        { {"oc", "OC"}, {"mma", "MMA"}, {"ksbc", "KSBC"}, {"ksal", "KSAL"} ,
          {"rol ksbc", "ROL KSBC"}, {"rol ksal", "ROL KSAL"}, {"derivativechecker", "DerivativeChecker"} };

    auto tLower = Plato::tolower(aXMLMetaData.optimization_algorithm);
    auto tOptimizerItr = tValidOptimizers.find(tLower);
    if(tOptimizerItr == tValidOptimizers.end())
    {
        THROWERR(std::string("Append Optimization Algorithm Option: Optimization algorithm '")
            + aXMLMetaData.optimization_algorithm + "' is not supported.")
    }

    XMLGen::append_children( { "Package" }, { tOptimizerItr->second }, aParentNode);
    XMLGen::append_optimization_algorithm_parameters_options(aXMLMetaData, aParentNode);
}
// function append_optimization_algorithm_options
/******************************************************************************/

/******************************************************************************/
void append_optimization_output_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    if(aXMLMetaData.mOutputMetaData.isOutputDisabled())
    {
        return;
    }
    auto tNode = aParentNode.append_child("Output");
    append_children({"OutputStage"}, {"Output To File"}, tNode);
}
// function append_optimization_output_options
/******************************************************************************/

/******************************************************************************/
void append_optimization_cache_stage_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    if(!aXMLMetaData.service(0u).cacheState())
    {
        return;
    }
    auto tNode = aParentNode.append_child("CacheStage");
    append_children({"Name"}, {"Cache State"}, tNode);
}
// function append_optimization_cache_stage_options
/******************************************************************************/

/******************************************************************************/
void append_optimization_update_problem_stage_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    if(!aXMLMetaData.service(0u).updateProblem())
    {
        return;
    }
    auto tNode = aParentNode.append_child("UpdateProblemStage");
    append_children({"Name"}, {"Update Problem"}, tNode);
}
// function append_optimization_cache_stage_options
/******************************************************************************/

/******************************************************************************/
void append_optimization_variables_options
(pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys =
        {"ValueName", "InitializationStage", "FilteredName", "LowerBoundValueName", "LowerBoundVectorName",
         "UpperBoundValueName", "UpperBoundVectorName", "SetLowerBoundsStage", "SetUpperBoundsStage"};
    std::vector<std::string> tValues =
        {"Control", "Initial Guess", "Topology", "Lower Bound Value", "Lower Bound Vector",
         "Upper Bound Value", "Upper Bound Vector", "Set Lower Bounds", "Set Upper Bounds"};
    auto tNode = aParentNode.append_child("OptimizationVariables");
    XMLGen::append_children(tKeys, tValues, tNode);
}
// function append_optimization_variables_options
/******************************************************************************/

/******************************************************************************/
void append_optimization_objective_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    std::unordered_map<std::string, std::string> tKeyToValueMap =
        { {"ValueName", ""}, {"ValueStageName", ""}, {"GradientName", ""}, {"GradientStageName", ""} };

    if(aXMLMetaData.objectives.size() > 1 || XMLGen::Analyze::is_robust_optimization_problem(aXMLMetaData))
        tKeyToValueMap.find("ValueName")->second = std::string("Objective Value");
    else
    {
        auto tValueName = std::string("Objective Value ID-") + aXMLMetaData.objectives[0].name;
        tKeyToValueMap.find("ValueName")->second = tValueName;
    }
    tKeyToValueMap.find("ValueStageName")->second = std::string("Compute Objective Value");
    tKeyToValueMap.find("GradientName")->second = std::string("Objective Gradient");
    tKeyToValueMap.find("GradientStageName")->second = std::string("Compute Objective Gradient");

    auto tKeys = XMLGen::transform_key_tokens(tKeyToValueMap);
    auto tValues = XMLGen::transform_value_tokens(tKeyToValueMap);
    auto tNode = aParentNode.append_child("Objective");
    XMLGen::append_children(tKeys, tValues, tNode);
}
// function append_optimization_objective_options
/******************************************************************************/

/******************************************************************************/
void append_optimization_constraint_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    std::unordered_map<std::string, std::string> tKeyToValueMap =
        { {"ValueName", ""}, {"ValueStageName", ""}, {"GradientName", ""}, {"GradientStageName", ""},
          {"ReferenceValueName", "Design Volume"}, {"NormalizedTargetValue", ""}, {"AbsoluteTargetValue", ""} };

    for (auto &tConstraint : aXMLMetaData.constraints)
    {
        tKeyToValueMap.find("ValueName")->second = std::string("Constraint Value ID-") + tConstraint.name();
        tKeyToValueMap.find("ValueStageName")->second = std::string("Compute Constraint Value ID-") + tConstraint.name();
        tKeyToValueMap.find("GradientName")->second = std::string("Constraint Gradient ID-") + tConstraint.name();
        tKeyToValueMap.find("GradientStageName")->second = std::string("Compute Constraint Gradient ID-") + tConstraint.name();
        XMLGen::set_key_value("AbsoluteTargetValue", tConstraint.absoluteTarget(), tKeyToValueMap);
        XMLGen::set_key_value("NormalizedTargetValue", tConstraint.normalizedTarget(), tKeyToValueMap);

        auto tKeys = XMLGen::transform_key_tokens(tKeyToValueMap);
        auto tValues = XMLGen::transform_value_tokens(tKeyToValueMap);
        auto tNode = aParentNode.append_child("Constraint");
        XMLGen::append_children(tKeys, tValues, tNode);
    }
}
// function append_optimization_constraint_options
/******************************************************************************/

/******************************************************************************/
void append_optimization_bound_constraints_options
(const std::vector<std::string>& aValues,
 pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys = {"Upper", "Lower"};
    auto tNode = aParentNode.append_child("BoundConstraint");
    XMLGen::append_children(tKeys, aValues, tNode);
}
// function append_optimization_bound_constraints_options
/******************************************************************************/

/******************************************************************************/
void append_optimizer_options
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode)
{
    auto tOptimizerNode = aParentNode.append_child("Optimizer");
    XMLGen::append_optimization_algorithm_options(aXMLMetaData, tOptimizerNode);
    XMLGen::append_optimization_update_problem_stage_options(aXMLMetaData, tOptimizerNode);
    XMLGen::append_optimization_cache_stage_options(aXMLMetaData, tOptimizerNode);
    XMLGen::append_optimization_output_options(aXMLMetaData, tOptimizerNode);
    XMLGen::append_optimization_variables_options(tOptimizerNode);
    XMLGen::append_optimization_objective_options(aXMLMetaData, tOptimizerNode);
    XMLGen::append_optimization_constraint_options(aXMLMetaData, tOptimizerNode);
    XMLGen::append_optimization_bound_constraints_options({"1.0", "0.0"}, tOptimizerNode);
}
// function append_optimizer_options
/******************************************************************************/

}
// namespace XMLGen
