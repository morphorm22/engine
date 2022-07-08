/*
 * Plato_ROLAugmentedLagrangianInterface.hpp
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

#ifndef PLATO_ROLAUGMENTEDLAGRANGIANINTERFACE_HPP_
#define PLATO_ROLAUGMENTEDLAGRANGIANINTERFACE_HPP_

#include <mpi.h>

#include <string>
#include <memory>
#include <sstream>
#include <cassert>
#include <fstream>

#include "Teuchos_XMLParameterListHelpers.hpp"

#include "ROL_Bounds.hpp"
#include "ROL_Solver.hpp"

#include "Plato_Parser.hpp"
#include "Plato_Interface.hpp"
#include "Plato_OptimizerInterface.hpp"
#include "Plato_SerialVectorROL.hpp"
#include "Plato_OptimizerUtilities.hpp"
#include "Plato_ReducedObjectiveROL.hpp"
#include "Plato_ReducedConstraintROL.hpp"
#include "Plato_DistributedVectorROL.hpp"
#include "Plato_OptimizerEngineStageData.hpp"

namespace Plato
{

template<typename ScalarType, typename OrdinalType = size_t>
class ROLAugmentedLagrangianInterface : public Plato::OptimizerInterface<ScalarType, OrdinalType>
{
public:
    /******************************************************************************/
    ROLAugmentedLagrangianInterface(Plato::Interface* aInterface, const MPI_Comm & aComm) :
        Plato::OptimizerInterface<ScalarType, OrdinalType>::OptimizerInterface(aInterface, aComm) { }

    virtual ~ROLAugmentedLagrangianInterface() = default;

    /******************************************************************************/
    Plato::optimizer::algorithm_t algorithm() const
    /******************************************************************************/
    {
        return (Plato::optimizer::algorithm_t::ROL_AUGMENTED_LAGRANGIAN);
    }

    /******************************************************************************/
    void initialize()
    /******************************************************************************/
    {
        Plato::initialize<ScalarType, OrdinalType>(this->mInterface, this->mInputData,
                                                   this->mOptimizerIndex);
    }

    /******************************************************************************/
    void run()
    /******************************************************************************/
    {
        this->initialize();

        /************************************ SET CONTROL BOUNDS ************************************/
        const OrdinalType tCONTROL_VECTOR_INDEX = 0;
        std::string tControlName = this->mInputData.getControlName(tCONTROL_VECTOR_INDEX);
        const OrdinalType tNumControls = this->mInterface->size(tControlName);
        std::vector<ScalarType> tInputBoundsData(tNumControls);

        // ********* GET LOWER BOUNDS INFORMATION *********
        Plato::getLowerBoundsInputData(this->mInputData, this->mInterface, tInputBoundsData);

        // ********* SET LOWER BOUNDS FOR OPTIMIZER *********
        Teuchos::RCP<Plato::DistributedVectorROL<ScalarType>> tControlLowerBounds =
                Teuchos::rcp(new Plato::DistributedVectorROL<ScalarType>(this->mComm, tNumControls));
        this->setBounds(tInputBoundsData, tControlLowerBounds.operator*());

        // ********* GET UPPER BOUNDS INFORMATION *********
        Plato::getUpperBoundsInputData(this->mInputData, this->mInterface, tInputBoundsData);

        // ********* SET UPPER BOUNDS FOR OPTIMIZER *********
        Teuchos::RCP<Plato::DistributedVectorROL<ScalarType>> tControlUpperBounds =
                Teuchos::rcp(new Plato::DistributedVectorROL<ScalarType>(this->mComm, tNumControls));
        this->setBounds(tInputBoundsData, tControlUpperBounds.operator*());

        // ********* CREATE BOUND CONSTRAINT FOR OPTIMIZER *********
        Teuchos::RCP<ROL::BoundConstraint<ScalarType>> tControlBoundsMng =
                Teuchos::rcp(new ROL::Bounds<ScalarType>(tControlLowerBounds, tControlUpperBounds));

        /******************************** SET CONTROL INITIAL GUESS *********************************/
        Teuchos::RCP<Plato::DistributedVectorROL<ScalarType>> tControls =
                Teuchos::rcp(new Plato::DistributedVectorROL<ScalarType>(this->mComm, tNumControls));
        this->setInitialGuess(tControlName, tControls.operator*());

        /****************************** SET DUAL VECTOR *******************************/
        const OrdinalType tNumConstraints = this->mInputData.getNumConstraints();
        Teuchos::RCP<Plato::SerialVectorROL<ScalarType>> tDual = Teuchos::rcp(new Plato::SerialVectorROL<ScalarType>(tNumConstraints));

        /********************************* SET OPTIMIZATION PROBLEM *********************************/
        Teuchos::RCP<ROL::Objective<ScalarType>> tObjective = Teuchos::rcp(new Plato::ReducedObjectiveROL<ScalarType>(this->mInputData, this->mInterface));
        Teuchos::RCP<ROL::Constraint<ScalarType>> tInequality = Teuchos::rcp(new Plato::ReducedConstraintROL<ScalarType>(this->mInputData, this->mInterface));
        ROL::Ptr<ROL::Problem<ScalarType>> tOptimizationProblem =
            ROL::makePtr<ROL::Problem<ScalarType>>(tObjective, tControls);
        tOptimizationProblem->addBoundConstraint(tControlBoundsMng);
        tOptimizationProblem->addConstraint("Constraint", tInequality, tDual);
        tOptimizationProblem->finalize(true, true, std::cout);

        /******************************** SOLVE OPTIMIZATION PROBLEM ********************************/
        this->solve(tOptimizationProblem);
    }

    /******************************************************************************//**
     * @brief All optimizing is done so do any optional final
     * stages. Called only once from the interface.
    **********************************************************************************/
    void finalize()
    /******************************************************************************/
    {
        this->mInterface->finalize();
    }

private:
    /******************************************************************************/
    void solve(const ROL::Ptr<ROL::Problem<ScalarType>> & aOptimizationProblem)
    /******************************************************************************/
    {
        std::string tFileName = this->mInputData.getInputFileName();
        Teuchos::RCP<Teuchos::ParameterList> tParameterList = Teuchos::rcp(new Teuchos::ParameterList);
        Teuchos::updateParametersFromXmlFile(tFileName, tParameterList.ptr());
        if(this->mInputData.getCheckGradient() == true)
        {
            /**************************** CHECK DERIVATIVES ****************************/
            aOptimizationProblem->check(true, std::cout);
        }
        else
        {
            /************************ SOLVE OPTIMIZATION PROBLEM ***********************/
            ROL::Solver<ScalarType> tOptimizer(aOptimizationProblem, *tParameterList);
            tOptimizer.solve(this->mOutputStream);
        }

        this->printControl(aOptimizationProblem);
    }

private:
    ROLAugmentedLagrangianInterface(const Plato::ROLAugmentedLagrangianInterface<ScalarType> & aRhs);
    Plato::ROLAugmentedLagrangianInterface<ScalarType> & operator=(const Plato::ROLAugmentedLagrangianInterface<ScalarType> & aRhs);
};

} // namespace Plato

#endif /* PLATO_ROLAUGMENTEDLAGRANGIANINTERFACE_HPP_ */
