/*
 * XMLGeneratorPlatoMainOperationFileUtilities.hpp
 *
 *  Created on: May 28, 2020
 */

#pragma once

#include "pugixml.hpp"

#include "XMLGeneratorDataStruct.hpp"

namespace XMLGen
{

/******************************************************************************//**
 * \fn write_plato_main_operations_xml_file_for_nondeterministic_usecase
 * \brief Write Plato main operation xml file for nondeterministic use cases.
 * \param [in] aXMLMetaData Plato problem input data
**********************************************************************************/
void write_plato_main_operations_xml_file_for_nondeterministic_usecase
(const XMLGen::InputData& aXMLMetaData);

/******************************************************************************//**
 * \fn is_volume_constraint_defined
 * \brief Return true if volume constraint is defined.
 * \param [in] aXMLMetaData Plato problem input data
 * \return flag
**********************************************************************************/
bool is_volume_constraint_defined
(const XMLGen::InputData& aXMLMetaData);

/******************************************************************************//**
 * \fn is_volume_constraint_defined_and_computed_by_platomain
 * \brief Return true if volume constraint is defined and plato main is responsible \n
 *  for its calculation.
 * \param [in] aXMLMetaData Plato problem input data
 * \return flag
**********************************************************************************/
bool is_volume_constraint_defined_and_computed_by_platomain
(const XMLGen::InputData& aXMLMetaData);

/******************************************************************************//**
 * \fn append_filter_options_to_operation
 * \brief Append filter options to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_filter_options_to_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node &aParentNode);

/******************************************************************************//**
 * \fn append_filter_options_to_plato_main_operation
 * \brief Append filter operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_filter_options_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document &aDocument);

/******************************************************************************//**
 * \fn append_constraint_gradient_input_to_output_operation
 * \brief Append constrain gradient inputs to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_constraint_gradient_input_to_output_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node &aParentNode);

/******************************************************************************//**
 * \fn append_objective_gradient_input_to_output_operation
 * \brief Append objective gradient inputs to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_objective_gradient_input_to_output_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node &aParentNode);

/******************************************************************************//**
 * \fn append_deterministic_qoi_inputs_to_output_operation
 * \brief Append deterministic Quantities of Interest (QoI) inputs to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_deterministic_qoi_inputs_to_output_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node &aParentNode);

/******************************************************************************//**
 * \fn append_nondeterministic_qoi_inputs_to_output_operation
 * \brief Append nondeterministic Quantities of Interest (QoI) inputs to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_nondeterministic_qoi_inputs_to_output_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node &aParentNode);

/******************************************************************************//**
 * \fn append_children_to_output_operation
 * \brief Append output operation children to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_children_to_output_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node &aParentNode);

/******************************************************************************//**
 * \fn append_children_to_output_operation
 * \brief Append default Quantities of Interest (QoI) inputs to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_default_qoi_to_output_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node &aParentNode);

/******************************************************************************//**
 * \fn append_surface_extraction_to_output_operation
 * \brief Append surface extraction operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_surface_extraction_to_output_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node &aParentNode);

/******************************************************************************//**
 * \fn append_output_to_plato_main_operation
 * \brief Append Plato main output operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_output_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document &aDocument);

/******************************************************************************//**
 * \fn append_stochastic_objective_value_to_plato_main_operation
 * \brief Append stochastic objective value operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_stochastic_objective_value_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document &aDocument);

/******************************************************************************//**
 * \fn append_stochastic_criterion_gradient_operation
 * \brief Append stochastic criterion gradient operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_stochastic_criterion_gradient_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_stochastic_criterion_value_operation
 * \brief Append stochastic criterion value operation to PUGI XML document.
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_stochastic_criterion_value_operation
(pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_stochastic_objective_gradient_to_plato_main_operation
 * \brief Append stochastic criterion value operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_stochastic_objective_gradient_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document &aDocument);

/******************************************************************************//**
 * \fn append_nondeterministic_qoi_statistics_to_plato_main_operation
 * \brief Append nondeterministic Quantities of Interest (QoI) plato main outputs \n
 * to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_nondeterministic_qoi_statistics_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document &aDocument);

/******************************************************************************//**
 * \fn append_update_problem_to_plato_main_operation
 * \brief Append update problem operation to PUGI XML document.
 * \param [in/out] aDocument  pugi::xml_document
**********************************************************************************/
void append_update_problem_to_plato_main_operation
(pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_filter_control_to_plato_main_operation
 * \brief Append filter control operation to PUGI XML document.
 * \param [in/out] aDocument  pugi::xml_document
**********************************************************************************/
void append_filter_control_to_plato_main_operation
(pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_filter_gradient_to_plato_main_operation
 * \brief Append filter gradient operation to PUGI XML document.
 * \param [in/out] aDocument  pugi::xml_document
**********************************************************************************/
void append_filter_gradient_to_plato_main_operation
(pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_initialize_density_field_operation
 * \brief Append initialize density field operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_initialize_density_field_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_initialize_field_from_file_operation
 * \brief Append initialize field from file operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_initialize_field_from_file_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_levelset_material_box
 * \brief Append levelset material box operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_levelset_material_box
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_initialize_levelset_primitives_operation
 * \brief Append initialize levelset primitives operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_initialize_levelset_primitives_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_initialize_levelset_swiss_cheese_operation
 * \brief Append initialize levelset swiss cheese operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_initialize_levelset_swiss_cheese_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_initialize_levelset_operation
 * \brief Append initialize levelset operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_initialize_levelset_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_initialize_field_operation
 * \brief Append initialize control field operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_initialize_field_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_initialize_field_to_plato_main_operation
 * \brief Append initialize control field to plato main operation to XML file.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_initialize_field_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_design_volume_to_plato_main_operation
 * \brief Append calculate design volume operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_design_volume_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_compute_volume_to_plato_main_operation
 * \brief Append compute volume operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_compute_volume_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_compute_volume_gradient_to_plato_main_operation
 * \brief Append compute volume gradient operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_compute_volume_gradient_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_fixed_blocks_identification_numbers_to_operation
 * \brief Append fixed blocks identification numbers operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_fixed_blocks_identification_numbers_to_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_fixed_sidesets_identification_numbers_to_operation
 * \brief Append fixed sidesets identification numbers operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_fixed_sidesets_identification_numbers_to_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_fixed_nodesets_identification_numbers_to_operation
 * \brief Append fixed nodesets identification numbers operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aParentNode  pugi::xml_node
**********************************************************************************/
void append_fixed_nodesets_identification_numbers_to_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_node& aParentNode);

/******************************************************************************//**
 * \fn append_set_lower_bounds_to_plato_main_operation
 * \brief Append set lower bounds operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_set_lower_bounds_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

/******************************************************************************//**
 * \fn append_set_upper_bounds_to_plato_main_operation
 * \brief Append set upper bounds operation to PUGI XML document.
 * \param [in]     aXMLMetaData Plato problem input data
 * \param [in/out] aDocument    pugi::xml_document
**********************************************************************************/
void append_set_upper_bounds_to_plato_main_operation
(const XMLGen::InputData& aXMLMetaData,
 pugi::xml_document& aDocument);

}
// namespace XMLGen