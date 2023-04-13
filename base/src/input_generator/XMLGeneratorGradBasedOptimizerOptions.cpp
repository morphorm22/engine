/*
 * XMLGeneratorGradBasedOptimizerOptions.cpp
 *
 *  Created on: Jan 17, 2022
 */

#include "XMLG_Macros.hpp"
#include "XMLGeneratorUtilities.hpp"
#include "XMLGeneratorServiceUtilities.hpp"
#include "XMLGeneratorGradBasedOptimizerOptions.hpp"

namespace XMLGen
{

namespace Private
{

/******************************************************************************/
std::string get_constraint_reference_value_name
(const XMLGen::InputData& aMetaData,
 const XMLGen::Constraint &aConstraint)
{
    std::string tReturn = "";
    
    auto tCriterion = aMetaData.criterion(aConstraint.criterion());
    if(tCriterion.type() == "volume")
    {
        tReturn = "Design Volume";
    }
    return tReturn; 
}

}
// namespace Private

/******************************************************************************/
void append_grad_based_optimizer_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    std::unordered_map<std::string, std::string> tValidOptimizers =
        { {"oc", "OC"}, {"mma", "MMA"}, {"ksbc", "KSBC"}, {"ksal", "KSAL"} };

    auto tLower = Plato::tolower(aMetaData.optimization_parameters().optimization_algorithm());
    auto tOptimizerItr = tValidOptimizers.find(tLower);
    if(tOptimizerItr == tValidOptimizers.end())
    {
        THROWERR(std::string("Append Optimization Algorithm Option: Optimization algorithm '")
            + aMetaData.optimization_parameters().optimization_algorithm() + "' is not supported.")
    }

    XMLGen::append_children( { "Package" }, { tOptimizerItr->second }, aParentNode);
    XMLGen::append_grad_based_optimizer_parameters(aMetaData, aParentNode);
}
// function append_grad_based_optimizer_options
/******************************************************************************/

/******************************************************************************/
void append_grad_based_optimizer_parameters
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    auto tLower = XMLGen::to_lower(aMetaData.optimization_parameters().optimization_algorithm());
    
    if(tLower.compare("oc") == 0)
    {
        XMLGen::append_optimality_criteria_options(aMetaData, aParentNode);
    }
    else if(tLower.compare("mma") == 0)
    {
        XMLGen::append_method_moving_asymptotes_options(aMetaData, aParentNode);
    }
    else if(tLower.compare("ksbc") == 0)
    {
        XMLGen::append_trust_region_kelley_sachs_options(aMetaData, aParentNode);
    }
    else if(tLower.compare("ksal") == 0)
    {
        XMLGen::append_trust_region_kelley_sachs_options(aMetaData, aParentNode);
        XMLGen::append_augmented_lagrangian_options(aMetaData, aParentNode);
    }
    else
    {
        THROWERR(std::string("Append Optimization Algorithm Options: ") + "Optimization algorithm '"
            + aMetaData.optimization_parameters().optimization_algorithm() + "' is not supported.")
    }
}
// function append_grad_based_optimizer_parameters
/******************************************************************************/

/******************************************************************************/
void append_optimality_criteria_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys = {"OCControlStagnationTolerance", 
                                      "OCObjectiveStagnationTolerance",
                                      "OCGradientTolerance",
                                      "ProblemUpdateFrequency"};
    std::vector<std::string> tValues = {aMetaData.optimization_parameters().oc_control_stagnation_tolerance(), 
                                        aMetaData.optimization_parameters().oc_objective_stagnation_tolerance(),
                                        aMetaData.optimization_parameters().oc_gradient_tolerance(),
                                        aMetaData.optimization_parameters().problem_update_frequency()};
    XMLGen::set_value_keyword_to_ignore_if_empty(tValues);
    auto tOptionsNode = aParentNode.child("Options");
    if(tOptionsNode.empty())
    {
        tOptionsNode = aParentNode.append_child("Options");
    }
    XMLGen::append_children(tKeys, tValues, tOptionsNode);
    auto tConvergenceNode = aParentNode.append_child("Convergence");
    XMLGen::append_children({"MaxIterations"}, {aMetaData.optimization_parameters().max_iterations()}, tConvergenceNode);
}
// function append_optimality_criteria_options
/******************************************************************************/

