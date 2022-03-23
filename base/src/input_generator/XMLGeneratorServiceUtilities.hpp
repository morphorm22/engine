/*
 * XMLGeneratorServiceUtilities.hpp
 *
 *  Created on: Mar 23, 2022
 *
 */

#pragma once

namespace XMLGen
{

struct InputData;

/******************************************************************************//**
 * \fn get_service_indices
 * \brief Return service container indices associated target code/app supported in Plato. 
 * \param [in] aTargetCodeName target code/app name
 * \param [in] aInputMetaData  input metadata read from Plato input deck
 * \return container of service indices
**********************************************************************************/
std::vector<size_t> 
get_service_indices
(const std::string& aTargetCodeName,
 const XMLGen::InputData& aInputMetaData);

/******************************************************************************//**
 * \fn get_service_indices
 * \brief Return service container indices associated target code/app supported in Plato. 
 * \param [in] aTargetCodeName target code/app name
 * \param [in] aInputMetaData  input metadata read from Plato input deck
 * \return container of service indices
**********************************************************************************/
std::vector<std::string> 
get_values_from_service_metadata
(const std::string& aCodeName,
 const std::string& aTargetKey,
 const XMLGen::InputData& aInputMetaData);

/******************************************************************************//**
 * \fn service_needs_decomp
 * \brief Determine if service needs decomp
 * \param [in] aService service to check
 * \param [in] hasBeenDecompedForThisNumberOfProcessors map to check if decomped
**********************************************************************************/
bool service_needs_decomp
(const XMLGen::Service& aService,
 std::map<std::string,int>& hasBeenDecompedForThisNumberOfProcessors);

/******************************************************************************//**
 * \fn get_unique_decomp_service
 * \brief Get service ID for unique application of decomp 
 * \param [in] aMetaData Input metadata
**********************************************************************************/
std::string get_unique_decomp_service
(const XMLGen::InputData& aXMLMetaData);

/******************************************************************************//**
 * \fn need_update_problem_stage
 * \brief Return whether any service has the update problem feature enabled.
 * \param [in] aMetaData  Plato problem input data
**********************************************************************************/
bool need_update_problem_stage(const XMLGen::InputData& aMetaData);

/******************************************************************************//**
 * \fn num_cache_states
 * \brief Return the number of services with the cache state feature enabled.
 * \param [in] aServices List of services to check
**********************************************************************************/
int num_cache_states(const std::vector<XMLGen::Service> &aServices);

}
// namespace XMLGen