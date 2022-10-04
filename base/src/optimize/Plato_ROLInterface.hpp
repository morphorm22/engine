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

#pragma once

#include "Plato_OptimizerInterface.hpp"

#include "ROL_Bounds.hpp"
#include "ROL_Solver.hpp"

#include "Plato_ReducedObjectiveROL.hpp"
#include "Plato_ReducedConstraintROL.hpp"
#include "Plato_DistributedVectorROL.hpp"

#include <mpi.h>

namespace Plato
{

template<typename ScalarType, typename OrdinalType = size_t>
class ROLInterface : public OptimizerInterface<ScalarType,OrdinalType>
{
public:
    ROLInterface(Plato::Interface* aInterface, const MPI_Comm & aComm, const Plato::optimizer::algorithm_t& aType):
        OptimizerInterface<ScalarType,OrdinalType>(aInterface,aComm),
        mAlgorithmType(aType)
        { }

    virtual ~ROLInterface() = default;

    void initialize() final override 
    {
        OptimizerInterface<ScalarType,OrdinalType>::initialize();
        mOutputBuffer = getOutputBuffer();
    }

    Plato::optimizer::algorithm_t algorithm() const
    {
        return mAlgorithmType;
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & aArchive, const unsigned int version)
    {
    //  aArchive & boost::serialization::make_nvp("OptimizerInterface",boost::serialization::base_object<Plato::OptimizerInterface<ScalarType,OridnalType>>(*this));
    }

    /*void saveXML()
    {
        Plato::OptimizerInterface::saveXML();
       
    }*/

    /******************************************************************************/
    void run()
    /******************************************************************************/
    {
        this->initialize();
        const OrdinalType tCONTROL_VECTOR_INDEX = 0;
        std::string tControlName = this->mInputData.getControlName(tCONTROL_VECTOR_INDEX);
        const OrdinalType tNumControls = this->mInterface->size(tControlName);
        auto tControlBoundsMng = this->setControlBounds(tNumControls);
        
        /******************************** SET CONTROL INITIAL GUESS *********************************/
        Teuchos::RCP<Plato::DistributedVectorROL<ScalarType>> tControls =
                Teuchos::rcp(new Plato::DistributedVectorROL<ScalarType>(this->mComm, tNumControls));
        this->setInitialGuess(tControlName, tControls.operator*());
        
        /********************************* SET OPTIMIZATION PROBLEM *********************************/
        Teuchos::RCP<ROL::Objective<ScalarType>> tObjective = Teuchos::rcp(new Plato::ReducedObjectiveROL<ScalarType>(this->mInputData, this->mInterface));
        ROL::Ptr<ROL::Problem<ScalarType>> tOptimizationProblem = ROL::makePtr<ROL::Problem<ScalarType>>(tObjective, tControls);
                
        tOptimizationProblem->addBoundConstraint(tControlBoundsMng);
        if(mAlgorithmType == Plato::optimizer::algorithm_t::ROL_LINEAR_CONSTRAINT)
            createOptimizationProblemLinearConstraint(tOptimizationProblem,tObjective,tControls,tControlBoundsMng);
        else if(mAlgorithmType == Plato::optimizer::algorithm_t::ROL_AUGMENTED_LAGRANGIAN)
            createOptimizationProblemAugmentedLagrangian(tOptimizationProblem,tObjective,tControls,tControlBoundsMng);
        
        bool tLumpConstraints = ( mAlgorithmType == Plato::optimizer::algorithm_t::ROL_LINEAR_CONSTRAINT ? false : true );
        bool tPrintToStream = true;
        
        tOptimizationProblem->finalize(tLumpConstraints, tPrintToStream, mOutputFile);

        if(this->mInputData.getCheckGradient() == true)
        {
            this->checkGradient(tOptimizationProblem);
            this->checkConstraint(tOptimizationProblem);
        }
        else if(mAlgorithmType == Plato::optimizer::algorithm_t::ROL_BOUND_CONSTRAINED)
            this->solveBoundConstrained(tOptimizationProblem);
        else if(mAlgorithmType == Plato::optimizer::algorithm_t::ROL_LINEAR_CONSTRAINT || mAlgorithmType == Plato::optimizer::algorithm_t::ROL_AUGMENTED_LAGRANGIAN)
            this->solve(tOptimizationProblem);
        
        this->finalize();
    }

private:
    std::ofstream mOutputFile;
    std::streambuf *getOutputBuffer() 
    {
        int tMyRank = -1;
        MPI_Comm_rank(this->mComm, &tMyRank);
        assert(tMyRank >= static_cast<int>(0));
        if(tMyRank == static_cast<int>(0))
        {
            if (this->mInputData.getOutputDiagnosticsToFile()) {
                mOutputFile.open("ROL_output.txt", std::ofstream::out);
                return mOutputFile.rdbuf();
            }
        }
        return std::cout.rdbuf();
    }

