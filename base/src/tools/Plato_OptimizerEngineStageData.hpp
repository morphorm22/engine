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
 * Plato_OptimizerEngineStageData.hpp
 *
 *  Created on: Oct 27, 2017
 */

#ifndef PLATO_OPTIMIZERENGINESTAGEDATA_HPP_
#define PLATO_OPTIMIZERENGINESTAGEDATA_HPP_

#include <map>
#include <string>
#include <vector>

#include "Plato_InputData.hpp"

namespace Plato
{

class OptimizerEngineStageData : public Plato::InputData
{
public:
    OptimizerEngineStageData();
    ~OptimizerEngineStageData();

    void setCheckGradient(const bool & aInput);
    void setCheckHessian(const bool & aInput);
    void setUserInitialGuess(const bool & aInput);
    void setOutputControlToFile(const bool & aInput);
    void setOutputDiagnosticsToFile(const bool & aInput);

    /******************************************************************************//**
     * @brief Enable mean norm calculations, yes = true or no = false
     * @param [in] aInput flag
    ***********************************************************************************/
    void setMeanNorm(const bool & aInput);

    /******************************************************************************//**
     * @brief Return mean norm flag
     * @return yes = true or no = false
    ***********************************************************************************/
    bool getMeanNorm() const;

    /******************************************************************************//**
     * @brief Disable post-smoothing operation in Kelley-Sachs trust region algorithm, yes = true or no = false
     * @param [in] aInput flag
    ***********************************************************************************/
    void setDisablePostSmoothing(const bool & aInput);

    bool getCheckGradient() const;
    bool getCheckHessian() const;
    bool getUserInitialGuess() const;
    bool getOutputControlToFile() const;
    bool getOutputDiagnosticsToFile() const;

    /******************************************************************************//**
     * @brief Numerical method used to compute application of vector to Hessian operator
     * @param [in] aInput numerical method
    ***********************************************************************************/
    void setHessianType(const std::string & aInput);

    /******************************************************************************//**
     * @brief Return numerical method used to compute application of vector to Hessian operator
     * @return numerical method
    ***********************************************************************************/
    std::string getHessianType() const;

    bool getDisablePostSmoothing() const;

    std::vector<double> getLowerBoundValues() const;
    std::vector<double> getUpperBoundValues() const;
    void setLowerBoundValues(const std::vector<double> & aInput);
    void setUpperBoundValues(const std::vector<double> & aInput);

    size_t getMaxNumIterations() const;
    void setMaxNumIterations(const size_t & aInput);

    int getDerivativeCheckerFinalSuperscript() const;
    void setDerivativeCheckerFinalSuperscript(const int & aInput);
    int getDerivativeCheckerInitialSuperscript() const;
    void setDerivativeCheckerInitialSuperscript(const int & aInput);

    std::vector<double> getInitialGuess() const;
    void setInitialGuess(const std::vector<double> & aInput);
    void setInitialGuess(const std::vector<std::string> & aInput);

    double getInitialMovingAsymptoteScaleFactor() const;
    void setInitialMovingAsymptoteScaleFactor(const double & aInput);

    double getGCMMAInnerKKTTolerance() const;
    void setGCMMAInnerKKTTolerance(const double & aInput);

    int getGCMMAMaxInnerIterations() const;
    void setGCMMAMaxInnerIterations(const int & aInput);

    double getGCMMAInnerControlStagnationTolerance() const;
    void setGCMMAInnerControlStagnationTolerance(const double & aInput);

    double getCCSAOuterKKTTolerance() const;
    void setCCSAOuterKKTTolerance(const double & aInput);

    double getCCSAOuterControlStagnationTolerance() const;
    void setCCSAOuterControlStagnationTolerance(const double & aInput);

    double getCCSAOuterObjectiveStagnationTolerance() const;
    void setCCSAOuterObjectiveStagnationTolerance(const double & aInput);

