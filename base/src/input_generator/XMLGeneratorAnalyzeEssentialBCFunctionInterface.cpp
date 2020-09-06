/*
 * XMLGeneratorAnalyzeEssentialBCFunctionInterface.cpp
 *
 *  Created on: Jun 15, 2020
 */

#include "XMLGeneratorUtilities.hpp"
#include "XMLGeneratorValidInputKeys.hpp"
#include "XMLGeneratorAnalyzeEssentialBCFunctionInterface.hpp"

namespace XMLGen
{

namespace Private
{

std::string check_essential_boundary_condition_application_name_keyword
(const XMLGen::BC& aBC)
{
    if(aBC.app_name.empty() && aBC.app_id.empty())
    {
        THROWERR(std::string("Check Essential Boundary Condition Application Set Name: ")
            + "Application set name, e.g. sideset or nodeset, for Essential Boundary Condition "
            + "with identification number '" + aBC.bc_id + "' is empty.")
    }
    auto tOutput = aBC.app_name.empty() ? aBC.app_id : aBC.app_name;
    return tOutput;
}
// function check_essential_boundary_condition_application_name_keyword

void check_essential_boundary_condition_value_keyword
(const XMLGen::BC& aBC)
{
    if(aBC.value.empty())
    {
        THROWERR(std::string("Check Essential Boundary Condition Value: Value parameter for Essential Boundary Condition ")
            + "with identification number '" + aBC.bc_id + "' is empty.")
    }
}
// function check_essential_boundary_condition_value_keyword

void append_rigid_essential_boundary_condition_to_plato_problem
(const std::string& aName,
 const XMLGen::BC& aBC,
 pugi::xml_node &aParentNode)
{
    XMLGen::ValidDofsKeys tValidDofs;
    auto tDofNames = tValidDofs.names(aBC.mPhysics);
    std::vector<std::string> tKeys = {"name", "type", "value"};
    auto tSetName = XMLGen::Private::check_essential_boundary_condition_application_name_keyword(aBC);
    for(auto& tDofName : tDofNames)
    {
        auto tBCName = aName + " applied to Dof with tag " + Plato::toupper(tDofName);
        auto tEssentialBoundaryCondParentNode = aParentNode.append_child("ParameterList");
        XMLGen::append_attributes({"name"}, {tBCName}, tEssentialBoundaryCondParentNode);
        std::vector<std::string> tValues = {"Type", "string", "Zero Value"};
        XMLGen::append_parameter_plus_attributes(tKeys, tValues, tEssentialBoundaryCondParentNode);
        auto tDof = tValidDofs.dof(aBC.mPhysics, tDofName);
        tValues = {"Index", "int", tDof};
        XMLGen::append_parameter_plus_attributes(tKeys, tValues, tEssentialBoundaryCondParentNode);
        tValues = {"Sides", "string", tSetName};
        XMLGen::append_parameter_plus_attributes(tKeys, tValues, tEssentialBoundaryCondParentNode);
    }
}
// function append_rigid_essential_boundary_condition_to_plato_problem

void append_zero_value_essential_boundary_condition_to_plato_problem
(const std::string& aName,
 const XMLGen::BC& aBC,
 pugi::xml_node &aParentNode)
{
    auto tBCName = aName + " applied to Dof with tag " + Plato::toupper(aBC.dof);
    auto tEssentialBoundaryCondParentNode = aParentNode.append_child("ParameterList");
    XMLGen::append_attributes({"name"}, {tBCName}, tEssentialBoundaryCondParentNode);

    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Type", "string", "Zero Value"};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, tEssentialBoundaryCondParentNode);

    XMLGen::ValidDofsKeys tValidDofs;
    auto tDofInteger = tValidDofs.dof(aBC.mPhysics, aBC.dof);
    tValues = {"Index", "int", tDofInteger};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, tEssentialBoundaryCondParentNode);

    auto tSetName = XMLGen::Private::check_essential_boundary_condition_application_name_keyword(aBC);
    tValues = {"Sides", "string", tSetName};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, tEssentialBoundaryCondParentNode);
}
// function append_zero_value_essential_boundary_condition_to_plato_problem

