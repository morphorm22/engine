/*
 * Plato_OptimizerInterface.hpp
 *
 *  Created on: Oct 30, 2017
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

#ifndef PLATO_OPTIMIZERINTERFACE_HPP_
#define PLATO_OPTIMIZERINTERFACE_HPP_

#include "Plato_Interface.hpp"
#include "Plato_OptimizerUtilities.hpp"

namespace Plato
{

struct optimizer
{

    enum algorithm_t
    {
        OPTIMALITY_CRITERIA = 1,
        METHOD_OF_MOVING_ASYMPTOTES = 2,
        GLOBALLY_CONVERGENT_METHOD_OF_MOVING_ASYMPTOTES = 3,
        KELLEY_SACHS_UNCONSTRAINED = 4,
        KELLEY_SACHS_BOUND_CONSTRAINED = 5,
        KELLEY_SACHS_AUGMENTED_LAGRANGIAN = 6,
        DERIVATIVE_CHECKER = 7,
        ROL_KSAL = 8,
        ROL_KSBC = 9,
        STOCHASTIC_REDUCED_ORDER_MODEL = 10,
        PARTICLE_SWARM_OPTMIZATION_ALPSO = 11,
        PARTICLE_SWARM_OPTMIZATION_BCPSO = 12,
        SO_PARAMETER_STUDIES = 13,
    }; // enum algorithm_t

};
// struct optimizer

/******************************************************************************//**
 * @brief Abstract interface to optimization algorithm
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
class OptimizerInterface
{
public:
    virtual ~OptimizerInterface()
    {
    }

    /******************************************************************************//**
     * @brief Interface to optimization algorithm - solves optimization problem
    **********************************************************************************/
    virtual void optimize() = 0;

    /******************************************************************************//**
     * @brief Initialize the optimizer
    **********************************************************************************/
    virtual void initialize() = 0;

    /******************************************************************************//**
     * @brief Finalize the optimizer
    **********************************************************************************/
    virtual void finalize() = 0;

    /******************************************************************************//**
     * @brief Get the optimizer node based on the index.
    **********************************************************************************/
    Plato::InputData getOptimizerNode(Plato::Interface* aInterface)
    {
        return Plato::getOptimizerNode(aInterface, mOptimizerIndex);
    }

    /******************************************************************************//**
     * @brief Set the optimizer name - Optional.
    **********************************************************************************/
    void setOptimizerName( std::string val ) { mOptimizerName = val; }

    /******************************************************************************//**
     * @brief Get the optimizer name.
    **********************************************************************************/
    std::string getOptimizerName() const { return mOptimizerName; }

    /******************************************************************************//**
     * @brief Set the index of the optimizer.

     // The optimizer block index is a vector of indices. The size of
     // the vector less 1 denotes the number of nesting levels. Each
     // index is the serial index of the optimizer block.

     // A basic run with one optimizer block would have a vector of
     // {0}. A simple nested run with one nested optimizer block would
     // have a vector of {0} for the outer optimizer block and {0,0}
     // for the inner optimizer block.

     // A more complicated vector for the inner most optimizer block
     // would be {2,1,0} which denotes the outer most third serial
     // optimizer block {2}, of which its second serial inner loop
     // optimizer block is wanted {2,1}, of which its first inner loop
     // optimizer block is wanted {2,1,0},

    **********************************************************************************/
  void setOptimizerIndex( std::vector<size_t> val ) { mOptimizerIndex = val; }

    /******************************************************************************//**
     * @brief Get the index of the optimizer.
    **********************************************************************************/
    std::vector<size_t> getOptimizerIndex() const { return mOptimizerIndex; }

    /******************************************************************************//**
     * @brief Set the inner loop boolean.
    **********************************************************************************/
    void setHasInnerLoop( bool val ) { mHasInnerLoop = val; }

    /******************************************************************************//**
     * @brief Get the inner loop boolean.
    **********************************************************************************/
    bool getHasInnerLoop() const { return mHasInnerLoop; }

    /******************************************************************************//**
     * @brief True if this optimizer is the last top level serial
     * optimizer to be executed.
    **********************************************************************************/
    void lastOptimizer( bool val ) { mLastOptimizer = val; }

    /******************************************************************************//**
     * @brief True if this optimizer is the last top level serial
     * optimizer to be executed.
    **********************************************************************************/
    bool lastOptimizer() { return mLastOptimizer; }

protected:
    /******************************************************************************//**
     * @brief String containing the optimizer name - Optional
    **********************************************************************************/
    std::string mOptimizerName{""};

    /******************************************************************************//**
     * @brief Vector of indices decribing this optimizer's location
     * in the input - used when there are multiple optimizers. See
     * Plato_OptimizerUtilities.hpp and the function getOptimizerNode().
    **********************************************************************************/
    std::vector<size_t> mOptimizerIndex;

    /******************************************************************************//**
     * @brief Boolean indicating if this optimizer is the last top
     * level serial optimizer to be executed and can issue a
     * finialize/terminate operation.
    **********************************************************************************/
    bool mLastOptimizer{false};

    /******************************************************************************//**
     * @brief Boolean indicating an inner optimizer loop is present
    **********************************************************************************/
    bool mHasInnerLoop{false};
};
// class OptimizerInterface

} // namespace Plato

#endif /* PLATO_OPTIMIZERINTERFACE_HPP_ */
