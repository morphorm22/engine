/*
 * XMLGeneratorDefineFile_UnitTester.cpp
 *
 *  Created on: Jan 13, 2022
 */

 #include <gtest/gtest.h>

#include "XMLGenerator_UnitTester_Tools.hpp"

#include "XMLGeneratorProblem.hpp"
#include "XMLGeneratorUtilities.hpp"
#include "XMLGeneratorUtilities.hpp"
#include "XMLGeneratorParseMetadata.hpp"
#include "XMLGeneratorDakotaProblem.hpp"
#include "XMLGeneratorProblemUtilities.hpp"
#include "XMLGeneratorDefinesFileUtilities.hpp"
#include "XMLGeneratorLaunchScriptUtilities.hpp"
#include "XMLGeneratorDakotaInterfaceFileUtilities.hpp"
#include "XMLGeneratorPlatoAnalyzeInputFileUtilities.hpp"
#include "XMLGeneratorDakotaDriverInputFileUtilities.hpp"
#include "XMLGeneratorPlatoMainOperationFileUtilities.hpp"
#include "XMLGeneratorParseMethodInputOptionsUtilities.hpp"
#include "XMLGeneratorPlatoAnalyzeOperationsFileUtilities.hpp"

namespace XMLGen
{

namespace dakota
{

void usecase_system_call
(XMLGen::InputData& aMetaData)
{
    XMLGen::write_define_xml_file(aMetaData);
}

}

}

namespace PlatoTestXMLGenerator
{

TEST(PlatoTestXMLGenerator, InsertMogaInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_dakota_moga_input_options(tTags);
    EXPECT_EQ(6u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"num_sampling_method_samples","15"}, {"sbgo_max_iterations","10"}, {"moga_population_size", "300"}, 
        {"moga_niching_distance", "0.2"}, {"moga_max_function_evaluations", "20000"}, {"sbgo_surrogate_output_name", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertMultiDimParamStudyInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_dakota_multidim_param_study_input_options(tTags);
    EXPECT_EQ(2u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"mdps_partitions",""}, {"mdps_response_functions",""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertDakotaInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_general_dakota_input_options(tTags);
    XMLGen::insert_dakota_moga_input_options(tTags);
    XMLGen::insert_dakota_multidim_param_study_input_options(tTags);
    EXPECT_EQ(10u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"dakota_workflow",""}, {"concurrent_evaluations",""}, {"mdps_partitions",""}, {"mdps_response_functions",""},
        {"num_sampling_method_samples","15"}, {"sbgo_max_iterations","10"}, {"moga_population_size","300"}, {"moga_niching_distance","0.2"}, 
        {"moga_max_function_evaluations","20000"}, {"sbgo_surrogate_output_name", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertMmaInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_plato_mma_input_options(tTags);
    EXPECT_EQ(11u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"mma_move_limit","0.5"}, {"mma_asymptote_expansion","1.2"}, 
        {"mma_asymptote_contraction", "0.7"}, {"mma_max_sub_problem_iterations", "50"}, {"mma_sub_problem_initial_penalty", "0.0015"},
        {"mma_sub_problem_penalty_multiplier", "1.025"}, {"mma_use_ipopt_sub_problem_solver", "false"},{"mma_control_stagnation_tolerance", "1e-6"}, 
        {"mma_objective_stagnation_tolerance", "1e-8"}, {"mma_sub_problem_feasibility_tolerance", "1e-8"}, {"mma_output_subproblem_diagnostics", "false"} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertLevelsetBasedShapeOptimizationInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_plato_levelset_input_options(tTags);
    EXPECT_EQ(7u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"levelset_nodesets",""}, {"levelset_sphere_radius",""}, 
        {"create_levelset_spheres", ""}, {"levelset_material_box_min", ""}, {"levelset_material_box_max", ""},
        {"levelset_sphere_packing_factor", ""}, {"levelset_initialization_method", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertKelleySachsTrustRegionInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_plato_kelley_sachs_trust_region_input_options(tTags);
    EXPECT_EQ(23u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"ks_max_radius_scale",""}, 
        {"ks_initial_radius_scale",""}, {"ks_trust_region_ratio_low", ""}, {"ks_trust_region_ratio_mid", ""}, 
        {"ks_min_trust_region_radius", ""}, {"ks_trust_region_ratio_high", ""}, {"ks_disable_post_smoothing", "true"}, 
        {"ks_outer_gradient_tolerance", ""}, {"ks_outer_stagnation_tolerance", ""}, {"ks_max_trust_region_iterations", "5"}, 
        {"ks_outer_stationarity_tolerance", ""}, {"ks_trust_region_expansion_factor", ""}, {"ks_trust_region_contraction_factor", ""}, 
        {"ks_outer_actual_reduction_tolerance", ""}, {"ks_outer_control_stagnation_tolerance", ""}, {"use_mean_norm", ""}, 
        {"al_penalty_parameter", ""}, {"feasibility_tolerance", ""}, {"max_trust_region_radius", ""}, {"al_penalty_scale_factor", ""}, 
        {"al_max_subproblem_iterations", ""}, {"hessian_type", ""}, {"limited_memory_storage", "8"} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertProjectionFilterInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_plato_projection_filter_input_options(tTags);
    EXPECT_EQ(8u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"projection_type", ""}, {"filter_heaviside_min",""}, 
        {"filter_heaviside_max",""}, {"filter_heaviside_scale", ""}, {"filter_heaviside_update", ""}, {"filter_use_additive_continuation", ""},
        {"filter_projection_start_iteration", ""}, {"filter_projection_update_interval", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertSymmetryFilterInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_plato_symmetry_filter_input_options(tTags);
    EXPECT_EQ(5u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"symmetry_plane_normal",""}, {"symmetry_plane_origin",""}, 
        {"mesh_map_filter_radius", ""}, {"filter_before_symmetry_enforcement", ""}, {"mesh_map_search_tolerance", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertPlatoFilterInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_plato_filter_input_options(tTags);
    EXPECT_EQ(12u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"filter_type","kernel"}, {"filter_service",""}, 
        {"filter_in_engine", "true"}, {"filter_radius_scale", "2.0"}, {"filter_radius_absolute", ""},
        {"filter_power", "1"}, {"filter_type_identity_generator_name", "identity"}, {"filter_type_kernel_generator_name", "kernel"},
        {"filter_type_kernel_then_heaviside_generator_name", "kernel_then_heaviside"}, 
        {"filter_type_kernel_then_tanh_generator_name", "kernel_then_tanh"}, {"boundary_sticking_penalty", "1.0"}, 
        {"symmetry_plane_location_names", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertFixedBlocksInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_fixed_blocks_input_options(tTags);
    EXPECT_EQ(6u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"fixed_block_ids",""}, {"fixed_sideset_ids",""}, 
        {"fixed_nodeset_ids", ""}, {"fixed_block_domain_values", ""}, {"fixed_block_boundary_values", ""},
        {"fixed_block_material_states", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertPruneAndRefineInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_prune_and_refine_input_options(tTags);
    EXPECT_EQ(6u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"prune_mesh",""}, {"prune_threshold",""},{"number_refines",""}, 
        {"number_buffer_layers", ""}, {"prune_and_refine_path", ""}, {"number_prune_and_refine_processors", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertOptimalityCriteriaInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_optimality_criteria_input_options(tTags);
    EXPECT_EQ(3u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"oc_gradient_tolerance","1e-8"}, 
        {"oc_control_stagnation_tolerance", "1e-2"}, {"oc_objective_stagnation_tolerance", "1e-5"} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertRestartInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_restart_input_options(tTags);
    EXPECT_EQ(4u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"restart_iteration","0"}, 
        {"write_restart_file","false"}, {"initial_guess_file_name", ""}, {"initial_guess_field_name", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertDerivativeCheckerInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_derivative_checker_input_options(tTags);
    EXPECT_EQ(2u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"check_hessian","false"}, {"check_gradient","false"} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertGeneralOptimizationInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_general_optimization_input_options(tTags);
    EXPECT_EQ(12u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"max_iterations",""}, {"verbose", "false"}, {"output_method", "epu"},
        {"output_frequency", "5"}, {"optimization_type", "topology"}, {"optimization_algorithm", "oc"}, 
        {"normalize_in_aggregator", ""}, {"problem_update_frequency", "5"}, {"objective_number_standard_deviations", ""},
        {"descriptors", ""}, {"lower_bounds", ""}, {"upper_bounds", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertShapeOptimizationInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_shape_optimization_input_options(tTags);
    EXPECT_EQ(2u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"csm_file", ""}, {"num_shape_design_variables", ""} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, InsertTopologyOptimizationInputs)
{
    XMLGen::MetaDataTags tTags;
    XMLGen::insert_topology_optimization_input_options(tTags);
    EXPECT_EQ(3u, tTags.size());

    std::unordered_map<std::string, std::string> tGoldValues = { {"enforce_bounds", "false"}, 
        {"discretization", "density"}, {"initial_density_value", "0.5"} };
    for(auto& tPair : tTags)
    {
        // TEST INPUT KEYWORDS
        auto tGoldItr = tGoldValues.find(tPair.first);
        ASSERT_FALSE(tGoldItr == tGoldValues.end());
        EXPECT_STREQ(tPair.first.c_str(), tGoldItr->first.c_str());

        // TEST DEFAULT VALUES
        EXPECT_STREQ(tPair.second.second.c_str(), tGoldItr->second.c_str());
    }
}

TEST(PlatoTestXMLGenerator, AppendDefinesFileToInterfaceFile)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::append_include_defines_xml_data(tMetaData, tDocument));

    auto tInclude = tDocument.child("include");
    ASSERT_FALSE(tInclude.empty());
    PlatoTestXMLGenerator::test_attributes({"filename"}, {"defines.xml"}, tInclude);
}

TEST(PlatoTestXMLGenerator, AppendPerformersToInterfaceFile_SingleObjective)
{
    XMLGen::InputData tMetaData;
    XMLGen::Service tServiceOne;
    tServiceOne.id("1");
    tServiceOne.code("platomain");
    tServiceOne.append("type", "plato_app");
    tMetaData.append(tServiceOne);
    XMLGen::Service tServiceTwo;
    tServiceTwo.id("2");
    tServiceTwo.code("plato_analyze");
    tServiceTwo.append("type", "plato_app");
    tMetaData.append(tServiceTwo);

    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tOptimizationParameters.append("concurrent_evaluations", "2");
    tMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    XMLGen::dakota::append_performer_data(tMetaData, tDocument);

    auto tPerformer = tDocument.child("Performer");
    ASSERT_FALSE(tPerformer.empty());
    ASSERT_STREQ("Performer", tPerformer.name());
    std::vector<std::string> tKeys = {"Name", "Code", "PerformerID"};
    std::vector<std::string> tValues = {"platomain_1", "platomain", "0"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tPerformer);

    tPerformer = tPerformer.next_sibling("Performer");
    ASSERT_FALSE(tPerformer.empty());
    ASSERT_STREQ("Performer", tPerformer.name());
    tKeys = {"Name", "Code", "PerformerID"};
    tValues = {"plato_analyze_2_0", "plato_analyze", "1"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tPerformer);

    tPerformer = tPerformer.next_sibling("Performer");
    ASSERT_FALSE(tPerformer.empty());
    ASSERT_STREQ("Performer", tPerformer.name());
    tKeys = {"Name", "Code", "PerformerID"};
    tValues = {"plato_analyze_2_1", "plato_analyze", "2"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tPerformer);

    tPerformer = tPerformer.next_sibling("Performer");
    ASSERT_FALSE(tPerformer.empty());
    ASSERT_STREQ("Performer", tPerformer.name());
    tKeys = {"Name", "Code", "PerformerID"};
    tValues = {"plato_services_0", "plato_services", "3"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tPerformer);

    tPerformer = tPerformer.next_sibling("Performer");
    ASSERT_FALSE(tPerformer.empty());
    ASSERT_STREQ("Performer", tPerformer.name());
    tKeys = {"Name", "Code", "PerformerID"};
    tValues = {"plato_services_1", "plato_services", "4"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tPerformer);

    tPerformer = tPerformer.next_sibling("Performer");
    ASSERT_TRUE(tPerformer.empty());
}

TEST(PlatoTestXMLGenerator, AppendSharedDataToInterfaceFile_SingleObjective)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tServiceOne;
    tServiceOne.id("1");
    tServiceOne.type("plato_app");
    tServiceOne.code("platomain");
    tMetaData.append(tServiceOne);
    XMLGen::Service tServiceTwo;
    tServiceTwo.id("2");
    tServiceTwo.type("plato_app");
    tServiceTwo.code("plato_analyze");
    tMetaData.append(tServiceTwo);

    // Create a criterion
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "single_criterion";
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.scenarioIDs.push_back("14");
    tMetaData.objective = tObjective;

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tOptimizationParameters.append("concurrent_evaluations", "2");
    tOptimizationParameters.append("num_shape_design_variables", "4");
    tMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::dakota::append_shared_data(tMetaData, tDocument));

    // TEST
    auto tOuterSharedData = tDocument.child("For");
    ASSERT_FALSE(tOuterSharedData.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tOuterSharedData);
    auto tSharedData = tOuterSharedData.child("SharedData");
    ASSERT_FALSE(tSharedData.empty());
    ASSERT_STREQ("SharedData", tSharedData.name());
    std::vector<std::string> tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName", "UserName"};
    std::vector<std::string> tValues = {"design_parameters_{I}", "Scalar", "Global", "4", "platomain_1", "platomain_1", "plato_analyze_2_{I}"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tSharedData);

    tOuterSharedData = tOuterSharedData.next_sibling("For");
    ASSERT_FALSE(tOuterSharedData.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tOuterSharedData);
    tSharedData = tOuterSharedData.child("SharedData");
    ASSERT_FALSE(tSharedData.empty());
    ASSERT_STREQ("SharedData", tSharedData.name());
    tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName"};
    tValues = {"criterion_3_service_2_scenario_14_{I}", "Scalar", "Global", "1", "plato_analyze_2_{I}", "platomain_1"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tSharedData);

    tOuterSharedData = tOuterSharedData.next_sibling("For");
    ASSERT_TRUE(tOuterSharedData.empty());

    tOuterSharedData = tOuterSharedData.next_sibling("SharedData");
    ASSERT_TRUE(tOuterSharedData.empty());
}

TEST(PlatoTestXMLGenerator, AppendSharedDataToInterfaceFile_MultiObjective)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tServiceOne;
    tServiceOne.id("1");
    tServiceOne.type("plato_app");
    tServiceOne.code("platomain");
    tMetaData.append(tServiceOne);
    XMLGen::Service tServiceTwo;
    tServiceTwo.id("2");
    tServiceTwo.type("plato_app");
    tServiceTwo.code("plato_analyze");
    tMetaData.append(tServiceTwo);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("volume");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "multi_objective";
    tObjective.serviceIDs.push_back("2");
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.criteriaIDs.push_back("7");
    tObjective.scenarioIDs.push_back("14");
    tObjective.scenarioIDs.push_back("14");
    tObjective.weights.push_back("1");
    tObjective.weights.push_back("1");
    tMetaData.objective = tObjective;

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tOptimizationParameters.append("concurrent_evaluations", "2");
    tOptimizationParameters.append("num_shape_design_variables", "4");
    tMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::dakota::append_shared_data(tMetaData, tDocument));

    // TEST
    auto tOuterSharedData = tDocument.child("For");
    ASSERT_FALSE(tOuterSharedData.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tOuterSharedData);
    auto tSharedData = tOuterSharedData.child("SharedData");
    ASSERT_FALSE(tSharedData.empty());
    ASSERT_STREQ("SharedData", tSharedData.name());
    std::vector<std::string> tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName", "UserName"};
    std::vector<std::string> tValues = {"design_parameters_{I}", "Scalar", "Global", "4", "platomain_1", "platomain_1", "plato_analyze_2_{I}"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tSharedData);

    tOuterSharedData = tOuterSharedData.next_sibling("For");
    ASSERT_FALSE(tOuterSharedData.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tOuterSharedData);
    tSharedData = tOuterSharedData.child("SharedData");
    ASSERT_FALSE(tSharedData.empty());
    ASSERT_STREQ("SharedData", tSharedData.name());
    tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName"};
    tValues = {"criterion_3_service_2_scenario_14_{I}", "Scalar", "Global", "1", "plato_analyze_2_{I}", "platomain_1"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tSharedData);

    tOuterSharedData = tOuterSharedData.next_sibling("For");
    ASSERT_FALSE(tOuterSharedData.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tOuterSharedData);
    tSharedData = tOuterSharedData.child("SharedData");
    ASSERT_FALSE(tSharedData.empty());
    ASSERT_STREQ("SharedData", tSharedData.name());
    tKeys = {"Name", "Type", "Layout", "Size", "OwnerName", "UserName"};
    tValues = {"criterion_7_service_2_scenario_14_{I}", "Scalar", "Global", "1", "plato_analyze_2_{I}", "platomain_1"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tSharedData);

    tOuterSharedData = tOuterSharedData.next_sibling("For");
    ASSERT_TRUE(tOuterSharedData.empty());

    tOuterSharedData = tOuterSharedData.next_sibling("SharedData");
    ASSERT_TRUE(tOuterSharedData.empty());
}

TEST(PlatoTestXMLGenerator, AppendInitializeMeshesStageToInterfaceFile_MultiPhysicsPerformers)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("1");
    tService.code("platomain");
    tMetaData.append(tService);
    tService.id("21");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tService.id("14");
    tService.code("plato_analyze");
    tMetaData.append(tService);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::dakota::append_initialize_stage(tMetaData, tDocument));

    // STAGE INPUTS
    auto tStage = tDocument.child("Stage");
    ASSERT_FALSE(tStage.empty());
    ASSERT_STREQ("Stage", tStage.name());
    auto tName = tStage.child("Name");
    ASSERT_STREQ("Initialize Meshes", tName.child_value());
    auto tForNode = tStage.child("For");
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tInput = tForNode.child("Input");
    ASSERT_STREQ("Input", tInput.name());
    PlatoTestXMLGenerator::test_children({"SharedDataName"}, {"design_parameters_{I}"}, tInput);

    // UPDATE GEOMETRY ON CHANGE OPERATION
    auto tOuterOperation = tStage.child("Operation");
    ASSERT_FALSE(tOuterOperation.empty());
    tForNode = tOuterOperation.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tOperation = tForNode.child("Operation");
    ASSERT_FALSE(tOperation.empty());
    std::vector<std::string> tKeys = {"Name", "PerformerName", "Input"};
    std::vector<std::string> tValues = {"update_geometry_on_change_{I}", "plato_services_{I}", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);
    auto tOpInputs = tOperation.child("Input");
    ASSERT_FALSE(tOpInputs.empty());
    PlatoTestXMLGenerator::test_children({ "SharedDataName" ,"ArgumentName"}, { "design_parameters_{I}","Parameters"}, tOpInputs);

    // REINITIALIZE ON CHANGE OPERATION
    tOuterOperation = tOuterOperation.next_sibling("Operation");
    ASSERT_FALSE(tOuterOperation.empty());
    tForNode = tOuterOperation.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    tOperation = tForNode.child("Operation");
    ASSERT_FALSE(tOperation.empty());
    tKeys = {"Name", "PerformerName", "Input"};
    tValues = {"reinitialize_on_change_plato_services", "plato_services_{I}", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);
    tOpInputs = tOperation.child("Input");
    ASSERT_FALSE(tOpInputs.empty());
    PlatoTestXMLGenerator::test_children({ "SharedDataName","ArgumentName"}, { "design_parameters_{I}","Parameters"}, tOpInputs);

    tOperation = tOperation.next_sibling("Operation");
    ASSERT_FALSE(tOperation.empty());
    tKeys = {"Name", "PerformerName", "Input"};
    tValues = {"reinitialize_on_change_plato_analyze_21", "plato_analyze_21_{I}", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);
    tOpInputs = tOperation.child("Input");
    ASSERT_FALSE(tOpInputs.empty());
    PlatoTestXMLGenerator::test_children({ "SharedDataName","ArgumentName"}, { "design_parameters_{I}", "Parameters"}, tOpInputs);

    tOperation = tOperation.next_sibling("Operation");
    ASSERT_FALSE(tOperation.empty());
    tKeys = {"Name", "PerformerName", "Input"};
    tValues = {"reinitialize_on_change_plato_analyze_14", "plato_analyze_14_{I}", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);
    tOpInputs = tOperation.child("Input");
    ASSERT_FALSE(tOpInputs.empty());
    PlatoTestXMLGenerator::test_children({ "SharedDataName", "ArgumentName"}, { "design_parameters_{I}","Parameters"}, tOpInputs);

    tOperation = tForNode.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());

    tOperation = tOuterOperation.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());
}

TEST(PlatoTestXMLGenerator, AppendComputeCriteriaStagesToInterfaceFile_SingleObjective)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("1");
    tService.code("platomain");
    tMetaData.append(tService);
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create a criterion
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "single_criterion";
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.scenarioIDs.push_back("14");
    tMetaData.objective = tObjective;

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::dakota::append_criterion_value_stages(tMetaData, tDocument));

