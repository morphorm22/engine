/*
 * Plato_ConservativeConvexSeparableApproximation.hpp
 *
 *  Created on: Nov 4, 2017
 */

/*
//@HEADER
// *************************************************************************
//   Plato Engine v.1.0: Copyright 2018, National Technology & Engineering
//                    Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Sandia Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact the Plato team (plato3D-help@sandia.gov)
//
// *************************************************************************
//@HEADER
*/

#ifndef PLATO_CONSERVATIVECONVEXSEPARABLEAPPROXIMATION_HPP_
#define PLATO_CONSERVATIVECONVEXSEPARABLEAPPROXIMATION_HPP_

namespace Plato
{

template<typename ScalarType, typename OrdinalType>
class ConservativeConvexSeparableAppxDataMng;
template<typename ScalarType, typename OrdinalType>
class ConservativeConvexSeparableAppxStageMng;

/******************************************************************************//**
 * @brief CCSA algorithm stopping criteria flags
**********************************************************************************/
struct ccsa
{
    enum stop_t
    {
        STATIONARITY_TOLERANCE = 1,
        KKT_CONDITIONS_TOLERANCE = 2,
        CONTROL_STAGNATION = 3,
        OBJECTIVE_STAGNATION = 4,
        MAX_NUMBER_ITERATIONS = 5,
        OPTIMALITY_AND_FEASIBILITY_MET = 6,
        NOT_CONVERGED = 7,
    };
};

/******************************************************************************//**
 * @brief Output a brief sentence explaining why the CCSA optimizer stopped.
 * @param [in] aStopCriterion stopping criterion flag
 * @param [in,out] aOutput string with brief description
**********************************************************************************/
inline void get_ccsa_stop_criterion(const Plato::ccsa::stop_t & aStopCriterion, std::string & aOutput)
{
    aOutput.clear();
    switch(aStopCriterion)
    {
        case Plato::ccsa::stop_t::STATIONARITY_TOLERANCE:
        {
            aOutput = "\n\n****** Optimization stopping due to stationary measure being met. ******\n\n";
            break;
        }
        case Plato::ccsa::stop_t::KKT_CONDITIONS_TOLERANCE:
        {
            aOutput = "\n\n****** Optimization stopping due to KKT tolerance being met. ******\n\n";
            break;
        }
        case Plato::ccsa::stop_t::OBJECTIVE_STAGNATION:
        {
            aOutput = "\n\n****** Optimization stopping due to objective stagnation. ******\n\n";
            break;
        }
        case Plato::ccsa::stop_t::CONTROL_STAGNATION:
        {
            aOutput = "\n\n****** Optimization stopping due to control (i.e. design variable) stagnation. ******\n\n";
            break;
        }
        case Plato::ccsa::stop_t::MAX_NUMBER_ITERATIONS:
        {
            aOutput = "\n\n****** Optimization stopping due to exceeding maximum number of iterations. ******\n\n";
            break;
        }
        case Plato::ccsa::stop_t::OPTIMALITY_AND_FEASIBILITY_MET:
        {
            aOutput = "\n\n****** Optimization stopping due to optimality and feasibility tolerance being met. ******\n\n";
            break;
        }
        case Plato::ccsa::stop_t::NOT_CONVERGED:
        {
            aOutput = "\n\n****** Optimization algorithm did not converge. ******\n\n";
            break;
        }
        default:
        {
            aOutput = "\n\n****** ERROR: Optimization algorithm stopping due to undefined behavior. ******\n\n";
            break;
        }
    }
}

/******************************************************************************//**
 * @brief Abstract interface for Conservative Convex Separable Approximation (CCSA) algorithm
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
class ConservativeConvexSeparableApproximation
{
public:
    virtual ~ConservativeConvexSeparableApproximation()
    {
    }

    virtual void solve(Plato::ConservativeConvexSeparableAppxStageMng<ScalarType, OrdinalType> & aStageMng,
                       Plato::ConservativeConvexSeparableAppxDataMng<ScalarType, OrdinalType> & aDataMng) = 0;
    virtual void initializeAuxiliaryVariables(Plato::ConservativeConvexSeparableAppxDataMng<ScalarType, OrdinalType> & aDataMng) = 0;
};

} // namespace Plato

#endif /* PLATO_CONSERVATIVECONVEXSEPARABLEAPPROXIMATION_HPP_ */