    double getCCSAOuterStationarityTolerance() const;
    void setCCSAOuterStationarityTolerance(const double & aInput);

    double getKSTrustRegionExpansionFactor() const;
    void setKSTrustRegionExpansionFactor(const double & aInput);

    double getKSTrustRegionContractionFactor() const;
    void setKSTrustRegionContractionFactor(const double & aInput);

    int getKSMaxTrustRegionIterations() const;
    void setKSMaxTrustRegionIterations(const int & aInput);

    double getKSOuterGradientTolerance() const;
    void setKSOuterGradientTolerance(const double & aInput);

    double getKSOuterStationarityTolerance() const;
    void setKSOuterStationarityTolerance(const double & aInput);

    double getKSOuterStagnationTolerance() const;
    void setKSOuterStagnationTolerance(const double & aInput);

    double getKSOuterControlStagnationTolerance() const;
    void setKSOuterControlStagnationTolerance(const double & aInput);

    double getKSOuterActualReductionTolerance() const;
    void setKSOuterActualReductionTolerance(const double & aInput);

    double getOCControlStagnationTolerance() const;
    void setOCControlStagnationTolerance(const double & aInput);

    double getOCObjectiveStagnationTolerance() const;
    void setOCObjectiveStagnationTolerance(const double & aInput);

    double getOCGradientTolerance() const;
    void setOCGradientTolerance(const double & aInput);

    /******************************************************************************//**
     * @brief Return limited memory storage capacity for LBFG Hessian method
     * @return limited memory storage capacity
    ***********************************************************************************/
    size_t getLimitedMemoryStorage() const;

    /******************************************************************************//**
     * @brief Set limited memory storage capacity for LBFG Hessian method
     * @param [in] aInput limited memory storage capacity
    ***********************************************************************************/
    void setLimitedMemoryStorage(const size_t & aInput);

    /******************************************************************************//**
     * @brief Return maximum number of augmented Lagrangian sub problem iterations
     * @return maximum number of augmented Lagrangian sub problem iterations
    ***********************************************************************************/
    size_t getMaxNumAugLagSubProbIter() const;

    /******************************************************************************//**
     * @brief Set maximum number of augmented Lagrangian sub problem iterations
     * @param [in] aInput maximum number of augmented Lagrangian sub problem iterations
    ***********************************************************************************/
    void setMaxNumAugLagSubProbIter(const size_t & aInput);

    /******************************************************************************//**
     * @brief Return penalty parameter for augmented Lagrangian algorithm
     * @return penalty parameter for augmented Lagrangian algorithm
    ***********************************************************************************/
    double getAugLagPenaltyParameter() const;

    /******************************************************************************//**
     * @brief Set penalty parameter for augmented Lagrangian algorithm
     * @param [in] aInput penalty parameter for augmented Lagrangian algorithm
    ***********************************************************************************/
    void setAugLagPenaltyParameter(const double & aInput);

    /******************************************************************************//**
     * @brief Return scaling penalty parameter for augmented Lagrangian algorithm
     * @return scaling penalty parameter for augmented Lagrangian algorithm
    ***********************************************************************************/
    double getAugLagPenaltyScaleParameter() const;

    /******************************************************************************//**
     * @brief Set scaling penalty parameter for augmented Lagrangian algorithm
     * @param [in] aInput scaling penalty parameter for augmented Lagrangian algorithm
    ***********************************************************************************/
    void setAugLagPenaltyScaleParameter(const double & aInput);

    /******************************************************************************//**
     * @brief Return minimum trust region radius
     * @return minimum trust region radius
    ***********************************************************************************/
    double getMinTrustRegionRadius() const;

    /******************************************************************************//**
     * @brief Set minimum trust region radius
     * @param [in] aInput minimum trust region radius
    ***********************************************************************************/
    void setMinTrustRegionRadius(const double & aInput);