    // COMPUTE CRITERION 0 VALUE STAGE
    auto tStage = tDocument.child("Stage");
    ASSERT_FALSE(tStage.empty());
    ASSERT_STREQ("Stage", tStage.name());
    auto tName = tStage.child("Name");
    ASSERT_STREQ("Compute Criterion 0 Value", tName.child_value());
    auto tOuterOperation = tStage.child("Operation");
    ASSERT_FALSE(tOuterOperation.empty());
    auto tForNode = tOuterOperation.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tOperation = tForNode.child("Operation");
    ASSERT_FALSE(tOperation.empty());
    std::vector<std::string> tKeys = {"Name", "PerformerName", "Output"};
    std::vector<std::string> tValues = {"Compute Criterion Value - criterion_3_service_2_scenario_14", "plato_analyze_2_{I}", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);
    auto tOpOutputs = tOperation.child("Output");
    ASSERT_FALSE(tOpOutputs.empty());
    PlatoTestXMLGenerator::test_children({ "SharedDataName","ArgumentName"}, { "criterion_3_service_2_scenario_14_{I}","criterion_3_service_2_scenario_14 value"}, tOpOutputs);

    tOperation = tOperation.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());

    tOperation = tOuterOperation.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());

    tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tOutputs = tForNode.child("Output");
    ASSERT_FALSE(tOutputs.empty());
    PlatoTestXMLGenerator::test_children({"SharedDataName"}, {"criterion_3_service_2_scenario_14_{I}"}, tOutputs);

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    tStage = tStage.next_sibling("Stage");
    ASSERT_TRUE(tStage.empty());
}

