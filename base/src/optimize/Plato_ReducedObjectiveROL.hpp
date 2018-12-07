/*
 * Plato_ReducedObjectiveROL.hpp
 *
 *  Created on: Feb 8, 2018
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

#ifndef PLATO_REDUCEDOBJECTIVEROL_HPP_
#define PLATO_REDUCEDOBJECTIVEROL_HPP_

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <algorithm>

#include "ROL_Objective.hpp"

#include "Plato_Interface.hpp"
#include "Plato_DistributedVectorROL.hpp"
#include "Plato_OptimizerEngineStageData.hpp"

namespace Plato
{

template<typename ScalarType>
class ReducedObjectiveROL : public ROL::Objective<ScalarType>
{
public:
    explicit ReducedObjectiveROL(const Plato::OptimizerEngineStageData & aInputData, Plato::Interface* aInterface = nullptr) :
            mControl(),
            mGradient(),
            mInterface(aInterface),
            mEngineInputData(aInputData),
            mParameterList(std::make_shared<Teuchos::ParameterList>())
    {
        this->initialize();
    }
    virtual ~ReducedObjectiveROL()
    {
    }

    /********************************************************************************/
    void update(const ROL::Vector<ScalarType> & aControl, bool aFlag, int aIteration = -1)
    /********************************************************************************/
    {
        // TODO: UNDERSTAND HOW TO CACHE STATE AND ADJOINT DATA WITH ROL. I THINK THE OBJECTIVE FUNCTION EVALUATION IS OUT OF SYNC INTERNALLY.
/*        std::vector<std::string> tStageNames;
        std::string tOutputStageName = mEngineStageData.getOutputStageName();
        tStageNames.push_back(tOutputStageName);
        mInterface->compute(tStageNames, *mParameterList);*/

        assert(mInterface != nullptr);
        std::vector<std::string> tStageNames;
        std::string tOutputStageName = mEngineInputData.getOutputStageName();
        tStageNames.push_back(tOutputStageName);
        mInterface->compute(tStageNames, *mParameterList);
    }
    /********************************************************************************/
    ScalarType value(const ROL::Vector<ScalarType> & aControl, ScalarType & aTolerance)
    /********************************************************************************/
    {
        // ********* Set view to control vector ********* //
        const Plato::DistributedVectorROL<ScalarType> & tControl =
                dynamic_cast<const Plato::DistributedVectorROL<ScalarType>&>(aControl);
        const std::vector<ScalarType> & tControlData = tControl.vector();
        assert(tControlData.size() == mControl.size());
        const size_t tCONTROL_VECTOR_INDEX = 0;
        std::string tControlName = mEngineInputData.getControlName(tCONTROL_VECTOR_INDEX);
        this->copy(tControlData, mControl);
        mParameterList->set(tControlName, mControl.data());

        // ********* Set view to objective function value ********* //
        ScalarType tObjectiveValue = 0;
        std::string tObjectiveValueName = mEngineInputData.getObjectiveValueOutputName();
        mParameterList->set(tObjectiveValueName, &tObjectiveValue);

        // ********* Compute objective function value ********* //
        std::vector<std::string> tStageNames;
        tStageNames.push_back(tObjectiveValueName);
        mInterface->compute(tStageNames, *mParameterList);

        return (tObjectiveValue);
    }
    /********************************************************************************/
    void gradient(ROL::Vector<ScalarType> & aGradient, const ROL::Vector<ScalarType> & aControl, ScalarType & aTolerance)
    /********************************************************************************/
    {
        assert(aControl.dimension() == aGradient.dimension());

        // ********* Set view to control vector ********* //
        const Plato::DistributedVectorROL<ScalarType> & tControl =
                dynamic_cast<const Plato::DistributedVectorROL<ScalarType>&>(aControl);
        const std::vector<ScalarType> & tControlData = tControl.vector();
        assert(tControlData.size() == mControl.size());
        const size_t tCONTROL_VECTOR_INDEX = 0;
        std::string tControlName = mEngineInputData.getControlName(tCONTROL_VECTOR_INDEX);
        this->copy(tControlData, mControl);
        mParameterList->set(tControlName, mControl.data());

        // ********* Set view to gradient vector ********* //
        std::string tObjectiveGradientName = mEngineInputData.getObjectiveGradientOutputName();
        std::fill(mGradient.begin(), mGradient.end(), static_cast<ScalarType>(0));
        mParameterList->set(tObjectiveGradientName, mGradient.data());

        // ********* Compute gradient vector ********* //
        std::vector<std::string> tStageNames;
        tStageNames.push_back(tObjectiveGradientName);
        mInterface->compute(tStageNames, *mParameterList);

        // ********* Set output gradient vector ********* //
        Plato::DistributedVectorROL<ScalarType> & tOutputGradient =
                dynamic_cast<Plato::DistributedVectorROL<ScalarType>&>(aGradient);
        std::vector<ScalarType> & tOutputGradientData = tOutputGradient.vector();
        this->copy(mGradient, tOutputGradientData);
    }
    /********************************************************************************/
    void hessVec(ROL::Vector<ScalarType> & aOutput,
                 const ROL::Vector<ScalarType> & aVector,
                 const ROL::Vector<ScalarType> & aControl,
                 ScalarType & aTolerance)
    /********************************************************************************/
    {
        assert(aVector.dimension() == aOutput.dimension());
        assert(aControl.dimension() == aOutput.dimension());
        aOutput.set(aVector);
    }

private:
    /********************************************************************************/
    void initialize()
    /********************************************************************************/
    {
        const size_t tCONTROL_INDEX = 0;
        std::vector<std::string> tControlNames = mEngineInputData.getControlNames();
        std::string tMyControlName = tControlNames[tCONTROL_INDEX];
        const size_t tNumDesignVariables = mInterface->size(tMyControlName);
        assert(tNumDesignVariables >= static_cast<ScalarType>(0));
        mControl.resize(tNumDesignVariables);
        mGradient.resize(tNumDesignVariables);
    }
    /********************************************************************************/
    void copy(const std::vector<ScalarType> & aFrom, std::vector<ScalarType> & aTo)
    /********************************************************************************/
    {
        const size_t tVectorIndex = 0;
        assert(aTo.size() == aFrom.size());
        for(size_t tIndex = 0; tIndex < aFrom.size(); tIndex++)
        {
            aTo[tIndex] = aFrom[tIndex];
        }
    }

private:
    std::vector<ScalarType> mControl;
    std::vector<ScalarType> mGradient;

    Plato::Interface* mInterface;
    Plato::OptimizerEngineStageData mEngineInputData;
    std::shared_ptr<Teuchos::ParameterList> mParameterList;

private:
    ReducedObjectiveROL(const Plato::ReducedObjectiveROL<ScalarType> & aRhs);
    Plato::ReducedObjectiveROL<ScalarType> & operator=(const Plato::ReducedObjectiveROL<ScalarType> & aRhs);
};

} // namespace Plato

#endif /* PLATO_REDUCEDOBJECTIVEROL_HPP_ */
