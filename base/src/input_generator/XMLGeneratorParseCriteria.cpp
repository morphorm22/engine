/*
 * XMLGeneratorParseCriteria.cpp
 *
 *  Created on: Jun 23, 2020
 */

#include <algorithm>

#include "XMLGeneratorCriterionMetadata.hpp"
#include "XMLGeneratorParseCriteria.hpp"
#include "XMLGeneratorValidInputKeys.hpp"

namespace XMLGen
{

void ParseCriteria::setTags(XMLGen::Criterion& aCriterion)
{
    XMLGen::ValidCriterionParameterKeys tValidKeys;
    for(auto& tTag : mTags)
    {
        auto tLowerKey = XMLGen::to_lower(tTag.first);
        auto tItr = std::find(tValidKeys.mKeys.begin(), tValidKeys.mKeys.end(), tLowerKey);
        if(tItr == tValidKeys.mKeys.end())
        {
            THROWERR(std::string("Check Keyword: keyword '") + tTag.first + std::string("' is not supported"))
        }

        if(tTag.second.first.second.empty())
        {
            auto tDefaultValue = tTag.second.second;
            aCriterion.append(tTag.first, tDefaultValue);
        }
        else
        {
            auto tInputValue = tTag.second.first.second;
            aCriterion.append(tTag.first, tInputValue);
        }
    }
}

std::string check_criterion_type_key
(const std::string& aKeyword)
{
    XMLGen::ValidCriterionTypeKeys tValidKeys;
    auto tValue = tValidKeys.value(aKeyword);
    if(tValue.empty())
    {
        auto tMsg = std::string("Input criterion keyword '") + aKeyword + "' is not defined in the list of supported Plato criterion. Supported Plato criterion options are: ";
        const auto& tKeys = tValidKeys.list();
        for(const auto& tKey : tKeys)
        {
            size_t tIndex = &tKey - &tKeys[0];
            auto tDelimiter = (tIndex + 1u) == tKeys.size() ? ".\n" : ", ";
            tMsg = tMsg + tKey + tDelimiter;
        }
        THROWERR(tMsg)
    }
    return tValue;
}

void ParseCriteria::allocate()
{
    mTags.clear();

    // core properties
    mTags.insert({ "type", { { {"type"}, ""}, "" } });
    mTags.insert({ "normalize", { { {"normalize"}, ""}, "false" } });
    mTags.insert({ "normalization_value", { { {"normalization_value"}, ""}, "1.0" } });
    mTags.insert({ "stress_limit", { { {"stress_limit"}, ""}, "" } });
    mTags.insert({ "scmm_initial_penalty", { { {"scmm_initial_penalty"}, ""}, "" } });
    mTags.insert({ "scmm_penalty_expansion_multiplier", { { {"scmm_penalty_expansion_multiplier"}, ""}, "" } });
    mTags.insert({ "scmm_constraint_exponent", { { {"scmm_constraint_exponent"}, ""}, "" } });
    mTags.insert({ "scmm_penalty_upper_bound", { { {"scmm_penalty_upper_bound"}, ""}, "" } });
    mTags.insert({ "scmm_stress_weight", { { {"scmm_stress_weight"}, ""}, "1.0" } });
    mTags.insert({ "scmm_mass_weight", { { {"scmm_mass_weight"}, ""}, "1.0" } });

    mTags.insert({ "stress_p_norm_exponent", { { {"stress_p_norm_exponent"}, ""}, "6.0" } });
    mTags.insert({ "mechanical_weighting_factor", { { {"mechanical_weighting_factor"}, ""}, "1.0" } });
    mTags.insert({ "thermal_weighting_factor", { { {"thermal_weighting_factor"}, ""}, "1.0" } });
    mTags.insert({ "local_measure", { { {"local_measure"}, ""}, "vonmises" } });
    mTags.insert({ "spatial_weighting_function", { { {"spatial_weighting_function"}, ""}, "1.0" } }); // function of x, y, and z
    mTags.insert({ "material_penalty_model", { { {"material_penalty_model"}, ""}, "simp" } });
    mTags.insert({ "material_penalty_exponent", { { {"material_penalty_exponent"}, ""}, "3.0" } });
    mTags.insert({ "minimum_ersatz_material_value", { { {"minimum_ersatz_material_value"}, ""}, "" } });
    mTags.insert({ "criterion_ids", { { {"criterion_ids"}, ""}, "" } });
    mTags.insert({ "criterion_weights", { { {"criterion_weights"}, ""}, "" } });
    mTags.insert({ "location_names", { { {"location_names"}, ""}, "" } });
    mTags.insert({ "blocks", { { {"blocks"}, ""}, "" } });
    mTags.insert({ "conductivity_ratios", { { {"conductivity_ratios"}, ""}, "" } });

    /* These are all related to stress-constrained mass minimization problems with Sierra/SD */
    mTags.insert({ "volume_misfit_target", { { {"volume_misfit_target"}, ""}, "" } });
    mTags.insert({ "relative_stress_limit", { { {"relative_stress_limit"}, ""}, "" } });
    mTags.insert({ "relaxed_stress_ramp_factor", { { {"relaxed_stress_ramp_factor"}, ""}, "" } });
    mTags.insert({ "limit_power_min", { { {"limit_power_min"}, ""}, "" } });
    mTags.insert({ "limit_power_max", { { {"limit_power_max"}, ""}, "" } });
    mTags.insert({ "limit_power_feasible_bias", { { {"limit_power_feasible_bias"}, ""}, "" } });
    mTags.insert({ "limit_power_feasible_slope", { { {"limit_power_feasible_slope"}, ""}, "" } });
    mTags.insert({ "limit_power_infeasible_bias", { { {"limit_power_infeasible_bias"}, ""}, "" } });
    mTags.insert({ "limit_power_infeasible_slope", { { {"limit_power_infeasible_slope"}, ""}, "" } });
    mTags.insert({ "limit_reset_subfrequency", { { {"limit_reset_subfrequency"}, ""}, "" } });
    mTags.insert({ "limit_reset_count", { { {"limit_reset_count"}, ""}, "" } });
    mTags.insert({ "inequality_allowable_feasibility_upper", { { {"inequality_allowable_feasibility_upper"}, ""}, "" } });
    mTags.insert({ "inequality_feasibility_scale", { { {"inequality_feasibility_scale"}, ""}, "" } });
    mTags.insert({ "inequality_infeasibility_scale", { { {"inequality_infeasibility_scale"}, ""}, "" } });
    mTags.insert({ "stress_inequality_power", { { {"stress_inequality_power"}, ""}, "" } });
    mTags.insert({ "stress_favor_final", { { {"stress_favor_final"}, ""}, "" } });
    mTags.insert({ "stress_favor_updates", { { {"stress_favor_updates"}, ""}, "" } });
    mTags.insert({ "volume_penalty_power", { { {"volume_penalty_power"}, ""}, "" } });
    mTags.insert({ "volume_penalty_divisor", { { {"volume_penalty_divisor"}, ""}, "" } });
    mTags.insert({ "volume_penalty_bias", { { {"volume_penalty_bias"}, ""}, "" } });
    mTags.insert({ "surface_area_sideset_id", { { {"surface_area_sideset_id"}, ""}, "" } });

    // Sierra/SD modal objectives
    mTags.insert({ "num_modes_compute", { { {"num_modes_compute"}, ""}, "30" } });
    mTags.insert({ "modes_to_exclude", { { {"modes_to_exclude"}, ""}, "" } });
    mTags.insert({ "eigen_solver_shift", { { {"eigen_solver_shift"}, ""}, "-1e6" } });
    mTags.insert({ "camp_solver_tol", { { {"camp_solver_tol"}, ""}, "1e-6" } });
    mTags.insert({ "camp_max_iter", { { {"camp_max_iter"}, ""}, "1000" } });
    mTags.insert({ "shape_sideset", { { {"shape_sideset"}, ""}, "" } });
    mTags.insert({ "ref_data_file", { { {"ref_data_file"}, ""}, "" } });
    mTags.insert({ "match_nodesets", { { {"match_nodesets"}, ""}, "" } });
}

void ParseCriteria::setModesToExclude(XMLGen::Criterion &aMetadata)
{
    auto tItr = mTags.find("modes_to_exclude");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty()) {
        std::vector<std::string> tModes;
        char tValuesBuffer[10000];
        strcpy(tValuesBuffer, tValues.c_str());
        XMLGen::parse_tokens(tValuesBuffer, tModes);
        aMetadata.modesToExclude(tModes);
    }
}