TEST(PlatoTestXMLGenerator, AppendComputeCriteriaStagesToInterfaceFile_MultiObjective)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("1");
    tService.code("platomain");
    tMetaData.append(tService);
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("volume");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "multi_objective";
    tObjective.serviceIDs.push_back("2");
    tObjective.serviceIDs.push_back("1");
    tObjective.criteriaIDs.push_back("3");
    tObjective.criteriaIDs.push_back("7");
    tObjective.scenarioIDs.push_back("14");
    tObjective.scenarioIDs.push_back("14");
    tObjective.weights.push_back("1");
    tObjective.weights.push_back("1");
    tMetaData.objective = tObjective;

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::dakota::append_criterion_value_stages(tMetaData, tDocument));

    // COMPUTE CRITERION 0 VALUE STAGE
    auto tStage = tDocument.child("Stage");
    ASSERT_FALSE(tStage.empty());
    ASSERT_STREQ("Stage", tStage.name());
    auto tName = tStage.child("Name");
    ASSERT_STREQ("Compute Criterion 0 Value", tName.child_value());
    auto tOuterOperation = tStage.child("Operation");
    ASSERT_FALSE(tOuterOperation.empty());
    auto tForNode = tOuterOperation.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tOperation = tForNode.child("Operation");
    ASSERT_FALSE(tOperation.empty());
    std::vector<std::string> tKeys = {"Name", "PerformerName", "Output"};
    std::vector<std::string> tValues = {"Compute Criterion Value - criterion_3_service_2_scenario_14", "plato_analyze_2_{I}", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);
    auto tOpOutputs = tOperation.child("Output");
    ASSERT_FALSE(tOpOutputs.empty());
    PlatoTestXMLGenerator::test_children({ "SharedDataName","ArgumentName"}, { "criterion_3_service_2_scenario_14_{I}","criterion_3_service_2_scenario_14 value"}, tOpOutputs);

    tOperation = tOperation.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());

    tOperation = tOuterOperation.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());

    tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tOutputs = tForNode.child("Output");
    ASSERT_FALSE(tOutputs.empty());
    PlatoTestXMLGenerator::test_children({"SharedDataName"}, {"criterion_3_service_2_scenario_14_{I}"}, tOutputs);

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    // COMPUTE CRITERION 1 VALUE STAGE
    tStage = tStage.next_sibling("Stage");
    ASSERT_FALSE(tStage.empty());
    ASSERT_STREQ("Stage", tStage.name());
    tName = tStage.child("Name");
    ASSERT_STREQ("Compute Criterion 1 Value", tName.child_value());
    tOuterOperation = tStage.child("Operation");
    ASSERT_FALSE(tOuterOperation.empty());
    tForNode = tOuterOperation.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    tOperation = tForNode.child("Operation");
    ASSERT_FALSE(tOperation.empty());
    tKeys = {"Name", "PerformerName", "Output"};
    tValues = {"Compute Criterion Value - criterion_7_service_1_scenario_14", "plato_services_{I}", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);
    tOpOutputs = tOperation.child("Output");
    ASSERT_FALSE(tOpOutputs.empty());
    PlatoTestXMLGenerator::test_children({ "SharedDataName","ArgumentName"}, { "criterion_7_service_1_scenario_14_{I}","criterion_7_service_1_scenario_14 value"}, tOpOutputs);

    tOperation = tOperation.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());

    tOperation = tOuterOperation.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());

    tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    tOutputs = tForNode.child("Output");
    ASSERT_FALSE(tOutputs.empty());
    PlatoTestXMLGenerator::test_children({"SharedDataName"}, {"criterion_7_service_1_scenario_14_{I}"}, tOutputs);

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    tStage = tStage.next_sibling("Stage");
    ASSERT_TRUE(tStage.empty());
}

TEST(PlatoTestXMLGenerator, AppendDakotaDriverOptionsToInterfaceFile_SingleObjective)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("1");
    tService.code("platomain");
    tMetaData.append(tService);
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create a criterion
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "single_criterion";
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.scenarioIDs.push_back("14");
    tMetaData.objective = tObjective;

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::dakota::append_driver_options(tMetaData, tDocument));

    // INITIALIZE MESHES STAGE
    auto tDriver = tDocument.child("DakotaDriver");
    ASSERT_FALSE(tDriver.empty());
    ASSERT_STREQ("DakotaDriver", tDriver.name());
    auto tStage = tDriver.child("Stage");
    ASSERT_FALSE(tStage.empty());
    std::vector<std::string> tKeys = {"StageTag", "StageName", "For"};
    std::vector<std::string> tValues = {"initialize", "Initialize Meshes", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tStage);
    auto tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tInput = tForNode.child("Input");
    ASSERT_FALSE(tInput.empty());
    tKeys = {"Tag", "SharedDataName"};
    tValues = {"continuous", "design_parameters_{I}"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tInput);

    tInput = tInput.next_sibling("Input");
    ASSERT_TRUE(tInput.empty());

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    // CRITERION VALUE 0 STAGE
    tStage = tStage.next_sibling("Stage");
    ASSERT_FALSE(tStage.empty());
    tKeys = {"StageTag", "StageName", "For"};
    tValues = {"criterion_value_0", "Compute Criterion 0 Value", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tStage);
    tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tOutput = tForNode.child("Output");
    ASSERT_FALSE(tOutput.empty());
    tKeys = {"SharedDataName"};
    tValues = {"criterion_3_service_2_scenario_14_{I}"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOutput);

    tInput = tInput.next_sibling("Input");
    ASSERT_TRUE(tInput.empty());

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    tStage = tStage.next_sibling("Stage");
    ASSERT_TRUE(tStage.empty());
}

TEST(PlatoTestXMLGenerator, AppendDakotaDriverOptionsToInterfaceFile_MultiObjective)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("1");
    tService.code("platomain");
    tMetaData.append(tService);
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("volume");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "multi_objective";
    tObjective.serviceIDs.push_back("2");
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.criteriaIDs.push_back("7");
    tObjective.scenarioIDs.push_back("14");
    tObjective.scenarioIDs.push_back("14");
    tObjective.weights.push_back("1");
    tObjective.weights.push_back("1");
    tMetaData.objective = tObjective;

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::dakota::append_driver_options(tMetaData, tDocument));

    // INITIALIZE MESHES STAGE
    auto tDriver = tDocument.child("DakotaDriver");
    ASSERT_FALSE(tDriver.empty());
    ASSERT_STREQ("DakotaDriver", tDriver.name());
    auto tStage = tDriver.child("Stage");
    ASSERT_FALSE(tStage.empty());
    std::vector<std::string> tKeys = {"StageTag", "StageName", "For"};
    std::vector<std::string> tValues = {"initialize", "Initialize Meshes", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tStage);
    auto tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tInput = tForNode.child("Input");
    ASSERT_FALSE(tInput.empty());
    tKeys = {"Tag", "SharedDataName"};
    tValues = {"continuous", "design_parameters_{I}"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tInput);

    tInput = tInput.next_sibling("Input");
    ASSERT_TRUE(tInput.empty());

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    // CRITERION VALUE 0 STAGE
    tStage = tStage.next_sibling("Stage");
    ASSERT_FALSE(tStage.empty());
    tKeys = {"StageTag", "StageName", "For"};
    tValues = {"criterion_value_0", "Compute Criterion 0 Value", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tStage);
    tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tOutput = tForNode.child("Output");
    ASSERT_FALSE(tOutput.empty());
    tKeys = {"SharedDataName"};
    tValues = {"criterion_3_service_2_scenario_14_{I}"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOutput);

    tInput = tInput.next_sibling("Input");
    ASSERT_TRUE(tInput.empty());

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    // CRITERION VALUE 1 STAGE
    tStage = tStage.next_sibling("Stage");
    ASSERT_FALSE(tStage.empty());
    tKeys = {"StageTag", "StageName", "For"};
    tValues = {"criterion_value_1", "Compute Criterion 1 Value", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tStage);
    tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    tOutput = tForNode.child("Output");
    ASSERT_FALSE(tOutput.empty());
    tKeys = {"SharedDataName"};
    tValues = {"criterion_7_service_2_scenario_14_{I}"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOutput);

    tInput = tInput.next_sibling("Input");
    ASSERT_TRUE(tInput.empty());

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    tStage = tStage.next_sibling("Stage");
    ASSERT_TRUE(tStage.empty());
}

TEST(PlatoTestXMLGenerator, AppendDakotaDriverOptionsToInterfaceFile_SingleObjectiveSingleConstraint)
{
    XMLGen::InputData tMetaData;

    // Create services
    XMLGen::Service tService;
    tService.id("1");
    tService.code("platomain");
    tMetaData.append(tService);
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tService.id("5");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("stress_p-norm");
    tMetaData.append(tCriterion);

    // Create scenarios
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    tScenario.id("3");
    tScenario.physics("transient_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "single_criterion";
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.scenarioIDs.push_back("14");
    tMetaData.objective = tObjective;

    // Create a constraint
    XMLGen::Constraint tConstraint;
    tConstraint.id("8");
    tConstraint.criterion("7");
    tConstraint.service("5");
    tConstraint.scenario("3");
    tMetaData.constraints.push_back(tConstraint);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::dakota::append_driver_options(tMetaData, tDocument));

    // INITIALIZE MESHES STAGE
    auto tDriver = tDocument.child("DakotaDriver");
    ASSERT_FALSE(tDriver.empty());
    ASSERT_STREQ("DakotaDriver", tDriver.name());
    auto tStage = tDriver.child("Stage");
    ASSERT_FALSE(tStage.empty());
    std::vector<std::string> tKeys = {"StageTag", "StageName", "For"};
    std::vector<std::string> tValues = {"initialize", "Initialize Meshes", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tStage);
    auto tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tInput = tForNode.child("Input");
    ASSERT_FALSE(tInput.empty());
    tKeys = {"Tag", "SharedDataName"};
    tValues = {"continuous", "design_parameters_{I}"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tInput);

    tInput = tInput.next_sibling("Input");
    ASSERT_TRUE(tInput.empty());

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    // CRITERION VALUE 0 STAGE
    tStage = tStage.next_sibling("Stage");
    ASSERT_FALSE(tStage.empty());
    tKeys = {"StageTag", "StageName", "For"};
    tValues = {"criterion_value_0", "Compute Criterion 0 Value", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tStage);
    tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    auto tOutput = tForNode.child("Output");
    ASSERT_FALSE(tOutput.empty());
    tKeys = {"SharedDataName"};
    tValues = {"criterion_3_service_2_scenario_14_{I}"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOutput);

    tInput = tInput.next_sibling("Input");
    ASSERT_TRUE(tInput.empty());

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    // CRITERION VALUE 1 STAGE
    tStage = tStage.next_sibling("Stage");
    ASSERT_FALSE(tStage.empty());
    tKeys = {"StageTag", "StageName", "For"};
    tValues = {"criterion_value_1", "Compute Criterion 1 Value", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tStage);
    tForNode = tStage.child("For");
    ASSERT_FALSE(tForNode.empty());
    PlatoTestXMLGenerator::test_attributes({"var", "in"}, {"I", "Parameters"}, tForNode);
    tOutput = tForNode.child("Output");
    ASSERT_FALSE(tOutput.empty());
    tKeys = {"SharedDataName"};
    tValues = {"criterion_7_service_5_scenario_3_{I}"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOutput);

    tInput = tInput.next_sibling("Input");
    ASSERT_TRUE(tInput.empty());

    tForNode = tForNode.next_sibling("For");
    ASSERT_TRUE(tForNode.empty());

    tStage = tStage.next_sibling("Stage");
    ASSERT_TRUE(tStage.empty());
}

