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
 * Plato_ParticleSwarmAlgorithmBCPSO.hpp
 *
 *  Created on: Jan 24, 2019
 */

#pragma once

#include <string>

#include "Plato_DataFactory.hpp"
#include "Plato_ParticleSwarmDataMng.hpp"
#include "Plato_ParticleSwarmOperations.hpp"
#include "Plato_ParticleSwarmIO_Utilities.hpp"
#include "Plato_ParticleSwarmStageMngBCPSO.hpp"

namespace Plato
{

/******************************************************************************//**
 * @brief Main interface to Bound Constrained Particle Swarm Optimization (BCPSO) algorithm
**********************************************************************************/
template<typename ScalarType, typename OrdinalType = size_t>
class ParticleSwarmAlgorithmBCPSO
{
public:
    /******************************************************************************//**
     * @brief Constructor
     * @param [in] aFactory PSO algorithm data factory
     * @param [in] aObjective gradient free objective function interface
    **********************************************************************************/
    explicit ParticleSwarmAlgorithmBCPSO(const std::shared_ptr<Plato::DataFactory<ScalarType, OrdinalType>> & aFactory,
                                 const std::shared_ptr<Plato::GradFreeCriterion<ScalarType, OrdinalType>> & aObjective) :
            mOutputDiagnostics(false),
            mStdDevStoppingTolActive(true),
            mNumIterations(0),
            mNumObjFuncEvals(0),
            mMaxNumIterations(1000),
            mMeanBestObjFuncTolerance(5e-4),
            mStdDevBestObjFuncTolerance(1e-6),
            mGlobalBestObjFuncTolerance(1e-10),
            mStopCriterion(Plato::particle_swarm::DID_NOT_CONVERGE),
            mDataMng(std::make_shared<Plato::ParticleSwarmDataMng<ScalarType, OrdinalType>>(aFactory)),
            mOperations(std::make_shared<Plato::ParticleSwarmOperations<ScalarType, OrdinalType>>(aFactory)),
            mStageMng(std::make_shared<Plato::ParticleSwarmStageMngBCPSO<ScalarType, OrdinalType>>(aFactory, aObjective))
    {
    }

    /******************************************************************************//**
     * @brief Constructor
     * @param [in] aFactory PSO algorithm data factory
     * @param [in] aStageMng stage manager (i.e. core interface to criteria evaluations)
    **********************************************************************************/
    explicit ParticleSwarmAlgorithmBCPSO(const std::shared_ptr<Plato::DataFactory<ScalarType, OrdinalType>> & aFactory,
                                 const std::shared_ptr<Plato::ParticleSwarmStageMng<ScalarType, OrdinalType>> & aStageMng) :
            mOutputDiagnostics(false),
            mStdDevStoppingTolActive(true),
            mNumIterations(0),
            mNumObjFuncEvals(0),
            mMaxNumIterations(1000),
            mMeanBestObjFuncTolerance(5e-4),
            mStdDevBestObjFuncTolerance(1e-6),
            mGlobalBestObjFuncTolerance(1e-10),
            mStopCriterion(Plato::particle_swarm::DID_NOT_CONVERGE),
            mDataMng(std::make_shared<Plato::ParticleSwarmDataMng<ScalarType, OrdinalType>>(aFactory)),
            mOperations(std::make_shared<Plato::ParticleSwarmOperations<ScalarType, OrdinalType>>(aFactory)),
            mStageMng(aStageMng)
    {
    }

    /******************************************************************************//**
     * @brief Destructor
    **********************************************************************************/
    ~ParticleSwarmAlgorithmBCPSO()
    {
    }

    /******************************************************************************//**
     * @brief Enable output of diagnostics (i.e. optimization problem status)
    **********************************************************************************/
    void enableDiagnostics()
    {
        mOutputDiagnostics = true;
    }

