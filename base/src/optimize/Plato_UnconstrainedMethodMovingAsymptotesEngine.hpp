/*
 * Plato_UnconstrainedMethodMovingAsymptotesEngine.hpp
 *
 *  Created on: Apr 30, 2023
 */

#pragma once

#include "Plato_Interface.hpp"
#include "Plato_AlgebraFactory.hpp"
#include "Plato_EngineObjective.hpp"
#include "Plato_OptimizerInterface.hpp"
#include "Plato_OptimizerUtilities.hpp"
#include "Plato_UnconstrainedMethodMovingAsymptotes.hpp"
#include "Plato_UnconstrainedMethodMovingAsymptotesParser.hpp"

namespace Plato
{

/******************************************************************************//**
 * \tparam ScalarType  data type for non-integer values
 * \tparam OrdinalType data type for integer values (default: std::size_t)
 * 
 * \brief Engine interface for the Unconstrained Method of Moving AsymptotesEngine 
 *        (UMMA) algorithm.
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
class UnconstrainedMethodMovingAsymptotesEngine : public Plato::OptimizerInterface<ScalarType, OrdinalType>
{
private:
    std::string mObjFuncStageName; /*!< name identifier for objective function stage */
    Plato::StageInputDataMng mObjFuncStageDataMng; /*!< objective function stage data manager */  
    
public:
    /******************************************************************************//**
     * @brief Constructor
     * @param [in] aInterface const pointer to Multiple Program, Multiple Data (MPMD) 
     *                        framework
     * @param [in] aComm      const reference to MPI communicator
    **********************************************************************************/
    explicit UnconstrainedMethodMovingAsymptotesEngine(Plato::Interface *aInterface, const MPI_Comm &aComm) :
        Plato::OptimizerInterface<ScalarType, OrdinalType>::OptimizerInterface(aInterface, aComm)    
    {}

    /******************************************************************************//**
     * \brief Destructor
    **********************************************************************************/
    ~UnconstrainedMethodMovingAsymptotesEngine()
    {}
    
    /******************************************************************************//**
     * \brief Return algorithm type
     * \return algorithm type
    **********************************************************************************/
    Plato::optimizer::algorithm_t algorithm() const
    {
        return (Plato::optimizer::UNCONSTRAINED_METHOD_OF_MOVING_ASYMPTOTES);
    }

    /******************************************************************************//**
     * \brief Solve optimization problem using the UMMA algorithm
    **********************************************************************************/ 
    void run()
    {
        // initialize engine data for optimization problem
        this->initialize();
        // parse input data for optimization algorithm
        Plato::AlgorithmInputsUMMA<ScalarType,OrdinalType> tInputs;
        this->parseInputs(tInputs);
        // parse inputs for objective function evaluation stage
        this->parseObjFuncStageInputs();
        // allocate containers and linear algebra operations
        Plato::AlgebraFactory<ScalarType,OrdinalType> tLinearAlgebraFactory;
        this->allocateContainers(tLinearAlgebraFactory,tInputs);
        this->allocateReductionOperations(tLinearAlgebraFactory,tInputs);
        // set initial guess
        this->setInitialGuess(tInputs);
        // set upper and lower bounds 
        this->setUpperBounds(tInputs);
        this->setLowerBounds(tInputs);
        // create objective function interface
        std::shared_ptr<Plato::Criterion<ScalarType,OrdinalType>> 
        tObjective = this->objective(tInputs);
        // solve optimization problem
        Plato::AlgorithmOutputsUMMA<ScalarType,OrdinalType> tOutputs;
        Plato::solve_unconstrained_method_moving_asymptotes(tObjective,tInputs,tOutputs);
    }

    /******************************************************************************//**
     * @brief Optimization problem has been solved, delete allocated memory. Called 
     *        once by the MPMD interface.
    **********************************************************************************/
    void finalize()
    {
        this->mInterface->finalize(this->mInputData.getFinalizationStageName());
    }

private:
    /******************************************************************************//**
     * \brief Create objective function interface
     * \param [out] aInputs reference to data structure holding UMMA inputs
     * \return standard shared pointer 
    **********************************************************************************/
    std::shared_ptr<Plato::Criterion<ScalarType,OrdinalType>> 
    objective(const Plato::AlgorithmInputsUMMA<ScalarType, OrdinalType> & aInput)
    {
        Plato::DataFactory<ScalarType,OrdinalType> tDataFactory;
        tDataFactory.allocateControl(aInput.mUpperBounds.operator*());
        std::shared_ptr<Plato::Criterion<ScalarType,OrdinalType>> tObjective = 
            std::make_shared<Plato::EngineObjective<ScalarType,OrdinalType>>(
                tDataFactory,this->mInputData,this->mInterface,this);
        return tObjective;
    }

    /******************************************************************************//**
     * \brief Parse inputs for UMMA algorithm
     * \param [out] aInputs reference to data structure holding UMMA inputs
    **********************************************************************************/
    void parseInputs(Plato::AlgorithmInputsUMMA<ScalarType, OrdinalType> &aInput)
    {
        auto tOptimizerNode = this->getOptimizerNode(this->mInterface);
        Plato::UnconstrainedMethodMovingAsymptotesParser<ScalarType,OrdinalType> tParser;
        tParser.parse(tOptimizerNode, aInput);
        mObjFuncStageName = tParser.getObjectiveStageName(tOptimizerNode);
    }