TEST(PlatoTestXMLGenerator, AppendUpdateGeometryOnChangeToPlatoMainOperationsFile)
{
    XMLGen::InputData tXMLMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tOptimizationParameters.append("concurrent_evaluations", "2");
    tOptimizationParameters.append("csm_file", "rocker.csm");
    tOptimizationParameters.append("csm_opt_file", "rocker_opt.csm");
    tOptimizationParameters.append("csm_tesselation_file", "rocker.eto");
    tOptimizationParameters.append("csm_exodus_file", "rocker.exo");
    tOptimizationParameters.append("num_shape_design_variables", "3");
    tXMLMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::append_update_geometry_on_change_operation_to_plato_main_operation(tXMLMetaData, tDocument));
    ASSERT_FALSE(tDocument.empty());

    // TEST RESULTS AGAINST GOLD VALUES
    auto tOperation = tDocument.child("Operation");
    ASSERT_FALSE(tOperation.empty());
    ASSERT_STREQ("Operation", tOperation.name());
    std::vector<std::string> tKeys = {"Function", "Name", 
        "Command", "OnChange", 
        "Argument", "Argument", "Argument", 
        "Argument", "Argument", 
        "AppendInput", "Input"};
    std::vector<std::string> tValues = {"SystemCall", "update_geometry_on_change_0", 
        "cd evaluations_0; plato-cli geometry esp", "true", 
        "--input rocker_0.csm", "--output-model rocker_opt_0.csm", "--output-mesh rocker_0.exo",
        "--tesselation rocker_0.eto", "--parameters",
        "true", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);
    auto tInput = tOperation.child("Input");
    ASSERT_FALSE(tInput.empty());
    tKeys = {"ArgumentName", "Layout", "Size"};
    tValues = {"Parameters", "scalar", "3"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tInput);

    tOperation = tOperation.next_sibling("Operation");
    ASSERT_FALSE(tOperation.empty());
    ASSERT_STREQ("Operation", tOperation.name());
    tKeys = {"Function", "Name", 
        "Command", "OnChange", 
        "Argument", "Argument", "Argument", 
        "Argument", "Argument", 
        "AppendInput", "Input"};
    tValues = {"SystemCall", "update_geometry_on_change_1", 
        "cd evaluations_1; plato-cli geometry esp", "true", 
        "--input rocker_1.csm", "--output-model rocker_opt_1.csm", "--output-mesh rocker_1.exo",
        "--tesselation rocker_1.eto", "--parameters",
        "true", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);
    tInput = tOperation.child("Input");
    ASSERT_FALSE(tInput.empty());
    tKeys = {"ArgumentName", "Layout", "Size"};
    tValues = {"Parameters", "scalar", "3"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tInput);

    tOperation = tOperation.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());
}

TEST(PlatoTestXMLGenerator, AppendReinitializeToPlatoMainOperationsFile)
{
    XMLGen::InputData tXMLMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tOptimizationParameters.append("concurrent_evaluations", "2");
    tXMLMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::append_reinitialize_operation_to_plato_main_operation(tXMLMetaData, tDocument));
    ASSERT_FALSE(tDocument.empty());

    // TEST RESULTS AGAINST GOLD VALUES
    auto tOperation = tDocument.child("Operation");
    ASSERT_FALSE(tOperation.empty());
    ASSERT_STREQ("Operation", tOperation.name());
    std::vector<std::string> tKeys = {"Function", "Name"};
    std::vector<std::string> tValues = {"Reinitialize", "reinitialize_on_change_plato_services"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);

    tOperation = tOperation.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());
}

TEST(PlatoTestXMLGenerator, AppendComputeCriterionValueToPlatoMainOperationsFile)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("2");
    tService.code("platomain");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("13");
    tCriterion.type("volume");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);

    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "single_criterion";
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("13");
    tObjective.scenarioIDs.push_back("14");
    tMetaData.objective = tObjective;

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tOptimizationParameters.append("concurrent_evaluations", "2");
    tMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::append_compute_volume_criterion_value_operation_to_plato_main_operation(tMetaData, tDocument));
    ASSERT_FALSE(tDocument.empty());

    // TEST RESULTS AGAINST GOLD VALUES
    auto tOperation = tDocument.child("Operation");
    ASSERT_FALSE(tOperation.empty());
    ASSERT_STREQ("Operation", tOperation.name());
    std::vector<std::string> tKeys = {"Function", "Name", "Output"};
    std::vector<std::string> tValues = {"DesignVolume", "Compute Criterion Value - criterion_13_service_2_scenario_14", ""};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOperation);

    auto tOutput = tOperation.child("Output");
    ASSERT_FALSE(tOutput.empty());
    tKeys = {"ArgumentName"};
    tValues = {"criterion_13_service_2_scenario_14 value"};
    PlatoTestXMLGenerator::test_children(tKeys, tValues, tOutput);

    tOutput = tOutput.next_sibling("Output");
    ASSERT_TRUE(tOutput.empty());

    tOperation = tOperation.next_sibling("Operation");
    ASSERT_TRUE(tOperation.empty());
}

TEST(PlatoTestXMLGenerator, AppendConcurrentEvaluationsToDefinesFile)
{
    XMLGen::InputData tXMLMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tOptimizationParameters.append("concurrent_evaluations", "2");
    tXMLMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::add_dakota_concurrent_evaluations_data_to_define_xml_file(tXMLMetaData, tDocument));
    ASSERT_FALSE(tDocument.empty());

    // TEST RESULTS AGAINST GOLD VALUES
    auto tDefine = tDocument.child("Define");
    ASSERT_FALSE(tDefine.empty());
    PlatoTestXMLGenerator::test_attributes({"name", "value"}, {"NumParameters", "2"}, tDefine);
    tDefine = tDefine.next_sibling("Define");
    ASSERT_TRUE(tDefine.empty());

    auto tArray = tDocument.child("Array");
    ASSERT_FALSE(tArray.empty());
    PlatoTestXMLGenerator::test_attributes({"name", "type", "from", "to"}, {"Parameters", "int", "0", "{NumParameters-1}"}, tArray);
    tArray = tArray.next_sibling("Array");
    ASSERT_TRUE(tArray.empty());
}

TEST(PlatoTestXMLGenerator, AppendMPIRunLinesToLaunchScript_PAPerformer)
{
    XMLGen::InputData tInputData;
    tInputData.m_UseLaunch = false;
    tInputData.mesh.run_name = "dummy_mesh.exo";
    XMLGen::Service tService;
    tService.numberProcessors("1");
    tService.id("1");
    tService.code("platomain");
    tService.path("/home/path/to/PlatoMain");
    tInputData.append(tService);
    tService.numberProcessors("1");
    tService.id("2");
    tService.code("plato_analyze");
    tService.path("");
    tInputData.append(tService);
    tService.numberProcessors("1");
    tService.id("3");
    tService.code("plato_services");
    tService.path("/home/path/to/PlatoEngineServices");
    tInputData.append(tService);

    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tOptimizationParameters.append("concurrent_evaluations", "2");
    tOptimizationParameters.append("csm_file", "rocker.csm");
    tOptimizationParameters.append("csm_opt_file", "rocker_opt.csm");
    tOptimizationParameters.append("csm_tesselation_file", "rocker.eto");
    tOptimizationParameters.append("csm_exodus_file", "rocker.exo");
    tInputData.set(tOptimizationParameters);

    ASSERT_NO_THROW(XMLGen::generate_mpirun_launch_script(tInputData));

    auto tReadData = XMLGen::read_data_from_file("mpirun.source");
    auto tGold = std::string("plato-cligeometryesp--inputrocker.csm--output-modelrocker_opt.csm--output-meshrocker.exo--tesselationrocker.eto;\\") + 
        std::string("cprocker.exoevaluations_0/rocker_0.exo") + 
        std::string("cprocker.exoevaluations_1/rocker_1.exo") + 
        std::string("mpiexec--oversubscribe-np1-xPLATO_PERFORMER_ID=0\\") + 
        std::string("-xPLATO_INTERFACE_FILE=interface.xml\\") + 
        std::string("-xPLATO_APP_FILE=plato_main_operations.xml\\") + 
        std::string("/home/path/to/PlatoMainplato_main_input_deck.xml\\") + 
        std::string(":-np1-xPLATO_PERFORMER_ID=1\\") + 
        std::string("-xPLATO_INTERFACE_FILE=interface.xml\\") + 
        std::string("-xPLATO_APP_FILE=plato_analyze_2_operations.xml\\") + 
        std::string("analyze_MPMD--input-config=evaluations_0/plato_analyze_2_input_deck_0.xml\\") + 
        std::string(":-np1-xPLATO_PERFORMER_ID=2\\") + 
        std::string("-xPLATO_INTERFACE_FILE=interface.xml\\") + 
        std::string("-xPLATO_APP_FILE=plato_analyze_2_operations.xml\\") + 
        std::string("analyze_MPMD--input-config=evaluations_1/plato_analyze_2_input_deck_1.xml\\") + 
        std::string(":-np1-xPLATO_PERFORMER_ID=3\\") + 
        std::string("-xPLATO_INTERFACE_FILE=interface.xml\\") + 
        std::string("-xPLATO_APP_FILE=plato_main_operations.xml\\") + 
        std::string("/home/path/to/PlatoEngineServicesevaluations_0/plato_main_input_deck_0.xml\\") +
        std::string(":-np1-xPLATO_PERFORMER_ID=4\\") + 
        std::string("-xPLATO_INTERFACE_FILE=interface.xml\\") + 
        std::string("-xPLATO_APP_FILE=plato_main_operations.xml\\") + 
        std::string("/home/path/to/PlatoEngineServicesevaluations_1/plato_main_input_deck_1.xml\\");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf mpirun.source");
}

TEST(PlatoTestXMLGenerator, WritePlatoAnalyzeOperationsXmlFileForDakotaDriver_MultiObjective)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("volume");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "multi_objective";
    tObjective.serviceIDs.push_back("2");
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.criteriaIDs.push_back("7");
    tObjective.scenarioIDs.push_back("14");
    tObjective.scenarioIDs.push_back("14");
    tObjective.weights.push_back("1");
    tObjective.weights.push_back("1");
    tMetaData.objective = tObjective;

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tMetaData.set(tOptimizationParameters);

    // Create output
    XMLGen::Output tOutputMetadata;
    tOutputMetadata.disableOutput();
    tOutputMetadata.appendParam("native_service_output", "false");
    tMetaData.mOutputMetaData.push_back(tOutputMetadata);

    ASSERT_NO_THROW(XMLGen::write_plato_analyze_operation_xml_file_dakota_problem(tMetaData));

    auto tReadData = XMLGen::read_data_from_file("plato_analyze_2_operations.xml");
    auto tGold = std::string("<?xmlversion=\"1.0\"?>") + 
        std::string("<Operation><Name>reinitialize_on_change_plato_analyze_2</Name>") + 
        std::string("<Function>Reinitialize</Function>") + 
        std::string("<OnChange>true</OnChange>") + 
        std::string("<Input><ArgumentName>Parameters</ArgumentName></Input></Operation>") + 
        std::string("<Operation><Function>ComputeCriterionValue</Function>") + 
        std::string("<Name>ComputeCriterionValue-criterion_3_service_2_scenario_14</Name>") + 
        std::string("<Criterion>my_mechanical_compliance_criterion_id_3</Criterion>") + 
        std::string("<Output><Argument>Value</Argument><ArgumentName>criterion_3_service_2_scenario_14value</ArgumentName></Output></Operation>") + 
        std::string("<Operation><Function>ComputeCriterionValue</Function>") + 
        std::string("<Name>ComputeCriterionValue-criterion_7_service_2_scenario_14</Name>") + 
        std::string("<Criterion>my_volume_criterion_id_7</Criterion>") + 
        std::string("<Output><Argument>Value</Argument><ArgumentName>criterion_7_service_2_scenario_14value</ArgumentName></Output></Operation>"); 

    ASSERT_STREQ(tGold.c_str(), tReadData.str().c_str());
    Plato::system("rm -f plato_analyze_2_operations.xml");
}

