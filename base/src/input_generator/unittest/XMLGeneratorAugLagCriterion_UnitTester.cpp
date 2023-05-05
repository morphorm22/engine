/*
 * XMLGeneratorAugLagCriterion_UnitTester.cpp
 *
 *  Created on: May 5, 2023
 */

#include <gtest/gtest.h>

#include "XMLGenerator_UnitTester.hpp"

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

}
// namespace PlatoTestXMLGenerator