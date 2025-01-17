/*
 * XMLGeneratorGradBasedOptimizerOptions.hpp
 *
 *  Created on: Jan 17, 2022
 */

#pragma once

#include "pugixml.hpp"

#include "XMLGeneratorDataStruct.hpp"

namespace XMLGen
{

/******************************************************************************//**
 * \fn append_grad_based_optimizer_options
 * \brief Append gradeitn-based optimizer options to interface xml file.
 * \param [in]  aMetaData Plato problem input data
 * \param [out] aParentNode  parent node to append data to
**********************************************************************************/
void append_grad_based_optimizer_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_optimizer_update_problem_stage_options
 * \brief Append update problem options to optimization block in the interface xml file.
 * \param [in]  aMetaData Plato problem input data
 * \param [out] aParentNode  parent node to append data to
**********************************************************************************/
void append_optimizer_update_problem_stage_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_grad_based_optimizer_parameters
 * \brief Append optimization algorithm's options to interface xml file.
 * \param [in]  aMetaData Plato problem input data
 * \param [out] aParentNode  parent node to append data to
**********************************************************************************/
void append_grad_based_optimizer_parameters
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_optimality_criteria_options
 * \brief Append optimality criteria options to interface xml file.
 * \param [in]  aMetaData Plato problem input data
 * \param [out] aParentNode  parent node to append data to
**********************************************************************************/
void append_optimality_criteria_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_method_moving_asymptotes_options
 * \brief Append Method of Moving Asymptotes (MMA) options to interface xml file.
 * \param [in]  aMetaData   Plato problem input data
 * \param [out] aParentNode parent node storage
**********************************************************************************/
void append_method_moving_asymptotes_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_unconstrained_method_moving_asymptotes_options
 * \brief Append Unconstrained Method of Moving Asymptotes (UMMA) options to 
 *        interface xml file.
 * \param [in]  aMetaData   Plato problem input data
 * \param [out] aParentNode parent node storage
**********************************************************************************/
void append_unconstrained_method_moving_asymptotes_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_trust_region_kelley_sachs_options
 * \brief Append trust region Kelley-Sachs options to interface xml file.
 * \param [in]  aMetaData Plato problem input data
 * \param [out] aParentNode  parent node to append data to
**********************************************************************************/
void append_trust_region_kelley_sachs_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_augmented_lagrangian_options
 * \brief Append augmented Lagrangian options to interface xml file.
 * \param [in]  aMetaData Plato problem input data
 * \param [out] aParentNode  parent node to append data to
**********************************************************************************/
void append_augmented_lagrangian_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_reset_algorithm_on_update_option
 * \brief Append option specifying whether to reset the algorithm at the update frequency
 * \param [in]  aMetaData  Plato problem input data
 * \param [out] aParentNode Parent to add data to
**********************************************************************************/
void append_reset_algorithm_on_update_option
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_optimizer_cache_stage_options
 * \brief Append cache state options to optimization block in the interface xml file.
 * \param [in]  aMetaData  Plato problem input data
 * \param [out] aParentNode Parent to add data to
**********************************************************************************/
void append_optimizer_cache_stage_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_grad_based_optimizer_output_options
 * \brief Append optimization output options to optimization block in the \n
          interface xml file.
 * \param [in]  aMetaData  Plato problem input data
 * \param [out] aParentNode Parent to add data to
**********************************************************************************/
void append_grad_based_optimizer_output_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_grad_based_optimizer_variables_options
 * \brief Append gradient-based optimizer variables options to optimization \n 
          block in the interface xml file.
 * \param [in]  aMetaData  Plato problem input data
 * \param [out] aParentNode Parent to add data to
**********************************************************************************/
void append_grad_based_optimizer_variables_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_grad_based_optimizer_objective_options
 * \brief Append gradient-based objective options to optimization block in \n 
          the interface xml file.
 * \param [in]  aMetaData  Plato problem input data
 * \param [out] aParentNode Parent to add data to
**********************************************************************************/
void append_grad_based_optimizer_objective_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_grad_based_optimizer_constraint_options
 * \brief Append gradient-based constraint options to optimization blcok in \n
          the interface xml file.
 * \param [in]  aMetaData  Plato problem input data
 * \param [out] aParentNode Parent to add data to
**********************************************************************************/
void append_grad_based_optimizer_constraint_options
(const XMLGen::InputData& aMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_bound_constraints_options_topology_optimization
 * \brief Append optimizer bound constraint options to optimization block in \n 
          the interface xml file.
 * \param [in]  aValues     upper and lower bounds, i.e. {upper_value, lower_value}
 * \param [out] aParentNode Parent to add data to
**********************************************************************************/
void append_bound_constraints_options_topology_optimization
(const std::vector<std::string>& aValues,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn generate_target_value_entries
 * \brief Append constraint target values to optimization block in the interface xml file.
 * \param [in]  aMetaData   Plato problem input data
 * \param [in]  aConstraint constraint input metadata
 * \param [out] aKeyToValueMap key to value map of constraint target values
**********************************************************************************/
void generate_target_value_entries
(const XMLGen::InputData& aMetaData,
 const XMLGen::Constraint &aConstraint,
 std::map<std::string, std::string> &aKeyToValueMap);

}
//  namespace XMLGen