/******************************************************************************/
void append_method_moving_asymptotes_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys = { "MaxNumOuterIterations", "MoveLimit", "AsymptoteExpansion", "AsymptoteContraction",
         "MaxNumSubProblemIter", "ControlStagnationTolerance", "ObjectiveStagnationTolerance", "OutputSubProblemDiagnostics", 
         "SubProblemInitialPenalty", "SubProblemPenaltyMultiplier", "SubProblemFeasibilityTolerance", 
         "UpdateFrequency", "UseIpoptForMMASubproblem"};
    std::vector<std::string> tValues = {aMetaData.optimization_parameters().max_iterations(), aMetaData.optimization_parameters().mma_move_limit(), aMetaData.optimization_parameters().mma_asymptote_expansion(),
        aMetaData.optimization_parameters().mma_asymptote_contraction(), aMetaData.optimization_parameters().mma_max_sub_problem_iterations(), aMetaData.optimization_parameters().mma_control_stagnation_tolerance(),
        aMetaData.optimization_parameters().mma_objective_stagnation_tolerance(), aMetaData.optimization_parameters().mma_output_subproblem_diagnostics(), aMetaData.optimization_parameters().mma_sub_problem_initial_penalty(),
        aMetaData.optimization_parameters().mma_sub_problem_penalty_multiplier(), aMetaData.optimization_parameters().mma_sub_problem_feasibility_tolerance(),
        aMetaData.optimization_parameters().problem_update_frequency(), aMetaData.optimization_parameters().mma_use_ipopt_sub_problem_solver() };
    XMLGen::set_value_keyword_to_ignore_if_empty(tValues);
    auto tOptionsNode = aParentNode.append_child("Options");
    XMLGen::append_children(tKeys, tValues, tOptionsNode);
}
// function append_method_moving_asymptotes_options
/******************************************************************************/

/******************************************************************************/
void append_trust_region_kelley_sachs_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys = {"MaxNumOuterIterations",
                                      "KSTrustRegionExpansionFactor", 
                                      "KSTrustRegionContractionFactor", 
                                      "KSMaxTrustRegionIterations", 
                                      "KSInitialRadiusScale",
                                      "KSMaxRadiusScale", 
                                      "HessianType", 
                                      "MinTrustRegionRadius", 
                                      "LimitedMemoryStorage", 
                                      "KSOuterGradientTolerance", 
                                      "KSOuterStationarityTolerance",
                                      "KSOuterStagnationTolerance", 
                                      "KSOuterControlStagnationTolerance", 
                                      "KSOuterActualReductionTolerance", 
                                      "ProblemUpdateFrequency", 
                                      "DisablePostSmoothing",
                                      "KSTrustRegionRatioLow",       
                                      "KSTrustRegionRatioMid", 
                                      "KSTrustRegionRatioUpper"};
    std::vector<std::string> tValues = {aMetaData.optimization_parameters().max_iterations(),
                                        aMetaData.optimization_parameters().ks_trust_region_expansion_factor(), 
                                        aMetaData.optimization_parameters().ks_trust_region_contraction_factor(), 
                                        aMetaData.optimization_parameters().ks_max_trust_region_iterations(),
                                        aMetaData.optimization_parameters().ks_initial_radius_scale(), 
                                        aMetaData.optimization_parameters().ks_max_radius_scale(), 
                                        aMetaData.optimization_parameters().hessian_type(), 
                                        aMetaData.optimization_parameters().ks_min_trust_region_radius(), 
                                        aMetaData.optimization_parameters().limited_memory_storage(),
                                        aMetaData.optimization_parameters().ks_outer_gradient_tolerance(), 
                                        aMetaData.optimization_parameters().ks_outer_stationarity_tolerance(), 
                                        aMetaData.optimization_parameters().ks_outer_stagnation_tolerance(), 
                                        aMetaData.optimization_parameters().ks_outer_control_stagnation_tolerance(),
                                        aMetaData.optimization_parameters().ks_outer_actual_reduction_tolerance(), 
                                        aMetaData.optimization_parameters().problem_update_frequency(), 
                                        aMetaData.optimization_parameters().ks_disable_post_smoothing(), 
                                        aMetaData.optimization_parameters().ks_trust_region_ratio_low(),
                                        aMetaData.optimization_parameters().ks_trust_region_ratio_mid(), 
                                        aMetaData.optimization_parameters().ks_trust_region_ratio_high()};
    XMLGen::set_value_keyword_to_ignore_if_empty(tValues);
    auto tOptionsNode = aParentNode.child("Options");
    if(tOptionsNode.empty())
    {
        tOptionsNode = aParentNode.append_child("Options");
    }
    XMLGen::append_children(tKeys, tValues, tOptionsNode);
    auto tConvergenceNode = aParentNode.append_child("Convergence");
    XMLGen::append_children({"MaxIterations"}, {aMetaData.optimization_parameters().max_iterations()}, tConvergenceNode);
}
// function append_trust_region_kelley_sachs_options
/******************************************************************************/