    /******************************************************************************//**
     * @brief Disables stopping tolerance based on the standard deviation of the objective function
    **********************************************************************************/
    void disableStdDevStoppingTolerance()
    {
        mStdDevStoppingTolActive = false;
    }

    /******************************************************************************//**
     * @brief Compute best particle positions statistics
    **********************************************************************************/
    void computeCurrentBestParticlesStatistics()
    {
        mDataMng->computeCurrentBestParticlesStatistics();
    }

    /******************************************************************************//**
     * @brief Set maximum number of iterations
     * @param [in] aInput maximum number of iterations
    **********************************************************************************/
    void setMaxNumIterations(const OrdinalType & aInput)
    {
        mMaxNumIterations = aInput;
    }

    /******************************************************************************//**
     * @brief Set maximum number of consecutive failures
     * @param [in] aInput maximum number of consecutive failures
    **********************************************************************************/
    void setMaxNumConsecutiveFailures(const OrdinalType & aInput)
    {
        mOperations->setMaxNumConsecutiveFailures(aInput);
    }

    /******************************************************************************//**
     * @brief Set maximum number of consecutive successes
     * @param [in] aInput maximum number of consecutive successes
    **********************************************************************************/
    void setMaxNumConsecutiveSuccesses(const OrdinalType & aInput)
    {
        mOperations->setMaxNumConsecutiveSuccesses(aInput);
    }

    /******************************************************************************//**
     * @brief Set time step used to compute particle velocities
     * @param [in] aInput time step
    **********************************************************************************/
    void setTimeStep(const ScalarType & aInput)
    {
        mDataMng->setTimeStep(aInput);
    }

    /******************************************************************************//**
     * @brief Set stopping tolerance based on the mean of the best objective function values
     * @param [in] aInput stopping tolerance
    **********************************************************************************/
    void setMeanBestObjFuncTolerance(const ScalarType & aInput)
    {
        mMeanBestObjFuncTolerance = aInput;
    }

    /******************************************************************************//**
     * @brief Set stopping tolerance based on the standard deviation of the best objective function values
     * @param [in] aInput stopping tolerance
    **********************************************************************************/
    void setStdDevBestObjFuncTolerance(const ScalarType & aInput)
    {
        mStdDevBestObjFuncTolerance = aInput;
    }

    /******************************************************************************//**
     * @brief Set stopping tolerance based on global best objective function value
     * @param [in] aInput stopping tolerance
    **********************************************************************************/
    void setGlobalBestObjFuncTolerance(const ScalarType & aInput)
    {
        mGlobalBestObjFuncTolerance = aInput;
    }

    /******************************************************************************//**
     * @brief Set inertia multiplier
     * @param [in] aInput inertia multiplier
    **********************************************************************************/
    void setInertiaMultiplier(const ScalarType & aInput)
    {
        mOperations->setInertiaMultiplier(aInput);
    }

    /******************************************************************************//**
     * @brief Set inertia multiplier
     * @param [in] aInput inertia multiplier
    **********************************************************************************/
    void setCognitiveBehaviorMultiplier(const ScalarType & aInput)
    {
        mOperations->setCognitiveBehaviorMultiplier(aInput);
    }

    /******************************************************************************//**
     * @brief Set social behavior multiplier
     * @param [in] aInput social behavior multiplier
    **********************************************************************************/
    void setSocialBehaviorMultiplier(const ScalarType & aInput)
    {
        mOperations->setSocialBehaviorMultiplier(aInput);
    }

    /******************************************************************************//**
     * @brief Set trust region expansion multiplier
     * @param [in] aInput trust region expansion multiplier
    **********************************************************************************/
    void setTrustRegionExpansionMultiplier(const ScalarType & aInput)
    {
        mOperations->setTrustRegionExpansionMultiplier(aInput);
    }

    /******************************************************************************//**
     * @brief Set trust region contraction multiplier
     * @param [in] aInput trust region contraction multiplier
    **********************************************************************************/
    void setTrustRegionContractionMultiplier(const ScalarType & aInput)
    {
        mOperations->setTrustRegionContractionMultiplier(aInput);
    }