    /******************************************************************************//**
     * @brief Return maximum trust region radius
     * @return maximum trust region radius
    ***********************************************************************************/
    double getMaxTrustRegionRadius() const;

    /******************************************************************************//**
     * @brief Set maximum trust region radius
     * @param [in] aInput maximum trust region radius
    ***********************************************************************************/
    void setMaxTrustRegionRadius(const double & aInput);

    /******************************************************************************//**
     * @brief Return feasibility tolerance
     * @return feasibility tolerance
    ***********************************************************************************/
    double getFeasibilityTolerance() const;

    /******************************************************************************//**
     * @brief Set feasibility tolerance
     * @param [in] aInput feasibility tolerance
    ***********************************************************************************/
    void setFeasibilityTolerance(const double & aInput);

    std::string getStateName() const;
    void setStateNames(const std::string & aInput);

    std::string getInputFileName() const;
    void setInputFileName(const std::string & aInput);

    size_t getNumControlVectors() const;
    std::vector<std::string> getControlNames() const;
    std::string getControlName(const size_t & aIndex) const;
    void addControlName(const std::string & aInput);
    void setControlNames(const std::vector<std::string> & aInput);

    void addFilteredControlName(const std::string & aInput);
    std::string getFilteredControlName(const size_t & aIndex) const;

    std::string getLowerBoundVectorName() const;
    std::string getLowerBoundValueName() const;
    void setLowerBoundValueName(const std::string & aInput);
    void setLowerBoundVectorName(const std::string & aInput);

    std::string getUpperBoundVectorName() const;
    std::string getUpperBoundValueName() const;
    void setUpperBoundValueName(const std::string & aInput);
    void setUpperBoundVectorName(const std::string & aInput);

    std::vector<std::string> getDescentDirectionNames() const;
    std::string getDescentDirectionName(const size_t & aIndex) const;
    void addDescentDirectionName(const std::string & aInput);
    void setDescentDirectionNames(const std::vector<std::string> & aInput);

    std::string getAlgebra() const;
    void setAlgebra(const std::string & aInput);

    std::string getOutputStageName() const;
    void setOutputStageName(const std::string & aInput);

    std::string getCacheStageName() const;
    void setCacheStageName(const std::string & aInput);

    std::vector<std::string> getUpdateProblemStageNames() const;
    void setUpdateProblemStageNames(const std::vector<std::string> & aInput);

    std::string  getObjectiveValueOutputName() const;
    void setObjectiveValueOutputName(const std::string & aInput);
    std::string  getObjectiveValueStageName() const;
    void setObjectiveValueStageName(const std::string & aInput);

    std::string getObjectiveHessianOutputName() const;
    void setObjectiveHessianOutputName(const std::string & aInput);
    std::string  getObjectiveHessianStageName() const;
    void setObjectiveHessianStageName(const std::string & aInput);

    std::string getObjectiveGradientOutputName() const;
    void setObjectiveGradientOutputName(const std::string & aInput);
    std::string getObjectiveGradientStageName() const;
    void setObjectiveGradientStageName(const std::string & aInput);

    std::string getInitializationStageName() const;
    void setInitializationStageName(const std::string & aInput);

    std::string getInitialControlDataName() const;
    void setInitialControlDataName(const std::string & aInput);

    /******************************************************************************//**
     * @brief Return finalization stage name: stage responsible for writing output files
     * @return stage name
    ***********************************************************************************/
    std::string getFinalizationStageName() const;

    /******************************************************************************//**
     * @brief Set finalization stage name: stage responsible for writing output files
     * @param [in] aInput stage name
    ***********************************************************************************/
    void setFinalizationStageName(const std::string & aInput);

    std::string getSetLowerBoundsStageName() const;
    void setSetLowerBoundsStageName(const std::string & aInput);

    std::string getSetUpperBoundsStageName() const;
    void setSetUpperBoundsStageName(const std::string & aInput);