TEST(PlatoTestXMLGenerator, WriteOnlyFirstOfMultiPlatoAnalyzeServiceOperationsXmlFilesForDakotaDriver_MultiObjective)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);
    tService.id("5");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("volume");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "multi_objective";
    tObjective.serviceIDs.push_back("2");
    tObjective.serviceIDs.push_back("5");
    tObjective.criteriaIDs.push_back("3");
    tObjective.criteriaIDs.push_back("7");
    tObjective.scenarioIDs.push_back("14");
    tObjective.scenarioIDs.push_back("14");
    tObjective.weights.push_back("1");
    tObjective.weights.push_back("1");
    tMetaData.objective = tObjective;

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tMetaData.set(tOptimizationParameters);

    // Create output
    XMLGen::Output tOutputMetadata;
    tOutputMetadata.disableOutput();
    tOutputMetadata.appendParam("native_service_output", "false");
    tMetaData.mOutputMetaData.push_back(tOutputMetadata);

    ASSERT_NO_THROW(XMLGen::write_plato_analyze_operation_xml_file_dakota_problem(tMetaData));

    auto tReadData = XMLGen::read_data_from_file("plato_analyze_2_operations.xml");
    auto tGold = std::string("<?xmlversion=\"1.0\"?>") + 
        std::string("<Operation><Name>reinitialize_on_change_plato_analyze_2</Name>") + 
        std::string("<Function>Reinitialize</Function>") + 
        std::string("<OnChange>true</OnChange>") + 
        std::string("<Input><ArgumentName>Parameters</ArgumentName></Input></Operation>") + 
        std::string("<Operation><Function>ComputeCriterionValue</Function>") + 
        std::string("<Name>ComputeCriterionValue-criterion_3_service_2_scenario_14</Name>") + 
        std::string("<Criterion>my_mechanical_compliance_criterion_id_3</Criterion>") + 
        std::string("<Output><Argument>Value</Argument><ArgumentName>criterion_3_service_2_scenario_14value</ArgumentName></Output></Operation>");

    ASSERT_STREQ(tGold.c_str(), tReadData.str().c_str());
    Plato::system("rm -f plato_analyze_2_operations.xml");
}

TEST(PlatoTestXMLGenerator, WritePlatoAnalyzeOperationsXmlFileForDakotaDriver_SingleObjectiveSingleConstraint)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("volume");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "multi_objective";
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.scenarioIDs.push_back("14");
    tObjective.weights.push_back("1");
    tMetaData.objective = tObjective;
    
    // Create a constraint
    XMLGen::Constraint tConstraint;
    tConstraint.id("8");
    tConstraint.criterion("7");
    tConstraint.service("2");
    tConstraint.scenario("14");
    tMetaData.constraints.push_back(tConstraint);

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tMetaData.set(tOptimizationParameters);

    // Create output
    XMLGen::Output tOutputMetadata;
    tOutputMetadata.disableOutput();
    tOutputMetadata.appendParam("native_service_output", "false");
    tMetaData.mOutputMetaData.push_back(tOutputMetadata);

    ASSERT_NO_THROW(XMLGen::write_plato_analyze_operation_xml_file_dakota_problem(tMetaData));

    auto tReadData = XMLGen::read_data_from_file("plato_analyze_2_operations.xml");
    auto tGold = std::string("<?xmlversion=\"1.0\"?>") + 
        std::string("<Operation><Name>reinitialize_on_change_plato_analyze_2</Name>") + 
        std::string("<Function>Reinitialize</Function>") + 
        std::string("<OnChange>true</OnChange>") + 
        std::string("<Input><ArgumentName>Parameters</ArgumentName></Input></Operation>") + 
        std::string("<Operation><Function>ComputeCriterionValue</Function>") + 
        std::string("<Name>ComputeCriterionValue-criterion_3_service_2_scenario_14</Name>") + 
        std::string("<Criterion>my_mechanical_compliance_criterion_id_3</Criterion>") + 
        std::string("<Output><Argument>Value</Argument><ArgumentName>criterion_3_service_2_scenario_14value</ArgumentName></Output></Operation>") + 
        std::string("<Operation><Function>ComputeCriterionValue</Function>") + 
        std::string("<Name>ComputeCriterionValue-criterion_7_service_2_scenario_14</Name>") + 
        std::string("<Criterion>my_volume_criterion_id_7</Criterion>") + 
        std::string("<Output><Argument>Value</Argument><ArgumentName>criterion_7_service_2_scenario_14value</ArgumentName></Output></Operation>"); 

    ASSERT_STREQ(tGold.c_str(), tReadData.str().c_str());
    Plato::system("rm -f plato_analyze_2_operations.xml");
}

TEST(PlatoTestXMLGenerator, WriteOnlyFirstOfMultiPlatoAnalyzeServiceOperationsXmlFilesForDakotaDriver_SingleObjectiveMultiConstraint)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);
    tService.id("20");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("volume");
    tMetaData.append(tCriterion);
    tCriterion.id("5");
    tCriterion.type("stress_p-norm");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    tScenario.id("1");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "multi_objective";
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.scenarioIDs.push_back("14");
    tObjective.weights.push_back("1");
    tMetaData.objective = tObjective;
    
    // Create constraints
    XMLGen::Constraint tConstraint;
    tConstraint.id("8");
    tConstraint.criterion("7");
    tConstraint.service("20");
    tConstraint.scenario("14");
    tMetaData.constraints.push_back(tConstraint);
    tConstraint.id("13");
    tConstraint.criterion("5");
    tConstraint.service("2");
    tConstraint.scenario("1");
    tMetaData.constraints.push_back(tConstraint);

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tMetaData.set(tOptimizationParameters);

    // Create output
    XMLGen::Output tOutputMetadata;
    tOutputMetadata.disableOutput();
    tOutputMetadata.appendParam("native_service_output", "false");
    tMetaData.mOutputMetaData.push_back(tOutputMetadata);

    ASSERT_NO_THROW(XMLGen::write_plato_analyze_operation_xml_file_dakota_problem(tMetaData));

    auto tReadData = XMLGen::read_data_from_file("plato_analyze_2_operations.xml");
    auto tGold = std::string("<?xmlversion=\"1.0\"?>") + 
        std::string("<Operation><Name>reinitialize_on_change_plato_analyze_2</Name>") + 
        std::string("<Function>Reinitialize</Function>") + 
        std::string("<OnChange>true</OnChange>") + 
        std::string("<Input><ArgumentName>Parameters</ArgumentName></Input></Operation>") + 
        std::string("<Operation><Function>ComputeCriterionValue</Function>") + 
        std::string("<Name>ComputeCriterionValue-criterion_3_service_2_scenario_14</Name>") + 
        std::string("<Criterion>my_mechanical_compliance_criterion_id_3</Criterion>") + 
        std::string("<Output><Argument>Value</Argument><ArgumentName>criterion_3_service_2_scenario_14value</ArgumentName></Output></Operation>") + 
        std::string("<Operation><Function>ComputeCriterionValue</Function>") + 
        std::string("<Name>ComputeCriterionValue-criterion_5_service_2_scenario_1</Name>") + 
        std::string("<Criterion>my_stress_p-norm_criterion_id_5</Criterion>") + 
        std::string("<Output><Argument>Value</Argument><ArgumentName>criterion_5_service_2_scenario_1value</ArgumentName></Output></Operation>"); 

    ASSERT_STREQ(tGold.c_str(), tReadData.str().c_str());
    Plato::system("rm -f plato_analyze_2_operations.xml");
}

TEST(PlatoTestXMLGenerator, AppendCriteriaListToPlatoAnalyzeInputFileForDakotaDriver_SingleObjective)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("1");
    tService.code("platomain");
    tMetaData.append(tService);
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create a criterion
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tCriterion.materialPenaltyExponent("5.0");
    tCriterion.minErsatzMaterialConstant("1e-8");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "single_criterion";
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.scenarioIDs.push_back("14");
    tMetaData.objective = tObjective;

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::append_criteria_list_to_plato_analyze_input_deck(tMetaData, tDocument));
    ASSERT_FALSE(tDocument.empty());

    // CRITERION LIST 
    auto tParamList = tDocument.child("ParameterList");
    ASSERT_FALSE(tParamList.empty());
    ASSERT_STREQ("ParameterList", tParamList.name());
    PlatoTestXMLGenerator::test_attributes({"name"}, {"Criteria"}, tParamList);

    // CRITERION 0
    tParamList = tParamList.child("ParameterList");
    ASSERT_FALSE(tParamList.empty());
    ASSERT_STREQ("ParameterList", tParamList.name());
    PlatoTestXMLGenerator::test_attributes({"name"}, {"my_mechanical_compliance_criterion_id_3"}, tParamList);
    auto tChild = tParamList.child("Parameter");
    ASSERT_FALSE(tChild.empty());
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Type", "string", "Scalar Function"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_FALSE(tChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Scalar Function Type", "string", "Internal Elastic Energy"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_TRUE(tChild.empty());

    auto tInnerParamList = tParamList.child("ParameterList");
    ASSERT_FALSE(tInnerParamList.empty());
    auto tInnerChild = tInnerParamList.child("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Type", "string", "SIMP"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Exponent", "double", "5.0"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Minimum Value", "double", "1e-8"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_TRUE(tInnerChild.empty());

    tInnerParamList = tInnerParamList.next_sibling("ParameterList");
    ASSERT_TRUE(tInnerParamList.empty());

    tParamList = tParamList.next_sibling("ParameterList");
    ASSERT_TRUE(tParamList.empty());
}