void append_fixed_value_essential_boundary_condition_to_plato_problem
(const std::string& aName,
 const XMLGen::BC& aBC,
 pugi::xml_node &aParentNode)
{
    auto tBCName = aName + " applied to Dof with tag " + Plato::toupper(aBC.dof);
    auto tEssentialBoundaryCondParentNode = aParentNode.append_child("ParameterList");
    XMLGen::append_attributes({"name"}, {tBCName}, tEssentialBoundaryCondParentNode);

    std::vector<std::string> tKeys = {"name", "type", "value"};
    std::vector<std::string> tValues = {"Type", "string", "Fixed Value"};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, tEssentialBoundaryCondParentNode);

    XMLGen::ValidDofsKeys tValidDofs;
    auto tDofInteger = tValidDofs.dof(aBC.mPhysics, aBC.dof);
    tValues = {"Index", "int", tDofInteger};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, tEssentialBoundaryCondParentNode);

    auto tSetName = XMLGen::Private::check_essential_boundary_condition_application_name_keyword(aBC);
    tValues = {"Sides", "string", tSetName};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, tEssentialBoundaryCondParentNode);

    XMLGen::Private::check_essential_boundary_condition_value_keyword(aBC);
    tValues = {"Value", "double", aBC.value};
    XMLGen::append_parameter_plus_attributes(tKeys, tValues, tEssentialBoundaryCondParentNode);
}
// function append_fixed_value_essential_boundary_condition_to_plato_problem

}
// namespace Private

}
// namespace XMLGen

namespace XMLGen
{

void AppendEssentialBoundaryCondition::insert()
{
    // rigid
    auto tFuncIndex = std::type_index(typeid(XMLGen::Private::append_rigid_essential_boundary_condition_to_plato_problem));
    mMap.insert(std::make_pair("rigid",
      std::make_pair((XMLGen::Analyze::EssentialBCFunc)XMLGen::Private::append_rigid_essential_boundary_condition_to_plato_problem, tFuncIndex)));

    // fixed = rigid
    tFuncIndex = std::type_index(typeid(XMLGen::Private::append_rigid_essential_boundary_condition_to_plato_problem));
    mMap.insert(std::make_pair("fixed",
      std::make_pair((XMLGen::Analyze::EssentialBCFunc)XMLGen::Private::append_rigid_essential_boundary_condition_to_plato_problem, tFuncIndex)));

    // zero value
    tFuncIndex = std::type_index(typeid(XMLGen::Private::append_zero_value_essential_boundary_condition_to_plato_problem));
    mMap.insert(std::make_pair("zero value",
      std::make_pair((XMLGen::Analyze::EssentialBCFunc)XMLGen::Private::append_zero_value_essential_boundary_condition_to_plato_problem, tFuncIndex)));

    // fixed_value
    tFuncIndex = std::type_index(typeid(XMLGen::Private::append_fixed_value_essential_boundary_condition_to_plato_problem));
    mMap.insert(std::make_pair("fixed_value",
      std::make_pair((XMLGen::Analyze::EssentialBCFunc)XMLGen::Private::append_fixed_value_essential_boundary_condition_to_plato_problem, tFuncIndex)));

    // insulated
    tFuncIndex = std::type_index(typeid(XMLGen::Private::append_zero_value_essential_boundary_condition_to_plato_problem));
    mMap.insert(std::make_pair("insulated",
      std::make_pair((XMLGen::Analyze::EssentialBCFunc)XMLGen::Private::append_zero_value_essential_boundary_condition_to_plato_problem, tFuncIndex)));
}

void AppendEssentialBoundaryCondition::call
(const std::string& aName,
 const XMLGen::BC& aBC,
 pugi::xml_node& aParentNode) const
{
    auto tLowerPerformer = Plato::tolower(aBC.mCode);
    if(tLowerPerformer.compare("plato_analyze") != 0)
    {
        return;
    }

    auto tCategory = Plato::tolower(aBC.mCategory);
    auto tMapItr = mMap.find(tCategory);
    if(tMapItr == mMap.end())
    {
        THROWERR(std::string("Essential Boundary Condition Function Interface: Did not find essential boundary condition function with tag '")
            + tCategory + "' in list. Essential boundary condition '" + tCategory + "' is not supported in Plato Analyze.")
    }
    auto tTypeCastedFunc = reinterpret_cast<void(*)(const std::string&, const XMLGen::BC&, pugi::xml_node&)>(tMapItr->second.first);
    if(tMapItr->second.second == std::type_index(typeid(tTypeCastedFunc)))
    {
        THROWERR(std::string("Essential Boundary Condition Interface: Reinterpret cast for essential boundary condition function with tag '")
            + tCategory + "' failed.")
    }
    tTypeCastedFunc(aName, aBC, aParentNode);
}

}
// namespace XMLGen