    void createOptimizationProblemLinearConstraint
         (ROL::Ptr<ROL::Problem<ScalarType>>& aOptimizationProblem,
         Teuchos::RCP<ROL::Objective<ScalarType>>& aObjective,
         Teuchos::RCP<Plato::DistributedVectorROL<ScalarType>>& aControls,
         Teuchos::RCP<ROL::BoundConstraint<ScalarType>>& aControlBoundsMng
         )
    {
        Teuchos::RCP<ROL::Constraint<ScalarType>> tEquality = Teuchos::rcp(new Plato::ReducedConstraintROL<ScalarType>(this->mInputData, this->mInterface));
        const OrdinalType tNumConstraints = this->mInputData.getNumConstraints();
        Teuchos::RCP<Plato::SerialVectorROL<ScalarType>> tDual = Teuchos::rcp(new Plato::SerialVectorROL<ScalarType>(tNumConstraints));
        
        aOptimizationProblem->addLinearConstraint("Equality Linear Constraint", tEquality, tDual);
        
        auto tParameterList = this->updateParameterListFromRolInputsFile();
        aOptimizationProblem->setProjectionAlgorithm(*tParameterList);   
    }

    void createOptimizationProblemAugmentedLagrangian
         (ROL::Ptr<ROL::Problem<ScalarType>>& aOptimizationProblem,
         Teuchos::RCP<ROL::Objective<ScalarType>>& aObjective,
         Teuchos::RCP<Plato::DistributedVectorROL<ScalarType>>& aControls,
         Teuchos::RCP<ROL::BoundConstraint<ScalarType>>& aControlBoundsMng
         )   
    {
        Teuchos::RCP<ROL::Constraint<ScalarType>> tEquality = Teuchos::rcp(new Plato::ReducedConstraintROL<ScalarType>(this->mInputData, this->mInterface));
        const OrdinalType tNumConstraints = this->mInputData.getNumConstraints();
        Teuchos::RCP<Plato::SerialVectorROL<ScalarType>> tDual = Teuchos::rcp(new Plato::SerialVectorROL<ScalarType>(tNumConstraints));
        
        aOptimizationProblem->addConstraint("Equality Constraint", tEquality, tDual);//, aControlBoundsMng);  
    }    

    /******************************************************************************/
    void solve(const ROL::Ptr<ROL::Problem<ScalarType>> & aOptimizationProblem)
    /******************************************************************************/
    {
        auto tParameterList = this->updateParameterListFromRolInputsFile();   
        ROL::Solver<ScalarType> tOptimizer(aOptimizationProblem, *tParameterList);
        std::ostream outputStream(this->mOutputBuffer);
        tOptimizer.solve(outputStream);
        outputStream.flush();
        this->printControl(aOptimizationProblem);
    }

    /******************************************************************************/
    void solveBoundConstrained(const ROL::Ptr<ROL::Problem<ScalarType>> & aOptimizationProblem)
    /******************************************************************************/
    {
        auto tParameterList = this->updateParameterListFromRolInputsFile();
        int tNumSolves=1;
        if(this->mInputData.getResetAlgorithmOnUpdate())
            tNumSolves = this->mInputData.getMaxNumIterations()/this->mInputData.getProblemUpdateFrequency();
        
        double tCurDelta;
        for(int i=0; i<tNumSolves; ++i)
        {
            if(i>0)
                tParameterList->sublist("Step").sublist("Trust Region").set("Initial Radius", tCurDelta);
            ROL::Solver<ScalarType> tOptimizer(aOptimizationProblem, *tParameterList);
            std::ostream outputStream(this->mOutputBuffer);
            tOptimizer.solve(outputStream);
            ROL::Ptr<const ROL::TypeB::AlgorithmState<ScalarType>> tAlgorithmState =
                    ROL::staticPtrCast<const ROL::TypeB::AlgorithmState<ScalarType>>(tOptimizer.getAlgorithmState());
            tCurDelta = tAlgorithmState->searchSize;
            outputStream << "Delta: " << tCurDelta << std::endl;
            outputStream.flush();
        }
        this->printControl(aOptimizationProblem);
    }
    
    
protected:

    std::streambuf *mOutputBuffer;
    Plato::optimizer::algorithm_t mAlgorithmType;

    /******************************************************************************/
    void printControl(const ROL::Ptr<ROL::Problem<ScalarType>> & aOptimizationProblem)
    /******************************************************************************/
    {
        int tMyRank = -1;
        MPI_Comm_rank(this->mComm, &tMyRank);
        assert(tMyRank >= static_cast<int>(0));
        if(tMyRank == static_cast<int>(0))
        {
            const bool tOutputControlToFile = this->mInputData.getOutputControlToFile();
            if(tOutputControlToFile == true)
            {
                std::ofstream tOutputFile;
                tOutputFile.open("ROL_control_output.txt");
                ROL::Ptr<ROL::Vector<ScalarType>> tSolutionPtr = aOptimizationProblem->getPrimalOptimizationVector();
                Plato::DistributedVectorROL<ScalarType> & tSolution =
                        dynamic_cast<Plato::DistributedVectorROL<ScalarType>&>(tSolutionPtr.operator*());
                std::vector<ScalarType> & tData = tSolution.vector();
                for(OrdinalType tIndex = 0; tIndex < tData.size(); tIndex++)
                    tOutputFile << tData[tIndex] << "\n";
                tOutputFile.close();
            }
        }
    }

    /******************************************************************************/
    void setBounds(const std::vector<ScalarType> & aInputs, Plato::DistributedVectorROL<ScalarType> & aBounds)
    /******************************************************************************/
    {
        assert(aInputs.empty() == false);
        if(aInputs.size() == static_cast<size_t>(1))
        {
            const ScalarType tValue = aInputs[0];
            aBounds.fill(tValue);
        }
        else
        {
            assert(aInputs.size() == static_cast<size_t>(aBounds.dimension()));
            aBounds.setVector(aInputs);
        }
    }

    /******************************************************************************/
    void setInitialGuess(const std::string & aMyName, Plato::DistributedVectorROL<ScalarType> & aControl)
    /******************************************************************************/
    {
        std::string tInitializationStageName = this->mInputData.getInitializationStageName();
        if(tInitializationStageName.empty() == false)
        {
            // Use user-defined stage to compute initial guess
            Teuchos::ParameterList tPlatoInitializationStageParameterList;
            tPlatoInitializationStageParameterList.set(aMyName, aControl.vector().data());
            this->mInterface->compute(tInitializationStageName, tPlatoInitializationStageParameterList);
        }
        else
        {
            // Use user-defined values to compute initial guess. Hence, a stage was not defined by the user.
            std::vector<ScalarType> tInitialGuess = this->mInputData.getInitialGuess();
            assert(tInitialGuess.empty() == false);
            if(tInitialGuess.size() == static_cast<size_t>(1))
            {
                const ScalarType tValue = tInitialGuess[0];
                aControl.fill(tValue);
            }
            else
            {
                assert(tInitialGuess.size() == static_cast<size_t>(aControl.dimension()));
                aControl.setVector(tInitialGuess);
            }
        }
    }

    Teuchos::RCP<Teuchos::ParameterList> updateParameterListFromRolInputsFile()
    {
        std::string tFileName = this->mInputData.getInputFileName();
        Teuchos::RCP<Teuchos::ParameterList> tParameterList = Teuchos::rcp(new Teuchos::ParameterList);
        Teuchos::updateParametersFromXmlFile(tFileName, tParameterList.ptr());
        return tParameterList;
    }
    
    void checkGradient(const ROL::Ptr<ROL::Problem<ScalarType>> & aOptimizationProblem)
    {
        std::cout<<"Checking gradient..."<<std::endl;
        auto tPerturbationScale = this->mInputData.getROLPerturbationScale();
        auto tCheckGradientSteps = this->mInputData.getROLCheckGradientSteps();
        auto tCheckGradientSeed = this->mInputData.getROLCheckGradientSeed();
        if(tCheckGradientSeed !=0)
        {
            std::srand((unsigned int)tCheckGradientSeed);
            std::cout<<"Setting seed to: "<<(unsigned int)tCheckGradientSeed<<std::endl;
        }
        std::ofstream tOutputFile;
        tOutputFile.open("ROL_gradient_check_output.txt");
        auto tObjective = aOptimizationProblem->getObjective();
        auto tControl = aOptimizationProblem->getPrimalOptimizationVector();
        auto tPerturbation = tControl->clone();
        tPerturbation->randomize(-tPerturbationScale, tPerturbationScale);
	    tObjective->checkGradient(*tControl, *tPerturbation,true,tOutputFile,tCheckGradientSteps);
        tOutputFile.close();
    }