TEST(PlatoTestXMLGenerator, AppendCriteriaListToPlatoAnalyzeInputFileForDakotaDriver_MultiObjective)
{
    XMLGen::InputData tMetaData;

    // Create a service
    XMLGen::Service tService;
    tService.id("1");
    tService.code("platomain");
    tMetaData.append(tService);
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tCriterion.materialPenaltyExponent("3.0");
    tCriterion.minErsatzMaterialConstant("1e-8");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("volume");
    tCriterion.materialPenaltyExponent("3.0");
    tCriterion.minErsatzMaterialConstant("1e-8");
    tMetaData.append(tCriterion);

    // Create a scenario
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "multi_objective";
    tObjective.serviceIDs.push_back("2");
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.criteriaIDs.push_back("7");
    tObjective.scenarioIDs.push_back("14");
    tObjective.scenarioIDs.push_back("14");
    tObjective.weights.push_back("1");
    tObjective.weights.push_back("1");
    tMetaData.objective = tObjective;

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::append_criteria_list_to_plato_analyze_input_deck(tMetaData, tDocument));
    ASSERT_FALSE(tDocument.empty());

    // CRITERION LIST 
    auto tParamList = tDocument.child("ParameterList");
    ASSERT_FALSE(tParamList.empty());
    ASSERT_STREQ("ParameterList", tParamList.name());
    PlatoTestXMLGenerator::test_attributes({"name"}, {"Criteria"}, tParamList);

    // CRITERION 0
    tParamList = tParamList.child("ParameterList");
    ASSERT_FALSE(tParamList.empty());
    ASSERT_STREQ("ParameterList", tParamList.name());
    PlatoTestXMLGenerator::test_attributes({"name"}, {"my_mechanical_compliance_criterion_id_3"}, tParamList);
    auto tChild = tParamList.child("Parameter");
    ASSERT_FALSE(tChild.empty());
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Type", "string", "Scalar Function"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_FALSE(tChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Scalar Function Type", "string", "Internal Elastic Energy"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_TRUE(tChild.empty());

    auto tInnerParamList = tParamList.child("ParameterList");
    ASSERT_FALSE(tInnerParamList.empty());
    auto tInnerChild = tInnerParamList.child("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Type", "string", "SIMP"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Exponent", "double", "3.0"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Minimum Value", "double", "1e-8"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_TRUE(tInnerChild.empty());

    tInnerParamList = tInnerParamList.next_sibling("ParameterList");
    ASSERT_TRUE(tInnerParamList.empty());

    // CRITERION 1
    tParamList = tParamList.next_sibling("ParameterList");
    ASSERT_FALSE(tParamList.empty());
    ASSERT_STREQ("ParameterList", tParamList.name());
    PlatoTestXMLGenerator::test_attributes({"name"}, {"my_volume_criterion_id_7"}, tParamList);
    tChild = tParamList.child("Parameter");
    ASSERT_FALSE(tChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Type", "string", "Scalar Function"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_FALSE(tChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Linear", "bool", "true"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_FALSE(tChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Scalar Function Type", "string", "Volume"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_TRUE(tChild.empty());

    tInnerParamList = tParamList.child("ParameterList");
    ASSERT_FALSE(tInnerParamList.empty());
    tInnerChild = tInnerParamList.child("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Type", "string", "SIMP"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Exponent", "double", "3.0"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Minimum Value", "double", "1e-8"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_TRUE(tInnerChild.empty());

    tInnerParamList = tInnerParamList.next_sibling("ParameterList");
    ASSERT_TRUE(tInnerParamList.empty());

    tParamList = tParamList.next_sibling("ParameterList");
    ASSERT_TRUE(tParamList.empty());
}

TEST(PlatoTestXMLGenerator, AppendCriteriaListToPlatoAnalyzeInputFileForDakotaDriver_SingleObjectiveSingleConstraint)
{
    XMLGen::InputData tMetaData;

    // Create services
    XMLGen::Service tService;
    tService.id("1");
    tService.code("platomain");
    tMetaData.append(tService);
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tService.id("5");
    tService.code("plato_analyze");
    tMetaData.append(tService);
    tMetaData.mPerformerServices.push_back(tService);

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tCriterion.materialPenaltyExponent("3.0");
    tCriterion.minErsatzMaterialConstant("1e-8");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("stress_p-norm");
    tCriterion.materialPenaltyExponent("3.0");
    tCriterion.minErsatzMaterialConstant("1e-8");
    tMetaData.append(tCriterion);

    // Create scenarios
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tMetaData.append(tScenario);
    tScenario.id("3");
    tScenario.physics("transient_mechanics");
    tMetaData.append(tScenario);
    
    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "single_criterion";
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.scenarioIDs.push_back("14");
    tMetaData.objective = tObjective;

    // Create a constraint
    XMLGen::Constraint tConstraint;
    tConstraint.id("8");
    tConstraint.criterion("7");
    tConstraint.service("5");
    tConstraint.scenario("3");
    tMetaData.constraints.push_back(tConstraint);

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tMetaData.set(tOptimizationParameters);

    pugi::xml_document tDocument;
    ASSERT_NO_THROW(XMLGen::append_criteria_list_to_plato_analyze_input_deck(tMetaData, tDocument));
    ASSERT_FALSE(tDocument.empty());

    // CRITERION LIST 
    auto tParamList = tDocument.child("ParameterList");
    ASSERT_FALSE(tParamList.empty());
    ASSERT_STREQ("ParameterList", tParamList.name());
    PlatoTestXMLGenerator::test_attributes({"name"}, {"Criteria"}, tParamList);

    // CRITERION 0
    tParamList = tParamList.child("ParameterList");
    ASSERT_FALSE(tParamList.empty());
    ASSERT_STREQ("ParameterList", tParamList.name());
    PlatoTestXMLGenerator::test_attributes({"name"}, {"my_mechanical_compliance_criterion_id_3"}, tParamList);
    auto tChild = tParamList.child("Parameter");
    ASSERT_FALSE(tChild.empty());
    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Type", "string", "Scalar Function"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_FALSE(tChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Scalar Function Type", "string", "Internal Elastic Energy"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_TRUE(tChild.empty());

    auto tInnerParamList = tParamList.child("ParameterList");
    ASSERT_FALSE(tInnerParamList.empty());
    auto tInnerChild = tInnerParamList.child("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Type", "string", "SIMP"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Exponent", "double", "3.0"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Minimum Value", "double", "1e-8"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_TRUE(tInnerChild.empty());

    tInnerParamList = tInnerParamList.next_sibling("ParameterList");
    ASSERT_TRUE(tInnerParamList.empty());

    // CRITERION 1
    tParamList = tParamList.next_sibling("ParameterList");
    ASSERT_FALSE(tParamList.empty());
    ASSERT_STREQ("ParameterList", tParamList.name());
    PlatoTestXMLGenerator::test_attributes({"name"}, {"my_stress_p-norm_criterion_id_7"}, tParamList);
    tChild = tParamList.child("Parameter");
    ASSERT_FALSE(tChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Type", "string", "Scalar Function"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_FALSE(tChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Scalar Function Type", "string", "Stress P-Norm"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_FALSE(tChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Exponent", "double", ""};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tChild);
    tChild = tChild.next_sibling("Parameter");
    ASSERT_TRUE(tChild.empty());

    tInnerParamList = tParamList.child("ParameterList");
    ASSERT_FALSE(tInnerParamList.empty());
    tInnerChild = tInnerParamList.child("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Type", "string", "SIMP"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Exponent", "double", "3.0"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_FALSE(tInnerChild.empty());
    tKeys = {"name", "type", "value"};
    tValues = {"Minimum Value", "double", "1e-8"};
    PlatoTestXMLGenerator::test_attributes(tKeys, tValues, tInnerChild);
    tInnerChild = tInnerChild.next_sibling("Parameter");
    ASSERT_TRUE(tInnerChild.empty());

    tInnerParamList = tInnerParamList.next_sibling("ParameterList");
    ASSERT_TRUE(tInnerParamList.empty());

    tParamList = tParamList.next_sibling("ParameterList");
    ASSERT_TRUE(tParamList.empty());
}

TEST(PlatoTestXMLGenerator, CheckThatDirectoryExists)
{
    std::string tDirectoryName = "evaluations_0";
    Plato::system("rm -rf evaluations_0");
    ASSERT_FALSE(XMLGen::subdirectory_exists(tDirectoryName));
    Plato::system("mkdir evaluations_0");
    ASSERT_TRUE(XMLGen::subdirectory_exists(tDirectoryName));
    Plato::system("rm -rf evaluations_0");
    ASSERT_FALSE(XMLGen::subdirectory_exists(tDirectoryName));
}

TEST(PlatoTestXMLGenerator, WritePlatoAnalyzeInputXmlFilesForDakotaDriver)
{
    XMLGen::InputData tMetaData1;
    XMLGen::InputData tMetaData2;

    // Create a service
    XMLGen::Service tService;
    tService.id("2");
    tService.code("plato_analyze");
    tMetaData1.append(tService);
    tMetaData1.mPerformerServices.push_back(tService);
    tService.id("7");
    tService.code("plato_analyze");
    tMetaData2.append(tService);
    tMetaData2.mPerformerServices.push_back(tService);

    // Create scenarios
    XMLGen::Scenario tScenario;
    tScenario.id("14");
    tScenario.physics("steady_state_mechanics");
    tScenario.dimensions("3");
    tMetaData1.append(tScenario);
    tScenario.id("9");
    tScenario.physics("steady_state_mechanics");
    tScenario.dimensions("3");
    tMetaData2.append(tScenario);

    // create mesh
    tMetaData1.mesh.run_name = "rocker.exo";
    tMetaData2.mesh.run_name = "rocker.exo";

    // create a block
    XMLGen::Block tBlock;
    tBlock.block_id = "1";
    tBlock.element_type = "tet4";
    tBlock.material_id = "1";
    tBlock.name = "block_1";
    tMetaData1.blocks.push_back(tBlock);
    tMetaData2.blocks.push_back(tBlock);

    // create a material
    XMLGen::Material tMaterial;
    tMaterial.id("1");
    tMaterial.code("plato_analyze");
    tMaterial.name("adamantium");
    tMaterial.materialModel("isotropic_linear_elastic");
    tMaterial.property("youngs_modulus", "1e9");
    tMaterial.property("poissons_ratio", "0.3");
    tMetaData1.materials.push_back(tMaterial);
    tMaterial.property("youngs_modulus", "3e9");
    tMaterial.property("poissons_ratio", "0.4");
    tMetaData2.materials.push_back(tMaterial);
    
    // Create objectives
    XMLGen::Objective tObjective1;
    tObjective1.serviceIDs.push_back("2");
    tObjective1.scenarioIDs.push_back("14");
    tMetaData1.objective = tObjective1;
    XMLGen::Objective tObjective2;
    tObjective2.serviceIDs.push_back("7");
    tObjective2.scenarioIDs.push_back("9");
    tMetaData2.objective = tObjective2;

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tOptimizationParameters.append("concurrent_evaluations", "2");
    tOptimizationParameters.append("csm_file", "rocker.csm");
    tMetaData1.set(tOptimizationParameters);
    tMetaData2.set(tOptimizationParameters);

    // Create dummy files to copy
    FILE* exo=fopen("rocker.exo", "w");
    FILE* csm=fopen("rocker.csm", "w");
    Plato::system("rm -rf evaluations_0");
    Plato::system("rm -rf evaluations_1");

    ASSERT_NO_THROW(XMLGen::Problem::create_concurrent_evaluation_subdirectories(tMetaData1));
    ASSERT_NO_THROW(XMLGen::dakota::write_performer_input_deck_file(tMetaData1));
    ASSERT_NO_THROW(XMLGen::dakota::write_performer_input_deck_file(tMetaData2));

    auto tReadData = XMLGen::read_data_from_file("evaluations_0/plato_analyze_2_input_deck_0.xml");
    auto tGold = std::string("<?xmlversion=\"1.0\"?><ParameterListname=\"Problem\"><Parametername=\"Physics\"type=\"string\"value=\"PlatoDriver\"/>") + 
                 std::string("<Parametername=\"SpatialDimension\"type=\"int\"value=\"3\"/><Parametername=\"InputMesh\"type=\"string\"value=\"evaluations_0/rocker_0.exo\"/>") + 
                 std::string("<ParameterListname=\"PlatoProblem\"><Parametername=\"Physics\"type=\"string\"value=\"Mechanical\"/>") + 
                 std::string("<Parametername=\"PDEConstraint\"type=\"string\"value=\"Elliptic\"/><Parametername=\"Self-Adjoint\"type=\"bool\"value=\"false\"/>") + 
                 std::string("<ParameterListname=\"Criteria\"/><ParameterListname=\"Elliptic\"><ParameterListname=\"PenaltyFunction\"><Parametername=\"Type\"type=\"string\"value=\"SIMP\"/>") + 
                 std::string("</ParameterList></ParameterList><ParameterListname=\"SpatialModel\"><ParameterListname=\"Domains\"><ParameterListname=\"Block1\"><Parametername=\"ElementBlock\"type=\"string\"value=\"block_1\"/><Parametername=\"MaterialModel\"type=\"string\"value=\"adamantium\"/>") + 
                 std::string("</ParameterList></ParameterList></ParameterList><ParameterListname=\"MaterialModels\"><ParameterListname=\"adamantium\"><ParameterListname=\"IsotropicLinearElastic\"><Parametername=\"PoissonsRatio\"type=\"double\"value=\"0.3\"/>") + 
                 std::string("<Parametername=\"YoungsModulus\"type=\"double\"value=\"1e9\"/></ParameterList></ParameterList></ParameterList><ParameterListname=\"NaturalBoundaryConditions\"/></ParameterList></ParameterList>");

    ASSERT_STREQ(tGold.c_str(), tReadData.str().c_str());

    tReadData = XMLGen::read_data_from_file("evaluations_1/plato_analyze_2_input_deck_1.xml");
    tGold = std::string("<?xmlversion=\"1.0\"?><ParameterListname=\"Problem\"><Parametername=\"Physics\"type=\"string\"value=\"PlatoDriver\"/>") + 
                 std::string("<Parametername=\"SpatialDimension\"type=\"int\"value=\"3\"/><Parametername=\"InputMesh\"type=\"string\"value=\"evaluations_1/rocker_1.exo\"/>") + 
                 std::string("<ParameterListname=\"PlatoProblem\"><Parametername=\"Physics\"type=\"string\"value=\"Mechanical\"/>") + 
                 std::string("<Parametername=\"PDEConstraint\"type=\"string\"value=\"Elliptic\"/><Parametername=\"Self-Adjoint\"type=\"bool\"value=\"false\"/>") + 
                 std::string("<ParameterListname=\"Criteria\"/><ParameterListname=\"Elliptic\"><ParameterListname=\"PenaltyFunction\"><Parametername=\"Type\"type=\"string\"value=\"SIMP\"/>") + 
                 std::string("</ParameterList></ParameterList><ParameterListname=\"SpatialModel\"><ParameterListname=\"Domains\"><ParameterListname=\"Block1\"><Parametername=\"ElementBlock\"type=\"string\"value=\"block_1\"/><Parametername=\"MaterialModel\"type=\"string\"value=\"adamantium\"/>") + 
                 std::string("</ParameterList></ParameterList></ParameterList><ParameterListname=\"MaterialModels\"><ParameterListname=\"adamantium\"><ParameterListname=\"IsotropicLinearElastic\"><Parametername=\"PoissonsRatio\"type=\"double\"value=\"0.3\"/>") + 
                 std::string("<Parametername=\"YoungsModulus\"type=\"double\"value=\"1e9\"/></ParameterList></ParameterList></ParameterList><ParameterListname=\"NaturalBoundaryConditions\"/></ParameterList></ParameterList>");

    ASSERT_STREQ(tGold.c_str(), tReadData.str().c_str());

    tReadData = XMLGen::read_data_from_file("evaluations_0/plato_analyze_7_input_deck_0.xml");
    tGold = std::string("<?xmlversion=\"1.0\"?><ParameterListname=\"Problem\"><Parametername=\"Physics\"type=\"string\"value=\"PlatoDriver\"/>") + 
                 std::string("<Parametername=\"SpatialDimension\"type=\"int\"value=\"3\"/><Parametername=\"InputMesh\"type=\"string\"value=\"evaluations_0/rocker_0.exo\"/>") + 
                 std::string("<ParameterListname=\"PlatoProblem\"><Parametername=\"Physics\"type=\"string\"value=\"Mechanical\"/>") + 
                 std::string("<Parametername=\"PDEConstraint\"type=\"string\"value=\"Elliptic\"/><Parametername=\"Self-Adjoint\"type=\"bool\"value=\"false\"/>") + 
                 std::string("<ParameterListname=\"Criteria\"/><ParameterListname=\"Elliptic\"><ParameterListname=\"PenaltyFunction\"><Parametername=\"Type\"type=\"string\"value=\"SIMP\"/>") + 
                 std::string("</ParameterList></ParameterList><ParameterListname=\"SpatialModel\"><ParameterListname=\"Domains\"><ParameterListname=\"Block1\"><Parametername=\"ElementBlock\"type=\"string\"value=\"block_1\"/><Parametername=\"MaterialModel\"type=\"string\"value=\"adamantium\"/>") + 
                 std::string("</ParameterList></ParameterList></ParameterList><ParameterListname=\"MaterialModels\"><ParameterListname=\"adamantium\"><ParameterListname=\"IsotropicLinearElastic\"><Parametername=\"PoissonsRatio\"type=\"double\"value=\"0.4\"/>") + 
                 std::string("<Parametername=\"YoungsModulus\"type=\"double\"value=\"3e9\"/></ParameterList></ParameterList></ParameterList><ParameterListname=\"NaturalBoundaryConditions\"/></ParameterList></ParameterList>");

    ASSERT_STREQ(tGold.c_str(), tReadData.str().c_str());

    tReadData = XMLGen::read_data_from_file("evaluations_1/plato_analyze_7_input_deck_1.xml");
    tGold = std::string("<?xmlversion=\"1.0\"?><ParameterListname=\"Problem\"><Parametername=\"Physics\"type=\"string\"value=\"PlatoDriver\"/>") + 
                 std::string("<Parametername=\"SpatialDimension\"type=\"int\"value=\"3\"/><Parametername=\"InputMesh\"type=\"string\"value=\"evaluations_1/rocker_1.exo\"/>") + 
                 std::string("<ParameterListname=\"PlatoProblem\"><Parametername=\"Physics\"type=\"string\"value=\"Mechanical\"/>") + 
                 std::string("<Parametername=\"PDEConstraint\"type=\"string\"value=\"Elliptic\"/><Parametername=\"Self-Adjoint\"type=\"bool\"value=\"false\"/>") + 
                 std::string("<ParameterListname=\"Criteria\"/><ParameterListname=\"Elliptic\"><ParameterListname=\"PenaltyFunction\"><Parametername=\"Type\"type=\"string\"value=\"SIMP\"/>") + 
                 std::string("</ParameterList></ParameterList><ParameterListname=\"SpatialModel\"><ParameterListname=\"Domains\"><ParameterListname=\"Block1\"><Parametername=\"ElementBlock\"type=\"string\"value=\"block_1\"/><Parametername=\"MaterialModel\"type=\"string\"value=\"adamantium\"/>") + 
                 std::string("</ParameterList></ParameterList></ParameterList><ParameterListname=\"MaterialModels\"><ParameterListname=\"adamantium\"><ParameterListname=\"IsotropicLinearElastic\"><Parametername=\"PoissonsRatio\"type=\"double\"value=\"0.4\"/>") + 
                 std::string("<Parametername=\"YoungsModulus\"type=\"double\"value=\"3e9\"/></ParameterList></ParameterList></ParameterList><ParameterListname=\"NaturalBoundaryConditions\"/></ParameterList></ParameterList>");

    ASSERT_STREQ(tGold.c_str(), tReadData.str().c_str());

    // Remove dummy files
    Plato::system("rm -rf evaluations_0");
    Plato::system("rm -rf evaluations_1");
    Plato::system("rm -rf rocker.exo");
    Plato::system("rm -rf rocker.csm");
}

TEST(PlatoTestXMLGenerator, WritePlatoServicesInputXmlFilesForDakotaDriver)
{
    XMLGen::InputData tMetaData;

    // Create service
    XMLGen::Service tService;
    tService.id("1");
    tService.code("platomain");
    tMetaData.append(tService);

    // create mesh
    tMetaData.mesh.run_name = "rocker.exo";

    // create a block
    XMLGen::Block tBlock;
    tBlock.block_id = "1";
    tBlock.element_type = "tet10";
    tBlock.material_id = "1";
    tBlock.name = "block_1";
    tMetaData.blocks.push_back(tBlock);

    // create materials
    XMLGen::Material tMaterial;
    tMaterial.id("1");
    tMaterial.name("adamantium");
    tMaterial.materialModel("isotropic_linear_elastic");
    tMaterial.property("youngs_modulus", "1e9");
    tMaterial.property("poissons_ratio", "0.3");
    tMetaData.materials.push_back(tMaterial);

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.optimizationType(XMLGen::OT_DAKOTA);
    tOptimizationParameters.append("concurrent_evaluations", "2");
    tOptimizationParameters.append("csm_file", "rocker.csm");
    tMetaData.set(tOptimizationParameters);

    // Create dummy files to copy
    FILE* exo=fopen("rocker.exo", "w");
    FILE* csm=fopen("rocker.csm", "w");
    Plato::system("rm -rf evaluations_0");
    Plato::system("rm -rf evaluations_1");

    ASSERT_NO_THROW(XMLGen::Problem::create_concurrent_evaluation_subdirectories(tMetaData));
    ASSERT_NO_THROW(XMLGen::Problem::write_plato_services_performer_input_deck_files(tMetaData));

    auto tReadData = XMLGen::read_data_from_file("evaluations_0/plato_main_input_deck_0.xml");
    auto tGold = std::string("<?xmlversion=\"1.0\"?><mesh><type>unstructured</type><format>exodus</format><ignore_node_map>true</ignore_node_map>") + 
                 std::string("<ignore_element_map>true</ignore_element_map><mesh>evaluations_0/rocker_0.exo</mesh>") + 
                 std::string("<block><index>1</index><material>1</material><integration><type>gauss</type><order>2</order></integration>") + 
                 std::string("</block></mesh><output><file>platomain</file><format>exodus</format></output>");

    ASSERT_STREQ(tGold.c_str(), tReadData.str().c_str());

    tReadData = XMLGen::read_data_from_file("evaluations_1/plato_main_input_deck_1.xml");
    tGold = std::string("<?xmlversion=\"1.0\"?><mesh><type>unstructured</type><format>exodus</format><ignore_node_map>true</ignore_node_map>") + 
                 std::string("<ignore_element_map>true</ignore_element_map><mesh>evaluations_1/rocker_1.exo</mesh>") + 
                 std::string("<block><index>1</index><material>1</material><integration><type>gauss</type><order>2</order></integration>") + 
                 std::string("</block></mesh><output><file>platomain</file><format>exodus</format></output>");

    ASSERT_STREQ(tGold.c_str(), tReadData.str().c_str());

    // Remove dummy files
    Plato::system("rm -rf evaluations_0");
    Plato::system("rm -rf evaluations_1");
    Plato::system("rm -rf rocker.exo");
    Plato::system("rm -rf rocker.csm");
}

TEST(PlatoTestXMLGenerator, CheckDakotaWorkflow_ErrorEmpty)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tMetaData.set(tOptimizationParameters);
    ASSERT_THROW(XMLGen::check_dakota_workflow(tMetaData), std::runtime_error);
}

TEST(PlatoTestXMLGenerator, CheckDakotaWorkflow_ErrorNotSupported)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tMetaData.set(tOptimizationParameters);
    tOptimizationParameters.append("dakota_workflow", "gradient_based");
    ASSERT_THROW(XMLGen::check_dakota_workflow(tMetaData), std::runtime_error);
}

TEST(PlatoTestXMLGenerator, CheckDakotaWorkflow_MDPS)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "mdps");
    tMetaData.set(tOptimizationParameters);
    ASSERT_NO_THROW(XMLGen::check_dakota_workflow(tMetaData));
}

TEST(PlatoTestXMLGenerator, CheckDakotaWorkflow_SBGO)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tMetaData.set(tOptimizationParameters);
    ASSERT_NO_THROW(XMLGen::check_dakota_workflow(tMetaData));
}

TEST(PlatoTestXMLGenerator, AppendTitleToDakotaDriverInputFile_MDPS)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "mdps");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendTitle.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_title(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendTitle.txt");
    auto tGold = std::string("#inputfilefordakotamulti-dimensionalparameterstudy(MDPS)workflow");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendTitle.txt");
}

TEST(PlatoTestXMLGenerator, AppendTitleToDakotaDriverInputFile_SBGO)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendTitle.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_title(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendTitle.txt");
    auto tGold = std::string("#inputfilefordakotasurrogate-basedglobaloptimization(SBGO)workflow");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendTitle.txt");
}

TEST(PlatoTestXMLGenerator, AppendEnvironmentToDakotaDriverInputFile_MDPS)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "mdps");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendEnvironment.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_environment_block(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendEnvironment.txt");
    auto tGold = std::string("environment") + 
        std::string("tabular_data") + 
        std::string("tabular_data_file='dakota_multi_dimensional_parameter_study.dat'");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendEnvironment.txt");
}