    /******************************************************************************//**
     * \brief Parse inputs for objective function stage
    **********************************************************************************/
    void parseObjFuncStageInputs()
    {
        auto tInputData = this->mInterface->getInputData();
        auto tStages = tInputData.template getByName<Plato::InputData>("Stage");
        for(auto tStageNode = tStages.begin(); tStageNode != tStages.end(); ++tStageNode)
        {
            std::string tStageName = tStageNode->template get<std::string>("Name");
            if(tStageName == mObjFuncStageName)
            {
                Plato::Parse::parseStageData(*tStageNode, mObjFuncStageDataMng);
            }
        }
    }

    /******************************************************************************//**
     * \brief Allocate interface to parallel linear algebra reduction operations
     * \param [in]  aFactory const reference to linear algebra factory
     * \param [out] aData    reference to data structure holding UMMA inputs
    **********************************************************************************/
    void allocateReductionOperations
    (const Plato::AlgebraFactory<ScalarType, OrdinalType>      &aFactory,
           Plato::AlgorithmInputsUMMA<ScalarType, OrdinalType> &aData)
    {
        aData.mComm = Plato::CommWrapper(this->mComm);
        aData.mReductionOperations = aFactory.createReduction(this->mComm, this->mInterface);
    }

    /******************************************************************************//**
     * \brief Create templates for control containers
     * \param [in]  aFactory const reference to linear algebra factory
     * \param [out] aData    reference to data structure holding UMMA inputs
    **********************************************************************************/
    void allocateContainers
    (const Plato::AlgebraFactory<ScalarType, OrdinalType>      &aFactory,
           Plato::AlgorithmInputsUMMA<ScalarType, OrdinalType> &aData)
    {
        Plato::StandardMultiVector<ScalarType, OrdinalType> tControlTemplate;
        for(OrdinalType tVectorIndex = 0; tVectorIndex < aData.mNumControlVectors; tVectorIndex++)
        {
            const std::string& tMySharedDataName = mObjFuncStageDataMng.getInput(mObjFuncStageName, tVectorIndex);
            const OrdinalType tNumControls = this->mInterface->size(tMySharedDataName);
            std::shared_ptr<Plato::Vector<ScalarType, OrdinalType>> tVector =
                aFactory.createVector(this->mComm, tNumControls, this->mInterface);
            tControlTemplate.add(tVector);
        }
        aData.mLowerBounds  = tControlTemplate.create();
        aData.mUpperBounds  = tControlTemplate.create();
        aData.mInitialGuess = tControlTemplate.create();
    }

    /******************************************************************************//**
     * \brief Set lower bounds for control variables
     * \param [out] aData reference to data structure holding UMMA inputs
    **********************************************************************************/
    void setLowerBounds(Plato::AlgorithmInputsUMMA<ScalarType, OrdinalType> &aData)
    {
        const OrdinalType tNumControls = aData.mLowerBounds->operator[](0 /*vector index*/).size();
        std::vector<ScalarType> tValues(tNumControls);
        Plato::getLowerBoundsInputData(this->mInputData,this->mInterface,tValues);
        for(OrdinalType tVecIndex = 0; tVecIndex < aData.mNumControlVectors; tVecIndex++)
        {
            Plato::copy(tValues,aData.mLowerBounds->operator[](tVecIndex));
        }
    }

    /******************************************************************************//**
     * \brief Set upper bounds for control variables
     * \param [out] aData reference to data structure holding UMMA inputs
    **********************************************************************************/
    void setUpperBounds(Plato::AlgorithmInputsUMMA<ScalarType, OrdinalType> &aData)
    {
        const OrdinalType tNumControls = aData.mUpperBounds->operator[](0 /*vector index*/).size();
        std::vector<ScalarType> tValues(tNumControls);
        Plato::getUpperBoundsInputData(this->mInputData,this->mInterface,tValues);
        for(OrdinalType tVecIndex = 0; tVecIndex < aData.mNumControlVectors; tVecIndex++)
        {
            Plato::copy(tValues,aData.mUpperBounds->operator[](tVecIndex));
        }
    }

    /******************************************************************************//**
     * \brief Set initial initial guess for control variables
     * \param [out] aData reference to data structure holding UMMA inputs
    **********************************************************************************/
    void setInitialGuess(Plato::AlgorithmInputsUMMA<ScalarType, OrdinalType> &aData)
    {
        const OrdinalType tNumControls = (*aData.mInitialGuess)[0 /* vector index */].size();
        std::vector<ScalarType> tData(tNumControls);
        for(OrdinalType tVecIndex = 0; tVecIndex < aData.mNumControlVectors; tVecIndex++)
        {
            const std::string & tMySharedDataName = mObjFuncStageDataMng.getInput(mObjFuncStageName, tVecIndex);
            Plato::getInitialGuessInputData(tMySharedDataName, this->mInputData, this->mInterface, tData);
            Plato::copy(tData, (*aData.mInitialGuess)[tVecIndex]);
            std::fill(tData.begin(), tData.end(), static_cast<ScalarType>(0));
        }
    }
};
// class UnconstrainedMethodMovingAsymptotesEngine

} 
// namespace Plato