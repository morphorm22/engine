/*
 * Plato_DiagnosticsInterface.hpp
 *
 *  Created on: Jan 9, 2018
 */

#ifndef PLATO_DIAGNOSTICSINTERFACE_HPP_
#define PLATO_DIAGNOSTICSINTERFACE_HPP_

#include <mpi.h>
#include <string>
#include <memory>
#include <cassert>
#include <sstream>

#include "Plato_Macros.hpp"
#include "Plato_Parser.hpp"
#include "Plato_Interface.hpp"
#include "Plato_HostBounds.hpp"
#include "Plato_Diagnostics.hpp"
#include "Plato_CommWrapper.hpp"
#include "Plato_AlgebraFactory.hpp"
#include "Plato_OptimizerInterface.hpp"
#include "Plato_EngineObjective.hpp"
#include "Plato_EngineConstraint.hpp"
#include "Plato_StandardMultiVector.hpp"
#include "Plato_OptimizerUtilities.hpp"

namespace Plato
{

template<typename ScalarType, typename OrdinalType = size_t>
class DiagnosticsInterface : public Plato::OptimizerInterface<ScalarType, OrdinalType>
{
public:
    explicit DiagnosticsInterface(Plato::Interface* aInterface, const MPI_Comm & aComm) :
        Plato::OptimizerInterface<ScalarType, OrdinalType>::OptimizerInterface(aInterface, aComm)
    {
    }
    virtual ~DiagnosticsInterface()
    {
    }

    /******************************************************************************/
    Plato::optimizer::algorithm_t algorithm() const
    /******************************************************************************/
    {
        return (Plato::optimizer::algorithm_t::DERIVATIVE_CHECKER);
    }

    /******************************************************************************/
    void initialize()
    /******************************************************************************/
    {
        auto tOptimizerNode = this->getOptimizerNode(mInterface);

        Plato::Parse::parseOptimizerStages(tOptimizerNode, mInputData);
    }

    /******************************************************************************/
    void run()
    /******************************************************************************/
    {
        this->initialize();

        // ********* ALLOCATE LINEAR ALGEBRA FACTORY ********* //
        Plato::AlgebraFactory<ScalarType, OrdinalType> tAlgebraFactory;

        // ********* ALLOCATE DERIVATIVE CHECKER BASELINE DATA STRUCTURES *********
        Plato::DataFactory<ScalarType, OrdinalType> tDataFactory;
        this->allocateBaselineDataStructures(tAlgebraFactory, tDataFactory);

        // ********* GET UPPER AND LOWER BOUNDS ********* //
        std::vector<ScalarType> tLowerBoundsData;
        std::vector<ScalarType> tUpperBoundsData;
        this->getControlBounds(tLowerBoundsData,tUpperBoundsData);

        std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> tLowerBounds = tDataFactory.control().create();
        std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> tUpperBounds = tDataFactory.control().create();
        const OrdinalType tCONTROL_VECTOR_INDEX = 0;
        Plato::copy(tLowerBoundsData, (*tLowerBounds)[tCONTROL_VECTOR_INDEX]);
        Plato::copy(tUpperBoundsData, (*tUpperBounds)[tCONTROL_VECTOR_INDEX]);

        // ********* SET INITIAL GUESS AND DIAGNOSTICS OPTIONS ********* //
        bool tDidUserDefinedInitialGuess = mInputData.getUserInitialGuess();
        std::shared_ptr<Plato::MultiVector<ScalarType, OrdinalType>> tInitialGuess = tDataFactory.control().create();
        if(tDidUserDefinedInitialGuess == true)
        {
            this->setInitialGuess(tAlgebraFactory, *tInitialGuess);
        }

        // ********* ALLOCATE DIAGNOSTICS TOOL ********* //
        Plato::Diagnostics<ScalarType, OrdinalType> tDiagnostics;
        int tFinalSuperscript = mInputData.getDerivativeCheckerFinalSuperscript();
        tDiagnostics.setFinalSuperscript(tFinalSuperscript);
        int tInitialSuperscript = mInputData.getDerivativeCheckerInitialSuperscript();
        tDiagnostics.setInitialSuperscript(tInitialSuperscript);

        // ********* ENFORCE BOUNDS ********* //
        Plato::HostBounds<ScalarType, OrdinalType> tProjector;
        tProjector.project(*tLowerBounds, *tUpperBounds, *tInitialGuess);


        // ********* CHECK OBJECTIVE FUNCTION ********* //
        this->checkObjectiveFunction(tDataFactory, *tInitialGuess, tDiagnostics);

        // ********* CHECK CONSTRAINTS ********* //
        this->checkConstraints(tDataFactory, *tInitialGuess, tDiagnostics);
    }