TEST(PlatoTestXMLGenerator, AppendEnvironmentToDakotaDriverInputFile_SBGO)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendEnvironment.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_environment_block(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendEnvironment.txt");
    auto tGold = std::string("environment") + 
        std::string("tabular_data") + 
        std::string("tabular_data_file='dakota_surrogate_based_global_optimization.dat'") + 
        std::string("top_method_pointer='SBGO'");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendEnvironment.txt");
}

TEST(PlatoTestXMLGenerator, AppendMethodsToDakotaDriverInputFile_MDPS)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "mdps");
    tOptimizationParameters.append("mdps_partitions", "6");
    tOptimizationParameters.append("num_shape_design_variables", "3");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendMethods.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_method_blocks(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendMethods.txt");
    auto tGold = std::string("method") + 
        std::string("multidim_parameter_study") + 
        std::string("partitions=666");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendMethods.txt");
}

TEST(PlatoTestXMLGenerator, AppendMethodsToDakotaDriverInputFile_SBGO)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tOptimizationParameters.append("sbgo_max_iterations", "50");
    tOptimizationParameters.append("moga_population_size", "1500");
    tOptimizationParameters.append("moga_max_function_evaluations", "1000");
    tOptimizationParameters.append("moga_niching_distance", "0.02");
    tOptimizationParameters.append("num_sampling_method_samples", "24");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendMethods.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_method_blocks(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendMethods.txt");
    auto tGold = std::string("method") + 
        std::string("id_method='SBGO'") + 
        std::string("surrogate_based_global") + 
        std::string("model_pointer='SURROGATE'") + 
        std::string("method_pointer='MOGA'") + 
        std::string("max_iterations=50") +
        std::string("replace_points") +
        std::string("outputquiet") +

        std::string("method") + 
        std::string("id_method='MOGA'") + 
        std::string("moga") + 
        std::string("seed=10983") + 
        std::string("population_size=1500") + 
        std::string("max_function_evaluations=1000") +
        std::string("initialization_typeunique_random") +
        std::string("crossover_typeshuffle_randomnum_offspring=2num_parents=2crossover_rate=0.8") +
        std::string("mutation_typereplace_uniformmutation_rate=0.1") + 
        std::string("fitness_typedomination_count") +
        std::string("replacement_typeelitist") +
        std::string("niching_typedistance0.02") +
        std::string("postprocessor_typeorthogonal_distance0.01") +
        std::string("convergence_typemetric_trackerpercent_change=0.01num_generations=10") +
        std::string("outputnormal") +
        std::string("scaling") + 

        std::string("method") + 
        std::string("id_method='SAMPLING'") + 
        std::string("sampling") + 
        std::string("samples=24") + 
        std::string("seed=777") + 
        std::string("sample_typelhs") + 
        std::string("model_pointer='TRUTH'");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendMethods.txt");
}