    /******************************************************************************//**
     * @brief Set all particle position's upper bounds to input scalar
     * @param [in] aInput upper bounds
    **********************************************************************************/
    void setUpperBounds(const ScalarType & aInput)
    {
        mDataMng->setUpperBounds(aInput);
    }

    /******************************************************************************//**
     * @brief Set all particle position's lower bounds to input scalar
     * @param [in] aInput lower bounds
    **********************************************************************************/
    void setLowerBounds(const ScalarType & aInput)
    {
        mDataMng->setLowerBounds(aInput);
    }

    /******************************************************************************//**
     * @brief Set particle position's upper bounds
     * @param [in] aInput upper bounds
    **********************************************************************************/
    void setUpperBounds(const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        mDataMng->setUpperBounds(aInput);
    }

    /******************************************************************************//**
     * @brief Set particle position's lower bounds
     * @param [in] aInput lower bounds
    **********************************************************************************/
    void setLowerBounds(const Plato::Vector<ScalarType, OrdinalType> & aInput)
    {
        mDataMng->setLowerBounds(aInput);
    }

    /******************************************************************************//**
     * @brief Set number of constraints. Required for ALPSO diagnostics output.
     * @param [in] aInput number of constraints
    **********************************************************************************/
    void setNumConstraints(const OrdinalType & aInput)
    {
        mOutputData.mNumConstraints = aInput;
    }

    /******************************************************************************//**
     * @brief Return current number of iterations
     * @return current number of iterations
    **********************************************************************************/
    OrdinalType getNumIterations() const
    {
        return (mNumIterations);
    }

    /******************************************************************************//**
     * @brief Return number of objective function evaluations
     * @return number of objective function evaluations
    **********************************************************************************/
    OrdinalType getNumObjFuncEvals() const
    {
        return (mNumObjFuncEvals);
    }

    /******************************************************************************//**
     * @brief Return mean of current best objective function values
     * @return mean of best objective function values
    **********************************************************************************/
    ScalarType getMeanCurrentBestObjFuncValues() const
    {
        return (mDataMng->getMeanCurrentBestObjFuncValues());
    }

    /******************************************************************************//**
     * @brief Return mean of previous best objective function values
     * @return mean of previous best objective function values
    **********************************************************************************/
    ScalarType getMeanPreviousBestObjFuncValues() const
    {
        return (mDataMng->getMeanPreviousBestObjFuncValues());
    }

    /******************************************************************************//**
     * @brief Return standard deviation of current best objective function values
     * @return standard deviation of current best objective function values
    **********************************************************************************/
    ScalarType getStdDevCurrentBestObjFuncValues() const
    {
        return (mDataMng->getStdDevCurrentBestObjFuncValues());
    }

    /******************************************************************************//**
     * @brief Return current global best objective function value
     * @return current global best objective function value
    **********************************************************************************/
    ScalarType getCurrentGlobalBestObjFuncValue() const
    {
        return (mDataMng->getCurrentGlobalBestObjFuncValue());
    }

    /******************************************************************************//**
     * @brief Get current best particle positions
     * @param [out] aInput 1D container of current best particle positions
    **********************************************************************************/
    void getMeanCurrentBestParticlePositions(std::shared_ptr<Plato::Vector<ScalarType, OrdinalType>> & aInput) const
    {
        const Plato::ParticleSwarmDataMng<ScalarType, OrdinalType> & tDataMng = this->getDataMng();
        const Plato::Vector<ScalarType, OrdinalType> & tMeanParticlePositions = tDataMng.getMeanParticlePositions();
        if(aInput.get() == nullptr)
        {
            aInput = tMeanParticlePositions.create();
        }
        assert(aInput->size() == tMeanParticlePositions.size());
        aInput->update(static_cast<ScalarType>(1), tMeanParticlePositions, static_cast<ScalarType>(0));
    }

