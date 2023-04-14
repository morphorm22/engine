#include "OptimizationAlgorithm.hpp"
#include "pugixml.hpp"
#include "XMLGeneratorUtilities.hpp"
#include <regex>
using namespace XMLGen;
namespace director
{

OptimizationAlgorithm::OptimizationAlgorithm(const XMLGen::OptimizationParameters& aParameters)
{
    mMaxIterations = aParameters.max_iterations();
}

void OptimizationAlgorithm::appendOutputStage(pugi::xml_node& aNode)
{
    auto tOutput = aNode.append_child("Output");
    addChild(tOutput,"OutputStage","Output To File");
}

void OptimizationAlgorithm::appendOptimizationVariables
(pugi::xml_node& aNode,
 StagePtr aInitialization,
 StagePtr aUpperBound,
 StagePtr aLowerBound)
{ 
    auto tOptimizationVar = aNode.append_child("OptimizationVariables");
    if (aInitialization)
    {
        addChild(tOptimizationVar, "ValueName", aInitialization->outputSharedDataName());
        addChild(tOptimizationVar, "InitializationStage", aInitialization->name());
    }
    if (aLowerBound)
    {
        addChild(tOptimizationVar, "LowerBoundValueName", aLowerBound->inputSharedDataName());
        addChild(tOptimizationVar, "LowerBoundVectorName", aLowerBound->outputSharedDataName());
        addChild(tOptimizationVar, "SetLowerBoundsStage", aLowerBound->name());
    }
    if (aUpperBound)
    {
        addChild(tOptimizationVar, "UpperBoundValueName", aUpperBound->inputSharedDataName());
        addChild(tOptimizationVar, "UpperBoundVectorName", aUpperBound->outputSharedDataName());
        addChild(tOptimizationVar, "SetUpperBoundsStage", aUpperBound->name());
    }
}

void OptimizationAlgorithm::appendObjectiveData
(pugi::xml_node& aNode,
 StagePtr aObjectiveValue,
 StagePtr aObjectiveGradient)
{ 
    auto tObjective = aNode.append_child("Objective");
    if (aObjectiveValue)
    {
        addChild(tObjective, "ValueStageName", aObjectiveValue->name());
        addChild(tObjective, "ValueName", aObjectiveValue->outputSharedDataName());
    }
    if (aObjectiveGradient)
    {
        addChild(tObjective, "GradientStageName", aObjectiveGradient->name());
        addChild(tObjective, "GradientName", aObjectiveGradient->outputSharedDataName());
    }
}

void OptimizationAlgorithm::appendConstraintData
(pugi::xml_node& aNode,
 StagePtr aConstraintValue,
 StagePtr aConstraintGradient)
{ 
    auto tConstraint = aNode.append_child("Constraint");
    if (aConstraintValue)
    {
        addChild(tConstraint, "ValueStageName", aConstraintValue->name());
        addChild(tConstraint, "ValueName", aConstraintValue->outputSharedDataName());
    }
    if (aConstraintGradient)
    {
        addChild(tConstraint, "GradientStageName", aConstraintGradient->name());
        addChild(tConstraint, "GradientName", aConstraintGradient->outputSharedDataName());
    }
    addChild(tConstraint, "NormalizedTargetValue", "");
    addChild(tConstraint, "AbsoluteTargetValue", "");
    addChild(tConstraint, "ReferenceValueName", "");
    addChild(tConstraint, "ReferenceValue", "");
}

void OptimizationAlgorithm::appendBoundsData
(pugi::xml_node& aNode)
{ 
    auto tBounds = aNode.append_child("BoundConstraint");
    addChild(tBounds, "Upper", "1.0");
    addChild(tBounds, "Lower", "0.0");
}

//****************************PLATO **********************************************//
OptimizationAlgorithmPlatoOC::OptimizationAlgorithmPlatoOC(const XMLGen::OptimizationParameters& aParameters)
: OptimizationAlgorithm(aParameters)
{
    mControlStagnationTolerance = aParameters.oc_control_stagnation_tolerance();
    mObjectiveStagnationTolerance = aParameters.oc_objective_stagnation_tolerance();
    mGradientTolerance = aParameters.oc_gradient_tolerance();
    mProblemUpdateFrequency = aParameters.problem_update_frequency();
}
void OptimizationAlgorithmPlatoOC::writeInterface
(pugi::xml_node& aNode,
 StagePtr aInitialization,
 StagePtr aUpperBound,
 StagePtr aLowerBound,
 StagePtr aObjectiveValue,
 StagePtr aObjectiveGradient,
 StagePtr aConstraintValue,
 StagePtr aConstraintGradient)
{
    auto tOptimizer = aNode.append_child("Optimizer");
    addChild(tOptimizer, "Package","OC");
    
    auto tOptions = tOptimizer.append_child("Options");    
    addChildCheckEmpty(tOptions, "OCControlStagnationTolerance",mControlStagnationTolerance);
    addChildCheckEmpty(tOptions, "OCObjectiveStagnationTolerance",mObjectiveStagnationTolerance);
    addChildCheckEmpty(tOptions, "OCGradientTolerance",mGradientTolerance);
    addChildCheckEmpty(tOptions, "ProblemUpdateFrequency",mProblemUpdateFrequency);

    auto tConvergence = tOptimizer.append_child("Convergence");
    addChildCheckEmpty(tConvergence, "MaxIterations",mMaxIterations);

    appendOutputStage(tOptimizer);
    appendOptimizationVariables(tOptimizer, aInitialization, aUpperBound, aLowerBound);
    appendObjectiveData(tOptimizer, aObjectiveValue, aObjectiveGradient);
    appendConstraintData(tOptimizer, aConstraintValue, aConstraintGradient);
    appendBoundsData(tOptimizer);
}

void OptimizationAlgorithmPlatoOC::writeAuxiliaryFiles(pugi::xml_node& aNode)
{

}

OptimizationAlgorithmPlatoKSBC::OptimizationAlgorithmPlatoKSBC(const XMLGen::OptimizationParameters& aParameters)
: OptimizationAlgorithm(aParameters)
{
    mMaxNumOuterIterations= aParameters.max_iterations(); ///<<<<<<<<<
    mTrustRegionExpansionFactor = aParameters.ks_trust_region_expansion_factor();
    mTrustRegionContractionFactor = aParameters.ks_trust_region_contraction_factor();
    mMaxTrustRegionIterations = aParameters.ks_max_trust_region_iterations();

    mInitialRadiusScale = aParameters.ks_initial_radius_scale();
    mMaxRadiusScale = aParameters.ks_max_radius_scale();
    mHessianType = aParameters.hessian_type();
    mMinTrustRegionRadius = aParameters.ks_min_trust_region_radius();
    mLimitedMemoryStorage = aParameters.limited_memory_storage();
    
    mOuterGradientTolerance = aParameters.ks_outer_gradient_tolerance();
    mOuterStationarityTolerance = aParameters.ks_outer_stationarity_tolerance();
    mOuterStagnationTolerance = aParameters.ks_outer_stagnation_tolerance();
    mOuterControlStagnationTolerance = aParameters.ks_outer_control_stagnation_tolerance();
    mOuterActualReductionTolerance = aParameters.ks_outer_actual_reduction_tolerance();
    
    mProblemUpdateFrequency = aParameters.problem_update_frequency();
    mDisablePostSmoothing = aParameters.ks_disable_post_smoothing();
    mTrustRegionRatioLow = aParameters.ks_trust_region_ratio_low();
    mTrustRegionRatioMid = aParameters.ks_trust_region_ratio_mid();
    mTrustRegionRatioUpper = aParameters.ks_trust_region_ratio_high();
}
void OptimizationAlgorithmPlatoKSBC::writeInterface
(pugi::xml_node& aNode,
 StagePtr aInitialization,
 StagePtr aUpperBound,
 StagePtr aLowerBound,
 StagePtr aObjectiveValue,
 StagePtr aObjectiveGradient,
 StagePtr aConstraintValue,
 StagePtr aConstraintGradient)
{
    auto tOptimizer = aNode.append_child("Optimizer");
    addChild(tOptimizer, "Package","KSBC");
    
    auto tOptions = tOptimizer.append_child("Options");
    addChildCheckEmpty(tOptions, "MaxNumOuterIterations",mMaxNumOuterIterations);
    addChildCheckEmpty(tOptions, "KSTrustRegionExpansionFactor",mTrustRegionExpansionFactor);
    addChildCheckEmpty(tOptions, "KSTrustRegionContractionFactor",mTrustRegionContractionFactor);
    addChildCheckEmpty(tOptions, "KSMaxTrustRegionIterations",mMaxTrustRegionIterations);
    addChildCheckEmpty(tOptions, "KSInitialRadiusScale",mInitialRadiusScale);
    addChildCheckEmpty(tOptions, "KSMaxRadiusScale",mMaxRadiusScale);
    addChildCheckEmpty(tOptions, "HessianType",mHessianType);
    addChildCheckEmpty(tOptions, "MinTrustRegionRadius",mMinTrustRegionRadius);
    addChildCheckEmpty(tOptions, "LimitedMemoryStorage",mLimitedMemoryStorage);
    addChildCheckEmpty(tOptions, "KSOuterGradientTolerance",mOuterGradientTolerance);
    addChildCheckEmpty(tOptions, "KSOuterStationarityTolerance",mOuterStationarityTolerance);
    addChildCheckEmpty(tOptions, "KSOuterStagnationTolerance",mOuterStagnationTolerance);
    addChildCheckEmpty(tOptions, "KSOuterControlStagnationTolerance",mOuterControlStagnationTolerance);
    addChildCheckEmpty(tOptions, "KSOuterActualReductionTolerance",mOuterActualReductionTolerance);
    addChildCheckEmpty(tOptions, "ProblemUpdateFrequency",mProblemUpdateFrequency);
    addChildCheckEmpty(tOptions, "DisablePostSmoothing",mDisablePostSmoothing);
    addChildCheckEmpty(tOptions, "KSTrustRegionRatioLow",mTrustRegionRatioLow);
    addChildCheckEmpty(tOptions, "KSTrustRegionRatioMid",mTrustRegionRatioMid);
    addChildCheckEmpty(tOptions, "KSTrustRegionRatioUpper",mTrustRegionRatioUpper);

    auto tConvergence = tOptimizer.append_child("Convergence");
    addChildCheckEmpty(tConvergence, "MaxIterations",mMaxIterations);

    appendOutputStage(tOptimizer);
    appendOptimizationVariables(tOptimizer, aInitialization, aUpperBound, aLowerBound);
    appendObjectiveData(tOptimizer, aObjectiveValue, aObjectiveGradient);
    appendConstraintData(tOptimizer, aConstraintValue, aConstraintGradient);
    appendBoundsData(tOptimizer);
}

void OptimizationAlgorithmPlatoKSBC::writeAuxiliaryFiles(pugi::xml_node& aNode)
{

}

OptimizationAlgorithmPlatoKSAL::OptimizationAlgorithmPlatoKSAL(const XMLGen::OptimizationParameters& aParameters)
: OptimizationAlgorithm(aParameters)
{
   mMaxNumOuterIterations= aParameters.max_iterations(); ///<<<<<<<<<
    mTrustRegionExpansionFactor = aParameters.ks_trust_region_expansion_factor();
    mTrustRegionContractionFactor = aParameters.ks_trust_region_contraction_factor();
    mMaxTrustRegionIterations = aParameters.ks_max_trust_region_iterations();

    mInitialRadiusScale = aParameters.ks_initial_radius_scale();
    mMaxRadiusScale = aParameters.ks_max_radius_scale();
    mHessianType = aParameters.hessian_type();
    mMinTrustRegionRadius = aParameters.ks_min_trust_region_radius();
    mLimitedMemoryStorage = aParameters.limited_memory_storage();
    
    mOuterGradientTolerance = aParameters.ks_outer_gradient_tolerance();
    mOuterStationarityTolerance = aParameters.ks_outer_stationarity_tolerance();
    mOuterStagnationTolerance = aParameters.ks_outer_stagnation_tolerance();
    mOuterControlStagnationTolerance = aParameters.ks_outer_control_stagnation_tolerance();
    mOuterActualReductionTolerance = aParameters.ks_outer_actual_reduction_tolerance();
    
    mProblemUpdateFrequency = aParameters.problem_update_frequency();
    mDisablePostSmoothing = aParameters.ks_disable_post_smoothing();
    mTrustRegionRatioLow = aParameters.ks_trust_region_ratio_low();
    mTrustRegionRatioMid = aParameters.ks_trust_region_ratio_mid();
    mTrustRegionRatioUpper = aParameters.ks_trust_region_ratio_high();

    mPenaltyParam = aParameters.al_penalty_parameter();
    mPenaltyParamScaleFactor = aParameters.al_penalty_scale_factor();
}
void OptimizationAlgorithmPlatoKSAL::writeInterface
(pugi::xml_node& aNode,
 StagePtr aInitialization,
 StagePtr aUpperBound,
 StagePtr aLowerBound,
 StagePtr aObjectiveValue,
 StagePtr aObjectiveGradient,
 StagePtr aConstraintValue,
 StagePtr aConstraintGradient)
{
    auto tOptimizer = aNode.append_child("Optimizer");
    addChild(tOptimizer, "Package","KSAL");
    
    auto tOptions = tOptimizer.append_child("Options");
    addChildCheckEmpty(tOptions, "MaxNumOuterIterations",mMaxNumOuterIterations);
    addChildCheckEmpty(tOptions, "KSTrustRegionExpansionFactor",mTrustRegionExpansionFactor);
    addChildCheckEmpty(tOptions, "KSTrustRegionContractionFactor",mTrustRegionContractionFactor);
    addChildCheckEmpty(tOptions, "KSMaxTrustRegionIterations",mMaxTrustRegionIterations);
    addChildCheckEmpty(tOptions, "KSInitialRadiusScale",mInitialRadiusScale);
    addChildCheckEmpty(tOptions, "KSMaxRadiusScale",mMaxRadiusScale);
    addChildCheckEmpty(tOptions, "HessianType",mHessianType);
    addChildCheckEmpty(tOptions, "MinTrustRegionRadius",mMinTrustRegionRadius);
    addChildCheckEmpty(tOptions, "LimitedMemoryStorage",mLimitedMemoryStorage);
    addChildCheckEmpty(tOptions, "KSOuterGradientTolerance",mOuterGradientTolerance);
    addChildCheckEmpty(tOptions, "KSOuterStationarityTolerance",mOuterStationarityTolerance);
    addChildCheckEmpty(tOptions, "KSOuterStagnationTolerance",mOuterStagnationTolerance);
    addChildCheckEmpty(tOptions, "KSOuterControlStagnationTolerance",mOuterControlStagnationTolerance);
    addChildCheckEmpty(tOptions, "KSOuterActualReductionTolerance",mOuterActualReductionTolerance);
    addChildCheckEmpty(tOptions, "ProblemUpdateFrequency",mProblemUpdateFrequency);
    addChildCheckEmpty(tOptions, "DisablePostSmoothing",mDisablePostSmoothing);
    addChildCheckEmpty(tOptions, "KSTrustRegionRatioLow",mTrustRegionRatioLow);
    addChildCheckEmpty(tOptions, "KSTrustRegionRatioMid",mTrustRegionRatioMid);
    addChildCheckEmpty(tOptions, "KSTrustRegionRatioUpper",mTrustRegionRatioUpper);

    addChildCheckEmpty(tOptions, "AugLagPenaltyParam",mPenaltyParam);
    addChildCheckEmpty(tOptions, "AugLagPenaltyParamScaleFactor",mPenaltyParamScaleFactor);
    
    auto tConvergence = tOptimizer.append_child("Convergence");
    addChildCheckEmpty(tConvergence, "MaxIterations",mMaxIterations);

    appendOutputStage(tOptimizer);
    appendOptimizationVariables(tOptimizer, aInitialization, aUpperBound, aLowerBound);
    appendObjectiveData(tOptimizer, aObjectiveValue, aObjectiveGradient);
    appendConstraintData(tOptimizer, aConstraintValue, aConstraintGradient);
    appendBoundsData(tOptimizer);
}

void OptimizationAlgorithmPlatoKSAL::writeAuxiliaryFiles(pugi::xml_node& aNode)
{

}

OptimizationAlgorithmPlatoMMA::OptimizationAlgorithmPlatoMMA(const XMLGen::OptimizationParameters& aParameters)
: OptimizationAlgorithm(aParameters)
{
    mMaxNumOuterIterations = aParameters.max_iterations(); //<<<<<<<<<<<<<<<<<
    mMoveLimit = aParameters.mma_move_limit();
    mAsymptoteExpansion = aParameters.mma_asymptote_expansion();
    mAsymptoteContraction = aParameters.mma_asymptote_contraction();
    mMaxNumSubProblemIter = aParameters.mma_max_sub_problem_iterations();
    mControlStagnationTolerance = aParameters.mma_control_stagnation_tolerance();
    mObjectiveStagnationTolerance = aParameters.mma_objective_stagnation_tolerance();
    mOutputSubProblemDiagnostics = aParameters.mma_output_subproblem_diagnostics();
    mSubProblemInitialPenalty = aParameters.mma_sub_problem_initial_penalty();
    mSubProblemPenaltyMultiplier = aParameters.mma_sub_problem_penalty_multiplier();
    mSubProblemFeasibilityTolerance = aParameters.mma_sub_problem_feasibility_tolerance();
    mUpdateFrequency = aParameters.problem_update_frequency();
    mUseIpoptForMMASubproblem = aParameters.mma_use_ipopt_sub_problem_solver();
}
void OptimizationAlgorithmPlatoMMA::writeInterface
(pugi::xml_node& aNode,
 StagePtr aInitialization,
 StagePtr aUpperBound,
 StagePtr aLowerBound,
 StagePtr aObjectiveValue,
 StagePtr aObjectiveGradient,
 StagePtr aConstraintValue,
 StagePtr aConstraintGradient)
{
    auto tOptimizer = aNode.append_child("Optimizer");
    addChild(tOptimizer, "Package","MMA");
    
    auto tOptions = tOptimizer.append_child("Options");
    addChildCheckEmpty(tOptions, "MaxNumOuterIterations",mMaxNumOuterIterations);
    addChildCheckEmpty(tOptions, "MoveLimit",mMoveLimit);
    addChildCheckEmpty(tOptions, "AsymptoteExpansion",mAsymptoteExpansion);
    addChildCheckEmpty(tOptions, "AsymptoteContraction",mAsymptoteContraction);
    addChildCheckEmpty(tOptions, "MaxNumSubProblemIter",mMaxNumSubProblemIter);
    addChildCheckEmpty(tOptions, "ControlStagnationTolerance",mControlStagnationTolerance);
    addChildCheckEmpty(tOptions, "ObjectiveStagnationTolerance",mObjectiveStagnationTolerance);
    addChildCheckEmpty(tOptions, "OutputSubProblemDiagnostics",mOutputSubProblemDiagnostics);
    addChildCheckEmpty(tOptions, "SubProblemInitialPenalty",mSubProblemInitialPenalty);
    addChildCheckEmpty(tOptions, "SubProblemPenaltyMultiplier",mSubProblemPenaltyMultiplier);
    addChildCheckEmpty(tOptions, "SubProblemFeasibilityTolerance",mSubProblemFeasibilityTolerance);
    addChildCheckEmpty(tOptions, "UpdateFrequency",mUpdateFrequency);
    addChildCheckEmpty(tOptions, "UseIpoptForMMASubproblem",mUseIpoptForMMASubproblem);

    auto tConvergence = tOptimizer.append_child("Convergence");
    addChildCheckEmpty(tConvergence, "MaxIterations",mMaxIterations);

    appendOutputStage(tOptimizer);
    appendOptimizationVariables(tOptimizer, aInitialization, aUpperBound, aLowerBound);
    appendObjectiveData(tOptimizer, aObjectiveValue, aObjectiveGradient);
    appendConstraintData(tOptimizer, aConstraintValue, aConstraintGradient);
    appendBoundsData(tOptimizer);
}

void OptimizationAlgorithmPlatoMMA::writeAuxiliaryFiles(pugi::xml_node& aNode)
{

}

}//namespace