    size_t getNumConstraints() const;

    /******************************************************************************//**
     * @brief Return list of names assign to constraint shared values
     * @return list with the names assign to constraint shared values
    ***********************************************************************************/
    std::vector<std::string> getConstraintValueNames() const;

    /******************************************************************************//**
     * @brief Return name assign to constraint shared value
     * @param [in] aIndex constraint index
     * @return name assign to constraint shared value
    ***********************************************************************************/
    std::string getConstraintValueName(const size_t & aIndex) const;

    /******************************************************************************//**
     * @brief Add constraint shared value name to list
     * @param [in] aInput constraint shared value name
    ***********************************************************************************/
    void addConstraintValueName(const std::string & aValueName);

    /******************************************************************************//**
     * @brief Set list of constraint shared value names
     * @param [in] aInput list of constraint shared value names
    ***********************************************************************************/
    void setConstraintValueNames(const std::vector<std::string> & aInput);

    double getConstraintNormalizedTargetValue(const size_t & aIndex) const;
    double getConstraintNormalizedTargetValue(const std::string & aValueName) const;
    void addConstraintNormalizedTargetValue(const std::string & aValueName, const double & aInput);
    bool constraintNormalizedTargetValueWasSet(const std::string & aValueName) const;

    double getConstraintAbsoluteTargetValue(const size_t & aIndex) const;
    double getConstraintAbsoluteTargetValue(const std::string & aValueName) const;
    void addConstraintAbsoluteTargetValue(const std::string & aValueName, const double & aInput);
    bool constraintAbsoluteTargetValueWasSet(const std::string & aValueName) const;

    double getConstraintReferenceValue(const size_t & aIndex) const;
    double getConstraintReferenceValue(const std::string & aValueName) const;
    void addConstraintReferenceValue(const std::string & aValueName, const double & aInput);
    bool constraintReferenceValueWasSet(const std::string & aValueName) const;

    std::string getConstraintReferenceValueName(const size_t & aIndex) const;
    std::string getConstraintReferenceValueName(const std::string & aValueName) const;
    void addConstraintReferenceValueName(const std::string & aValueName, const std::string & aReferenceValueName);

    std::string getConstraintHessianName(const size_t & aIndex) const;
    std::string getConstraintHessianName(const std::string & aValueName) const;
    void addConstraintHessianName(const std::string & aValueName, const std::string & aHessianName);

    std::string getConstraintGradientName(const size_t & aIndex) const;
    std::string getConstraintGradientName(const std::string & aValueName) const;
    void addConstraintGradientName(const std::string & aValueName, const std::string & aGradientName);

    /******************************************************************************//**
     * @brief Return list of names assign to the constraint value stages
     * @return list of names assign to the constraint value stages
    ***********************************************************************************/
    std::vector<std::string> getConstraintValueStageNames() const;

    /******************************************************************************//**
     * @brief Return name assign to constraint value stage
     * @param [in] aIndex constraint index
     * @return name assign to constraint value stage
    ***********************************************************************************/
    std::string getConstraintValueStageName(const size_t & aIndex) const;

    /******************************************************************************//**
     * @brief Add constraint value stage name to list
     * @param [in] aInput constraint value stage name
    ***********************************************************************************/
    void addConstraintValueStageName(const std::string & aInput);

    /******************************************************************************//**
     * @brief Set list of constraint value stage names
     * @param [in] aInput list of constraint value stage names
    ***********************************************************************************/
    void setConstraintValueStageNames(const std::vector<std::string> & aInput);

    /******************************************************************************//**
     * @brief Return list of names assign to the constraint gradient stages
     * @return list of names assign to the constraint gradient stages
    ***********************************************************************************/
    std::vector<std::string> getConstraintGradientStageNames() const;

    /******************************************************************************//**
     * @brief Return name assign to constraint gradient stage
     * @param [in] aIndex constraint index
     * @return name assign to constraint gradient stage
    ***********************************************************************************/
    std::string getConstraintGradientStageName(const size_t & aIndex) const;