    /******************************************************************************//**
     * @brief Get standard deviation associated with the current best particle positions
     * @param [out] aInput 1D container of the standard deviation of the current best particle positions
    **********************************************************************************/
    void getStdDevCurrentBestParticlePositions(std::shared_ptr<Plato::Vector<ScalarType, OrdinalType>> & aInput) const
    {
        const Plato::ParticleSwarmDataMng<ScalarType, OrdinalType> & tDataMng = this->getDataMng();
        const Plato::Vector<ScalarType, OrdinalType> & tStdDevParticlePositions = tDataMng.getStdDevParticlePositions();
        if(aInput.get() == nullptr)
        {
            aInput = tStdDevParticlePositions.create();
        }
        assert(aInput->size() == tStdDevParticlePositions.size());
        aInput->update(static_cast<ScalarType>(1), tStdDevParticlePositions, static_cast<ScalarType>(0));
    }

    /******************************************************************************//**
     * @brief Get current global best particle positions
     * @param [out] aInput 1D container of the current global best particle positions
    **********************************************************************************/
    void getCurrentGlobalBestParticlePosition(std::shared_ptr<Plato::Vector<ScalarType, OrdinalType>> & aInput) const
    {
        const Plato::ParticleSwarmDataMng<ScalarType, OrdinalType> & tDataMng = this->getDataMng();
        const Plato::Vector<ScalarType, OrdinalType> & tGlobalBestParticlePosition = tDataMng.getGlobalBestParticlePosition();
        if(aInput.get() == nullptr)
        {
            aInput = tGlobalBestParticlePosition.create();
        }
        assert(aInput->size() == tGlobalBestParticlePosition.size());
        aInput->update(static_cast<ScalarType>(1), tGlobalBestParticlePosition, static_cast<ScalarType>(0));
    }

    /******************************************************************************//**
     * @brief Return stopping criterion
     * @return stopping criterion string
    **********************************************************************************/
    std::string getStoppingCriterion() const
    {
        std::string tReason;
        Plato::pso::get_stop_criterion(mStopCriterion, tReason);
        return (tReason);
    }

    /******************************************************************************//**
     * @brief Return PSO data manager
     * @return const reference to PSO data manager
    **********************************************************************************/
    const Plato::ParticleSwarmDataMng<ScalarType, OrdinalType> & getDataMng() const
    {
        return (*mDataMng);
    }

    /******************************************************************************//**
     * @brief Solve particle swarm bound constrained optimization problem
    **********************************************************************************/
    void solve()
    {
        assert(static_cast<OrdinalType>(mDataMng.use_count()) > static_cast<OrdinalType>(0));
        assert(static_cast<OrdinalType>(mStageMng.use_count()) > static_cast<OrdinalType>(0));

        this->openOutputFile();
        this->initialize();

        mNumIterations = 0;
        while(1)
        {
            mNumIterations++;
            mStageMng->evaluateObjective(*mDataMng);
            mStageMng->findBestParticlePositions(*mDataMng);

            mOperations->checkGlobalBestParticleUpdateSuccessRate(*mDataMng);
            mOperations->updateParticleVelocities(*mDataMng);
            mOperations->updateParticlePositions(*mDataMng);
            mOperations->updateTrustRegionMultiplier();

            mDataMng->computeCurrentBestObjFuncStatistics();
            this->outputDiagnostics();

            if(this->checkStoppingCriteria())
            {
                mDataMng->computeCurrentBestParticlesStatistics();
                this->outputStoppingCriterion();
                this->closeOutputFile();
                break;
            }
        }
    }