void ParseCriteria::setMatchNodesetIDs(XMLGen::Criterion &aMetadata)
{
    auto tItr = mTags.find("match_nodesets");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        std::vector<std::string> tNodesetIDs;
        char tValuesBuffer[10000];
        strcpy(tValuesBuffer, tValues.c_str());
        XMLGen::parse_tokens(tValuesBuffer, tNodesetIDs);
        aMetadata.setMatchNodesetIDs(tNodesetIDs);
    }
}


void ParseCriteria::setCriterionWeights(XMLGen::Criterion &aMetadata)
{
    if(aMetadata.type() == "composite")
    {
        auto tItr = mTags.find("criterion_weights");
        std::string tValues = tItr->second.first.second;
        if (tItr != mTags.end() && !tValues.empty())
        {
            std::vector<std::string> tWeights;
            char tValuesBuffer[10000];
            strcpy(tValuesBuffer, tValues.c_str());
            XMLGen::parse_tokens(tValuesBuffer, tWeights);
            aMetadata.criterionWeights(tWeights);
        }
        else
        {
            THROWERR("Criterion Weights ('criterion_weights' keyword) are not defined for composite criterion with criterion block id '" + aMetadata.id() + "'.");
        }
    }
}

void ParseCriteria::setCriterionIDs(XMLGen::Criterion &aMetadata)
{
    if(aMetadata.type() == "composite")
    {
        auto tItr = mTags.find("criterion_ids");
        std::string tValues = tItr->second.first.second;
        if (tItr != mTags.end() && !tValues.empty())
        {
            std::vector<std::string> tIDs;
            char tValuesBuffer[10000];
            strcpy(tValuesBuffer, tValues.c_str());
            XMLGen::parse_tokens(tValuesBuffer, tIDs);
            aMetadata.criterionIDs(tIDs);
        }
        else
        {
            THROWERR("Criterion IDs ('criterion_ids' keyword) are not defined for composite criterion with criterion block id '" + aMetadata.id() + "'.");
        }
    }
}