    /******************************************************************************//**
     * @brief All optimizing is done so do any optional final
     * stages. Called only once from the interface.
    **********************************************************************************/
    void finalize()
    /******************************************************************************/
    {
        mInterface->finalize();
    }

private:
    void getControlBounds(std::vector<ScalarType> & aLowerBoundsData,
                          std::vector<ScalarType> & aUpperBoundsData)
    {
        // diagnostics does not currently need the control bounds, but if they are
        // not constructed here, then they are not used in other parts of the code.
        const OrdinalType tCONTROL_VECTOR_INDEX = 0;
        std::string tControlName = mInputData.getControlName(tCONTROL_VECTOR_INDEX);
        const OrdinalType tNumControls = mInterface->size(tControlName);
        aLowerBoundsData.resize(tNumControls);
        aUpperBoundsData.resize(tNumControls);
        Plato::getLowerBoundsInputData(mInputData, mInterface, aLowerBoundsData);
        Plato::getUpperBoundsInputData(mInputData, mInterface, aUpperBoundsData);
    }

    /******************************************************************************/
    void setInitialGuess(const Plato::AlgebraFactory<ScalarType, OrdinalType> & aAlgebraFactory,
                        Plato::MultiVector<ScalarType, OrdinalType> & aMultiVector)
    /******************************************************************************/
    {
        // ********* Allocate Plato::Vector of controls *********
        const OrdinalType tCONTROL_VECTOR_INDEX = 0;
        std::string tControlName = mInputData.getControlName(tCONTROL_VECTOR_INDEX);
        const OrdinalType tNumControls = mInterface->size(tControlName);
        std::vector<ScalarType> tInputIntitalGuessData(tNumControls);
        std::shared_ptr<Plato::Vector<ScalarType, OrdinalType>> tVector =
                aAlgebraFactory.createVector(mComm, tNumControls, mInterface);

        // ********* Set initial guess for each control vector *********
        Plato::getInitialGuessInputData(tControlName, mInputData, mInterface, tInputIntitalGuessData);
        Plato::copy(tInputIntitalGuessData, *tVector);
        aMultiVector[tCONTROL_VECTOR_INDEX].update(1., *tVector, 0.);
    }
    /******************************************************************************/
    void checkConstraints(const Plato::DataFactory<ScalarType, OrdinalType> & aDataFactory,
                          Plato::MultiVector<ScalarType, OrdinalType> & aInitialGuess,
                          Plato::Diagnostics<ScalarType, OrdinalType> & aDiagnostics)
    /******************************************************************************/
    {
        // ********* Allocate and Check Constraints ********* //
        std::ostringstream tOutputMsg;
        const OrdinalType tNumConstraints = mInputData.getNumConstraints();
        bool tDidUserDefinedInitialGuess = mInputData.getUserInitialGuess();
        if(tNumConstraints > static_cast<OrdinalType>(0))
        {
            this->parseConstraintReferenceValues();
            for(OrdinalType tIndex = 0; tIndex < tNumConstraints; tIndex++)
            {
                std::string tMyName = mInputData.getConstraintValueName(tIndex);
                Plato::EngineConstraint<ScalarType, OrdinalType> tConstraint(tIndex, aDataFactory, mInputData, mInterface);

                if(mInputData.getCheckGradient() == true)
                {
                    tOutputMsg << "\n\n********** CHECK CONSTRAINT: " << tMyName.c_str() << " GRADIENT **********\n\n";
                    aDiagnostics.checkCriterionGradient(tConstraint, aInitialGuess, tOutputMsg, tDidUserDefinedInitialGuess);
                }

                if(mInputData.getCheckHessian() == true)
                {
                    tOutputMsg << "\n\n********** CHECK CONSTRAINT: " << tMyName.c_str() << " HESSIAN **********\n\n";
                    aDiagnostics.checkCriterionHessian(tConstraint, aInitialGuess, tOutputMsg, tDidUserDefinedInitialGuess);
                }

                if(mInputData.getCheckGradient() == false && mInputData.getCheckHessian() == false)
                {
                    tOutputMsg << "\n\n********** NO DERIVATIVE CHECK REQUESTED FOR CONSTRAINT: " << tMyName.c_str() << " **********\n\n";
                }
            }

            // ********* Print Output Message ********* //
            const Plato::CommWrapper& tMyCommWrapper = aDataFactory.getCommWrapper();
            assert(tMyCommWrapper.myProcID() >= static_cast<int>(0));
            if(tMyCommWrapper.myProcID() == static_cast<int>(0))
            {
                std::cout << tOutputMsg.str().c_str() << std::flush;
            }
        }
    }
    /******************************************************************************/
    void checkObjectiveFunction(const Plato::DataFactory<ScalarType, OrdinalType> & aDataFactory,
                                Plato::MultiVector<ScalarType, OrdinalType> & aInitialGuess,
                                Plato::Diagnostics<ScalarType, OrdinalType> & aDiagnostics)
    /******************************************************************************/
    {
        // ********* Check Objective Function ********* //
      Plato::EngineObjective<ScalarType, OrdinalType> tObjective(aDataFactory, mInputData, mInterface, this);

        std::ostringstream tOutputMsg;
        std::string tMyName = mInputData.getObjectiveValueOutputName();
        bool tDidUserDefinedInitialGuess = mInputData.getUserInitialGuess();
        if(mInputData.getCheckGradient() == true)
        {
            tOutputMsg << "\n\n********** CHECK OBJECTIVE FUNCTION: " << tMyName.c_str() << " GRADIENT **********\n\n";
            aDiagnostics.checkCriterionGradient(tObjective, aInitialGuess, tOutputMsg, tDidUserDefinedInitialGuess);
        }

        if(mInputData.getCheckHessian() == true)
        {
            tOutputMsg << "\n\n********** CHECK OBJECTIVE FUNCTION: " << tMyName.c_str() << " HESSIAN **********\n\n";
            aDiagnostics.checkCriterionHessian(tObjective, aInitialGuess, tOutputMsg, tDidUserDefinedInitialGuess);
        }

        if(mInputData.getCheckGradient() == false && mInputData.getCheckHessian() == false)
        {
            tOutputMsg << "\n\n********** NO DERIVATIVE CHECK REQUESTED FOR OBJECTIVE: " << tMyName.c_str() << " **********\n\n";
        }

        // ********* Print Output Message ********* //
        const Plato::CommWrapper& tMyCommWrapper = aDataFactory.getCommWrapper();
        assert(tMyCommWrapper.myProcID() >= static_cast<int>(0));
        if(tMyCommWrapper.myProcID() == static_cast<int>(0))
        {
            std::cout << tOutputMsg.str().c_str() << std::flush;
        }
    }
    /******************************************************************************/
    void parseConstraintReferenceValues()
    /******************************************************************************/
    {
        const OrdinalType tNumConstraints = mInputData.getNumConstraints();
        for(OrdinalType tIndex = 0; tIndex < tNumConstraints; tIndex++)
        {
            std::string tReferenceValueName = mInputData.getConstraintReferenceValueName(tIndex);
            if(tReferenceValueName.empty() == false)
            {
                ScalarType tReferenceValue = 0;
                Teuchos::ParameterList tArguments;
                tArguments.set(tReferenceValueName, &tReferenceValue);
                mInterface->compute(tReferenceValueName, tArguments);
                std::string tConstraintValueName = mInputData.getConstraintValueName(tIndex);
                mInputData.addConstraintReferenceValue(tConstraintValueName, tReferenceValue);
            }
        }
    }
    /******************************************************************************/
    void allocateBaselineDataStructures(const Plato::AlgebraFactory<ScalarType, OrdinalType> & aAlgebraFactory,
                                        Plato::DataFactory<ScalarType, OrdinalType> & aDataFactory)
    /******************************************************************************/
    {
        // ********* Allocate control vectors baseline data structures *********
        const OrdinalType tNumVectors = mInputData.getNumControlVectors();
        assert(tNumVectors > static_cast<OrdinalType>(0));
        Plato::StandardMultiVector<ScalarType, OrdinalType> tMultiVector;
        for(OrdinalType tIndex = 0; tIndex < tNumVectors; tIndex++)
        {
            std::string tControlName = mInputData.getControlName(tIndex);
            const OrdinalType tNumControls = mInterface->size(tControlName);
            std::shared_ptr<Plato::Vector<ScalarType, OrdinalType>> tVector =
                    aAlgebraFactory.createVector(mComm, tNumControls, mInterface);
            tMultiVector.add(tVector);
        }
        aDataFactory.allocateControl(tMultiVector);
        std::shared_ptr<Plato::ReductionOperations<ScalarType, OrdinalType>> tReductionOperations =
                aAlgebraFactory.createReduction(mComm, mInterface);
        aDataFactory.allocateControlReductionOperations(*tReductionOperations);

        Plato::CommWrapper tCommWrapper(mComm);
        aDataFactory.setCommWrapper(tCommWrapper);
    }

private:
    MPI_Comm mComm;
    Plato::Interface* mInterface;
    Plato::OptimizerEngineStageData mInputData;

private:
    DiagnosticsInterface(const Plato::DiagnosticsInterface<ScalarType, OrdinalType> & aRhs);
    Plato::DiagnosticsInterface<ScalarType, OrdinalType> & operator=(const Plato::DiagnosticsInterface<ScalarType, OrdinalType> & aRhs);
};

} // namespace Plato

#endif /* PLATO_DIAGNOSTICSINTERFACE_HPP_ */
