/*
 * XMLGeneratorAugLagCriterion_UnitTester.cpp
 *
 *  Created on: May 5, 2023
 */

#include <gtest/gtest.h>

#include "XMLGenerator_UnitTester.hpp"
#include "XMLGenerator_UnitTester_Tools.hpp"

#include "XMLGeneratorAnalyzeCriterionUtilities.hpp"
#include "XMLGeneratorPlatoAnalyzeInputFileUtilities.hpp"

namespace PlatoTestXMLGenerator
{

TEST(PlatoTestXMLGenerator, ParseStrengthConstraintMassMinOptions)
{
    // set inputs
    std::istringstream tIss;
    std::string tStringInput = 
    "begin objective\n"
      "  type weighted_sum\n"
      "  scenarios 1 1\n"
      "  criteria  1 2\n"
      "  services  1 1\n"
      "  weights   1 1\n"
      "end objective\n"
      "begin criterion 1\n"
      "  type mass\n"
      "end criterion\n"
      "begin criterion 2\n"
      "  type strength_constraint\n"
      "  limits 5.0\n"
      "  local_measures vonmises\n"
      "end criterion\n";

    // parse inputs
    XMLGenerator_UnitTester tTester;
    tIss.str(tStringInput);
    tIss.clear();
    tIss.seekg(0);
    ASSERT_NO_THROW(tTester.publicParseObjective(tIss));
    tIss.clear();
    tIss.seekg(0);
    ASSERT_NO_THROW(tTester.publicParseCriteria(tIss));
    tIss.clear();

    // test data
    auto tInputData = tTester.getInputData();
    // test objective block
    ASSERT_EQ(2u,tInputData.objective.serviceIDs.size());
    ASSERT_STREQ("1",tInputData.objective.serviceIDs.back().c_str()); 
    ASSERT_STREQ("1",tInputData.objective.serviceIDs.front().c_str());
    ASSERT_STREQ("2",tInputData.objective.criteriaIDs.back().c_str()); 
    ASSERT_STREQ("1",tInputData.objective.criteriaIDs.front().c_str());
    ASSERT_STREQ("1",tInputData.objective.weights.back().c_str()); 
    ASSERT_STREQ("1",tInputData.objective.weights.front().c_str());
    ASSERT_STREQ("1",tInputData.objective.scenarioIDs.back().c_str()); 
    ASSERT_STREQ("1",tInputData.objective.scenarioIDs.front().c_str());
    // test criterion 1 block
    ASSERT_STREQ("1",tInputData.criterion("1").id().c_str());
    ASSERT_STREQ("mass",tInputData.criterion("1").type().c_str());
    // test criterion 2 block
    ASSERT_STREQ("2",tInputData.criterion("2").id().c_str());
    ASSERT_STREQ("strength_constraint",tInputData.criterion("2").type().c_str());
    ASSERT_EQ(1u,tInputData.criterion("2").values("limits").size());
    ASSERT_STREQ("5.0",tInputData.criterion("2").values("limits").back().c_str());
    ASSERT_EQ(1u,tInputData.criterion("2").values("local_measures").size());
    ASSERT_STREQ("vonmises",tInputData.criterion("2").values("local_measures").back().c_str());
}

TEST(PlatoTestXMLGenerator, AppendObjectiveCriteriaToPlatoProblem_StrengthConstraintMassMinimization)
{
    XMLGen::Criterion tCriterionOne;
    tCriterionOne.id("1");
    tCriterionOne.type("strength_constraint");
    tCriterionOne.append("limits", "2.0");
    tCriterionOne.append("max_penalty","100");
    tCriterionOne.append("initial_penalty","1.0");
    tCriterionOne.append("penalty_increment","1.1");
    tCriterionOne.append("local_measures","vonmises");
    tCriterionOne.append("penalty_update_constant","0.2");
    tCriterionOne.append("initial_lagrange_multiplier","0.0");
    
    XMLGen::InputData tXMLMetaData;
    tXMLMetaData.append(tCriterionOne);

    XMLGen::Criterion tCriterionTwo;
    tCriterionTwo.id("2");
    tCriterionTwo.type("mass");
    tXMLMetaData.append(tCriterionTwo);
    
    XMLGen::Service tService;
    tService.id("1");
    tService.code("plato_analyze");
    tXMLMetaData.append(tService);

    XMLGen::Objective tObjective;
    tObjective.type = "weigthed_sum";
    tObjective.criteriaIDs.push_back("1");
    tObjective.criteriaIDs.push_back("2");
    tObjective.serviceIDs.push_back("1");
    tObjective.serviceIDs.push_back("1");
    tXMLMetaData.objective = tObjective;

    pugi::xml_document tDocument;
    XMLGen::append_objective_criteria_to_plato_problem(tXMLMetaData, tDocument);

    // test strength criterion
    auto tParamList = tDocument.child("ParameterList");
    ASSERT_FALSE(tParamList.empty());
    ASSERT_STREQ("ParameterList", tParamList.name());
    PlatoTestXMLGenerator::test_attributes({"name"}, {"my_strength_constraint_criterion_id_1"}, tParamList);
    //tDocument.save_file("dummy.xml");

    std::vector<std::string> tGoldKeys = {"name", "type", "value"};
    std::vector<std::vector<std::string>> tGoldValues =
        {
          {"Type", "string", "Scalar Function"},
          {"Scalar Function Type", "string", "Strength Constraint"},
          {"Limits", "Array(double)", "{2.0}"},
          {"Local Measures", "Array(string)", "{vonmises}"},
          {"Maximum Penalty", "double", "100"},
          {"Initial Penalty", "double", "1.0"},
          {"Penalty Increment", "double", "1.1"},
          {"Penalty Update Constant", "double", "0.2"},
          {"Initial Lagrange Multiplier", "double", "0.0"},
        };

    auto tParam = tParamList.child("Parameter");
    auto tValuesItr = tGoldValues.begin();
    while(!tParam.empty())
    {
        ASSERT_FALSE(tParam.empty());
        ASSERT_STREQ("Parameter", tParam.name());
        PlatoTestXMLGenerator::test_attributes(tGoldKeys, tValuesItr.operator*(), tParam);
        tParam = tParam.next_sibling();
        std::advance(tValuesItr, 1);
    }

    // test mass criterion
    tParamList = tParamList.next_sibling("ParameterList");
    ASSERT_FALSE(tParamList.empty());
    ASSERT_STREQ("ParameterList", tParamList.name());
    PlatoTestXMLGenerator::test_attributes({"name"}, {"my_mass_criterion_id_2"}, tParamList);

    tGoldValues =
        {
          {"Type", "string", "Scalar Function"},
          {"Scalar Function Type", "string", "Mass"},
          {"Normalize Criterion", "bool", "true"}
        };

    tParam = tParamList.child("Parameter");
    tValuesItr = tGoldValues.begin();
    while(!tParam.empty())
    {
        ASSERT_FALSE(tParam.empty());
        ASSERT_STREQ("Parameter", tParam.name());
        PlatoTestXMLGenerator::test_attributes(tGoldKeys, tValuesItr.operator*(), tParam);
        tParam = tParam.next_sibling();
        std::advance(tValuesItr, 1);
    }
}

}
// namespace PlatoTestXMLGenerator