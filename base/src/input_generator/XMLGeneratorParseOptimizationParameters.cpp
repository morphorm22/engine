/*
 * XMLGeneratorParseOptimizationParameters.cpp
 *
 *  Created on: Jan 11, 2021
 */

#include <algorithm>

#include "XMLGeneratorValidInputKeys.hpp"
#include "XMLGeneratorParserUtilities.hpp"
#include "XMLGeneratorFixedBlockUtilities.hpp"
#include "XMLGeneratorParseOptimizationParameters.hpp"
#include "XMLGeneratorOptimizationParametersMetadata.hpp"
#include "XMLGeneratorParseMethodInputOptionsUtilities.hpp"

namespace XMLGen
{

void ParseOptimizationParameters::setTags(XMLGen::OptimizationParameters &aOptimizationParameters)
{
    XMLGen::ValidOptimizationParameterKeys tValidKeys;
    for(auto& tTag : mTags)
    {
        if(tValidKeys.mKeys.find(tTag.first) == tValidKeys.mKeys.end())
        {
            THROWERR("Parse Optimization Parameters: Invalid keyword '" + tTag.first + "'\n");
        }

        if(tTag.second.first.second.empty())
        {
            auto tDefaultValue = tTag.second.second;
            aOptimizationParameters.append(tTag.first, tDefaultValue, true);
        }
        else
        {
            auto tInputValue = tTag.second.first.second;
            aOptimizationParameters.append(tTag.first, tInputValue, false);
        }
    }
}

void ParseOptimizationParameters::allocate()
{
    mTags.clear();
    XMLGen::insert_amgx_input_options(mTags);
    XMLGen::insert_restart_input_options(mTags);
    XMLGen::insert_plato_mma_input_options(mTags);
    XMLGen::insert_dakota_moga_input_options(mTags);
    XMLGen::insert_plato_filter_input_options(mTags);
    XMLGen::insert_fixed_blocks_input_options(mTags);
    XMLGen::insert_general_dakota_input_options(mTags);
    XMLGen::insert_plato_levelset_input_options(mTags);
    XMLGen::insert_prune_and_refine_input_options(mTags);
    XMLGen::insert_shape_optimization_input_options(mTags);
    XMLGen::insert_derivative_checker_input_options(mTags);
    XMLGen::insert_optimality_criteria_input_options(mTags);
    XMLGen::insert_general_optimization_input_options(mTags);
    XMLGen::insert_topology_optimization_input_options(mTags);
    XMLGen::insert_plato_symmetry_filter_input_options(mTags);
    XMLGen::insert_plato_projection_filter_input_options(mTags);
    XMLGen::insert_dakota_multidim_param_study_input_options(mTags);
    XMLGen::insert_plato_kelley_sachs_trust_region_input_options(mTags);
}

std::vector<XMLGen::OptimizationParameters> ParseOptimizationParameters::data() const 
{
    return mData;
}

void ParseOptimizationParameters::setMeshMapData(XMLGen::OptimizationParameters &aMetadata)
{
    auto tItr = mTags.find("symmetry_plane_origin");
    if(tItr != mTags.end())
    {
        std::string tValues = tItr->second.first.second;
        if (!tValues.empty())
        {
            std::vector<std::string> tOrigin;
            char tValuesBuffer[10000];
            strcpy(tValuesBuffer, tValues.c_str());
            XMLGen::parse_tokens(tValuesBuffer, tOrigin);
            aMetadata.symmetryOrigin(tOrigin);
        }
    }
    tItr = mTags.find("symmetry_plane_normal");
    if(tItr != mTags.end())
    {
        std::string tValues = tItr->second.first.second;
        if (!tValues.empty())
        {
            std::vector<std::string> tNormal;
            char tValuesBuffer[10000];
            strcpy(tValuesBuffer, tValues.c_str());
            XMLGen::parse_tokens(tValuesBuffer, tNormal);
            aMetadata.symmetryNormal(tNormal);
        }
    }
}

void ParseOptimizationParameters::parse(std::istream &aInputFile)
{
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

        std::string tDummy;
        if (XMLGen::parse_single_value(tTokens, { "begin", "optimization_parameters" }, tDummy))
        {
            XMLGen::OptimizationParameters tOptimizationParameters;
            XMLGen::is_metadata_block_id_valid(tTokens);
            XMLGen::erase_tag_values(mTags);
            XMLGen::parse_input_metadata( { "end", "optimization_parameters" }, aInputFile, mTags);
            this->setTags(tOptimizationParameters);
            this->setMetaData(tOptimizationParameters);
            mData.push_back(tOptimizationParameters);
        }
    }
}