/******************************************************************************/
void append_augmented_lagrangian_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys = {"AugLagPenaltyParam", 
                                      "AugLagPenaltyParamScaleFactor"};
    std::vector<std::string> tValues = {aMetaData.optimization_parameters().al_penalty_parameter(), 
                                        aMetaData.optimization_parameters().al_penalty_scale_factor()};
    XMLGen::set_value_keyword_to_ignore_if_empty(tValues);
    auto tOptionsNode = aParentNode.child("Options");
    if(tOptionsNode.empty())
    {
        tOptionsNode = aParentNode.append_child("Options");
    }
    XMLGen::append_children(tKeys, tValues, tOptionsNode);
}
// function append_augmented_lagrangian_options
/******************************************************************************/

// function append_reset_algorithm_on_update_option
/******************************************************************************/
void append_reset_algorithm_on_update_option
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys = {"ResetAlgorithmOnUpdate"};
    std::vector<std::string> tValues = {aMetaData.optimization_parameters().reset_algorithm_on_update()};
    XMLGen::set_value_keyword_to_ignore_if_empty(tValues);
    auto tOptionsNode = aParentNode.child("Options");
    if(tOptionsNode.empty())
    {
        tOptionsNode = aParentNode.append_child("Options");
    }
    XMLGen::append_children(tKeys, tValues, tOptionsNode);

}

/******************************************************************************/
void append_optimizer_update_problem_stage_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    if(!XMLGen::need_update_problem_stage(aMetaData))
    {
        return;
    }
    auto tNode = aParentNode.append_child("UpdateProblemStage");
    XMLGen::append_children({"Name"}, {"Update Problem"}, tNode);
}
// function append_optimizer_update_problem_stage_options
/******************************************************************************/

/******************************************************************************/
void append_optimizer_cache_stage_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    if(XMLGen::num_cache_states(aMetaData.services()) > 0)
    {
        auto tNode = aParentNode.append_child("CacheStage");
        XMLGen::append_children({"Name"}, {"Cache State"}, tNode);
    }
}
// function append_optimizer_cache_stage_options
/******************************************************************************/

/******************************************************************************/
void append_grad_based_optimizer_output_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    if(aMetaData.mOutputMetaData.size() > 0 && 
       aMetaData.mOutputMetaData[0].isOutputDisabled())
    {
        return;
    }
    auto tNode = aParentNode.append_child("Output");
    append_children({"OutputStage"}, {"Output To File"}, tNode);
}
// function append_grad_based_optimizer_output_options
/******************************************************************************/

/******************************************************************************/
void append_grad_based_optimizer_variables_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys =
        {"ValueName", "InitializationStage", "FilteredName", "LowerBoundValueName", "LowerBoundVectorName",
         "UpperBoundValueName", "UpperBoundVectorName", "SetLowerBoundsStage", "SetUpperBoundsStage"};
    std::vector<std::string> tValues;
    if(aMetaData.optimization_parameters().optimizationType() == OT_TOPOLOGY)
    {
        tValues =
            {"Control", "Initial Guess", "Topology", "Lower Bound Value", "Lower Bound Vector",
             "Upper Bound Value", "Upper Bound Vector", "Set Lower Bounds", "Set Upper Bounds"};
    }
    else if(aMetaData.optimization_parameters().optimizationType() == OT_SHAPE)
    {
        tValues =
            {"Design Parameters", "Initialize Design Parameters", "Topology", "Lower Bound Value", "Lower Bound Vector",
             "Upper Bound Value", "Upper Bound Vector", "Set Lower Bounds", "Set Upper Bounds"};
    }
    auto tNode = aParentNode.append_child("OptimizationVariables");
    XMLGen::append_children(tKeys, tValues, tNode);
}
// function append_grad_based_optimizer_variables_options
/******************************************************************************/

/******************************************************************************/
void append_grad_based_optimizer_objective_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    std::map<std::string, std::string> tKeyToValueMap =
        { {"ValueName", ""}, {"ValueStageName", ""}, {"GradientName", ""}, {"GradientStageName", ""} };

    std::string tValueNameString = "Objective Value";

    tKeyToValueMap.find("ValueName")->second = tValueNameString;
    tKeyToValueMap.find("ValueStageName")->second = std::string("Compute Objective Value");
    tKeyToValueMap.find("GradientName")->second = std::string("Objective Gradient");
    tKeyToValueMap.find("GradientStageName")->second = std::string("Compute Objective Gradient");

    auto tKeys = XMLGen::transform_key_tokens(tKeyToValueMap);
    auto tValues = XMLGen::transform_value_tokens(tKeyToValueMap);
    auto tNode = aParentNode.append_child("Objective");
    XMLGen::append_children(tKeys, tValues, tNode);
}
// function append_grad_based_optimizer_objective_options
/******************************************************************************/