TEST(PlatoTestXMLGenerator, AppendModelsToDakotaDriverInputFile_SBGO)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendModels.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_model_blocks(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendModels.txt");
    auto tGold = std::string("model") + 
        std::string("id_model='SURROGATE'") +
        std::string("surrogateglobal") +
        std::string("dace_method_pointer='SAMPLING'") +
        std::string("gaussian_processsurfpack") +

        std::string("model") + 
        std::string("id_model='TRUTH'") +
        std::string("single") +
        std::string("interface_pointer='TRUE_FN'");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendModels.txt");
}

TEST(PlatoTestXMLGenerator, AppendModelsToDakotaDriverInputFile_SBGO_OutputSurrogate)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tOptimizationParameters.append("sbgo_surrogate_output_name", "my_model");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendModels.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_model_blocks(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendModels.txt");
    auto tGold = std::string("model") + 
        std::string("id_model='SURROGATE'") +
        std::string("surrogateglobal") +
        std::string("dace_method_pointer='SAMPLING'") +
        std::string("gaussian_processsurfpack") +
        std::string("export_model") +
        std::string("filename_prefix='my_model'") +
        std::string("formatsbinary_archive") +

        std::string("model") + 
        std::string("id_model='TRUTH'") +
        std::string("single") +
        std::string("interface_pointer='TRUE_FN'");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendModels.txt");
}

TEST(PlatoTestXMLGenerator, AppendParametersToDakotaDriverInputFile_ErrorNumberDesignVariablesDontMatchCsm)
{
    // Create test csm file
    FILE* fp1=fopen("rocker.csm", "w");
    fprintf(fp1, "%s", "# Constant, Design, and Output Parameters:\n");
    fprintf(fp1, "%s", "despmtr Py 2.0 lbound 2.0 ubound 3.0 initial 2.0\n");
    fprintf(fp1, "%s", "despmtr Px 2.0 lbound 1.6 ubound 2.4 initial 2.0\n");
    fprintf(fp1, "%s", "conpmtr Boffset 0.25 # lbound 0.05 ubound 0.38 initial 0.25\n");
    fprintf(fp1, "%s", "conpmtr Lx 6.5 # lbound 5.0 ubound 8.0 initial 6.5\n");
    fclose(fp1);

    // create metadata
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tOptimizationParameters.append("concurrent_evaluations", "6");
    tOptimizationParameters.append("csm_file", "rocker.csm");
    tOptimizationParameters.append("num_shape_design_variables", "3");
    tMetaData.set(tOptimizationParameters);

    FILE* fp2=fopen("appendParameters.txt", "w");
    ASSERT_THROW(XMLGen::append_dakota_driver_variables_block(tMetaData, fp2), std::runtime_error);
    fclose(fp2);

    Plato::system("rm -rf appendParameters.txt");
    Plato::system("rm -rf rocker.csm");
}

TEST(PlatoTestXMLGenerator, AppendParametersToDakotaDriverInputFile)
{
    // Create test csm file
    FILE* fp1=fopen("rocker.csm", "w");
    fprintf(fp1, "%s", "# Constant, Design, and Output Parameters:\n");
    fprintf(fp1, "%s", "despmtr Py 2.0 lbound 2.0 ubound 3.0 initial 2.0\n");
    fprintf(fp1, "%s", "despmtr Px 2.0 lbound 1.6 ubound 2.4 initial 2.0\n");
    fprintf(fp1, "%s", "conpmtr Boffset 0.25 # lbound 0.05 ubound 0.38 initial 0.25\n");
    fprintf(fp1, "%s", "conpmtr Lx 6.5 # lbound 5.0 ubound 8.0 initial 6.5\n");
    fclose(fp1);

    // create metadata
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tOptimizationParameters.append("concurrent_evaluations", "6");
    tOptimizationParameters.append("csm_file", "rocker.csm");
    tOptimizationParameters.append("num_shape_design_variables", "2");
    tMetaData.set(tOptimizationParameters);

    FILE* fp2=fopen("appendParameters.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_variables_block(tMetaData, fp2));
    fclose(fp2);

    auto tReadData = XMLGen::read_data_from_file("appendParameters.txt");
    auto tGold = std::string("variables") + 
        std::string("continuous_design=2") + 
        std::string("descriptors'Py''Px'") +
        std::string("lower_bounds2.0000001.600000") +
        std::string("upper_bounds3.0000002.400000") +
        std::string("initial_point2.0000002.000000");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());

    Plato::system("rm -rf appendParameters.txt");
    Plato::system("rm -rf rocker.csm");
}

TEST(PlatoTestXMLGenerator, AppendParametersToDakotaDriverInputFileFromDescriptors)
{
    // create metadata
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tOptimizationParameters.append("concurrent_evaluations", "6");
    tOptimizationParameters.descriptors({"sl" ,  "sw"});
    tOptimizationParameters.lower_bounds({"0",  "0.1"});
    tOptimizationParameters.upper_bounds({"1",  "1.1"});
    tOptimizationParameters.mdps_partitions({"2",  "3"});
    
    tMetaData.set(tOptimizationParameters);

    FILE* fp2=fopen("appendParameters.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_variables_block(tMetaData, fp2));
    fclose(fp2);

    auto tReadData = XMLGen::read_data_from_file("appendParameters.txt");
    auto tGold = std::string("variables") + 
        std::string("continuous_design=2") + 
        std::string("descriptors'sl''sw'") +
        std::string("lower_bounds00.1") +
        std::string("upper_bounds11.1");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());

    Plato::system("rm -rf appendParameters.txt");
}

TEST(PlatoTestXMLGenerator, AppendInterfaceToDakotaDriverInputFile_MDPS)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "mdps");
    tOptimizationParameters.append("concurrent_evaluations", "6");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendInterface.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_interface_block(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendInterface.txt");
    auto tGold = std::string("interface") + 
        std::string("analysis_drivers='plato_dakota_plugin'") + 
        std::string("direct") +
        std::string("asynchronousevaluation_concurrency6");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendInterface.txt");
}

TEST(PlatoTestXMLGenerator, AppendInterfaceToDakotaDriverInputFile_SBGO)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tOptimizationParameters.append("concurrent_evaluations", "6");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendInterface.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_interface_block(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendInterface.txt");
    auto tGold = std::string("interface") + 
        std::string("id_interface='TRUE_FN'") + 
        std::string("analysis_drivers='plato_dakota_plugin'") + 
        std::string("direct") +
        std::string("asynchronousevaluation_concurrency6");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendInterface.txt");
}

TEST(PlatoTestXMLGenerator, AppendResponsesToDakotaDriverInputFile_MDPS)
{
    XMLGen::InputData tMetaData;
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "mdps");
    tOptimizationParameters.append("mdps_response_functions", "3");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendResponses.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_responses_block(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendResponses.txt");
    auto tGold = std::string("responses") + 
        std::string("response_functions=3") + 
        std::string("no_gradients") +
        std::string("no_hessians");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendResponses.txt");
}

TEST(PlatoTestXMLGenerator, AppendResponsesToDakotaDriverInputFile_SBGO_MultiObjective)
{
    XMLGen::InputData tMetaData;

    // Create criteria
    XMLGen::Criterion tCriterion;
    tCriterion.id("3");
    tCriterion.type("mechanical_compliance");
    tMetaData.append(tCriterion);
    tCriterion.id("7");
    tCriterion.type("volume");
    tMetaData.append(tCriterion);

    // Create an objective
    XMLGen::Objective tObjective;
    tObjective.type = "multi_objective";
    tObjective.serviceIDs.push_back("2");
    tObjective.serviceIDs.push_back("2");
    tObjective.criteriaIDs.push_back("3");
    tObjective.criteriaIDs.push_back("7");
    tObjective.scenarioIDs.push_back("14");
    tObjective.scenarioIDs.push_back("14");
    tObjective.weights.push_back("1.3");
    tObjective.weights.push_back("5.4");
    tMetaData.objective = tObjective;

    // Create optimization parameters
    XMLGen::OptimizationParameters tOptimizationParameters;
    tOptimizationParameters.append("dakota_workflow", "sbgo");
    tMetaData.set(tOptimizationParameters);

    FILE* fp=fopen("appendResponses.txt", "w");
    ASSERT_NO_THROW(XMLGen::append_dakota_driver_responses_block(tMetaData, fp));
    fclose(fp);

    auto tReadData = XMLGen::read_data_from_file("appendResponses.txt");
    auto tGold = std::string("responses") + 
        std::string("objective_functions=2") + 
        std::string("descriptors'mechanical_compliance_scenario14''volume_scenario14'") + 
        std::string("primary_scales=1.35.4") + 
        std::string("no_gradients") +
        std::string("no_hessians");

    EXPECT_STREQ(tReadData.str().c_str(),tGold.c_str());
    Plato::system("rm -rf appendResponses.txt");
}

}
// PlatoTestXMLGenerator
