/*
 * XMLGeneratorMaterialFunctionInterface.cpp
 *
 *  Created on: Jun 5, 2020
 */

#include <utility>

#include "XMLGeneratorUtilities.hpp"
#include "XMLGeneratorValidInputKeys.hpp"
#include "XMLGeneratorMaterialFunctionInterface.hpp"
#include "XMLGeneratorPlatoAnalyzeOperationsFileUtilities.hpp"

namespace XMLGen
{

namespace Private
{

/******************************************************************************//**
 * \fn append_orthotropic_linear_elastic_material_properties_to_plato_analyze_operation
 * \brief Append orthotropic material model and corresponding properties to the \n
 *     plato_analyze_operation.xml file.
 * \param [in]     aMaterialTags list of material tags, i.e. \n
 *     vector<pair<material_property_argument_name_tag, material_property_tag>>
 * \param [in/out] aParentNode   pugi::xml_node
**********************************************************************************/
inline void append_orthotropic_linear_elastic_material_properties_to_plato_analyze_operation
(const std::vector<std::pair<std::string, std::string>>& aMaterialTags,
 pugi::xml_node& aParentNode)
{
    if(aMaterialTags.empty())
    {
        THROWERR("Append Orthotropic Material Properties to Plato Analyze Operation: Input vector of material property tags is empty.")
    }

    XMLGen::ValidMaterialPropertyKeys tValidMaterialModels;
    std::vector<std::string> tKeys = {"ArgumentName", "Target", "InitialValue"};
    for(auto& tPair : aMaterialTags)
    {
        auto tValidAnalyzeMaterialPropertyTag = tValidMaterialModels.tag("orthotropic linear elastic", tPair.second);
        auto tTarget = std::string("[Plato Problem]:[Material Model]:[Orthotropic Linear Elastic]:") + tValidAnalyzeMaterialPropertyTag;
        std::vector<std::string> tValues = {tPair.first, tTarget, "0.0"};
        auto tParameter = aParentNode.append_child("Parameter");
        XMLGen::append_children(tKeys, tValues, tParameter);
    }
}

/******************************************************************************//**
 * \fn append_isotropic_linear_thermoelastic_material_properties_to_plato_analyze_operation
 * \brief Append isotropic linear thermo-elastic material model and respective \n
 *     properties to the plato analyze operation xml file.
 * \param [in]     aMaterialTags list of material tags, i.e. \n
 *      vector<pair<material_property_argument_name_tag, material_property_tag>>
 * \param [in/out] aParentNode   pugi::xml_node
**********************************************************************************/
inline void append_isotropic_linear_thermoelastic_material_properties_to_plato_analyze_operation
(const std::vector<std::pair<std::string, std::string>>& aMaterialTags,
 pugi::xml_node& aParentNode)
{
    if(aMaterialTags.empty())
    {
        THROWERR("Append Isotropic Thermo-elastic Material Properties to Plato Analyze Operation: Input vector of material property tags is empty.")
    }

    XMLGen::ValidMaterialPropertyKeys tValidMaterialModels;
    std::vector<std::string> tKeys = {"ArgumentName", "Target", "InitialValue"};
    for(auto& tPair : aMaterialTags)
    {
        auto tValidAnalyzeMaterialPropertyTag = tValidMaterialModels.tag("isotropic linear thermoelastic", tPair.second);
        auto tTarget = std::string("[Plato Problem]:[Material Model]:[Isotropic Linear Thermoelastic]:") + tValidAnalyzeMaterialPropertyTag;
        std::vector<std::string> tValues = {tPair.first, tTarget, "0.0"};
        auto tParameter = aParentNode.append_child("Parameter");
        XMLGen::append_children(tKeys, tValues, tParameter);
    }
}

/******************************************************************************//**
 * \fn append_isotropic_linear_elastic_material_properties_to_plato_analyze_operation
 * \brief Append isotropic linear thermal material model and respective properties \n
 *     to the plato analyze operation xml file.
 * \param [in]     aMaterialTags list of material tags, i.e. \n
 *     vector<pair<material_property_argument_name_tag, material_property_tag>>
 * \param [in/out] aParentNode    pugi::xml_node
**********************************************************************************/
inline void append_isotropic_linear_elastic_material_properties_to_plato_analyze_operation
(const std::vector<std::pair<std::string, std::string>>& aMaterialTags,
 pugi::xml_node& aParentNode)
{
    if(aMaterialTags.empty())
    {
        THROWERR("Append Isotropic Material Properties to Plato Analyze Operation: Input vector of material property tags is empty.")
    }

    XMLGen::ValidMaterialPropertyKeys tValidMaterialModels;
    std::vector<std::string> tKeys = {"ArgumentName", "Target", "InitialValue"};
    for(auto& tPair : aMaterialTags)
    {
        auto tValidAnalyzeMaterialPropertyTag = tValidMaterialModels.tag("isotropic linear elastic", tPair.second);
        auto tTarget = std::string("[Plato Problem]:[Material Model]:[Isotropic Linear Elastic]:") + tValidAnalyzeMaterialPropertyTag;
        std::vector<std::string> tValues = {tPair.first, tTarget, "0.0"};
        auto tParameter = aParentNode.append_child("Parameter");
        XMLGen::append_children(tKeys, tValues, tParameter);
    }
}

/******************************************************************************//**
 * \fn append_j2_plasticity_material_properties_to_plato_analyze_operation
 * \brief Append J2 plasticity material model and respective material properties \n
 *   to the plato analyze operation xml file.
 * \param [in]     aMaterialTags list of material tags, i.e. \n
 *     vector<pair<material_property_argument_name_tag, material_property_tag>>
 * \param [in/out] aParentNode    pugi::xml_node
**********************************************************************************/
inline void append_j2_plasticity_material_properties_to_plato_analyze_operation
(const std::vector<std::pair<std::string, std::string>>& aMaterialTags,
 pugi::xml_node& aParentNode)
{
    if(aMaterialTags.empty())
    {
        THROWERR("Append J2 Plasticity Material Properties to Plato Analyze Operation: Input vector of material property tags is empty.")
    }

    XMLGen::ValidMaterialPropertyKeys tValidMaterialModels;
    std::vector<std::string> tKeys = {"ArgumentName", "Target", "InitialValue"};
    for(auto& tPair : aMaterialTags)
    {
        auto tValidAnalyzeMaterialPropertyTag = tValidMaterialModels.tag("j2 plasticity", tPair.second);
        auto tTarget = std::string("[Plato Problem]:[Plasticity Model]:[J2 Plasticity]:") + tValidAnalyzeMaterialPropertyTag;
        std::vector<std::string> tValues = {tPair.first, tTarget, "0.0"};
        auto tParameter = aParentNode.append_child("Parameter");
        XMLGen::append_children(tKeys, tValues, tParameter);
    }
}

}
// namespace Private

MaterialFunctionInterface::MaterialFunctionInterface()
{
    this->insert();
}

void MaterialFunctionInterface::insert()
{
    auto tFuncIndex = std::type_index(typeid(XMLGen::Private::append_isotropic_linear_elastic_material_properties_to_plato_analyze_operation));
    mMap.insert(std::make_pair("isotropic linear elastic",
      std::make_pair((XMLGen::Analyze::MaterialOperationFunc)XMLGen::Private::append_isotropic_linear_elastic_material_properties_to_plato_analyze_operation, tFuncIndex)));

    tFuncIndex = std::type_index(typeid(XMLGen::Private::append_isotropic_linear_thermoelastic_material_properties_to_plato_analyze_operation));
    mMap.insert(std::make_pair("isotropic linear thermoelastic",
      std::make_pair((XMLGen::Analyze::MaterialOperationFunc)XMLGen::Private::append_isotropic_linear_thermoelastic_material_properties_to_plato_analyze_operation, tFuncIndex)));

    tFuncIndex = std::type_index(typeid(XMLGen::Private::append_orthotropic_linear_elastic_material_properties_to_plato_analyze_operation));
    mMap.insert(std::make_pair("orthotropic linear elastic",
      std::make_pair((XMLGen::Analyze::MaterialOperationFunc)XMLGen::Private::append_orthotropic_linear_elastic_material_properties_to_plato_analyze_operation, tFuncIndex)));

    tFuncIndex = std::type_index(typeid(XMLGen::Private::append_j2_plasticity_material_properties_to_plato_analyze_operation));
    mMap.insert(std::make_pair("j2 plasticity",
      std::make_pair((XMLGen::Analyze::MaterialOperationFunc)XMLGen::Private::append_j2_plasticity_material_properties_to_plato_analyze_operation, tFuncIndex)));
}

void MaterialFunctionInterface::call
(const std::string &aCategory,
 const std::vector<std::pair<std::string,std::string>>& aMaterialTags,
 pugi::xml_node &aParentNode)
{
    auto tLowerFuncLabel = Plato::tolower(aCategory);
    auto tMapItr = mMap.find(tLowerFuncLabel);
    if(tMapItr == mMap.end())
    {
        THROWERR(std::string("Material Function Interface: Did not find material function with tag '") + aCategory + "' in list.")
    }
    auto tTypeCastedFunc =
        reinterpret_cast<void(*)(const std::vector<std::pair<std::string,std::string>>&, pugi::xml_node&)>(tMapItr->second.first);
    if(tMapItr->second.second == std::type_index(typeid(tTypeCastedFunc)))
    {
        THROWERR(std::string("Material Function Interface: Reinterpret cast for material function with tag '") + aCategory + "' failed.")
    }
    tTypeCastedFunc(aMaterialTags, aParentNode);
}

}
// namespace XMLGen