/******************************************************************************/
void append_grad_based_optimizer_constraint_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode)
{
    for (auto &tConstraint : aMetaData.constraints)
    {
        std::map<std::string, std::string> tKeyToValueMap;
        if(aMetaData.optimization_parameters().optimizationType() == OT_TOPOLOGY  && aMetaData.optimization_parameters().discretization() == "density")
        {
            tKeyToValueMap =
            { {"ValueName", ""}, {"ValueStageName", ""}, {"GradientName", ""}, {"GradientStageName", ""},
                 {"ReferenceValueName", XMLGen::Private::get_constraint_reference_value_name(aMetaData, tConstraint)} };
        }
        else if(aMetaData.optimization_parameters().optimizationType() == OT_SHAPE)
        {
            tKeyToValueMap =
            { {"ValueName", ""}, {"ValueStageName", ""}, {"GradientName", ""}, {"GradientStageName", ""} };
        }
        else if(aMetaData.optimization_parameters().optimization_type() == "topology" && aMetaData.optimization_parameters().discretization() == "levelset")
        {
            tKeyToValueMap =
            { {"ValueName", ""}, {"ValueStageName", ""}, {"GradientName", ""}, {"GradientStageName", ""},
              {"ReferenceValueName", "Design Volume"} };
        }

        tKeyToValueMap.find("ValueName")->second = std::string("Constraint Value ") + tConstraint.id();
        tKeyToValueMap.find("ValueStageName")->second = std::string("Compute Constraint Value ") + tConstraint.id();
        tKeyToValueMap.find("GradientName")->second = std::string("Constraint Gradient ") + tConstraint.id();
        tKeyToValueMap.find("GradientStageName")->second = std::string("Compute Constraint Gradient ") + tConstraint.id();

        XMLGen::generate_target_value_entries(aMetaData, tConstraint, tKeyToValueMap);

        auto tKeys = XMLGen::transform_key_tokens(tKeyToValueMap);
        auto tValues = XMLGen::transform_value_tokens(tKeyToValueMap);
        auto tNode = aParentNode.append_child("Constraint");
        XMLGen::append_children(tKeys, tValues, tNode);
    }
}
// function append_grad_based_optimizer_constraint_options
/******************************************************************************/

/******************************************************************************/
void generate_target_value_entries
(const XMLGen::InputData& aXMLMetaData,
 const XMLGen::Constraint &aConstraint,
 std::map<std::string, std::string> &aKeyToValueMap)
{
    auto tCriterion = aXMLMetaData.criterion(aConstraint.criterion());
    std::string tCriterionType = tCriterion.type();
    if(tCriterionType == "volume")
    {
        if(aConstraint.absoluteTarget().length() > 0)
        {
            double tAbsoluteTargetValue = std::atof(aConstraint.absoluteTarget().c_str());
            if(fabs(tAbsoluteTargetValue) < 1e-16)
            {
                THROWERR("You must specify a non-zero volume constraint value.")
            }
            double tReferenceValue = std::atof(aConstraint.absoluteTarget().c_str());
            tReferenceValue *= std::atof(aConstraint.divisor().c_str());
            aKeyToValueMap["AbsoluteTargetValue"] = aConstraint.absoluteTarget();
            aKeyToValueMap["ReferenceValue"] = std::to_string(tReferenceValue);
        }
        else if(aConstraint.relativeTarget().length() > 0)
        {
            double tRelativeTargetValue = std::atof(aConstraint.relativeTarget().c_str());
            if(fabs(tRelativeTargetValue) < 1e-16)
            {
                THROWERR("You must specify a non-zero volume constraint value.")
            }
            aKeyToValueMap["NormalizedTargetValue"] = aConstraint.relativeTarget();
        }
        else
            THROWERR("Append Optimization Constraint Options: Constraint target was not set.")
    }
    else
    {
        if(aConstraint.absoluteTarget().length() == 0)
        {
            THROWERR("You must use absolute target values with non-volume constraints.")
        }
        double tAbsoluteTargetValue = std::atof(aConstraint.absoluteTarget().c_str());
        aKeyToValueMap["AbsoluteTargetValue"] = aConstraint.absoluteTarget();
        double tReferenceValue = fabs(tAbsoluteTargetValue) < 1e-16 ? 1.0 : std::atof(aConstraint.absoluteTarget().c_str());
        tReferenceValue *= std::atof(aConstraint.divisor().c_str());
        aKeyToValueMap["ReferenceValue"] = std::to_string(tReferenceValue);
        if(aConstraint.greater_than())
        {
            aKeyToValueMap["AbsoluteTargetValue"].insert(0,1,'-');
        }
    }
}
// function generate_target_value_entries
/******************************************************************************/

/******************************************************************************/
void append_bound_constraints_options_topology_optimization
(const std::vector<std::string>& aValues,
 pugi::xml_node& aParentNode)
{
    std::vector<std::string> tKeys = {"Upper", "Lower"};
    auto tNode = aParentNode.append_child("BoundConstraint");
    XMLGen::append_children(tKeys, aValues, tNode);
}
// function append_bound_constraints_options_topology_optimization
/******************************************************************************/

}
//  namespace XMLGen