    /******************************************************************************//**
     * @brief Add constraint gradient stage name to list
     * @param [in] aInput constraint gradient stage name
    ***********************************************************************************/
    void addConstraintGradientStageName(const std::string & aInput);

    /******************************************************************************//**
     * @brief Set list of constraint gradient stage names
     * @param [in] aInput list of constraint gradient stage names
    ***********************************************************************************/
    void setConstraintGradientStageNames(const std::vector<std::string> & aInput);

    /******************************************************************************//**
     * @brief Return list of names assign to the constraint Hessian stages
     * @return list of names assign to the constraint Hessian stages
    ***********************************************************************************/
    std::vector<std::string> getConstraintHessianStageNames() const;

    /******************************************************************************//**
     * @brief Return name assign to constraint Hessian stage
     * @param [in] aIndex constraint index
     * @return name assign to constraint Hessian stage
    ***********************************************************************************/
    std::string getConstraintHessianStageName(const size_t & aIndex) const;

    /******************************************************************************//**
     * @brief Add constraint Hessian stage name to list
     * @param [in] aInput constraint Hessian stage name
    ***********************************************************************************/
    void addConstraintHessianStageName(const std::string & aInput);

    /******************************************************************************//**
     * @brief Set list of constraint Hessian stage names
     * @param [in] aInput list of constraint Hessian stage names
    ***********************************************************************************/
    void setConstraintHessianStageNames(const std::vector<std::string> & aInput);

    double getKSInitialRadiusScale() const;
    void setKSInitialRadiusScale(const double& aInput);
    double getKSMaxRadiusScale() const;
    void setKSMaxRadiusScale(const double& aInput);

    size_t getProblemUpdateFrequency() const;
    void setProblemUpdateFrequency(const size_t& aInput);

    double getKSTrustRegionRatioLow() const;
    void setKSTrustRegionRatioLow(const double& aInput);
    double getKSTrustRegionRatioMid() const;
    void setKSTrustRegionRatioMid(const double& aInput);
    double getKSTrustRegionRatioUpper() const;
    void setKSTrustRegionRatioUpper(const double& aInput);