void ParseOptimizationParameters::autoFillRestartParameters(XMLGen::OptimizationParameters &aMetadata)
{
    // First set a flag that can be used by other code.
    if(aMetadata.restart_iteration() != "0")
    {
        aMetadata.isARestartRun(true);
    }

    // If this is a restart run but no filename specified then we will
    // assume we are coming from the gui and we will use the default "restart_XXX.exo"
    // filename, iteration 1 from the file, and field name "optimizationdofs".
    if(aMetadata.isARestartRun())
    {
        if(aMetadata.initial_guess_file_name() == "" &&
           aMetadata.initial_guess_field_name() == "")
        {
            // This block indicates that we are coming from the gui so only the
            // restart iteration was specified.  We will fill in the other values
            // based on what we know the gui will be providing for the run.
            aMetadata.append("initial_guess_file_name",  "restart_" + aMetadata.restart_iteration() + ".exo", false);
            aMetadata.append("restart_iteration", "1", false);
            aMetadata.append("initial_guess_field_name", "control", false);
        }
        else
        {
            // This block indicates that the user is manually setting up the
            // restart file and so we depend on him having specified a filename
            // and field name.  If either of these is empty we need to error out.
            if(aMetadata.initial_guess_field_name() == "" ||
               aMetadata.initial_guess_file_name() == "")
            {
                THROWERR("ERROR:XMLGenerator:parseOptimizationParameters: You must specify a valid initial guess mesh filename and a valid field name on that mesh from which initial values will be obtained.\n");
            }
        }
    }
}

void ParseOptimizationParameters::setMetaData(XMLGen::OptimizationParameters &aMetadata)
{
    this->setOptimizationType(aMetadata);
    this->setFilterInEngine(aMetadata);
    this->setNormalizeInAggregator(aMetadata);
    
    this->setFixedBlockIDs(aMetadata);
    this->setFixedSidesetIDs(aMetadata);
    this->setFixedNodesetIDs(aMetadata);
    this->setFixedBlockDomainValues(aMetadata);
    this->setFixedBlockBoundaryValues(aMetadata);
    this->setFixedBlockMaterialStates(aMetadata);
    XMLGen::FixedBlock::check_fixed_block_metadata(aMetadata);

    this->setEnforceBounds(aMetadata); // this needs to be called after the fixed blocks processing to get correct defaults.
    this->setLevelsetNodesetIDs(aMetadata);
    this->setMaterialBoxExtents(aMetadata);
    this->setDakotaDescriptorsAndBounds(aMetadata);
    this->checkHeavisideFilterParams(aMetadata);
    this->setMeshMapData(aMetadata);
    this->setCSMParameters(aMetadata);
    this->autoFillRestartParameters(aMetadata);
    this->setSymmetryPlaneLocationNames(aMetadata);
    this->setMMAStagnationDefaultsForShapeOptimizationProblems(aMetadata);
}

