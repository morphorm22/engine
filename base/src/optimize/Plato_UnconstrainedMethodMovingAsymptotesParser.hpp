/*
 * Plato_UnconstrainedMethodMovingAsymptotesParser.hpp
 *
 *  Created on: Apr 30, 2023
 */

#pragma once

#include "Plato_OptimizerParser.hpp"
#include "Plato_OptimizersIO_Utilities.hpp"
#include "Plato_UnconstrainedMethodMovingAsymptotesDataIO.hpp"
#include "Plato_UnconstrainedMethodMovingAsymptotesFileIO.hpp"

namespace Plato
{

/******************************************************************************//**
 * @tparam ScalarType  data type for non-integer values
 * @tparam OrdinalType data type for integer values (default: std::size_t)
 * 
 * @brief Parses input options for the Unconstrained Method of Moving Asymptotes 
 *        (UMMA) algorithm
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
class UnconstrainedMethodMovingAsymptotesParser : public Plato::OptimizerParser<ScalarType, OrdinalType>
{
public:
    /******************************************************************************//**
     * @brief Constructor
    **********************************************************************************/
    UnconstrainedMethodMovingAsymptotesParser()
    {}

    /******************************************************************************//**
     * @brief Destructor
    **********************************************************************************/
    ~UnconstrainedMethodMovingAsymptotesParser()
    {}

    /******************************************************************************//**
     * @brief Parse input options for the UMMA algorithm
     * @param [in]  aNode containes input data read from xml files
     * @param [out] aData data structure with MMA algorithmic options
    **********************************************************************************/
    void parse(
        const Plato::InputData                                    &aNode, 
              Plato::AlgorithmInputsUMMA<ScalarType, OrdinalType> &aData
    )
    {
        this->read(aNode, aData);
    }

private:
    /******************************************************************************//**
     * @brief Read inputs from xml node and store data in UMMA input data registry
     * @param [in]  aNode xml node with data read from xml files
     * @param [out] aData input data registry for UMMA algorithm
    **********************************************************************************/
    void read(const Plato::InputData                                    &aNode, 
                    Plato::AlgorithmInputsUMMA<ScalarType, OrdinalType> &aData)
    {
        if(aNode.size<Plato::InputData>("Options"))
        {
            Plato::InputData tOptionsNode = aNode.get<Plato::InputData>("Options");

            aData.mMemorySpace = this->memorySpace(tOptionsNode);
            aData.mWriteDiagnostics = this->outputDiagnostics(tOptionsNode);

            aData.mMoveLimit = this->moveLimit(tOptionsNode);
            aData.mNumControlVectors = this->numControlVectors(tOptionsNode);
            aData.mMaxNumOuterIterations = this->maxNumOuterIterations(tOptionsNode);
            aData.mControlChangeTolerance = this->controlStagnationTolerance(tOptionsNode);
            aData.mObjectiveChangeTolerance = this->objectiveStagnationTolerance(tOptionsNode);
            aData.mMaxNumSubProblemIterations = this->maxNumSubProblemIter(tOptionsNode);
            aData.mAsymptotesIncrementConstant = this->asymptoteExpansion(tOptionsNode);
            aData.mAsymptotesDecrementConstant = this->asymptoteContraction(tOptionsNode);
            aData.mAsymptotesInitialMultiplier = this->initialAymptoteScaling(tOptionsNode);
        }
    }

    /******************************************************************************//**
     * @brief Parse output diagnostics keyword
     * @param  [in] aNode xml node with data read from xml files
     * @return boolean 
    **********************************************************************************/
    bool outputDiagnostics(const Plato::InputData & aOptionsNode)
    {
        bool tOuput = true;
        if(aOptionsNode.size<std::string>("OutputDiagnosticsToFile"))
        {
            tOuput = Plato::Get::Bool(aOptionsNode, "OutputDiagnosticsToFile");
        }
        return (tOuput);
    }

    /******************************************************************************//**
     * @brief Parse memory space keyword, default is set to HOST memory space
     * @param  [in] aNode xml node with data read from xml files
     * @return memory space
    **********************************************************************************/
    Plato::MemorySpace::type_t memorySpace(const Plato::InputData & aNode)
    {
        std::string tInput("HOST");
        if(aNode.size<std::string>("MemorySpace"))
        {
            tInput = Plato::Get::String(aNode, "MemorySpace", true);
        }
        Plato::MemorySpace::type_t tOutput = Plato::get_memory_space(tInput);
        return (tOutput);
    }

    /******************************************************************************//**
     * @brief Parse number of control vectors, default is set to 1
     * @param  [in] aNode xml node with data read from xml files
     * @return integer 
    **********************************************************************************/
    OrdinalType numControlVectors(const Plato::InputData & aOptionsNode)
    {
        OrdinalType tOutput = 1;
        if(aOptionsNode.size<std::string>("NumControlVectors"))
        {
            tOutput = Plato::Get::Int(aOptionsNode, "NumControlVectors");
        }
        return (tOutput);
    }