void ParseCriteria::setCriterionType(XMLGen::Criterion& aMetadata)
{
    auto tItr = mTags.find("type");
    if (tItr->second.first.second.empty())
    {
        THROWERR("Parse Criteria: Criterion type is not defined.")
    }
    else
    {
        auto tValidCriterionType = XMLGen::check_criterion_type_key(tItr->second.first.second);
        aMetadata.type(tValidCriterionType);
    }
}

void ParseCriteria::setMetadata(XMLGen::Criterion& aMetadata)
{
    this->setCriterionType(aMetadata);
    this->setCriterionIDs(aMetadata);
    this->setCriterionWeights(aMetadata);
    setModesToExclude(aMetadata);
    setMatchNodesetIDs(aMetadata);
    this->setTags(aMetadata);
}

void ParseCriteria::checkUniqueIDs()
{
    std::vector<std::string> tIDs;
    for(auto& tCriterion : mData)
    {
        tIDs.push_back(tCriterion.id());
    }

    if(!XMLGen::unique(tIDs))
    {
        THROWERR("Parse Criteria: Criterion block identification numbers, i.e. IDs, are not unique.  Criterion block IDs must be unique.")
    }
}

std::vector<XMLGen::Criterion> ParseCriteria::data() const
{
    return mData;
}

void ParseCriteria::parse(std::istream &aInputFile)
{
    mData.clear();
    this->allocate();
    constexpr int MAX_CHARS_PER_LINE = 10000;
    std::vector<char> tBuffer(MAX_CHARS_PER_LINE);
    while (!aInputFile.eof())
    {
        // read an entire line into memory
        std::vector<std::string> tTokens;
        aInputFile.getline(tBuffer.data(), MAX_CHARS_PER_LINE);
        XMLGen::parse_tokens(tBuffer.data(), tTokens);
        XMLGen::to_lower(tTokens);

        std::string tCriterionBlockID;
        if (XMLGen::parse_single_value(tTokens, { "begin", "criterion" }, tCriterionBlockID))
        {
            XMLGen::Criterion tMetadata;
            if(tCriterionBlockID.empty())
            {
                THROWERR(std::string("Parse Criteria: criterion identification number is empty. ")
                    + "A unique criterion identification number must be assigned to each criterion block.")
            }
            tMetadata.id(tCriterionBlockID);
            XMLGen::erase_tag_values(mTags);
            XMLGen::parse_input_metadata( { "end", "criterion" }, aInputFile, mTags);
            this->setMetadata(tMetadata);
            mData.push_back(tMetadata);
        }
    }
    this->checkUniqueIDs();
}

}
// namespace XMLGen