    bool getResetAlgorithmOnUpdate() const;
    void setResetAlgorithmOnUpdate(const bool& aInput);

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & aArchive, const unsigned int version)
    {
      aArchive & boost::serialization::make_nvp("InputData",boost::serialization::base_object<Plato::InputData>(*this));
      aArchive & boost::serialization::make_nvp("MeanNorm",mMeanNorm);
      aArchive & boost::serialization::make_nvp("CheckGradient",mCheckGradient);
      aArchive & boost::serialization::make_nvp("CheckHessian",mCheckHessian);
      aArchive & boost::serialization::make_nvp("UserInitialGuess",mUserInitialGuess);
      aArchive & boost::serialization::make_nvp("OutputControlToFile",mOutputControlToFile);
      aArchive & boost::serialization::make_nvp("OutputDiagnosticsToFile",mOutputDiagnosticsToFile);
      aArchive & boost::serialization::make_nvp("DisablePostSmoothing",mDisablePostSmoothing);
      aArchive & boost::serialization::make_nvp("ResetAlgorithmOnUpdate",mResetAlgorithmOnUpdate);

      aArchive & boost::serialization::make_nvp("MaxNumAugLagSubProbIter",mMaxNumAugLagSubProbIter);
      aArchive & boost::serialization::make_nvp("MaxNumIterations",mMaxNumIterations);
      aArchive & boost::serialization::make_nvp("LimitedMemoryStorage",mLimitedMemoryStorage);
      aArchive & boost::serialization::make_nvp("ProblemUpdateFrequency",mProblemUpdateFrequency);

      aArchive & boost::serialization::make_nvp("GCMMAMaxInnerIterations",mGCMMAMaxInnerIterations);
      aArchive & boost::serialization::make_nvp("KSMaxTrustRegionIterations",mKSMaxTrustRegionIterations);

      aArchive & boost::serialization::make_nvp("InitialMovingAsymptoteScaleFactor",mInitialMovingAsymptoteScaleFactor);
      aArchive & boost::serialization::make_nvp("GCMMAInnerKKTTolerance",mGCMMAInnerKKTTolerance);
      aArchive & boost::serialization::make_nvp("CCSAOuterKKTTolerance",mCCSAOuterKKTTolerance);
      aArchive & boost::serialization::make_nvp("CCSAOuterControlStagnationTolerance",mCCSAOuterControlStagnationTolerance);
      aArchive & boost::serialization::make_nvp("GCMMAInnerControlStagnationTolerance",mGCMMAInnerControlStagnationTolerance);
      aArchive & boost::serialization::make_nvp("CCSAOuterObjectiveStagnationTolerance",mCCSAOuterObjectiveStagnationTolerance);
      aArchive & boost::serialization::make_nvp("CCSAOuterStationarityTolerance",mCCSAOuterStationarityTolerance);
      aArchive & boost::serialization::make_nvp("KSTrustRegionExpansionFactor",mKSTrustRegionExpansionFactor);
      aArchive & boost::serialization::make_nvp("KSTrustRegionContractionFactor",mKSTrustRegionContractionFactor);
      aArchive & boost::serialization::make_nvp("KSOuterGradientTolerance",mKSOuterGradientTolerance);
      aArchive & boost::serialization::make_nvp("KSOuterStationarityTolerance",mKSOuterStationarityTolerance);
      aArchive & boost::serialization::make_nvp("KSOuterStagnationTolerance",mKSOuterStagnationTolerance);
      aArchive & boost::serialization::make_nvp("KSOuterControlStagnationTolerance",mKSOuterControlStagnationTolerance);
      aArchive & boost::serialization::make_nvp("KSOuterActualReductionTolerance",mKSOuterActualReductionTolerance);
      aArchive & boost::serialization::make_nvp("KSInitialRadiusScale",mKSInitialRadiusScale);
      aArchive & boost::serialization::make_nvp("KSMaxRadiusScale",mKSMaxRadiusScale);
      aArchive & boost::serialization::make_nvp("KSTrustRegionRatioLow",mKSTrustRegionRatioLow);
      aArchive & boost::serialization::make_nvp("KSTrustRegionRatioMid",mKSTrustRegionRatioMid);
      aArchive & boost::serialization::make_nvp("KSTrustRegionRatioUpper",mKSTrustRegionRatioUpper);
      aArchive & boost::serialization::make_nvp("FeasibilityTolerance",mFeasibilityTolerance);
      aArchive & boost::serialization::make_nvp("MinTrustRegionRadius",mMinTrustRegionRadius);
      aArchive & boost::serialization::make_nvp("MaxTrustRegionRadius",mMaxTrustRegionRadius);
      aArchive & boost::serialization::make_nvp("AugLagPenaltyParameter",mAugLagPenaltyParameter);
      aArchive & boost::serialization::make_nvp("AugLagPenaltyScaleParameter",mAugLagPenaltyScaleParameter);
      aArchive & boost::serialization::make_nvp("OCControlStagnationTolerance",mOCControlStagnationTolerance);
      aArchive & boost::serialization::make_nvp("OCObjectiveStagnationTolerance",mOCObjectiveStagnationTolerance);
      aArchive & boost::serialization::make_nvp("OCGradientTolerance",mOCGradientTolerance);
      
      aArchive & boost::serialization::make_nvp("Algebra",mAlgebra);
      aArchive & boost::serialization::make_nvp("StateName",mStateName);
      aArchive & boost::serialization::make_nvp("HessianType",mHessianType);
      aArchive & boost::serialization::make_nvp("InputFileName",mInputFileName);
      aArchive & boost::serialization::make_nvp("CacheStageName",mCacheStageName);
      aArchive & boost::serialization::make_nvp("OutputStageName",mOutputStageName);
      aArchive & boost::serialization::make_nvp("ObjectiveValueOutputName",mObjectiveValueOutputName);
      aArchive & boost::serialization::make_nvp("ObjectiveGradientOutputName",mObjectiveGradientOutputName);
      aArchive & boost::serialization::make_nvp("ObjectiveHessianOutputName",mObjectiveHessianOutputName);
      aArchive & boost::serialization::make_nvp("InitializationStageName",mInitializationStageName);
      aArchive & boost::serialization::make_nvp("InitialControlDataName",mInitialControlDataName);
      aArchive & boost::serialization::make_nvp("FinalizationStageName",mFinalizationStageName);
      aArchive & boost::serialization::make_nvp("SetLowerBoundsStageName",mSetLowerBoundsStageName);
      aArchive & boost::serialization::make_nvp("SetUpperBoundsStageName",mSetUpperBoundsStageName);
      aArchive & boost::serialization::make_nvp("LowerBoundValueName",mLowerBoundValueName);
      aArchive & boost::serialization::make_nvp("LowerBoundVectorName",mLowerBoundVectorName);
      aArchive & boost::serialization::make_nvp("UpperBoundValueName",mUpperBoundValueName);
      aArchive & boost::serialization::make_nvp("UpperBoundVectorName",mUpperBoundVectorName);
      aArchive & boost::serialization::make_nvp("ObjectiveValueStageName",mObjectiveValueStageName);
      aArchive & boost::serialization::make_nvp("ObjectiveGradientStageName",mObjectiveGradientStageName);
      aArchive & boost::serialization::make_nvp("ObjectiveHessianStageName",mObjectiveHessianStageName);
      
      aArchive & boost::serialization::make_nvp("InitialGuess",mInitialGuess);
      aArchive & boost::serialization::make_nvp("LowerBoundValues",mLowerBoundValues);
      aArchive & boost::serialization::make_nvp("UpperBoundValues",mUpperBoundValues);

      aArchive & boost::serialization::make_nvp("ConstraintNormalizedTargetValues",mConstraintNormalizedTargetValues);
      aArchive & boost::serialization::make_nvp("ConstraintAbsoluteTargetValues",mConstraintAbsoluteTargetValues);
      aArchive & boost::serialization::make_nvp("ConstraintReferenceValues",mConstraintReferenceValues);

      aArchive & boost::serialization::make_nvp("ControlNames",mControlNames);
      aArchive & boost::serialization::make_nvp("FilteredControlNames",mFilteredControlNames);
      aArchive & boost::serialization::make_nvp("ConstraintValueNames",mConstraintValueNames);
      aArchive & boost::serialization::make_nvp("DescentDirectionNames",mDescentDirectionNames);
      aArchive & boost::serialization::make_nvp("ConstraintValueStageNames",mConstraintValueStageNames);
      aArchive & boost::serialization::make_nvp("ConstraintGradientStageNames",mConstraintGradientStageNames);
      aArchive & boost::serialization::make_nvp("ConstraintHessianStageNames",mConstraintHessianStageNames);
      aArchive & boost::serialization::make_nvp("UpdateProblemStageNames",mUpdateProblemStageNames);

      aArchive & boost::serialization::make_nvp("ConstraintGradientNames",mConstraintGradientNames);
      aArchive & boost::serialization::make_nvp("ConstraintHessianNames",mConstraintHessianNames);
      aArchive & boost::serialization::make_nvp("ConstraintReferenceValueNames",mConstraintReferenceValueNames);
      
    }

private:
    bool mMeanNorm;
    bool mCheckGradient;
    bool mCheckHessian;
    bool mUserInitialGuess;
    bool mOutputControlToFile;
    bool mOutputDiagnosticsToFile;
    bool mDisablePostSmoothing;
    bool mResetAlgorithmOnUpdate;