void ParseOptimizationParameters::setNormalizeInAggregator(XMLGen::OptimizationParameters &aMetadata)
{
    bool tValue = false;

    // User-set flag trumps everything else
    std::string tNormalizeValue = aMetadata.normalize_in_aggregator();
    if(tNormalizeValue != "")
    {
        tValue = (XMLGen::to_lower(tNormalizeValue) == "true");
    }
    else
    {
        std::cout << "INFO: Auto-determining whether to normalize in aggregator because normalize_in_aggregator parameter was not set." << std::endl;
        std::string tOptAlg = aMetadata.optimization_algorithm();
        if(tOptAlg == "ksal" ||
           tOptAlg == "ksbc" ||
           tOptAlg == "oc")
        {
            tValue = true;
            std::cout << "INFO: Auto-determined to normalize in aggregator." << std::endl;
        }   
        else
        { 
            std::cout << "INFO: Auto-determined not to normalize in aggregator." << std::endl;
        }
    }
    aMetadata.normalizeInAggregator(tValue);
}

void ParseOptimizationParameters::setEnforceBounds(XMLGen::OptimizationParameters &aMetadata)
{
    bool tUserSpecifiedEnforceBoundsOption = false;
    auto tItr = mTags.find("enforce_bounds");
    if(tItr != mTags.end())
    {
        std::string tValues = tItr->second.first.second;
        if (!tValues.empty())
        {
            tUserSpecifiedEnforceBoundsOption = true;
        }
    }
    bool tFixedBlockIDsExist = (aMetadata.fixed_block_ids().size() > 0);

    // Turn enforce bounds on by default if there are fixed blocks
    if(tFixedBlockIDsExist && !tUserSpecifiedEnforceBoundsOption)
    {
        aMetadata.enforceBounds(true);
    }
    else
    {
        std::string tValue = aMetadata.enforce_bounds();
        if(tValue == "true")
        {
            aMetadata.enforceBounds(true);
        }
        else if(tValue == "false")
        {
            aMetadata.enforceBounds(false);
        }
        else
        {
            THROWERR(std::string("Parse Optimization Parameters: Unrecognized enforce_bounds value: ") + tValue);
        }
    }
}

void ParseOptimizationParameters::setFilterInEngine(XMLGen::OptimizationParameters &aMetadata)
{
    std::string tValue = aMetadata.filter_in_engine();
    if(tValue == "true")
    {
        if(aMetadata.filter_type() == "helmholtz")
        {
            aMetadata.filterInEngine(false);
        }
        else
        {
            aMetadata.filterInEngine(true);
        }
    }
    else if(tValue == "false")
    {
        aMetadata.filterInEngine(false);
    }
    else
    {
        THROWERR(std::string("Parse Optimization Parameters: Unrecognized filter_in_engine value: ") + tValue);
    }
}

void ParseOptimizationParameters::setOptimizationType(XMLGen::OptimizationParameters &aMetadata)
{
    std::string tType = aMetadata.optimization_type();
    if(tType == "shape")
    {
        aMetadata.optimizationType(OT_SHAPE);
    }
    else if(tType == "topology")
    {
        aMetadata.optimizationType(OT_TOPOLOGY);
    }
    else if(tType == "dakota")
    {
        aMetadata.optimizationType(OT_DAKOTA);
    }
    else
    {
        THROWERR(std::string("Parse Optimization Parameters: Unrecognized optimization type: ") + tType);
    }
}

void ParseOptimizationParameters::checkHeavisideFilterParams(XMLGen::OptimizationParameters &aMetadata)
{
    std::string tFilterType = aMetadata.filter_type();
    if(tFilterType == "kernel_then_heaviside" ||
       tFilterType == "kernel_then_tanh")
    {
        auto tMinString = aMetadata.filter_heaviside_min();
        auto tMaxString = aMetadata.filter_heaviside_max();
        auto tScaleString = aMetadata.filter_heaviside_scale();
        if(!tScaleString.empty() && (!tMinString.empty() || !tMaxString.empty()))
        {
            THROWERR("Parse Optimization Parameters: You can't specify filter heaviside scale along with filter heaviside min or max.");
        }
        if(!tScaleString.empty() && tMinString.empty() && tMaxString.empty())
        {
            aMetadata.append("filter_heaviside_min", tScaleString, true);
            aMetadata.append("filter_heaviside_max", tScaleString, true);
        }
    }
}