    /******************************************************************************//**
     * @brief Solve constrained optimization problem
     * @param [in,out] aOutputStream output/diagnostics file
    **********************************************************************************/
    void solve(std::ofstream & aOutputStream)
    {
        mNumIterations = 0;

        while(1)
        {
            mNumIterations++;
            mStageMng->evaluateObjective(*mDataMng);
            mStageMng->findBestParticlePositions(*mDataMng);

            mOperations->checkGlobalBestParticleUpdateSuccessRate(*mDataMng);
            mOperations->updateParticleVelocities(*mDataMng);
            mOperations->updateParticlePositions(*mDataMng);
            mOperations->updateTrustRegionMultiplier();

            mDataMng->computeCurrentBestObjFuncStatistics();
            this->outputDiagnostics(aOutputStream);

            if(this->checkStoppingCriteria())
            {
                break;
            }
        }
    }

    /******************************************************************************//**
     * @brief Set initial particle positions and check user inputs
    **********************************************************************************/
    void initialize()
    {
        mOperations->checkInertiaMultiplier();
        mDataMng->setInitialParticles();
    }

private:
    /******************************************************************************//**
     * @brief Check stopping criteria
    **********************************************************************************/
    bool checkStoppingCriteria()
    {
        bool tStop = false;
        const ScalarType tMeanCurrentBestObjFunValue = mDataMng->getMeanCurrentBestObjFuncValues();
        const ScalarType tCurrentGlobalBestObjFunValue = mDataMng->getCurrentGlobalBestObjFuncValue();
        const ScalarType tStdDevCurrentBestObjFunValue = mDataMng->getStdDevCurrentBestObjFuncValues();

        if(mNumIterations >= mMaxNumIterations)
        {
            tStop = true;
            mStopCriterion = Plato::particle_swarm::MAX_NUMBER_ITERATIONS;
        }
        else if(tCurrentGlobalBestObjFunValue < mGlobalBestObjFuncTolerance)
        {
            tStop = true;
            mStopCriterion = Plato::particle_swarm::TRUE_OBJECTIVE_TOLERANCE;
        }
        else if(tMeanCurrentBestObjFunValue < mMeanBestObjFuncTolerance)
        {
            tStop = true;
            mStopCriterion = Plato::particle_swarm::MEAN_OBJECTIVE_TOLERANCE;
        }
        else if(tStdDevCurrentBestObjFunValue < mStdDevBestObjFuncTolerance && mStdDevStoppingTolActive)
        {
            tStop = true;
            mStopCriterion = Plato::particle_swarm::STDDEV_OBJECTIVE_TOLERANCE;
        }

        return (tStop);
    }

    /******************************************************************************//**
     * @brief Open output file (i.e. diagnostics file)
    **********************************************************************************/
    void openOutputFile()
    {
        if(mOutputDiagnostics == true)
        {
            const Plato::CommWrapper& tMyCommWrapper = mDataMng->getCommWrapper();
            if(tMyCommWrapper.myProcID() == 0)
            {
                mOutputStream.open("plato_pso_algorithm_diagnostics.txt");
                Plato::pso::print_bcpso_diagnostics_header(mOutputData, mOutputStream, mOutputDiagnostics);
            }
        }
    }

    /******************************************************************************//**
     * @brief Close output file (i.e. diagnostics file)
    **********************************************************************************/
    void closeOutputFile()
    {
        if(mOutputDiagnostics == true)
        {
            const Plato::CommWrapper& tMyCommWrapper = mDataMng->getCommWrapper();
            if(tMyCommWrapper.myProcID() == 0)
            {
                mOutputStream.close();
            }
        }
    }

    /******************************************************************************//**
     * @brief Print stopping criterion into diagnostics file.
    **********************************************************************************/
    void outputStoppingCriterion()
    {
        if(mOutputDiagnostics == true)
        {
            const Plato::CommWrapper& tMyCommWrapper = mDataMng->getCommWrapper();
            if(tMyCommWrapper.myProcID() == 0)
            {
                std::string tReason;
                Plato::pso::get_stop_criterion(mStopCriterion, tReason);
                mOutputStream << tReason.c_str();
            }
        }
    }