    size_t mMaxNumAugLagSubProbIter;
    size_t mMaxNumIterations;
    size_t mLimitedMemoryStorage;
    size_t mProblemUpdateFrequency;

    int mGCMMAMaxInnerIterations;
    int mKSMaxTrustRegionIterations;
    int mDerivativeCheckerFinalSuperscript;
    int mDerivativeCheckerInitialSuperscript;

    double mInitialMovingAsymptoteScaleFactor;
    double mGCMMAInnerKKTTolerance;
    double mCCSAOuterKKTTolerance;
    double mCCSAOuterControlStagnationTolerance;
    double mGCMMAInnerControlStagnationTolerance;
    double mCCSAOuterObjectiveStagnationTolerance;
    double mCCSAOuterStationarityTolerance;
    double mKSTrustRegionExpansionFactor;
    double mKSTrustRegionContractionFactor;
    double mKSOuterGradientTolerance;
    double mKSOuterStationarityTolerance;
    double mKSOuterStagnationTolerance;
    double mKSOuterControlStagnationTolerance;
    double mKSOuterActualReductionTolerance;
    double mKSInitialRadiusScale;
    double mKSMaxRadiusScale;
    double mKSTrustRegionRatioLow;
    double mKSTrustRegionRatioMid;
    double mKSTrustRegionRatioUpper;
    double mFeasibilityTolerance;
    double mMinTrustRegionRadius;
    double mMaxTrustRegionRadius;
    double mAugLagPenaltyParameter;
    double mAugLagPenaltyScaleParameter;
    double mOCControlStagnationTolerance;
    double mOCObjectiveStagnationTolerance;
    double mOCGradientTolerance;

