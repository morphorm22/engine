/*
 * Plato_DriverInterface.hpp
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

#ifndef PLATO_DRIVERINTERFACE_HPP_
#define PLATO_DRIVERINTERFACE_HPP_

#include "Plato_Interface.hpp"

namespace Plato
{

enum class driver_t
{
    PLATO_OPTIMIZER_DRIVER = 1,
    PLATO_DAKOTA_DRIVER = 2,
}; 

/******************************************************************************//**
 * @brief Abstract interface to driver algorithm
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
class DriverInterface
{
public:
    virtual ~DriverInterface()
    {
    }

    /******************************************************************************//**
     * @brief Interface to driver algorithm
    **********************************************************************************/
    virtual void run() = 0;

    /******************************************************************************//**
     * @brief Initialize the driver
    **********************************************************************************/
    virtual void initialize() = 0;

    /******************************************************************************//**
     * @brief Finalize the driver
    **********************************************************************************/
    virtual void finalize() = 0;

    /******************************************************************************//**
     * @brief Return true if the last driver
    **********************************************************************************/
    virtual bool lastDriver() const = 0;

    /******************************************************************************//**
     * @brief Return the driver type
     * \return driver type
    **********************************************************************************/
    virtual Plato::driver_t driver() const = 0;
};
// class DriverInterface

} // namespace Plato

#endif /* PLATO_DRIVERINTERFACE_HPP_ */