void ParseOptimizationParameters::setMaterialBoxExtents(XMLGen::OptimizationParameters &aMetadata)
{
    auto tMinItr = mTags.find("levelset_material_box_min");
    auto tMaxItr = mTags.find("levelset_material_box_max");
    if(tMinItr != mTags.end() && tMaxItr != mTags.end())
    {
        auto tMinValString = tMinItr->second.first.second.empty() ? tMinItr->second.second : tMinItr->second.first.second;
        auto tMaxValString = tMaxItr->second.first.second.empty() ? tMaxItr->second.second : tMaxItr->second.first.second;
        if(tMinValString.empty() && tMaxValString.empty()) 
        {
            return;
        }
        if(tMinValString.empty() || tMaxValString.empty())
        {
            THROWERR("Parse Optimization Parameters: Levelset material box min or max is not defined.");
        }
        std::vector<std::string> tMinValues;
        std::vector<std::string> tMaxValues;
        char tValuesBuffer[10000];
        strcpy(tValuesBuffer, tMinValString.c_str());
        XMLGen::parse_tokens(tValuesBuffer, tMinValues);
        strcpy(tValuesBuffer, tMaxValString.c_str());
        XMLGen::parse_tokens(tValuesBuffer, tMaxValues);
        if(tMinValues.size() != 3 || tMaxValues.size() != 3)
        {
            THROWERR("Parse Optimization Parameters: Levelset material box min or max is not fully defined.");
        }
    }
}

void ParseOptimizationParameters::setDakotaDescriptorsAndBounds(XMLGen::OptimizationParameters& aMetadata)
{
    auto tItr = mTags.find("descriptors");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        std::vector<std::string> tList;
        char tValuesBuffer[10000];
        strcpy(tValuesBuffer, tValues.c_str());
        XMLGen::parse_tokens(tValuesBuffer, tList);
        aMetadata.descriptors(tList);
    }

    tItr = mTags.find("lower_bounds");
    tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        std::vector<std::string> tList;
        char tValuesBuffer[10000];
        strcpy(tValuesBuffer, tValues.c_str());
        XMLGen::parse_tokens(tValuesBuffer, tList);
        aMetadata.lower_bounds(tList);
    }

    tItr = mTags.find("upper_bounds");
    tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        std::vector<std::string> tList;
        char tValuesBuffer[10000];
        strcpy(tValuesBuffer, tValues.c_str());
        XMLGen::parse_tokens(tValuesBuffer, tList);
        aMetadata.upper_bounds(tList);
    }

    tItr = mTags.find("mdps_partitions");
    tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        std::vector<std::string> tList;
        char tValuesBuffer[10000];
        strcpy(tValuesBuffer, tValues.c_str());
        XMLGen::parse_tokens(tValuesBuffer, tList);
        aMetadata.mdps_partitions(tList);
    }

}

void ParseOptimizationParameters::setFixedNodesetIDs(XMLGen::OptimizationParameters &aMetadata)
{
    auto tItr = mTags.find("fixed_nodeset_ids");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        XMLGen::FixedBlock::set_fixed_nodeset_ids(tValues, aMetadata);
    }
}

void ParseOptimizationParameters::setFixedSidesetIDs(XMLGen::OptimizationParameters &aMetadata)
{
    auto tItr = mTags.find("fixed_sideset_ids");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        XMLGen::FixedBlock::set_fixed_sideset_ids(tValues, aMetadata);
    }
}

void ParseOptimizationParameters::setFixedBlockIDs(XMLGen::OptimizationParameters &aMetadata)
{
    auto tItr = mTags.find("fixed_block_ids");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        XMLGen::FixedBlock::set_fixed_block_ids(tValues, aMetadata);
    }
}

void ParseOptimizationParameters::setFixedBlockDomainValues(XMLGen::OptimizationParameters &aMetadata)
{
    auto tItr = mTags.find("fixed_block_domain_values");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        XMLGen::FixedBlock::set_fixed_block_domain_values(tValues, aMetadata);
    }
}