    /******************************************************************************//**
     * @brief Print diagnostics particle swarm bound constrained optimization algorithm.
    **********************************************************************************/
    void outputDiagnostics()
    {
        if(mOutputDiagnostics == false)
        {
            return;
        }

        const Plato::CommWrapper& tMyCommWrapper = mDataMng->getCommWrapper();
        if(tMyCommWrapper.myProcID() == 0)
        {
            this->cacheOutputData();
            Plato::pso::print_bcpso_diagnostics(mOutputData, mOutputStream, mOutputDiagnostics);
        }
    }

    /******************************************************************************//**
     * @brief Print diagnostics particle swarm constrained optimization algorithm.
    **********************************************************************************/
    void outputDiagnostics(std::ofstream & aOutputStream)
    {
        if(mOutputDiagnostics == false)
        {
            return;
        }

        const Plato::CommWrapper& tMyCommWrapper = mDataMng->getCommWrapper();
        if(tMyCommWrapper.myProcID() == 0)
        {
            this->cacheOutputData();
            Plato::pso::print_alpso_inner_diagnostics(mOutputData, aOutputStream, mOutputDiagnostics);
        }
    }

    /******************************************************************************//**
     * @brief Cache output/diagnostics data.
    **********************************************************************************/
    void cacheOutputData()
    {
        mNumObjFuncEvals = mNumIterations * mDataMng->getNumParticles();
        mOutputData.mNumIter = mNumIterations;
        mOutputData.mObjFuncCount = mNumObjFuncEvals;
        mOutputData.mCurrentGlobalBestObjFuncValue = mDataMng->getCurrentGlobalBestObjFuncValue();
        mOutputData.mMeanCurrentBestObjFuncValues = mDataMng->getMeanCurrentBestObjFuncValues();
        mOutputData.mStdDevCurrentBestObjFuncValues = mDataMng->getStdDevCurrentBestObjFuncValues();
        mOutputData.mTrustRegionMultiplier = mOperations->getTrustRegionMultiplier();
    }

private:
    bool mOutputDiagnostics; /*!< flag - print diagnostics (default = false) */
    bool mStdDevStoppingTolActive; /*!< activate standard deviation stopping tolerance (default = true) */
    std::ofstream mOutputStream; /*!< output stream for the algorithm's diagnostics */

    OrdinalType mNumIterations; /*!< current number of iterations */
    OrdinalType mNumObjFuncEvals; /*!< current number of objective function values */
    OrdinalType mMaxNumIterations; /*!< maximum number of iterations */

    ScalarType mMeanBestObjFuncTolerance; /*!< stopping tolerance on the mean of the best objective function values */
    ScalarType mStdDevBestObjFuncTolerance; /*!< stopping tolerance on the standard deviation of the best objective function values */
    ScalarType mGlobalBestObjFuncTolerance; /*!< stopping tolerance on global best objective function value */

    Plato::particle_swarm::stop_t mStopCriterion; /*!< stopping criterion enum */
    Plato::DiagnosticsBCPSO<ScalarType, OrdinalType> mOutputData; /*!< PSO algorithm output/diagnostics data structure */
    std::shared_ptr<Plato::ParticleSwarmDataMng<ScalarType, OrdinalType>> mDataMng; /*!< PSO algorithm data manager */
    std::shared_ptr<Plato::ParticleSwarmOperations<ScalarType, OrdinalType>> mOperations; /*!< interface to core PSO operations */
    std::shared_ptr<Plato::ParticleSwarmStageMng<ScalarType, OrdinalType>> mStageMng; /*!< interface to criteria evaluations/calls */

private:
    ParticleSwarmAlgorithmBCPSO(const Plato::ParticleSwarmAlgorithmBCPSO<ScalarType, OrdinalType>&);
    Plato::ParticleSwarmAlgorithmBCPSO<ScalarType, OrdinalType> & operator=(const Plato::ParticleSwarmAlgorithmBCPSO<ScalarType, OrdinalType>&);
};
// class ParticleSwarmAlgorithmBCPSO

} // namespace Plato
