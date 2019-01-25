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

/*
 * Plato_GradFreeEngineObjective.hpp
 *
 *  Created on: Jan 24, 2019
*/

#pragma once

#include <vector>
#include <memory>

#include "Plato_Vector.hpp"
#include "Plato_Interface.hpp"
#include "Plato_MultiVector.hpp"
#include "Plato_GradFreeCriterion.hpp"
#include "Plato_OptimizerEngineStageData.hpp"

namespace Plato
{

/******************************************************************************//**
 * @brief Interface for gradient free engine objective
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
class GradFreeEngineObjective : public Plato::GradFreeCriterion<ScalarType, OrdinalType>
{
public:
    /******************************************************************************//**
     * @brief Constructor
     * @param [in] aDataFactory data factory
     * @param [in] aInputData data structure with engine's options and input keywords
     * @param [in] aInterface interface to data motion coordinator
    **********************************************************************************/
    explicit GradFreeEngineObjective(const Plato::DataFactory<ScalarType, OrdinalType> & aDataFactory,
                                     const Plato::OptimizerEngineStageData & aInputData,
                                     Plato::Interface* aInterface = nullptr) :
            mParticles(),
            mObjFuncValues(),
            mInterface(aInterface),
            mEngineInputData(aInputData),
            mParameterList(std::make_shared<Teuchos::ParameterList>())
    {
        this->initialize(aDataFactory);
    }

    /******************************************************************************//**
     * @brief Destructor
    **********************************************************************************/
    virtual ~GradFreeEngineObjective()
    {
    }

    /******************************************************************************//**
     * @brief Set interface to data motion coordinator
     * @param [in] aInterface interface to data motion coordinator
    **********************************************************************************/
    void set(Plato::Interface* aInterface)
    {
        assert(aInterface != nullptr);
        mInterface = aInterface;
    }

    /******************************************************************************//**
     * @brief Evaluates generic objective function through the Plato Engine
     * @param [in] aControl set of particles, each particle denotes a set of optimization variables
     * @param [out] aOutput criterion value for each particle
     **********************************************************************************/
    void value(const Plato::MultiVector<ScalarType, OrdinalType> & aControl,
               Plato::Vector<ScalarType, OrdinalType> & aOutput)
    {
        assert(mInterface != nullptr);
        this->exportParticlesSharedData(aControl);
        this->exportObjFuncSharedData(aOutput);
        this->compute();
        this->importObjFuncSharedData(aOutput);
    }

private:
    /******************************************************************************//**
     * @brief Allocate class member containers
    **********************************************************************************/
    void initialize(const Plato::DataFactory<ScalarType, OrdinalType> & aDataFactory)
    {
        const OrdinalType tNumControls = aDataFactory.getNumControls();
        const OrdinalType tNumParticles = aDataFactory.getNumCriterionValues();
        mParticles.resize(tNumParticles, std::vector<ScalarType>(tNumControls));
        const OrdinalType tNumObjFuncVals = 1;
        mObjFuncValues.resize(tNumParticles, std::vector<ScalarType>(tNumObjFuncVals));
    }

    /******************************************************************************//**
     * @brief Compute gradient free objective function
    **********************************************************************************/
    void compute()
    {
        std::string tCriterionStageName = mEngineInputData.getObjectiveValueStageName();
        assert(tCriterionStageName.empty() == false);
        std::vector<std::string> tStageNames;
        tStageNames.push_back(tCriterionStageName);
        mInterface->compute(tStageNames, *mParameterList);
    }

    /******************************************************************************//**
     * @brief Export particles to applications (e.g. simulation software)
     * @param [in] aControl 2D container of optimization variables
    **********************************************************************************/
    void exportParticlesSharedData(const Plato::MultiVector<ScalarType, OrdinalType> & aControl)
    {
        std::string tCriterionStageName = mEngineInputData.getObjectiveValueStageName();
        Plato::Stage* tObjFuncStage = mInterface->getStage(tCriterionStageName);
        std::vector<std::string> tInputDataNames = tObjFuncStage->getInputDataNames();

        const OrdinalType tNumParticles = aControl.getNumVectors();
        for(OrdinalType tParticleIndex = 0; tParticleIndex < tNumParticles; tParticleIndex++)
        {
            const Plato::Vector<ScalarType> & tMyControls = aControl[tParticleIndex];
            assert(tMyControls.size() == mParticles[tParticleIndex].size());

            const OrdinalType tNumControls = tMyControls.size();
            for(OrdinalType tControlIndex = 0; tControlIndex < tNumControls; tControlIndex++)
            {
                mParticles[tParticleIndex][tControlIndex] = tMyControls[tControlIndex];
            }

            std::string tControlSharedDataName = tInputDataNames[tParticleIndex];
            mParameterList->set(tControlSharedDataName, mParticles[tParticleIndex].data());
        }
    }

    /******************************************************************************//**
     * @brief Export objective function values to application (e.g. simulation software)
     * @param [in] aNumParticles number of particles
    **********************************************************************************/
    void exportObjFuncSharedData(const OrdinalType & aNumParticles)
    {
        std::string tCriterionStageName = mEngineInputData.getObjectiveValueStageName();
        Plato::Stage* tObjFuncStage = mInterface->getStage(tCriterionStageName);
        std::vector<std::string> tOutputDataNames = tObjFuncStage->getOutputDataNames();

        for(OrdinalType tParticleIndex = 0; tParticleIndex < aNumParticles; tParticleIndex++)
        {
            assert(aNumParticles == mObjFuncValues.size());
            std::fill(mObjFuncValues[tParticleIndex].begin(), mObjFuncValues[tParticleIndex].end(), 0.0);
            std::string tObjFuncSharedDataName = tOutputDataNames[tParticleIndex];
            mParameterList->set(tObjFuncSharedDataName, mObjFuncValues[tParticleIndex].data());
        }
    }

    /******************************************************************************//**
     * @brief Import objective function values from application (e.g. simulation software)
     * @param [in] aOutput 1D container of objective function values
    **********************************************************************************/
    void importObjFuncSharedData(Plato::Vector<ScalarType, OrdinalType> & aOutput)
    {
        const OrdinalType tOBJ_FUNC_VAL_INDEX = 0;
        const OrdinalType tNumParticles = aOutput.size();
        for(OrdinalType tParticleIndex = 0; tParticleIndex < tNumParticles; tParticleIndex++)
        {
            assert(aOutput.size() == mObjFuncValues.size());
            aOutput[tParticleIndex] = mObjFuncValues[tParticleIndex][tOBJ_FUNC_VAL_INDEX];
        }
    }

private:
    std::vector<std::vector<ScalarType>> mParticles; /*!< set of particles */
    std::vector<std::vector<ScalarType>> mObjFuncValues; /*!< set of objective function values */

    Plato::Interface* mInterface; /*!< interface to data motion coordinator */
    Plato::OptimizerEngineStageData mEngineInputData; /*!< holds Plato Engine's options and inputs */
    std::shared_ptr<Teuchos::ParameterList> mParameterList; /*!< Plato Engine parameter list */

private:
    GradFreeEngineObjective(const Plato::GradFreeEngineObjective<ScalarType, OrdinalType>&);
    Plato::GradFreeEngineObjective<ScalarType, OrdinalType> & operator=(const Plato::GradFreeEngineObjective<ScalarType, OrdinalType>&);
};
// class GradFreeEngineObjective

} // namespace Plato
