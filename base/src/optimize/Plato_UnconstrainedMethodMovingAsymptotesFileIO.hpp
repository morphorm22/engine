#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>

#include "Plato_Types.hpp"
#include "Plato_CommWrapper.hpp"

namespace Plato
{

/*****************************************************************************************//**
 * @brief Diagnostic data for the Unconstrained Method of Moving Asymptotes (U-MMA) algorithm
*********************************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
struct OutputDataUMMA
{
    OrdinalType mNumOuterIter;   /*!< number of outer iterations */
    OrdinalType mObjFuncEvals;   /*!< number of objective function evaluations */
    OrdinalType mObjGradEvals;   /*!< number of objective function gradient evaluations */

    ScalarType mFeasibility;     /*!< change between two subsequent control solutions */
    ScalarType mCurrentObjFuncValue;    /*!< objective function value */
    ScalarType mControlChange;   /*!< change between two subsequent control solutions */
    ScalarType mObjFuncChange;   /*!< change between two subsequent objective function values */
    ScalarType mNormObjFuncGrad; /*!< norm of the objective function gradient */
};
// struct OutputDataUMMA

/******************************************************************************//**
 * @brief Check is diagnostic file is closed
 * @param [in] aComm MPI communicator interface
 * @param [in] aOutputStream output file
 **********************************************************************************/
template<typename OutputType>
void is_diagnostic_file_close(const Plato::CommWrapper &aComm,OutputType &aOutputStream)
{
    if(aComm.myProcID() == 0)
    {
        if(aOutputStream.is_open() == false)
        {
            std::string tErrorMsg("Diagnostics requested but write file is closed!");
            std::cout << std::string("\nFILE: ") + __FILE__ \
                + std::string("\nFUNCTION: ") + __PRETTY_FUNCTION__ \
                + std::string("\nLINE:") + std::to_string(__LINE__) \
                + std::string("\nMESSAGE: ") + tErrorMsg << std::endl << std::flush;
            aComm.abort();
        }
        aComm.barrier();
    }
}
// function is_diagnostic_file_close

/******************************************************************************//**
 * @brief Write header for U-MMA diagnostics file
 * @param [in]  aComm         interface to MPI communicator
 * @param [in]  aData         diagnostic data for U-MMA algorithm
 * @param [out] aOutputStream output stream to be written to file
**********************************************************************************/
template<typename ScalarType, typename OrdinalType, typename OutputType>
void write_umma_diagnostics_header(
 const Plato::CommWrapper &aComm,
 const Plato::OutputDataUMMA<ScalarType, OrdinalType> &aData,
 OutputType &aOutputStream
)
{
    aOutputStream << std::scientific << std::setprecision(6) << std::right << "Iter" << std::setw(10) << "F-count"
            << std::setw(12) << "G-count"<< std::setw(12) << "F(X)" << std::setw(16) << "Norm(F')" << std::setw(15) 
            << "abs(dX)" << std::setw(15) << "abs(dF)" << "\n" << std::flush;
}
// function write_umma_diagnostics_header

/******************************************************************************//**
 * @brief Output a brief sentence describing reason for convergence.
 * @param [in] aStopCriterion stopping criterion flag
**********************************************************************************/
inline std::string get_umma_stop_criterion_description(const Plato::algorithm::stop_t &aStopCriterion)
{
    std::string tOutput;
    switch(aStopCriterion)
    {
        case Plato::algorithm::stop_t::CONTROL_STAGNATION:
        {
            tOutput = "\n\n****** Optimization stopping due to control (i.e. design variable) stagnation. ******\n\n";
            break;
        }
        case Plato::algorithm::stop_t::OBJECTIVE_STAGNATION:
        {
            tOutput = "\n\n****** Optimization stopping due to objective stagnation. ******\n\n";
            break;
        }
        case Plato::algorithm::stop_t::MAX_NUMBER_ITERATIONS:
        {
            tOutput = "\n\n****** Optimization stopping due to exceeding maximum number of iterations. ******\n\n";
            break;
        }
        case Plato::algorithm::stop_t::NOT_CONVERGED:
        {
            tOutput = "\n\n****** Optimization algorithm did not converge. ******\n\n";
            break;
        }
        default:
        {
            tOutput = "\n\n****** ERROR: Optimization algorithm stopping due to undefined behavior. ******\n\n";
            break;
        }
    }
    return tOutput;
}
// function get_umma_stop_criterion_description

/******************************************************************************//**
 * @brief Print diagnostics for MMA algorithm
 * @param [in]  aComm         interface to MPI communicator
 * @param [in]  aData         diagnostic data for U-MMA algorithm
 * @param [out] aOutputStream output string stream
**********************************************************************************/
template<typename ScalarType, typename OrdinalType, typename OutputType>
void write_umma_diagnostics(
 const Plato::CommWrapper &aComm,
 const Plato::OutputDataUMMA<ScalarType, OrdinalType> &aData,
 OutputType &aOutputStream
)
{
    aOutputStream << std::scientific << std::setprecision(6) << std::right << aData.mNumOuterIter 
        << std::setw(10) << aData.mObjFuncEvals  << std::setw(12) << aData.mObjGradEvals << std::setw(18) 
        << aData.mCurrentObjFuncValue << std::setw(15) << aData.mNormObjFuncGrad << std::setw(15) 
        << aData.mControlChange << std::setw(15) << aData.mObjFuncChange << "\n" << std::flush;
}
// function write_umma_diagnostics

}
// namespace Plato