    std::string mAlgebra;
    std::string mStateName;
    std::string mHessianType;
    std::string mInputFileName;
    std::string mCacheStageName;
    std::string mOutputStageName;
    std::string mObjectiveValueOutputName;
    std::string mObjectiveGradientOutputName;
    std::string mObjectiveHessianOutputName;
    std::string mInitializationStageName;
    std::string mInitialControlDataName;
    std::string mFinalizationStageName;
    std::string mSetLowerBoundsStageName;
    std::string mSetUpperBoundsStageName;
    std::string mLowerBoundValueName;
    std::string mLowerBoundVectorName;
    std::string mUpperBoundValueName;
    std::string mUpperBoundVectorName;
    std::string mObjectiveValueStageName;
    std::string mObjectiveGradientStageName;
    std::string mObjectiveHessianStageName;

    std::vector<double> mInitialGuess;
    std::vector<double> mLowerBoundValues;
    std::vector<double> mUpperBoundValues;

    std::map<std::string, double> mConstraintNormalizedTargetValues;
    std::map<std::string, double> mConstraintAbsoluteTargetValues;
    std::map<std::string, double> mConstraintReferenceValues;

    std::vector<std::string> mControlNames;
    std::vector<std::string> mFilteredControlNames;
    std::vector<std::string> mConstraintValueNames;
    std::vector<std::string> mDescentDirectionNames;
    std::vector<std::string> mConstraintValueStageNames;
    std::vector<std::string> mConstraintGradientStageNames;
    std::vector<std::string> mConstraintHessianStageNames;
    std::vector<std::string> mUpdateProblemStageNames;

    std::map<std::string, std::string> mConstraintGradientNames;
    std::map<std::string, std::string> mConstraintHessianNames;
    std::map<std::string, std::string> mConstraintReferenceValueNames;

    // USING DEFAULT COPY AND ASSIGNMENT CONSTRUCTORS
};

} // namespace Plato

#endif /* PLATO_OPTIMIZERENGINESTAGEDATA_HPP_ */