void ParseOptimizationParameters::setFixedBlockBoundaryValues(XMLGen::OptimizationParameters &aMetadata)
{
    auto tItr = mTags.find("fixed_block_boundary_values");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        XMLGen::FixedBlock::set_fixed_block_boundary_values(tValues, aMetadata);
    }
}

void ParseOptimizationParameters::setFixedBlockMaterialStates(XMLGen::OptimizationParameters &aMetadata)
{
    auto tItr = mTags.find("fixed_block_material_states");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        XMLGen::FixedBlock::set_fixed_block_material_states(tValues, aMetadata);
    }
}

void ParseOptimizationParameters::setSymmetryPlaneLocationNames(XMLGen::OptimizationParameters &aMetadata)
{
    auto tItr = mTags.find("symmetry_plane_location_names");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        std::vector<std::string> tNames;
        char tValuesBuffer[10000];
        strcpy(tValuesBuffer, tValues.c_str());
        XMLGen::parse_tokens(tValuesBuffer, tNames);
        aMetadata.symmetryPlaneLocationNames(tNames);
    }
}

void ParseOptimizationParameters::setLevelsetNodesetIDs(XMLGen::OptimizationParameters &aMetadata)
{
    auto tItr = mTags.find("levelset_nodesets");
    std::string tValues = tItr->second.first.second;
    if (tItr != mTags.end() && !tValues.empty())
    {
        std::vector<std::string> tIDs;
        char tValuesBuffer[10000];
        strcpy(tValuesBuffer, tValues.c_str());
        XMLGen::parse_tokens(tValuesBuffer, tIDs);
        aMetadata.setLevelsetNodesets(tIDs);
    }
}

void ParseOptimizationParameters::setCSMParameters(XMLGen::OptimizationParameters &aMetadata)
{
    auto tItr = mTags.find("csm_file");
    std::string tValue = tItr->second.first.second;
    if (tItr != mTags.end() && !tValue.empty())
    {
        size_t tPos = tValue.rfind(".csm");
        if(tPos != std::string::npos)
        {
            std::string tBaseName = tValue.substr(0, tPos);
            std::string tTessName = tBaseName + ".eto";
            std::string tExoName = tBaseName + ".exo";
            std::string tOptName = tBaseName + "_opt.csm";
            aMetadata.append("csm_opt_file", tOptName, false);
            aMetadata.append("csm_tesselation_file", tTessName, false);
            aMetadata.append("csm_exodus_file", tExoName, false);
        }
    }
}

void ParseOptimizationParameters::setMMAStagnationDefaultsForShapeOptimizationProblems(XMLGen::OptimizationParameters &aMetadata)
{
    // If this is a shape optimization problem using mma and the user has not specified values for 
    // the objective and control stagnation tolerance parameters we will set them to -1
    // so that the optimizer will not exit on these stopping criteria. This helps it not 
    // exit early for the wrong reason.
    auto tAlgorithm = aMetadata.optimization_algorithm();
    if(tAlgorithm == "mma")
    {
        if(aMetadata.optimizationType() == OT_SHAPE)
        {
            auto tItr = mTags.find("mma_control_stagnation_tolerance");
            if (tItr != mTags.end())
            {
                std::string tValue = tItr->second.first.second;
                if (tValue.empty())
                {
                    aMetadata.append("mma_control_stagnation_tolerance", "-1", false);
                    std::cout << "INFO: Setting the mma_control_stagnation_tolerance parameter to -1. Explicitly set this value to something else if desired." << std::endl; 
                }
            }
            tItr = mTags.find("mma_objective_stagnation_tolerance");
            if (tItr != mTags.end())
            {
                std::string tValue = tItr->second.first.second;
                if (tValue.empty())
                {
                    aMetadata.append("mma_objective_stagnation_tolerance", "-1", false);
                    std::cout << "INFO: Setting the mma_objective_stagnation_tolerance parameter to -1. Explicitly set this value to something else if desired." << std::endl; 
                }
            }
        }
    }
}


}
// namespace XMLGen