    void checkConstraint(const ROL::Ptr<ROL::Problem<ScalarType>> & aOptimizationProblem)
    {
        std::cout<<"Checking constraint..."<<std::endl;
        auto tPerturbationScale = this->mInputData.getROLPerturbationScale();
        auto tCheckGradientSteps = this->mInputData.getROLCheckGradientSteps();
        auto tCheckGradientSeed = this->mInputData.getROLCheckGradientSeed();
        if(tCheckGradientSeed !=0)
        {
            std::srand((unsigned int)tCheckGradientSeed);
            std::cout<<"Setting seed to: "<<(unsigned int)tCheckGradientSeed<<std::endl;
        }
        auto tx = aOptimizationProblem->getPrimalOptimizationVector();
        tx->randomize(0, tPerturbationScale);
        auto tv = aOptimizationProblem->getPrimalOptimizationVector();
        tv->randomize(0, tPerturbationScale);
        std::ofstream tOutputFile;
        if(mAlgorithmType == Plato::optimizer::algorithm_t::ROL_AUGMENTED_LAGRANGIAN)
        {
            auto tg = aOptimizationProblem->getDualOptimizationVector();
            tg->randomize(0, tPerturbationScale);
            auto tc = aOptimizationProblem->getResidualVector();
            tc->randomize(-tPerturbationScale, tPerturbationScale);
            auto tw = aOptimizationProblem->getMultiplierVector();
            tw->randomize(-tPerturbationScale, tPerturbationScale);
            
            auto tConstraint = aOptimizationProblem->getConstraint();
            
            tOutputFile.open("ROL_constraint_check_output.txt");
            tConstraint->checkApplyJacobian(*tx, *tv, *tc, true, tOutputFile);
            tOutputFile.close();

            tOutputFile.open("ROL_adjoint_consistency_output.txt");
            tConstraint->checkAdjointConsistencyJacobian(*tw, *tv, *tx, true, tOutputFile);
            tOutputFile.close();   
        }
        else if(mAlgorithmType == Plato::optimizer::algorithm_t::ROL_LINEAR_CONSTRAINT)
        {
            tOutputFile.open("ROL_linearity_check_output.txt");
            aOptimizationProblem->checkLinearity(true, tOutputFile);
            tOutputFile.close();
        }
        else
            std::cout<<"Nothing to check on ROL_Bound_Constrained Problem"<<std::endl;
    }

    Teuchos::RCP<ROL::BoundConstraint<ScalarType>> setControlBounds(const OrdinalType& aNumControls)
    {
        std::vector<ScalarType> tInputBoundsData(aNumControls);

        // ********* GET LOWER BOUNDS INFORMATION *********
        Plato::getLowerBoundsInputData(this->mInputData, this->mInterface, tInputBoundsData);

        // ********* SET LOWER BOUNDS FOR OPTIMIZER *********
        Teuchos::RCP<Plato::DistributedVectorROL<ScalarType>> tControlLowerBounds =
                Teuchos::rcp(new Plato::DistributedVectorROL<ScalarType>(this->mComm, aNumControls));
        this->setBounds(tInputBoundsData, tControlLowerBounds.operator*());

        // ********* GET UPPER BOUNDS INFORMATION *********
        Plato::getUpperBoundsInputData(this->mInputData, this->mInterface, tInputBoundsData);

        // ********* SET UPPER BOUNDS FOR OPTIMIZER *********
        Teuchos::RCP<Plato::DistributedVectorROL<ScalarType>> tControlUpperBounds =
                Teuchos::rcp(new Plato::DistributedVectorROL<ScalarType>(this->mComm, aNumControls));
        this->setBounds(tInputBoundsData, tControlUpperBounds.operator*());

        // ********* CREATE BOUND CONSTRAINT FOR OPTIMIZER *********
        Teuchos::RCP<ROL::BoundConstraint<ScalarType>> tControlBoundsMng =
                Teuchos::rcp(new ROL::Bounds<ScalarType>(tControlLowerBounds, tControlUpperBounds));
        return tControlBoundsMng;
    }

    /******************************************************************************//**
     * @brief All optimizing is done so do any optional final
     * stages. Called only once from the interface.
    **********************************************************************************/
    void finalize()
    {
        this->mInterface->finalize();
    }
};

} // namespace Plato