    /******************************************************************************//**
     * @brief Parse maximum number of subproblem iterations, default is set to 5
     * @param [in] aNode xml node with data read from xml files
     * @return integer
    **********************************************************************************/
    OrdinalType maxNumSubProblemIter(const Plato::InputData & aOptionsNode)
    {
        OrdinalType tOutput = 5;
        if(aOptionsNode.size<std::string>("MaxNumSubProblemIter"))
        {
            tOutput = Plato::Get::Int(aOptionsNode, "MaxNumSubProblemIter");
        }
        return (tOutput);
    }

    /******************************************************************************//**
     * @brief Parse maximum number of outer iterations, default is set to 50
     * @param  [in] aNode xml node with data read from xml files
     * @return integer
    **********************************************************************************/
    OrdinalType maxNumOuterIterations(const Plato::InputData & aOptionsNode)
    {
        OrdinalType tOutput = 50;
        if(aOptionsNode.size<std::string>("MaxNumOuterIterations"))
        {
            tOutput = Plato::Get::Int(aOptionsNode, "MaxNumOuterIterations");
        }
        return (tOutput);
    }

    /******************************************************************************//**
     * @brief Parse move limit, default is set to 0.15
     * @param  [in] aNode xml node with data read from xml files
     * @return maximum move limit, default = 500
    **********************************************************************************/
    ScalarType moveLimit(const Plato::InputData & aOptionsNode)
    {
        ScalarType tOutput = 0.15;
        if(aOptionsNode.size<std::string>("MoveLimit"))
        {
            tOutput = Plato::Get::Double(aOptionsNode, "MoveLimit");
        }
        return (tOutput);
    }

    /******************************************************************************//**
     * @brief Parse asymptotes expansion multiplier, default is set to 1.2
     * @param  [in] aNode xml node with data read from xml files
     * @return scalar
    **********************************************************************************/
    ScalarType asymptoteExpansion(const Plato::InputData & aOptionsNode)
    {
        ScalarType tOutput = 1.2;
        if(aOptionsNode.size<std::string>("AsymptoteExpansion"))
        {
            tOutput = Plato::Get::Double(aOptionsNode, "AsymptoteExpansion");
        }
        return (tOutput);
    }

    /******************************************************************************//**
     * @brief Parse asymptotes contraction multiplier, default is set to 0.7
     * @param  [in] aNode xml node with data read from xml files
     * @return scalar
    **********************************************************************************/
    ScalarType asymptoteContraction(const Plato::InputData & aOptionsNode)
    {
        ScalarType tOutput = 0.7;
        if(aOptionsNode.size<std::string>("AsymptoteContraction"))
        {
            tOutput = Plato::Get::Double(aOptionsNode, "AsymptoteContraction");
        }
        return (tOutput);
    }

    /******************************************************************************//**
     * @brief Parse initial asymptotes multiplier, default is set to 0.2
     * @param  [in] aNode xml node with data read from xml files
     * @return scalar
    **********************************************************************************/
    ScalarType initialAymptoteScaling(const Plato::InputData & aOptionsNode)
    {
        ScalarType tOutput = 0.2;
        if(aOptionsNode.size<std::string>("InitialAymptoteScaling"))
        {
            tOutput = Plato::Get::Double(aOptionsNode, "InitialAymptoteScaling");
        }
        return (tOutput);
    }

    /******************************************************************************//**
     * @brief Parse control change, i.e., stagnation, tolerance, default is set to 1e-3
     * @param  [in] aNode xml node with data read from xml files
     * @return scalar
    **********************************************************************************/
    ScalarType controlStagnationTolerance(const Plato::InputData & aOptionsNode)
    {
        ScalarType tOutput = 1e-3;
        if(aOptionsNode.size<std::string>("ControlStagnationTolerance"))
        {
            tOutput = Plato::Get::Double(aOptionsNode, "ControlStagnationTolerance");
        }
        return (tOutput);
    }

    /******************************************************************************//**
     * @brief Parse objective change , i.e, stagnation, tolerance, default is set to 1e-10
     * @param  [in] aNode xml node with data read from xml files
     * @return scalar
    **********************************************************************************/
    ScalarType objectiveStagnationTolerance(const Plato::InputData & aOptionsNode)
    {
        ScalarType tOutput = 1e-10;
        if(aOptionsNode.size<std::string>("ObjectiveStagnationTolerance"))
        {
            tOutput = Plato::Get::Double(aOptionsNode, "ObjectiveStagnationTolerance");
        }
        return (tOutput);
    }

private:
    UnconstrainedMethodMovingAsymptotesParser(
        const Plato::UnconstrainedMethodMovingAsymptotesParser<ScalarType, OrdinalType>&
    );
    Plato::UnconstrainedMethodMovingAsymptotesParser<ScalarType, OrdinalType> &operator=(
        const Plato::UnconstrainedMethodMovingAsymptotesParser<ScalarType, OrdinalType>&
    );
};
// class UnconstrainedMethodMovingAsymptotesParser

}
// namespace Plato 