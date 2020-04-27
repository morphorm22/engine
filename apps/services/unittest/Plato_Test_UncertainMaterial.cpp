/*
 * Plato_Test_UncertainMaterial.cpp
 *
 *  Created on: Apr 22, 2020
 */

#include "gtest/gtest.h"

#include "Plato_SromXMLUtils.hpp"
#include "Plato_SromMetadata.hpp"

namespace Plato
{

namespace srom
{

inline std::string to_string(const double& aInput, double aPrecision = 16)
{
    std::ostringstream tValueString;
    tValueString << std::fixed; // forces fix-point notation instead of default scientific notation
    tValueString << std::setprecision(aPrecision); // sets precision for fixed-point notation
    tValueString << aInput;
    return (tValueString.str());
}

/******************************************************************************//**
 * \brief Deterministic variable metadata for Stochastic Reduced Order Model (SROM) problem.
**********************************************************************************/
struct DeterministicVariable
{
    std::string mTag;        /*!< main variable attribute , e.g. Poisson's ratio */
    std::string mValue;      /*!< attribute's deterministic value */
    std::string mAttribute;  /*!< attribute's category, e.g. homogeneous or heterogeneous */
};
// struct DeterministicVariable

struct Material
{
private:
    std::string mBlockID;     /*!< element block identification number */
    std::string mCategory;    /*!< isotropic, orthotropic, anisotropic, etc. */
    std::string mMaterialID;  /*!< material identification number */

    std::vector<Plato::srom::RandomVariable> mRandomVars; /*!< non-deterministic material property */
    std::vector<Plato::srom::DeterministicVariable> mDeterministicVars; /*!< deterministic material property */

public:
    bool isRandom() const
    {
        return (!mRandomVars.empty());
    }
    bool isDeterministic() const
    {
        return (mRandomVars.empty());
    }
    bool undefined() const
    {
        auto tIsMaterialUndefined = mDeterministicVars.empty() && mRandomVars.empty();
        return tIsMaterialUndefined;
    }

    void check() const
    {
        if(mBlockID.empty())
        {
            THROWERR("SROM::Material: Material block identification number is not defined.")
        }

        if(mMaterialID.empty())
        {
            THROWERR("SROM::Material: Material identification number is not defined.")
        }

        auto tNumMaterialProperties = mRandomVars.size() + mDeterministicVars.size();
        if(tNumMaterialProperties == static_cast<size_t>(0))
        {
            std::ostringstream tMsg;
            tMsg << "SROM Material: Material properties for material with identification number '" << mMaterialID.c_str()
                << "' and block with identification number '" << mBlockID << "' are not defined.";
            THROWERR(tMsg.str().c_str())
        }
    }

    void blockID(const std::string& aID)
    {
        mBlockID = aID;
    }
    std::string blockID() const
    {
        return (mBlockID);
    }

    void materialID(const std::string& aID)
    {
        mMaterialID = aID;
    }
    std::string materialID() const
    {
        return (mMaterialID);
    }

    void category(const std::string& aCategory)
    {
        mCategory = aCategory;
    }
    std::string category() const
    {
        return mCategory;
    }
    std::vector<std::string> tags() const
    {
        std::vector<std::string> tTags;

        // append random variables tags
        for(auto& tRandomVar : mRandomVars)
        {
            tTags.push_back(tRandomVar.mTag);
        }

        // append deterministic variables tags
        for(auto& tDeterministicVar : mDeterministicVars)
        {
            tTags.push_back(tDeterministicVar.mTag);
        }

        return (tTags);
    }
    std::vector<std::string> attributes() const
    {
        std::vector<std::string> tAttributes;

        // append random variables tags
        for(auto& tRandomVar : mRandomVars)
        {
            tAttributes.push_back(tRandomVar.mAttribute);
        }

        // append deterministic variables tags
        for(auto& tDeterministicVar : mDeterministicVars)
        {
            tAttributes.push_back(tDeterministicVar.mAttribute);
        }

        return (tAttributes);
    }

    void append(const std::string &aTag,
                const std::string &aAttribute,
                const Plato::srom::Statistics &aStats)
    {
        Plato::srom::RandomVariable tVariable;
        tVariable.mTag = aTag;
        tVariable.mStatistics = aStats;
        tVariable.mAttribute = aAttribute;
        mRandomVars.push_back(tVariable);
    }
    std::vector<Plato::srom::RandomVariable> randomVars() const
    {
        return (mRandomVars);
    }

    void append(const std::string &aTag,
                const std::string &aAttribute,
                const std::string &aValue)
    {
        Plato::srom::DeterministicVariable tVariable;
        tVariable.mTag = aTag;
        tVariable.mValue = aValue;
        tVariable.mAttribute = aAttribute;
        mDeterministicVars.push_back(tVariable);
    }
    std::vector<Plato::srom::DeterministicVariable> deterministicVars() const
    {
        return (mDeterministicVars);
    }
};
// struct Material

using MaterialProps = std::map<std::string,std::pair<std::string, std::string>>;
struct RandomMaterial
{
private:
    double mProbability;       /*!< probability for this material instance */
    std::string mBlockID;      /*!< element block identification number */
    std::string mCategory;     /*!< isotropic, orthotropic, anisotropic, etc. */
    std::string mMaterialID;   /*!< material identification number */

    MaterialProps mTags; /*!< map between material property tag/label and its attribute and value, i.e. tags(tag,(attribute,value)) */

public:
    void check() const
    {
        if(mBlockID.empty())
        {
            THROWERR("SROM::RandomMaterial: Material block identification number is not defined.")
        }

        if(mMaterialID.empty())
        {
            THROWERR("SROM::RandomMaterial: Material identification number is not defined.")
        }

        if(mTags.empty())
        {
            std::ostringstream tMsg;
            tMsg << "SROM::RandomMaterial: Tags for random material with material identification number '"
                << mMaterialID << "', block identification number '" << mBlockID << ", and category '"<< mCategory
                << "' are not defined.";
            THROWERR(tMsg.str().c_str())
        }

        if(std::isfinite(mProbability) == false)
        {
            std::ostringstream tMsg;
            tMsg << "SROM::RandomMaterial: Probability for random material with material identification number '"
                << mMaterialID << "', block identification number '" << mBlockID << ", and category '"<< mCategory
                << "' is not a finite number.";
            THROWERR(tMsg.str().c_str())
        }

        if(mProbability <= static_cast<double>(0.0))
        {
            std::ostringstream tMsg;
            tMsg << "SROM::RandomMaterial: Probability for random material with material identification number '"
                << mMaterialID << "', block identification number '" << mBlockID << ", and category '"<< mCategory
                << "' is not a positive number or it's set to zero." << "The probability value is set to '"
                << mProbability << "'.";
            THROWERR(tMsg.str().c_str())
        }
    }

    void blockID(const std::string& aID)
    {
        mBlockID = aID;
    }
    std::string blockID() const
    {
        return (mBlockID);
    }

    void materialID(const std::string& aID)
    {
        mMaterialID = aID;
    }
    std::string materialID() const
    {
        return (mMaterialID);
    }

    void category(const std::string& aCategory)
    {
        mCategory = aCategory;
    }
    std::string category() const
    {
        return mCategory;
    }

    void probability(const double& aProbability)
    {
        mProbability = aProbability;
    }
    double probability() const
    {
        return mProbability;
    }

    std::string value(const std::string& aTag) const
    {
        auto tItr = mTags.find(aTag);
        if(tItr == mTags.end())
        {
            std::ostringstream tMsg;
            tMsg << "Random Material: Material property with tag '" << aTag.c_str() << "' is not defined.";
            THROWERR(tMsg.str().c_str())
        }
        return (tItr->second.second);
    }
    std::string attribute(const std::string& aTag) const
    {
        auto tItr = mTags.find(aTag);
        if(tItr == mTags.end())
        {
            std::ostringstream tMsg;
            tMsg << "Random Material: Material property with tag '" << aTag.c_str() << "' is not defined.";
            THROWERR(tMsg.str().c_str())
        }
        return (tItr->second.first);
    }
    std::vector<std::string> tags() const
    {
        std::vector<std::string> tTags;
        for(auto& tItr : mTags)
        {
            tTags.push_back(tItr.first);
        }
        return tTags;
    }
    void append(const std::string& aTag, const std::string& aAttribute, const std::string& aValue)
    {
        mTags.insert(std::pair<std::string, std::pair<std::string, std::string>>
            (aTag, std::pair<std::string, std::string>(aAttribute, aValue)));
    }
    size_t size() const
    {
        return (mTags.size());
    }
};
// struct RandomMaterial

struct RandomMaterialCase
{
private:
    double mProbability; /*!< probability associated with this random load case */
    std::string mCaseID; /*!< random material case identification number */
    std::map<std::string, Plato::srom::RandomMaterial> mMaterialCases; /*!< random material cases, map(material_id, random material) */

public:
    void caseID(const std::string& aCaseID)
    {
        mCaseID = aCaseID;
    }
    std::string caseID() const
    {
        return (mCaseID);
    }
    void probability(const double& aProbability)
    {
        mProbability = aProbability;
    }
    double probability() const
    {
        return (mProbability);
    }
    size_t numMaterials() const
    {
        return (mMaterialCases.size());
    }
    std::string probabilityToString() const
    {
        auto tOutput = Plato::srom::to_string(mProbability);
        return (tOutput);
    }
    std::string blockID(const std::string& aMaterialID) const
    {
        auto tItr = mMaterialCases.find(aMaterialID);
        if(tItr == mMaterialCases.end())
        {
            std::ostringstream tMsg;
            tMsg << "Random Material Case: Material with identification number '" << aMaterialID.c_str() << "' is not defined.";
            THROWERR(tMsg.str().c_str())
        }
        return (tItr->second.blockID());
    }
    std::string category(const std::string& aMaterialID) const
    {
        auto tItr = mMaterialCases.find(aMaterialID);
        if(tItr == mMaterialCases.end())
        {
            std::ostringstream tMsg;
            tMsg << "Random Material Case: Material with identification number '" << aMaterialID.c_str() << "' is not defined.";
            THROWERR(tMsg.str().c_str())
        }
        return (tItr->second.category());
    }
    std::vector<std::string> tags(const std::string& aMaterialID) const
    {
        auto tItr = mMaterialCases.find(aMaterialID);
        if(tItr == mMaterialCases.end())
        {
            std::ostringstream tMsg;
            tMsg << "Random Material Case: Material with identification number '" << aMaterialID.c_str() << "' is not defined.";
            THROWERR(tMsg.str().c_str())
        }
        return (tItr->second.tags());
    }
    std::string value(const std::string& aMaterialID, const std::string& aTag) const
    {
        auto tItr = mMaterialCases.find(aMaterialID);
        if(tItr == mMaterialCases.end())
        {
            std::ostringstream tMsg;
            tMsg << "Random Material Case: Material with identification number '" << aMaterialID.c_str() << "' is not defined.";
            THROWERR(tMsg.str().c_str())
        }
        return (tItr->second.value(aTag));
    }
    std::string attribute(const std::string& aMaterialID, const std::string& aTag) const
    {
        auto tItr = mMaterialCases.find(aMaterialID);
        if(tItr == mMaterialCases.end())
        {
            std::ostringstream tMsg;
            tMsg << "Random Material Case: Material with identification number '" << aMaterialID.c_str() << "' is not defined.";
            THROWERR(tMsg.str().c_str())
        }
        return (tItr->second.attribute(aTag));
    }
    std::vector<std::string> materialIDs() const
    {
        std::vector<std::string> tIDs;
        for(auto& tItr : mMaterialCases)
        {
            tIDs.push_back(tItr.first);
        }
        return tIDs;
    }
    std::vector<Plato::srom::RandomMaterial> materials() const
    {
        std::vector<Plato::srom::RandomMaterial> tMaterials;
        for(auto& tItr : mMaterialCases)
        {
            tMaterials.push_back(tItr.second);
        }
        return tMaterials;
    }
    void append(const std::string& aMaterialID, const Plato::srom::RandomMaterial& aMaterial)
    {
        mMaterialCases.insert(std::pair<std::string, Plato::srom::RandomMaterial>(aMaterialID, aMaterial));
    }
};
// struct RandomMaterialCase

/******************************************************************************//**
 * \brief Assign an identification number to each randome material case in the set.
 * \param [out] aRandomMaterialCases random material cases
**********************************************************************************/
inline void assign_material_case_identification_number(std::vector<Plato::srom::RandomMaterialCase> & aRandomMaterialCases)
{
    if(aRandomMaterialCases.empty())
    {
        THROWERR("Assign Material Case Identification Number: Input container of random material cases is empty.")
    }

    Plato::UniqueCounter tCaseCounter;
    tCaseCounter.mark(0);
    for(auto& tRandomMaterialCase : aRandomMaterialCases)
    {
        auto tID = std::to_string(tCaseCounter.assignNextUnique());
        tRandomMaterialCase.caseID(tID);
    }
}

inline void define_deterministic_material(const Plato::srom::Material& aInMaterial, Plato::srom::RandomMaterial& aOutMaterial)
{
    aOutMaterial.probability(1.0);
    aOutMaterial.blockID(aInMaterial.blockID());
    aOutMaterial.category(aInMaterial.category());
    aOutMaterial.materialID(aInMaterial.materialID());
    auto tDeterministicVars = aInMaterial.deterministicVars();
    for (auto& tVariable : tDeterministicVars)
    {
        aOutMaterial.append(tVariable.mTag, tVariable.mAttribute, tVariable.mValue);
    }
}

inline void append_deterministic_materials(const std::vector<Plato::srom::Material>& tDeterministicMaterials,
                                           std::vector<Plato::srom::RandomMaterialCase>& RandomMaterialCases)
{
    if(tDeterministicMaterials.empty())
    {
        return;
    }

    for (auto& tMaterialCase : RandomMaterialCases)
    {
        for (auto& tMaterial : tDeterministicMaterials)
        {
            if (tMaterial.isRandom())
            {
                std::ostringstream tMsg;
                tMsg << "Append Deterministic Materials: Material with identification number '" << tMaterial.materialID()
                    << "' in block with identification number '" << tMaterial.blockID() << "' is not defined.";
                THROWERR(tMsg.str().c_str())
            }
            tMaterial.check();
            Plato::srom::RandomMaterial tOutMaterial;
            Plato::srom::define_deterministic_material(tMaterial, tOutMaterial);
            tOutMaterial.check();
            tMaterialCase.append(tMaterial.materialID(), tOutMaterial);
        }
    }
}

inline void split_random_and_deterministic_materials(const std::vector<Plato::srom::Material>& aMaterials,
                                                     std::vector<Plato::srom::Material>& aRandomMats,
                                                     std::vector<Plato::srom::Material>& aDeterministicMats)
{
    if(aMaterials.empty())
    {
        THROWERR("Expand Random And Deterministic Materials: Input material container is empty.");
    }

    for(auto& tMaterial : aMaterials)
    {
        if(tMaterial.isDeterministic())
        {
            aDeterministicMats.push_back(tMaterial);
        }
        else
        {
            aRandomMats.push_back(tMaterial);
        }
    }
}

inline void initialize_random_material_set(const Plato::srom::Material& aMaterial,
                                           const Plato::srom::SromVariable& aSromVariable,
                                           std::vector<Plato::srom::RandomMaterial>& aRandomMaterialSet)
{
    for(auto& tSample : aSromVariable.mSampleProbPairs.mSamples)
    {
        Plato::srom::RandomMaterial tRandomMaterial;
        tRandomMaterial.blockID(aMaterial.blockID());
        tRandomMaterial.category(aMaterial.category());
        tRandomMaterial.materialID(aMaterial.materialID());

        auto tIndex = &tSample - &aSromVariable.mSampleProbPairs.mSamples[0];
        tRandomMaterial.probability(aSromVariable.mSampleProbPairs.mProbabilities[tIndex]);

        auto tValue = Plato::srom::to_string(aSromVariable.mSampleProbPairs.mSamples[tIndex]);
        tRandomMaterial.append(aSromVariable.mTag, aSromVariable.mAttribute, tValue);

        aRandomMaterialSet.push_back(tRandomMaterial);
    }
}

inline void update_random_material_set(const Plato::srom::Material& aMaterial,
                                       const Plato::srom::SromVariable& aSromVariable,
                                       std::vector<Plato::srom::RandomMaterial>& aRandomMaterialSet)
{
    auto tOriginalRandomMaterialSet = aRandomMaterialSet;

    for(auto& tRandomMaterial : aRandomMaterialSet)
    {
        auto tUpdatedProbability = tRandomMaterial.probability()
            * aSromVariable.mSampleProbPairs.mProbabilities[0];
        tRandomMaterial.probability(tUpdatedProbability);
        auto tValue = Plato::srom::to_string(aSromVariable.mSampleProbPairs.mSamples[0]);
        tRandomMaterial.append(aSromVariable.mTag, aSromVariable.mAttribute, tValue);
    }

    auto tBeginItr = aSromVariable.mSampleProbPairs.mSamples.begin();
    for(auto tItr = std::next(tBeginItr, 1); tItr != aSromVariable.mSampleProbPairs.mSamples.end(); tItr++)
    {
        for(auto& tOriginalRandomMaterial : tOriginalRandomMaterialSet)
        {
            Plato::srom::RandomMaterial tNewRandomMaterial;
            tNewRandomMaterial.blockID(aMaterial.blockID());
            tNewRandomMaterial.category(aMaterial.category());
            tNewRandomMaterial.materialID(aMaterial.materialID());

            auto tIndex = std::distance(tBeginItr, tItr);
            auto tUpdatedProbability = tOriginalRandomMaterial.probability()
                * aSromVariable.mSampleProbPairs.mProbabilities[tIndex];
            tNewRandomMaterial.probability(tUpdatedProbability);
            auto tValue = Plato::srom::to_string(aSromVariable.mSampleProbPairs.mSamples[tIndex]);
            tNewRandomMaterial.append(aSromVariable.mTag, aSromVariable.mAttribute, tValue);

            auto tOriginalTags = tOriginalRandomMaterial.tags();
            for(auto& tTag : tOriginalTags)
            {
                auto tValue = tOriginalRandomMaterial.value(tTag);
                auto tAttribute = tOriginalRandomMaterial.attribute(tTag);
                tNewRandomMaterial.append(tTag, tAttribute, tValue);
            }

            aRandomMaterialSet.push_back(tNewRandomMaterial);
        }
    }
}

inline void append_random_material_properties
(const Plato::srom::Material& aMaterial,
 const std::vector<Plato::srom::SromVariable>& aSromVariables,
 std::vector<Plato::srom::RandomMaterial>& aRandomMaterialSet)
{
    for(auto& tSromVar : aSromVariables)
    {
        if(aRandomMaterialSet.empty())
        {
            Plato::srom::initialize_random_material_set(aMaterial, tSromVar, aRandomMaterialSet);
        }
        else
        {
            Plato::srom::update_random_material_set(aMaterial, tSromVar, aRandomMaterialSet);
        }
    }
}

inline void append_deterministic_material_properties
(const Plato::srom::Material &aMaterial,
 std::vector<Plato::srom::RandomMaterial>& aMaterialsSet)
{
    auto tDeterministicVars = aMaterial.deterministicVars();
    for(auto& tRandomMaterial : aMaterialsSet)
    {
        for(auto& tVar : tDeterministicVars)
        {
            tRandomMaterial.append(tVar.mTag, tVar.mAttribute, tVar.mValue);
        }
    }
}

inline void build_random_material_set
(const Plato::srom::Material& aMaterial,
 const std::vector<Plato::srom::SromVariable>& aSromVariables,
 std::vector<Plato::srom::RandomMaterial>& aRandomMaterials)
{
    if(aSromVariables.empty())
    {
        THROWERR("Build Random Material: Input set of SROM variables is empty.")
    }

    Plato::srom::append_random_material_properties(aMaterial, aSromVariables, aRandomMaterials);
    Plato::srom::append_deterministic_material_properties(aMaterial, aRandomMaterials);
}

inline void initialize_random_material_cases
(const std::vector<Plato::srom::RandomMaterial>& aRandomMaterials,
 std::vector<Plato::srom::RandomMaterialCase>& aRandomMaterialCases)
{
    if(aRandomMaterials.empty())
    {
        THROWERR("Initialize Random Material Cases: Input container of random materials is empty.");
    }

    for(auto& tRandomMaterial : aRandomMaterials)
    {
        tRandomMaterial.check();
        Plato::srom::RandomMaterialCase tRandomMaterialCase;
        auto tMaterialID = tRandomMaterial.materialID();
        tRandomMaterialCase.append(tMaterialID, tRandomMaterial);
        tRandomMaterialCase.probability(tRandomMaterial.probability());
        aRandomMaterialCases.push_back(tRandomMaterialCase);
    }
}

inline void update_random_material_cases
(const std::vector<Plato::srom::RandomMaterial>& aRandomMaterials,
 std::vector<Plato::srom::RandomMaterialCase>& aRandomMaterialCases)
{
    if(aRandomMaterials.empty())
    {
        THROWERR("Update Random Material Cases: Input container of random materials is empty.");
    }

    auto tOriginalRandomMaterialCases = aRandomMaterialCases;

    for(auto& tRandomMaterialCase : aRandomMaterialCases)
    {
        aRandomMaterials[0].check();
        auto tUpdatedProbability = tRandomMaterialCase.probability()
            * aRandomMaterials[0].probability();
        tRandomMaterialCase.probability(tUpdatedProbability);
        auto tID = aRandomMaterials[0].materialID();
        tRandomMaterialCase.append(tID, aRandomMaterials[0]);
    }

    auto tBeginItr = aRandomMaterials.begin();
    for(auto tItr = std::next(tBeginItr, 1); tItr != aRandomMaterials.end(); tItr++)
    {
        tItr->check();
        for(auto& tOriginalRandomMaterialCase : tOriginalRandomMaterialCases)
        {
            Plato::srom::RandomMaterialCase tNewRandomMaterialCase;
            auto tUpdatedProbability =
                tOriginalRandomMaterialCase.probability() * tItr->probability();
            tNewRandomMaterialCase.probability(tUpdatedProbability);
            auto tID = tItr->materialID();
            tNewRandomMaterialCase.append(tID, tItr.operator*());

            auto tOriginalMaterials = tOriginalRandomMaterialCase.materials();
            for(auto& tMaterial : tOriginalMaterials)
            {
                auto tID = tMaterial.materialID();
                tNewRandomMaterialCase.append(tID, tMaterial);
            }

            aRandomMaterialCases.push_back(tNewRandomMaterialCase);
        }
    }
}

inline void append_random_material_set
(const std::vector<Plato::srom::RandomMaterial>& aRandomMaterials,
 std::vector<Plato::srom::RandomMaterialCase>& aRandomMaterialCases)
{
    if(aRandomMaterials.empty())
    {
        THROWERR("Append Random Material Cases: Input container of random materials is empty.");
    }

    if(aRandomMaterialCases.empty())
    {
        Plato::srom::initialize_random_material_cases(aRandomMaterials, aRandomMaterialCases);
    }
    else
    {
        Plato::srom::update_random_material_cases(aRandomMaterials, aRandomMaterialCases);
    }
}

inline void check_input_set_of_materials(const std::vector<Plato::srom::Material>& aMaterials)
{
    for(auto& tMaterial : aMaterials)
    {
        if(tMaterial.undefined())
        {
            std::ostringstream tMsg;
            auto tIndex = &tMaterial - &aMaterials[0];
            tMsg << "Check Input Materials Set: Material in position '" << tIndex << "' with material identification number '"
                << tMaterial.materialID() << "', block with identification number '" << tMaterial.blockID()
                << "', and category '" << tMaterial.category() << "' is undefined.";
            THROWERR(tMsg.str().c_str())
        }
    }
}

inline void build_material_sroms(const std::vector<Plato::srom::Material>& aInput,
                                 std::vector<Plato::srom::RandomMaterialCase>& aOutput)
{
    if(aInput.empty())
    {
        THROWERR("Generate Material SROMS: Input container of materials is empty.")
    }
    Plato::srom::check_input_set_of_materials(aInput);

    aOutput.clear();
    std::vector<Plato::srom::Material> tRandomMaterials, tDeterministicMaterials;
    Plato::srom::split_random_and_deterministic_materials(aInput, tRandomMaterials, tDeterministicMaterials);

    for(auto& tMaterial : tRandomMaterials)
    {
        tMaterial.check();
        if(tMaterial.isDeterministic())
        {
            THROWERR("Generate Material SROMS: Detected a deterministic material while looping over random materials set.")
        }

        std::vector<Plato::srom::SromVariable> tMySampleProbPairs;
        if(Plato::compute_sample_probability_pairs(tMaterial.randomVars(), tMySampleProbPairs) == false)
        {
            std::ostringstream tMsg;
            tMsg << "Generate Material SROMS: Failed to compute Sample-Probability pairs for material "
                << "with identification number '" << tMaterial.materialID() << "' in block with identification "
                << "number '" << tMaterial.blockID() << "'.";
            PRINTERR(tMsg.str().c_str());
        }

        std::vector<Plato::srom::RandomMaterial> tRandomMaterialSet;
        Plato::srom::build_random_material_set(tMaterial, tMySampleProbPairs, tRandomMaterialSet);
        Plato::srom::append_random_material_set(tRandomMaterialSet, aOutput);
    }

    Plato::srom::append_deterministic_materials(tDeterministicMaterials, aOutput);
    Plato::srom::assign_material_case_identification_number(aOutput);
}

}
// namespace srom

}
// namespace Plato

namespace UncertainMaterial
{

TEST(PlatoTest, SROM_ComputeSampleProbabilityPairs_HomogeneousElasticModulus_Beta)
{
    // DEFINE INPUTS
    Plato::srom::RandomVariable tMyRandomVar;
    tMyRandomVar.mTag = "elastic modulus";
    tMyRandomVar.mAttribute = "homogeneous";
    tMyRandomVar.mStatistics.mDistribution = "beta";
    tMyRandomVar.mStatistics.mMean = "1e9";
    tMyRandomVar.mStatistics.mUpperBound = "1e10";
    tMyRandomVar.mStatistics.mLowerBound = "1e8";
    tMyRandomVar.mStatistics.mStandardDeviation = "2e8";
    tMyRandomVar.mStatistics.mNumSamples = "3";
    std::vector<Plato::srom::RandomVariable> tRandomVarsSet;
    tRandomVarsSet.push_back(tMyRandomVar);

    // SOLVE SROM PROBLEM
    std::vector<Plato::srom::SromVariable> tMySampleProbPairs;
    Plato::compute_sample_probability_pairs(tRandomVarsSet, tMySampleProbPairs);

    // TEST RESULTS
    ASSERT_EQ(1u, tMySampleProbPairs.size());
    ASSERT_STREQ("elastic modulus", tMySampleProbPairs[0].mTag.c_str());
    ASSERT_STREQ("homogeneous", tMySampleProbPairs[0].mAttribute.c_str());
    ASSERT_EQ(3, tMySampleProbPairs[0].mSampleProbPairs.mNumSamples);
    ASSERT_EQ(3u, tMySampleProbPairs[0].mSampleProbPairs.mSamples.size());
    ASSERT_EQ(3u, tMySampleProbPairs[0].mSampleProbPairs.mProbabilities.size());

    // TEST RESULTS
    double tSum = 0;
    double tTolerance = 1e-4;
    const Plato::srom::SampleProbabilityPairs& tSampleProbabilityPairs = tMySampleProbPairs[0].mSampleProbPairs;
    std::vector<double> tGoldSamples = {1312017818.6197019, 659073448.54796219, 656356599.33196139};
    std::vector<double> tGoldProbabilities = {0.43210087774761252, 0.31840063469163404, 0.24868340186995475};
    for(int tIndex = 0; tIndex < tSampleProbabilityPairs.mNumSamples; tIndex++)
    {
        tSum += tSampleProbabilityPairs.mProbabilities[tIndex];
        ASSERT_NEAR(tGoldSamples[tIndex], tSampleProbabilityPairs.mSamples[tIndex], tTolerance);
        ASSERT_NEAR(tGoldProbabilities[tIndex], tSampleProbabilityPairs.mProbabilities[tIndex], tTolerance);
    }
    tTolerance = 1e-2;
    ASSERT_NEAR(1.0, tSum, tTolerance);

    std::system("rm -f plato_cdf_output.txt");
    std::system("rm -f plato_srom_diagnostics.txt");
    std::system("rm -f plato_ksal_algorithm_diagnostics.txt");
}

TEST(PlatoTest, SROM_SplitRandomAndDeterministicMaterials_Error)
{
    std::vector<Plato::srom::Material> tMaterials;
    std::vector<Plato::srom::Material> tRandomMaterial, tDeterministicMaterial;
    EXPECT_THROW(Plato::srom::split_random_and_deterministic_materials(tMaterials, tRandomMaterial, tDeterministicMaterial), std::runtime_error);
}

TEST(PlatoTest, SROM_SplitRandomAndDeterministicMaterials)
{
    // DEFINE MATERIAL ONE - RANDOM
    Plato::srom::Material tMaterialOne;
    tMaterialOne.blockID("1");
    tMaterialOne.materialID("1");
    tMaterialOne.category("isotropic");
    tMaterialOne.append("poissons ratio", "homogeneous", "0.3");
    Plato::srom::Statistics tElasticModulusStats;
    tElasticModulusStats.mDistribution = "beta";
    tElasticModulusStats.mMean = "1e9";
    tElasticModulusStats.mUpperBound = "1e10";
    tElasticModulusStats.mLowerBound = "1e8";
    tElasticModulusStats.mStandardDeviation = "2e8";
    tElasticModulusStats.mNumSamples = "3";
    tMaterialOne.append("elastic modulus", "homogeneous", tElasticModulusStats);

    // DEFINE MATERIAL TWO - DETERMINISTIC
    Plato::srom::Material tMaterialTwo;
    tMaterialTwo.blockID("22");
    tMaterialTwo.materialID("10");
    tMaterialTwo.category("isotropic");
    tMaterialTwo.append("poissons ratio", "homogeneous", "0.35");
    tMaterialTwo.append("elastic modulus", "homogeneous", "2.5e8");

    // APPEND MATERIALS TO MATERIALS SET
    std::vector<Plato::srom::Material> tMaterials;
    tMaterials.push_back(tMaterialOne);
    tMaterials.push_back(tMaterialTwo);

    // SPLIT MATERIALS INTO RANDOM AND DETERMINIST MATERIAL SETS
    std::vector<Plato::srom::Material> tRandomMaterial, tDeterministicMaterial;
    Plato::srom::split_random_and_deterministic_materials(tMaterials, tRandomMaterial, tDeterministicMaterial);

    // 1) TEST RANDOM MATERIAL
    ASSERT_EQ(1u, tRandomMaterial.size());
    ASSERT_TRUE(tRandomMaterial[0].isRandom());
    ASSERT_FALSE(tRandomMaterial[0].isDeterministic());
    ASSERT_STREQ("1", tRandomMaterial[0].blockID().c_str());
    ASSERT_STREQ("1", tRandomMaterial[0].materialID().c_str());
    ASSERT_STREQ("isotropic", tRandomMaterial[0].category().c_str());

    auto tRandomVars = tRandomMaterial[0].randomVars();
    ASSERT_EQ(1u, tRandomVars.size());
    ASSERT_STREQ("elastic modulus", tRandomVars[0].mTag.c_str());
    ASSERT_STREQ("homogeneous", tRandomVars[0].mAttribute.c_str());
    ASSERT_STREQ("1e9", tRandomVars[0].mStatistics.mMean.c_str());
    ASSERT_STREQ("3", tRandomVars[0].mStatistics.mNumSamples.c_str());
    ASSERT_STREQ("1e8", tRandomVars[0].mStatistics.mLowerBound.c_str());
    ASSERT_STREQ("1e10", tRandomVars[0].mStatistics.mUpperBound.c_str());
    ASSERT_STREQ("beta", tRandomVars[0].mStatistics.mDistribution.c_str());
    ASSERT_STREQ("2e8", tRandomVars[0].mStatistics.mStandardDeviation.c_str());

    auto tDeterministicVars = tRandomMaterial[0].deterministicVars();
    ASSERT_EQ(1u, tDeterministicVars.size());
    ASSERT_STREQ("poissons ratio", tDeterministicVars[0].mTag.c_str());
    ASSERT_STREQ("homogeneous", tDeterministicVars[0].mAttribute.c_str());
    ASSERT_STREQ("0.3", tDeterministicVars[0].mValue.c_str());

    // 2) TEST DETERMINISTIC MATERIAL
    ASSERT_EQ(1u, tDeterministicMaterial.size());
    ASSERT_TRUE(tDeterministicMaterial[0].isDeterministic());
    ASSERT_FALSE(tDeterministicMaterial[0].isRandom());
    tRandomVars = tDeterministicMaterial[0].randomVars();
    ASSERT_EQ(0u, tRandomVars.size());

    tDeterministicVars = tDeterministicMaterial[0].deterministicVars();
    ASSERT_EQ(2u, tDeterministicVars.size());
    ASSERT_STREQ("poissons ratio", tDeterministicVars[0].mTag.c_str());
    ASSERT_STREQ("homogeneous", tDeterministicVars[0].mAttribute.c_str());
    ASSERT_STREQ("0.35", tDeterministicVars[0].mValue.c_str());
    ASSERT_STREQ("elastic modulus", tDeterministicVars[1].mTag.c_str());
    ASSERT_STREQ("homogeneous", tDeterministicVars[1].mAttribute.c_str());
    ASSERT_STREQ("2.5e8", tDeterministicVars[1].mValue.c_str());
}

TEST(PlatoTest, SROM_CheckInputSetOfMaterials_Error)
{
    // DEFINE MATERIAL ONE - RANDOM
    Plato::srom::Material tMaterialOne;
    tMaterialOne.blockID("1");
    tMaterialOne.materialID("1");
    tMaterialOne.category("isotropic");
    tMaterialOne.append("poissons ratio", "homogeneous", "0.3");
    Plato::srom::Statistics tElasticModulusStats;
    tElasticModulusStats.mDistribution = "beta";
    tElasticModulusStats.mMean = "1e9";
    tElasticModulusStats.mUpperBound = "1e10";
    tElasticModulusStats.mLowerBound = "1e8";
    tElasticModulusStats.mStandardDeviation = "2e8";
    tElasticModulusStats.mNumSamples = "3";
    tMaterialOne.append("elastic modulus", "homogeneous", tElasticModulusStats);

    // DEFINE MATERIAL TWO - DETERMINISTIC
    Plato::srom::Material tMaterialTwo;
    tMaterialTwo.blockID("22");
    tMaterialTwo.materialID("10");
    tMaterialTwo.category("isotropic");

    std::vector<Plato::srom::Material> tInput;
    tInput.push_back(tMaterialOne);
    tInput.push_back(tMaterialTwo);
    EXPECT_THROW(Plato::srom::check_input_set_of_materials(tInput), std::runtime_error);
}

TEST(PlatoTest, SROM_InitializeRandomMaterialSet)
{
    // DEFINE MATERIAL ONE - RANDOM
    Plato::srom::Material tMaterial;
    tMaterial.blockID("1");
    tMaterial.materialID("2");
    tMaterial.category("isotropic");
    Plato::srom::Statistics tPoissonsRatioStats;
    tPoissonsRatioStats.mDistribution = "beta";
    tPoissonsRatioStats.mMean = "0.3";
    tPoissonsRatioStats.mUpperBound = "0.4";
    tPoissonsRatioStats.mLowerBound = "0.25";
    tPoissonsRatioStats.mStandardDeviation = "0.05";
    tPoissonsRatioStats.mNumSamples = "2";
    tMaterial.append("poissons ratio", "homogeneous", tPoissonsRatioStats);
    Plato::srom::Statistics tElasticModulusStats;
    tElasticModulusStats.mDistribution = "beta";
    tElasticModulusStats.mMean = "1e9";
    tElasticModulusStats.mUpperBound = "1e10";
    tElasticModulusStats.mLowerBound = "1e8";
    tElasticModulusStats.mStandardDeviation = "2e8";
    tElasticModulusStats.mNumSamples = "3";
    tMaterial.append("elastic modulus", "homogeneous", tElasticModulusStats);

    // INITIALIZE RANDOM MATERIAL SET
    Plato::srom::SromVariable tSromVariable;
    tSromVariable.mTag = "poissons ratio";
    tSromVariable.mAttribute = "homogeneous";
    tSromVariable.mSampleProbPairs.mNumSamples = 2;
    tSromVariable.mSampleProbPairs.mSamples = {0.32, 0.27};
    tSromVariable.mSampleProbPairs.mProbabilities = {0.75, 0.25};
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSet;
    EXPECT_NO_THROW(Plato::srom::initialize_random_material_set(tMaterial, tSromVariable, tRandomMaterialSet));

    const double tTolerance = 1e-4;
    ASSERT_EQ(2u, tRandomMaterialSet.size());
    const std::vector<double> tGoldProbs = {0.75, 0.25};
    const std::vector<std::string> tGoldSamples = {"0.3200000000000000", "0.2700000000000000"};
    for(auto& tRandomMaterial : tRandomMaterialSet)
    {
        ASSERT_STREQ("1", tRandomMaterial.blockID().c_str());
        ASSERT_STREQ("2", tRandomMaterial.materialID().c_str());
        ASSERT_STREQ("isotropic", tRandomMaterial.category().c_str());

        auto tIndex = &tRandomMaterial - &tRandomMaterialSet[0];
        ASSERT_NEAR(tGoldProbs[tIndex], tRandomMaterial.probability(), tTolerance);

        auto tTags = tRandomMaterial.tags();
        ASSERT_EQ(1u, tTags.size());
        for(auto& tTag : tTags)
        {
            ASSERT_STREQ("poissons ratio", tTag.c_str());
            ASSERT_STREQ("homogeneous", tRandomMaterial.attribute(tTag).c_str());
            ASSERT_STREQ(tGoldSamples[tIndex].c_str(), tRandomMaterial.value(tTag).c_str());
        }
    }
}

TEST(PlatoTest, SROM_UpdateRandomMaterialSet)
{
    // 1. DEFINE MATERIAL ONE - RANDOM
    Plato::srom::Material tMaterial;
    tMaterial.blockID("1");
    tMaterial.materialID("2");
    tMaterial.category("isotropic");
    Plato::srom::Statistics tPoissonsRatioStats;
    tPoissonsRatioStats.mDistribution = "beta";
    tPoissonsRatioStats.mMean = "0.3";
    tPoissonsRatioStats.mUpperBound = "0.4";
    tPoissonsRatioStats.mLowerBound = "0.25";
    tPoissonsRatioStats.mStandardDeviation = "0.05";
    tPoissonsRatioStats.mNumSamples = "2";
    tMaterial.append("poissons ratio", "homogeneous", tPoissonsRatioStats);
    Plato::srom::Statistics tElasticModulusStats;
    tElasticModulusStats.mDistribution = "beta";
    tElasticModulusStats.mMean = "1e9";
    tElasticModulusStats.mUpperBound = "1e10";
    tElasticModulusStats.mLowerBound = "1e8";
    tElasticModulusStats.mStandardDeviation = "2e8";
    tElasticModulusStats.mNumSamples = "3";
    tMaterial.append("elastic modulus", "homogeneous", tElasticModulusStats);

    // 2. INITIALIZE RANDOM MATERIAL SET
    Plato::srom::SromVariable tSromVariableOne;
    tSromVariableOne.mTag = "poissons ratio";
    tSromVariableOne.mAttribute = "homogeneous";
    tSromVariableOne.mSampleProbPairs.mNumSamples = 2;
    tSromVariableOne.mSampleProbPairs.mSamples = {0.32, 0.27};
    tSromVariableOne.mSampleProbPairs.mProbabilities = {0.75, 0.25};
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSet;
    EXPECT_NO_THROW(Plato::srom::initialize_random_material_set(tMaterial, tSromVariableOne, tRandomMaterialSet));

    // 3. UPDATE RANDOM MATERIAL SET
    Plato::srom::SromVariable tSromVariableTwo;
    tSromVariableTwo.mTag = "elastic modulus";
    tSromVariableTwo.mAttribute = "homogeneous";
    tSromVariableTwo.mSampleProbPairs.mNumSamples = 2;
    tSromVariableTwo.mSampleProbPairs.mSamples = {1e9, 2.5e9};
    tSromVariableTwo.mSampleProbPairs.mProbabilities = {0.45, 0.55};
    EXPECT_NO_THROW(Plato::srom::update_random_material_set(tMaterial, tSromVariableTwo, tRandomMaterialSet));
    ASSERT_EQ(4u, tRandomMaterialSet.size());

    const std::vector<double> tGoldProbs = {0.3375, 0.1125, 0.4125, 0.1375};
    const std::vector<std::string> tGoldTags = {"elastic modulus", "poissons ratio"};
    const std::vector<std::vector<std::string>> tGoldSamples =
        { {"1000000000.0000000000000000", "0.3200000000000000"},
          {"1000000000.0000000000000000", "0.2700000000000000"},
          {"2500000000.0000000000000000", "0.3200000000000000"},
          {"2500000000.0000000000000000", "0.2700000000000000"}};

    double tProbSum = 0;
    const double tTolerance = 1e-4;
    for(auto& tRandomMaterial : tRandomMaterialSet)
    {
        ASSERT_STREQ("1", tRandomMaterial.blockID().c_str());
        ASSERT_STREQ("2", tRandomMaterial.materialID().c_str());
        ASSERT_STREQ("isotropic", tRandomMaterial.category().c_str());

        auto tMatIndex = &tRandomMaterial - &tRandomMaterialSet[0];
        ASSERT_NEAR(tGoldProbs[tMatIndex], tRandomMaterial.probability(), tTolerance);
        tProbSum += tRandomMaterial.probability();

        auto tTags = tRandomMaterial.tags();
        ASSERT_EQ(2u, tTags.size());
        for(auto& tTag : tTags)
        {
            auto tTagIndex = &tTag - &tTags[0];
            ASSERT_STREQ(tGoldTags[tTagIndex].c_str(), tTag.c_str());
            ASSERT_STREQ("homogeneous", tRandomMaterial.attribute(tTag).c_str());
            ASSERT_STREQ(tGoldSamples[tMatIndex][tTagIndex].c_str(), tRandomMaterial.value(tTag).c_str());
        }
    }
    ASSERT_NEAR(1.0, tProbSum, tTolerance);
}

TEST(PlatoTest, SROM_AppendRandomMaterialProperties)
{
    // 1. DEFINE MATERIAL ONE - RANDOM
    Plato::srom::Material tMaterial;
    tMaterial.blockID("1");
    tMaterial.materialID("2");
    tMaterial.category("isotropic");
    Plato::srom::Statistics tPoissonsRatioStats;
    tPoissonsRatioStats.mDistribution = "beta";
    tPoissonsRatioStats.mMean = "0.3";
    tPoissonsRatioStats.mUpperBound = "0.4";
    tPoissonsRatioStats.mLowerBound = "0.25";
    tPoissonsRatioStats.mStandardDeviation = "0.05";
    tPoissonsRatioStats.mNumSamples = "2";
    tMaterial.append("poissons ratio", "homogeneous", tPoissonsRatioStats);
    Plato::srom::Statistics tElasticModulusStats;
    tElasticModulusStats.mDistribution = "beta";
    tElasticModulusStats.mMean = "1e9";
    tElasticModulusStats.mUpperBound = "1e10";
    tElasticModulusStats.mLowerBound = "1e8";
    tElasticModulusStats.mStandardDeviation = "2e8";
    tElasticModulusStats.mNumSamples = "3";
    tMaterial.append("elastic modulus", "homogeneous", tElasticModulusStats);

    // 2. INITIALIZE SROM VARIABLE SET
    Plato::srom::SromVariable tSromVariableOne;
    tSromVariableOne.mTag = "poissons ratio";
    tSromVariableOne.mAttribute = "homogeneous";
    tSromVariableOne.mSampleProbPairs.mNumSamples = 2;
    tSromVariableOne.mSampleProbPairs.mSamples = {0.32, 0.27};
    tSromVariableOne.mSampleProbPairs.mProbabilities = {0.75, 0.25};

    Plato::srom::SromVariable tSromVariableTwo;
    tSromVariableTwo.mTag = "elastic modulus";
    tSromVariableTwo.mAttribute = "homogeneous";
    tSromVariableTwo.mSampleProbPairs.mNumSamples = 2;
    tSromVariableTwo.mSampleProbPairs.mSamples = {1e9, 2.5e9};
    tSromVariableTwo.mSampleProbPairs.mProbabilities = {0.45, 0.55};

    std::vector<Plato::srom::SromVariable> tSromVariableSet;
    tSromVariableSet.push_back(tSromVariableOne);
    tSromVariableSet.push_back(tSromVariableTwo);

    // 3. APPEND RANDOM MATERIAL PROPERTIES TO RANDOM MATERIAL SET
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSet;
    EXPECT_NO_THROW(Plato::srom::append_random_material_properties(tMaterial, tSromVariableSet, tRandomMaterialSet));
    ASSERT_EQ(4u, tRandomMaterialSet.size());

    const std::vector<double> tGoldProbs = {0.3375, 0.1125, 0.4125, 0.1375};
    const std::vector<std::string> tGoldTags = {"elastic modulus", "poissons ratio"};
    const std::vector<std::vector<std::string>> tGoldSamples =
        { {"1000000000.0000000000000000", "0.3200000000000000"},
          {"1000000000.0000000000000000", "0.2700000000000000"},
          {"2500000000.0000000000000000", "0.3200000000000000"},
          {"2500000000.0000000000000000", "0.2700000000000000"}};

    double tProbSum = 0;
    const double tTolerance = 1e-4;
    for(auto& tRandomMaterial : tRandomMaterialSet)
    {
        ASSERT_STREQ("1", tRandomMaterial.blockID().c_str());
        ASSERT_STREQ("2", tRandomMaterial.materialID().c_str());
        ASSERT_STREQ("isotropic", tRandomMaterial.category().c_str());

        auto tMatIndex = &tRandomMaterial - &tRandomMaterialSet[0];
        ASSERT_NEAR(tGoldProbs[tMatIndex], tRandomMaterial.probability(), tTolerance);
        tProbSum += tRandomMaterial.probability();

        auto tTags = tRandomMaterial.tags();
        ASSERT_EQ(2u, tTags.size());
        for(auto& tTag : tTags)
        {
            auto tTagIndex = &tTag - &tTags[0];
            ASSERT_STREQ(tGoldTags[tTagIndex].c_str(), tTag.c_str());
            ASSERT_STREQ("homogeneous", tRandomMaterial.attribute(tTag).c_str());
            ASSERT_STREQ(tGoldSamples[tMatIndex][tTagIndex].c_str(), tRandomMaterial.value(tTag).c_str());
        }
    }
    ASSERT_NEAR(1.0, tProbSum, tTolerance);
}

TEST(PlatoTest, SROM_AppendDeterministicMaterialProperties)
{
    // DEFINE RANDOM MATERIAL
    Plato::srom::Material tMaterial;
    tMaterial.blockID("1");
    tMaterial.materialID("2");
    tMaterial.category("isotropic");
    Plato::srom::Statistics tPoissonsRatioStats;
    tPoissonsRatioStats.mDistribution = "beta";
    tPoissonsRatioStats.mMean = "0.3";
    tPoissonsRatioStats.mUpperBound = "0.4";
    tPoissonsRatioStats.mLowerBound = "0.25";
    tPoissonsRatioStats.mStandardDeviation = "0.05";
    tPoissonsRatioStats.mNumSamples = "2";
    tMaterial.append("poissons ratio", "homogeneous", tPoissonsRatioStats);
    tMaterial.append("elastic modulus", "homogeneous", "1e9");
    auto tTags = tMaterial.tags();
    ASSERT_EQ(2u, tTags.size());
    ASSERT_STREQ("poissons ratio", tTags[0].c_str());
    ASSERT_STREQ("elastic modulus", tTags[1].c_str());

    // INITIALIZE RANDOM MATERIAL SET
    Plato::srom::SromVariable tSromVariable;
    tSromVariable.mTag = "poissons ratio";
    tSromVariable.mAttribute = "homogeneous";
    tSromVariable.mSampleProbPairs.mNumSamples = 2;
    tSromVariable.mSampleProbPairs.mSamples = {0.32, 0.27};
    tSromVariable.mSampleProbPairs.mProbabilities = {0.75, 0.25};

    std::vector<Plato::srom::SromVariable> tSromVariableSet;
    tSromVariableSet.push_back(tSromVariable);

    // APPEND MATERIAL PROPERTIES
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSet;
    EXPECT_NO_THROW(Plato::srom::append_random_material_properties(tMaterial, tSromVariableSet, tRandomMaterialSet));
    ASSERT_EQ(2u, tRandomMaterialSet.size());
    EXPECT_NO_THROW(Plato::srom::append_deterministic_material_properties(tMaterial, tRandomMaterialSet));

    const double tTolerance = 1e-4;
    const std::vector<double> tGoldProbs = {0.75, 0.25};
    const std::vector<std::string> tGoldTags = {"elastic modulus", "poissons ratio"};
    const std::vector<std::vector<std::string>> tGoldSamples = { {"1e9", "0.3200000000000000"}, {"1e9", "0.2700000000000000"}};

    for(auto& tRandomMaterial : tRandomMaterialSet)
    {
        ASSERT_STREQ("1", tRandomMaterial.blockID().c_str());
        ASSERT_STREQ("2", tRandomMaterial.materialID().c_str());
        ASSERT_STREQ("isotropic", tRandomMaterial.category().c_str());

        auto tMatIndex = &tRandomMaterial - &tRandomMaterialSet[0];
        ASSERT_NEAR(tGoldProbs[tMatIndex], tRandomMaterial.probability(), tTolerance);

        auto tTags = tRandomMaterial.tags();
        ASSERT_EQ(2u, tTags.size());
        for(auto& tTag : tTags)
        {
            auto tTagIndex = &tTag - &tTags[0];
            ASSERT_STREQ(tGoldTags[tTagIndex].c_str(), tTag.c_str());
            ASSERT_STREQ("homogeneous", tRandomMaterial.attribute(tTag).c_str());
            ASSERT_STREQ(tGoldSamples[tMatIndex][tTagIndex].c_str(), tRandomMaterial.value(tTag).c_str());
        }
    }
}

TEST(PlatoTest, SROM_BuildRandomMaterialSet)
{
    // DEFINE RANDOM MATERIAL
    Plato::srom::Material tMaterial;
    tMaterial.blockID("1");
    tMaterial.materialID("2");
    tMaterial.category("isotropic");
    Plato::srom::Statistics tPoissonsRatioStats;
    tPoissonsRatioStats.mDistribution = "beta";
    tPoissonsRatioStats.mMean = "0.3";
    tPoissonsRatioStats.mUpperBound = "0.4";
    tPoissonsRatioStats.mLowerBound = "0.25";
    tPoissonsRatioStats.mStandardDeviation = "0.05";
    tPoissonsRatioStats.mNumSamples = "2";
    tMaterial.append("poissons ratio", "homogeneous", tPoissonsRatioStats);
    tMaterial.append("elastic modulus", "homogeneous", "1e9");
    auto tTags = tMaterial.tags();
    ASSERT_EQ(2u, tTags.size());
    ASSERT_STREQ("poissons ratio", tTags[0].c_str());
    ASSERT_STREQ("elastic modulus", tTags[1].c_str());

    // INITIALIZE RANDOM MATERIAL SET
    Plato::srom::SromVariable tSromVariable;
    tSromVariable.mTag = "poissons ratio";
    tSromVariable.mAttribute = "homogeneous";
    tSromVariable.mSampleProbPairs.mNumSamples = 2;
    tSromVariable.mSampleProbPairs.mSamples = {0.32, 0.27};
    tSromVariable.mSampleProbPairs.mProbabilities = {0.75, 0.25};

    std::vector<Plato::srom::SromVariable> tSromVariableSet;
    tSromVariableSet.push_back(tSromVariable);

    // BUILD RANDOM MATERIAL SET
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSet;
    EXPECT_NO_THROW(Plato::srom::build_random_material_set(tMaterial, tSromVariableSet, tRandomMaterialSet));

    const double tTolerance = 1e-4;
    const std::vector<double> tGoldProbs = {0.75, 0.25};
    const std::vector<std::string> tGoldTags = {"elastic modulus", "poissons ratio"};
    const std::vector<std::vector<std::string>> tGoldSamples = { {"1e9", "0.3200000000000000"}, {"1e9", "0.2700000000000000"}};

    for(auto& tRandomMaterial : tRandomMaterialSet)
    {
        ASSERT_STREQ("1", tRandomMaterial.blockID().c_str());
        ASSERT_STREQ("2", tRandomMaterial.materialID().c_str());
        ASSERT_STREQ("isotropic", tRandomMaterial.category().c_str());

        auto tMatIndex = &tRandomMaterial - &tRandomMaterialSet[0];
        ASSERT_NEAR(tGoldProbs[tMatIndex], tRandomMaterial.probability(), tTolerance);

        auto tTags = tRandomMaterial.tags();
        ASSERT_EQ(2u, tTags.size());
        for(auto& tTag : tTags)
        {
            auto tTagIndex = &tTag - &tTags[0];
            ASSERT_STREQ(tGoldTags[tTagIndex].c_str(), tTag.c_str());
            ASSERT_STREQ("homogeneous", tRandomMaterial.attribute(tTag).c_str());
            ASSERT_STREQ(tGoldSamples[tMatIndex][tTagIndex].c_str(), tRandomMaterial.value(tTag).c_str());
        }
    }
}

TEST(PlatoTest, SROM_InitializeRandomMaterialCases_Error)
{
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSet;
    std::vector<Plato::srom::RandomMaterialCase> tRandomMaterialCases;
    EXPECT_THROW(Plato::srom::initialize_random_material_cases(tRandomMaterialSet, tRandomMaterialCases), std::runtime_error);
}

TEST(PlatoTest, SROM_InitializeRandomMaterialCases)
{
    // DEFINE RANDOM MATERIAL
    Plato::srom::Material tMaterial;
    tMaterial.blockID("1");
    tMaterial.materialID("2");
    tMaterial.category("isotropic");
    Plato::srom::Statistics tPoissonsRatioStats;
    tPoissonsRatioStats.mDistribution = "beta";
    tPoissonsRatioStats.mMean = "0.3";
    tPoissonsRatioStats.mUpperBound = "0.4";
    tPoissonsRatioStats.mLowerBound = "0.25";
    tPoissonsRatioStats.mStandardDeviation = "0.05";
    tPoissonsRatioStats.mNumSamples = "2";
    tMaterial.append("poissons ratio", "homogeneous", tPoissonsRatioStats);
    tMaterial.append("elastic modulus", "homogeneous", "1e9");
    auto tTags = tMaterial.tags();
    ASSERT_EQ(2u, tTags.size());
    ASSERT_STREQ("poissons ratio", tTags[0].c_str());
    ASSERT_STREQ("elastic modulus", tTags[1].c_str());

    // INITIALIZE RANDOM MATERIAL SET
    Plato::srom::SromVariable tSromVariable;
    tSromVariable.mTag = "poissons ratio";
    tSromVariable.mAttribute = "homogeneous";
    tSromVariable.mSampleProbPairs.mNumSamples = 2;
    tSromVariable.mSampleProbPairs.mSamples = {0.32, 0.27};
    tSromVariable.mSampleProbPairs.mProbabilities = {0.75, 0.25};

    std::vector<Plato::srom::SromVariable> tSromVariableSet;
    tSromVariableSet.push_back(tSromVariable);

    // BUILD RANDOM MATERIAL SET
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSet;
    EXPECT_NO_THROW(Plato::srom::build_random_material_set(tMaterial, tSromVariableSet, tRandomMaterialSet));
    ASSERT_EQ(2u, tRandomMaterialSet.size());

    // BUILD INITIAL RANDOM MATERIAL CASES
    std::vector<Plato::srom::RandomMaterialCase> tRandomMaterialCases;
    EXPECT_NO_THROW(Plato::srom::initialize_random_material_cases(tRandomMaterialSet, tRandomMaterialCases));
    ASSERT_EQ(2u, tRandomMaterialCases.size());


    const double tTolerance = 1e-4;
    const std::vector<double> tGoldProbs = {0.75, 0.25};
    const std::vector<std::vector<std::string>> tGoldTags =
        { {"elastic modulus", "poissons ratio"}, {"elastic modulus", "poissons ratio"} };
    const std::vector<std::vector<std::string>> tGoldSamples = { {"1e9", "0.3200000000000000"}, {"1e9", "0.2700000000000000"}};

    for(auto& tRandomMaterialCase : tRandomMaterialCases)
    {
        ASSERT_EQ(1u, tRandomMaterialCase.numMaterials());

        auto tCaseIndex = &tRandomMaterialCase - &tRandomMaterialCases[0];
        ASSERT_NEAR(tGoldProbs[tCaseIndex], tRandomMaterialCase.probability(), tTolerance);

        auto tMaterialIDs = tRandomMaterialCase.materialIDs();
        ASSERT_EQ(1u, tMaterialIDs.size());
        for(auto& tMatID : tMaterialIDs)
        {
            ASSERT_STREQ("2", tMatID.c_str());
            ASSERT_STREQ("1", tRandomMaterialCase.blockID(tMatID).c_str());
            ASSERT_STREQ("isotropic", tRandomMaterialCase.category(tMatID).c_str());

            auto tTags = tRandomMaterialCase.tags(tMatID);
            ASSERT_EQ(2u, tTags.size());
            for(auto& tTag : tTags)
            {
                auto tTagIndex = &tTag - &tTags[0];
                ASSERT_STREQ(tGoldTags[tCaseIndex][tTagIndex].c_str(), tTag.c_str());
                ASSERT_STREQ("homogeneous", tRandomMaterialCase.attribute(tMatID, tTag).c_str());
                ASSERT_STREQ(tGoldSamples[tCaseIndex][tTagIndex].c_str(), tRandomMaterialCase.value(tMatID, tTag).c_str());
            }
        }
    }
}

TEST(PlatoTest, SROM_UpdateRandomMaterialCases_Error)
{
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSet;
    std::vector<Plato::srom::RandomMaterialCase> tRandomMaterialCases;
    EXPECT_THROW(Plato::srom::update_random_material_cases(tRandomMaterialSet, tRandomMaterialCases), std::runtime_error);
}

TEST(PlatoTest, SROM_UpdateRandomMaterialCases)
{
    // 1.1 DEFINE RANDOM MATERIAL ONE
    Plato::srom::Material tMaterialOne;
    tMaterialOne.blockID("1");
    tMaterialOne.materialID("2");
    tMaterialOne.category("isotropic");
    Plato::srom::Statistics tPoissonsRatioStats;
    tPoissonsRatioStats.mDistribution = "beta";
    tPoissonsRatioStats.mMean = "0.3";
    tPoissonsRatioStats.mUpperBound = "0.4";
    tPoissonsRatioStats.mLowerBound = "0.25";
    tPoissonsRatioStats.mStandardDeviation = "0.05";
    tPoissonsRatioStats.mNumSamples = "2";
    tMaterialOne.append("poissons ratio", "homogeneous", tPoissonsRatioStats);
    tMaterialOne.append("elastic modulus", "homogeneous", "1e9");
    auto tTags = tMaterialOne.tags();
    ASSERT_EQ(2u, tTags.size());
    ASSERT_STREQ("poissons ratio", tTags[0].c_str());
    ASSERT_STREQ("elastic modulus", tTags[1].c_str());

    // 1.2 INITIALIZE RANDOM MATERIAL SET ONE
    Plato::srom::SromVariable tSromVariableOne;
    tSromVariableOne.mTag = "poissons ratio";
    tSromVariableOne.mAttribute = "homogeneous";
    tSromVariableOne.mSampleProbPairs.mNumSamples = 2;
    tSromVariableOne.mSampleProbPairs.mSamples = {0.32, 0.27};
    tSromVariableOne.mSampleProbPairs.mProbabilities = {0.75, 0.25};

    std::vector<Plato::srom::SromVariable> tSromVariableSetOne;
    tSromVariableSetOne.push_back(tSromVariableOne);

    // 1.3 BUILD RANDOM MATERIAL SET ONE
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSetOne;
    EXPECT_NO_THROW(Plato::srom::build_random_material_set(tMaterialOne, tSromVariableSetOne, tRandomMaterialSetOne));
    ASSERT_EQ(2u, tRandomMaterialSetOne.size());

    // 1.4 BUILD INITIAL RANDOM MATERIAL CASES
    std::vector<Plato::srom::RandomMaterialCase> tRandomMaterialCases;
    EXPECT_NO_THROW(Plato::srom::initialize_random_material_cases(tRandomMaterialSetOne, tRandomMaterialCases));
    ASSERT_EQ(2u, tRandomMaterialCases.size());

    // 2.1 DEFINE RANDOM MATERIAL TWO
    Plato::srom::Material tMaterialTwo;
    tMaterialTwo.blockID("2");
    tMaterialTwo.materialID("3");
    tMaterialTwo.category("isotropic");
    Plato::srom::Statistics tElasticModulusStats;
    tElasticModulusStats.mDistribution = "beta";
    tElasticModulusStats.mMean = "10";
    tElasticModulusStats.mUpperBound = "20";
    tElasticModulusStats.mLowerBound = "5";
    tElasticModulusStats.mStandardDeviation = "2";
    tElasticModulusStats.mNumSamples = "2";
    tMaterialTwo.append("elastic modulus", "homogeneous", tElasticModulusStats);
    tMaterialTwo.append("poissons ratio", "homogeneous", "0.28");
    tTags = tMaterialTwo.tags();
    ASSERT_EQ(2u, tTags.size());
    ASSERT_STREQ("elastic modulus", tTags[0].c_str());
    ASSERT_STREQ("poissons ratio", tTags[1].c_str());

    // 2.2 INITIALIZE RANDOM MATERIAL SET TWO
    Plato::srom::SromVariable tSromVariableTwo;
    tSromVariableTwo.mTag = "elastic modulus";
    tSromVariableTwo.mAttribute = "homogeneous";
    tSromVariableTwo.mSampleProbPairs.mNumSamples = 2;
    tSromVariableTwo.mSampleProbPairs.mSamples = {8, 17};
    tSromVariableTwo.mSampleProbPairs.mProbabilities = {0.65, 0.35};

    std::vector<Plato::srom::SromVariable> tSromVariableSetTwo;
    tSromVariableSetTwo.push_back(tSromVariableTwo);

    // 2.3 BUILD RANDOM MATERIAL SET TWO
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSetTwo;
    EXPECT_NO_THROW(Plato::srom::build_random_material_set(tMaterialTwo, tSromVariableSetTwo, tRandomMaterialSetTwo));
    ASSERT_EQ(2u, tRandomMaterialSetTwo.size());

    // 2.4 UPDATE SET OF RANDOM MATERIAL CASES
    EXPECT_NO_THROW(Plato::srom::update_random_material_cases(tRandomMaterialSetTwo, tRandomMaterialCases));
    ASSERT_EQ(4u, tRandomMaterialCases.size());

    // TEST RESULTS
    const double tTolerance = 1e-4;
    const std::vector<double> tGoldProbs = { 0.4875, 0.1625, 0.2625, 0.0875 };
    const std::vector<std::vector<std::string>> tGoldMatIDs =
        { {"2", "3"}, {"2", "3"}, {"2", "3"}, {"2", "3"} };
    const std::vector<std::vector<std::string>> tGoldBlockIDs =
        { {"1", "2"}, {"1", "2"}, {"1", "2"}, {"1", "2"} };
    const std::vector<std::vector<std::string>> tGoldTags =
        { {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"} };
    const std::vector<std::vector<std::vector<std::string>>> tGoldSamples =
        {
          { {"1e9", "0.3200000000000000"}, {"8.0000000000000000", "0.28"} },
          { {"1e9", "0.2700000000000000"}, {"8.0000000000000000", "0.28"} },
          { {"1e9", "0.3200000000000000"}, {"17.0000000000000000", "0.28"} },
          { {"1e9", "0.2700000000000000"}, {"17.0000000000000000", "0.28"} },
        };

    for(auto& tRandomMaterialCase : tRandomMaterialCases)
    {
        ASSERT_EQ(2u, tRandomMaterialCase.numMaterials());

        auto tCaseIndex = &tRandomMaterialCase - &tRandomMaterialCases[0];
        ASSERT_NEAR(tGoldProbs[tCaseIndex], tRandomMaterialCase.probability(), tTolerance);

        auto tMaterialIDs = tRandomMaterialCase.materialIDs();
        ASSERT_EQ(2u, tMaterialIDs.size());
        for(auto& tMatID : tMaterialIDs)
        {
            auto tMatIdIndex = &tMatID - &tMaterialIDs[0];
            ASSERT_STREQ(tGoldMatIDs[tCaseIndex][tMatIdIndex].c_str(), tMatID.c_str());
            ASSERT_STREQ(tGoldBlockIDs[tCaseIndex][tMatIdIndex].c_str(), tRandomMaterialCase.blockID(tMatID).c_str());
            ASSERT_STREQ("isotropic", tRandomMaterialCase.category(tMatID).c_str());

            auto tTags = tRandomMaterialCase.tags(tMatID);
            ASSERT_EQ(2u, tTags.size());
            for(auto& tTag : tTags)
            {
                auto tTagIndex = &tTag - &tTags[0];
                ASSERT_STREQ(tGoldTags[tCaseIndex][tTagIndex].c_str(), tTag.c_str());
                ASSERT_STREQ("homogeneous", tRandomMaterialCase.attribute(tMatID, tTag).c_str());
                ASSERT_STREQ(tGoldSamples[tCaseIndex][tMatIdIndex][tTagIndex].c_str(), tRandomMaterialCase.value(tMatID, tTag).c_str());
            }
        }
    }
}

TEST(PlatoTest, SROM_AppendRandomMaterialSet_Error)
{
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSet;
    std::vector<Plato::srom::RandomMaterialCase> tRandomMaterialCases;
    EXPECT_THROW(Plato::srom::append_random_material_set(tRandomMaterialSet, tRandomMaterialCases), std::runtime_error);
}

TEST(PlatoTest, SROM_AppendRandomMaterialSet)
{
    // 1.1 DEFINE RANDOM MATERIAL ONE
    Plato::srom::Material tMaterialOne;
    tMaterialOne.blockID("1");
    tMaterialOne.materialID("2");
    tMaterialOne.category("isotropic");
    Plato::srom::Statistics tPoissonsRatioStats;
    tPoissonsRatioStats.mDistribution = "beta";
    tPoissonsRatioStats.mMean = "0.3";
    tPoissonsRatioStats.mUpperBound = "0.4";
    tPoissonsRatioStats.mLowerBound = "0.25";
    tPoissonsRatioStats.mStandardDeviation = "0.05";
    tPoissonsRatioStats.mNumSamples = "2";
    tMaterialOne.append("poissons ratio", "homogeneous", tPoissonsRatioStats);
    tMaterialOne.append("elastic modulus", "homogeneous", "1e9");
    auto tTags = tMaterialOne.tags();
    ASSERT_EQ(2u, tTags.size());
    ASSERT_STREQ("poissons ratio", tTags[0].c_str());
    ASSERT_STREQ("elastic modulus", tTags[1].c_str());

    // 1.2 INITIALIZE RANDOM MATERIAL SET ONE
    Plato::srom::SromVariable tSromVariableOne;
    tSromVariableOne.mTag = "poissons ratio";
    tSromVariableOne.mAttribute = "homogeneous";
    tSromVariableOne.mSampleProbPairs.mNumSamples = 2;
    tSromVariableOne.mSampleProbPairs.mSamples = {0.32, 0.27};
    tSromVariableOne.mSampleProbPairs.mProbabilities = {0.75, 0.25};

    std::vector<Plato::srom::SromVariable> tSromVariableSetOne;
    tSromVariableSetOne.push_back(tSromVariableOne);

    // 1.3 BUILD RANDOM MATERIAL SET ONE
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSetOne;
    EXPECT_NO_THROW(Plato::srom::build_random_material_set(tMaterialOne, tSromVariableSetOne, tRandomMaterialSetOne));
    ASSERT_EQ(2u, tRandomMaterialSetOne.size());

    // 1.4 APPEND RANDOM MATERIAL SET
    std::vector<Plato::srom::RandomMaterialCase> tRandomMaterialCases;
    EXPECT_NO_THROW(Plato::srom::append_random_material_set(tRandomMaterialSetOne, tRandomMaterialCases));

    // 2.1 DEFINE RANDOM MATERIAL TWO
    Plato::srom::Material tMaterialTwo;
    tMaterialTwo.blockID("2");
    tMaterialTwo.materialID("3");
    tMaterialTwo.category("isotropic");
    Plato::srom::Statistics tElasticModulusStats;
    tElasticModulusStats.mDistribution = "beta";
    tElasticModulusStats.mMean = "10";
    tElasticModulusStats.mUpperBound = "20";
    tElasticModulusStats.mLowerBound = "5";
    tElasticModulusStats.mStandardDeviation = "2";
    tElasticModulusStats.mNumSamples = "2";
    tMaterialTwo.append("elastic modulus", "homogeneous", tElasticModulusStats);
    tMaterialTwo.append("poissons ratio", "homogeneous", "0.28");
    tTags = tMaterialTwo.tags();
    ASSERT_EQ(2u, tTags.size());
    ASSERT_STREQ("elastic modulus", tTags[0].c_str());
    ASSERT_STREQ("poissons ratio", tTags[1].c_str());

    // 2.2 INITIALIZE RANDOM MATERIAL SET TWO
    Plato::srom::SromVariable tSromVariableTwo;
    tSromVariableTwo.mTag = "elastic modulus";
    tSromVariableTwo.mAttribute = "homogeneous";
    tSromVariableTwo.mSampleProbPairs.mNumSamples = 2;
    tSromVariableTwo.mSampleProbPairs.mSamples = {8, 17};
    tSromVariableTwo.mSampleProbPairs.mProbabilities = {0.65, 0.35};

    std::vector<Plato::srom::SromVariable> tSromVariableSetTwo;
    tSromVariableSetTwo.push_back(tSromVariableTwo);

    // 2.3 BUILD RANDOM MATERIAL SET TWO
    std::vector<Plato::srom::RandomMaterial> tRandomMaterialSetTwo;
    EXPECT_NO_THROW(Plato::srom::build_random_material_set(tMaterialTwo, tSromVariableSetTwo, tRandomMaterialSetTwo));
    ASSERT_EQ(2u, tRandomMaterialSetTwo.size());

    // 2.4 APPEND RANDOM MATERIAL SET
    EXPECT_NO_THROW(Plato::srom::append_random_material_set(tRandomMaterialSetTwo, tRandomMaterialCases));

    // TEST RESULTS
    const double tTolerance = 1e-4;
    const std::vector<double> tGoldProbs = { 0.4875, 0.1625, 0.2625, 0.0875 };
    const std::vector<std::vector<std::string>> tGoldMatIDs =
        { {"2", "3"}, {"2", "3"}, {"2", "3"}, {"2", "3"} };
    const std::vector<std::vector<std::string>> tGoldBlockIDs =
        { {"1", "2"}, {"1", "2"}, {"1", "2"}, {"1", "2"} };
    const std::vector<std::vector<std::string>> tGoldTags =
        { {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"} };
    const std::vector<std::vector<std::vector<std::string>>> tGoldSamples =
        {
          { {"1e9", "0.3200000000000000"}, {"8.0000000000000000", "0.28"} },
          { {"1e9", "0.2700000000000000"}, {"8.0000000000000000", "0.28"} },
          { {"1e9", "0.3200000000000000"}, {"17.0000000000000000", "0.28"} },
          { {"1e9", "0.2700000000000000"}, {"17.0000000000000000", "0.28"} },
        };

    for(auto& tRandomMaterialCase : tRandomMaterialCases)
    {
        ASSERT_EQ(2u, tRandomMaterialCase.numMaterials());

        auto tCaseIndex = &tRandomMaterialCase - &tRandomMaterialCases[0];
        ASSERT_NEAR(tGoldProbs[tCaseIndex], tRandomMaterialCase.probability(), tTolerance);

        auto tMaterialIDs = tRandomMaterialCase.materialIDs();
        ASSERT_EQ(2u, tMaterialIDs.size());
        for(auto& tMatID : tMaterialIDs)
        {
            auto tMatIdIndex = &tMatID - &tMaterialIDs[0];
            ASSERT_STREQ(tGoldMatIDs[tCaseIndex][tMatIdIndex].c_str(), tMatID.c_str());
            ASSERT_STREQ(tGoldBlockIDs[tCaseIndex][tMatIdIndex].c_str(), tRandomMaterialCase.blockID(tMatID).c_str());
            ASSERT_STREQ("isotropic", tRandomMaterialCase.category(tMatID).c_str());

            auto tTags = tRandomMaterialCase.tags(tMatID);
            ASSERT_EQ(2u, tTags.size());
            for(auto& tTag : tTags)
            {
                auto tTagIndex = &tTag - &tTags[0];
                ASSERT_STREQ(tGoldTags[tCaseIndex][tTagIndex].c_str(), tTag.c_str());
                ASSERT_STREQ("homogeneous", tRandomMaterialCase.attribute(tMatID, tTag).c_str());
                ASSERT_STREQ(tGoldSamples[tCaseIndex][tMatIdIndex][tTagIndex].c_str(), tRandomMaterialCase.value(tMatID, tTag).c_str());
            }
        }
    }
}

TEST(PlatoTest, SROM_AppendDeterministicMaterials_Test1)
{
    // 1.1 MATERIAL CASE 1: CREATE RANDOM MATERIAL 1
    Plato::srom::RandomMaterial tRandomMaterial11;
    tRandomMaterial11.blockID("1");
    tRandomMaterial11.category("isotropic");
    tRandomMaterial11.append("elastic modulus", "homogeneous", "1e9");
    tRandomMaterial11.append("poissons ratio", "homogeneous", "0.32");
    Plato::srom::RandomMaterialCase tRandomMatCase1;
    tRandomMatCase1.append("2", tRandomMaterial11);

    // 1.2 MATERIAL CASE 1: CREATE RANDOM MATERIAL 2
    Plato::srom::RandomMaterial tRandomMaterial12;
    tRandomMaterial12.blockID("2");
    tRandomMaterial12.category("isotropic");
    tRandomMaterial12.append("elastic modulus", "homogeneous", "8.0");
    tRandomMaterial12.append("poissons ratio", "homogeneous", "0.28");
    tRandomMatCase1.append("3", tRandomMaterial12);
    tRandomMatCase1.probability(0.4875);

    // 2.1 MATERIAL CASE 2: CREATE RANDOM MATERIAL 1
    Plato::srom::RandomMaterial tRandomMaterial21;
    tRandomMaterial21.blockID("1");
    tRandomMaterial21.category("isotropic");
    tRandomMaterial21.append("elastic modulus", "homogeneous", "1e9");
    tRandomMaterial21.append("poissons ratio", "homogeneous", "0.27");
    Plato::srom::RandomMaterialCase tRandomMatCase2;
    tRandomMatCase2.append("2", tRandomMaterial21);

    // 2.2 MATERIAL CASE 2: CREATE RANDOM MATERIAL 2
    Plato::srom::RandomMaterial tRandomMaterial22;
    tRandomMaterial22.blockID("2");
    tRandomMaterial22.category("isotropic");
    tRandomMaterial22.append("elastic modulus", "homogeneous", "8.0");
    tRandomMaterial22.append("poissons ratio", "homogeneous", "0.28");
    tRandomMatCase2.append("3", tRandomMaterial22);
    tRandomMatCase2.probability(0.1625);

    // 3.1 MATERIAL CASE 3: CREATE RANDOM MATERIAL 1
    Plato::srom::RandomMaterial tRandomMaterial31;
    tRandomMaterial31.blockID("1");
    tRandomMaterial31.category("isotropic");
    tRandomMaterial31.append("elastic modulus", "homogeneous", "1e9");
    tRandomMaterial31.append("poissons ratio", "homogeneous", "0.32");
    Plato::srom::RandomMaterialCase tRandomMatCase3;
    tRandomMatCase3.append("2", tRandomMaterial31);

    // 3.2 MATERIAL CASE 3: CREATE RANDOM MATERIAL 2
    Plato::srom::RandomMaterial tRandomMaterial32;
    tRandomMaterial32.blockID("2");
    tRandomMaterial32.category("isotropic");
    tRandomMaterial32.append("elastic modulus", "homogeneous", "17.0");
    tRandomMaterial32.append("poissons ratio", "homogeneous", "0.28");
    tRandomMatCase3.append("3", tRandomMaterial32);
    tRandomMatCase3.probability(0.2625);

    // 4.1 MATERIAL CASE 4: CREATE RANDOM MATERIAL 1
    Plato::srom::RandomMaterial tRandomMaterial41;
    tRandomMaterial41.blockID("1");
    tRandomMaterial41.category("isotropic");
    tRandomMaterial41.append("elastic modulus", "homogeneous", "1e9");
    tRandomMaterial41.append("poissons ratio", "homogeneous", "0.27");
    Plato::srom::RandomMaterialCase tRandomMatCase4;
    tRandomMatCase4.append("2", tRandomMaterial41);

    // 4.2 MATERIAL CASE 4: CREATE RANDOM MATERIAL 2
    Plato::srom::RandomMaterial tRandomMaterial42;
    tRandomMaterial42.blockID("2");
    tRandomMaterial42.category("isotropic");
    tRandomMaterial42.append("elastic modulus", "homogeneous", "17.0");
    tRandomMaterial42.append("poissons ratio", "homogeneous", "0.28");
    tRandomMatCase4.append("3", tRandomMaterial42);
    tRandomMatCase4.probability(0.0875);

    // 5. APPEND RANDOM MATERIAL CASES
    std::vector<Plato::srom::RandomMaterialCase> tRandomMaterialCases;
    tRandomMaterialCases.push_back(tRandomMatCase1);
    tRandomMaterialCases.push_back(tRandomMatCase2);
    tRandomMaterialCases.push_back(tRandomMatCase3);
    tRandomMaterialCases.push_back(tRandomMatCase4);

    // 6. TEST: NO DETERMINISTIC MATERIALS WILL BE APPENDED.
    std::vector<Plato::srom::Material> tDeterministicMaterials;
    EXPECT_NO_THROW(Plato::srom::append_deterministic_materials(tDeterministicMaterials, tRandomMaterialCases));

    const double tTolerance = 1e-4;
    const std::vector<double> tGoldProbs = { 0.4875, 0.1625, 0.2625, 0.0875 };
    const std::vector<std::vector<std::string>> tGoldMatIDs =
        { {"2", "3"}, {"2", "3"}, {"2", "3"}, {"2", "3"} };
    const std::vector<std::vector<std::string>> tGoldBlockIDs =
        { {"1", "2"}, {"1", "2"}, {"1", "2"}, {"1", "2"} };
    const std::vector<std::vector<std::string>> tGoldTags =
        { {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"} };
    const std::vector<std::vector<std::vector<std::string>>> tGoldSamples =
        {
          { {"1e9", "0.32"}, {"8.0", "0.28"} },
          { {"1e9", "0.27"}, {"8.0", "0.28"} },
          { {"1e9", "0.32"}, {"17.0", "0.28"} },
          { {"1e9", "0.27"}, {"17.0", "0.28"} },
        };

    for(auto& tRandomMaterialCase : tRandomMaterialCases)
    {
        ASSERT_EQ(2u, tRandomMaterialCase.numMaterials());

        auto tCaseIndex = &tRandomMaterialCase - &tRandomMaterialCases[0];
        ASSERT_NEAR(tGoldProbs[tCaseIndex], tRandomMaterialCase.probability(), tTolerance);

        auto tMaterialIDs = tRandomMaterialCase.materialIDs();
        ASSERT_EQ(2u, tMaterialIDs.size());
        for(auto& tMatID : tMaterialIDs)
        {
            auto tMatIdIndex = &tMatID - &tMaterialIDs[0];
            ASSERT_STREQ(tGoldMatIDs[tCaseIndex][tMatIdIndex].c_str(), tMatID.c_str());
            ASSERT_STREQ(tGoldBlockIDs[tCaseIndex][tMatIdIndex].c_str(), tRandomMaterialCase.blockID(tMatID).c_str());
            ASSERT_STREQ("isotropic", tRandomMaterialCase.category(tMatID).c_str());

            auto tTags = tRandomMaterialCase.tags(tMatID);
            ASSERT_EQ(2u, tTags.size());
            for(auto& tTag : tTags)
            {
                auto tTagIndex = &tTag - &tTags[0];
                ASSERT_STREQ(tGoldTags[tCaseIndex][tTagIndex].c_str(), tTag.c_str());
                ASSERT_STREQ("homogeneous", tRandomMaterialCase.attribute(tMatID, tTag).c_str());
                ASSERT_STREQ(tGoldSamples[tCaseIndex][tMatIdIndex][tTagIndex].c_str(), tRandomMaterialCase.value(tMatID, tTag).c_str());
            }
        }
    }
}

TEST(PlatoTest, SROM_AppendDeterministicMaterials_Test2)
{
    // 1.1 MATERIAL CASE 1: CREATE RANDOM MATERIAL 1
    Plato::srom::RandomMaterial tRandomMaterial11;
    tRandomMaterial11.blockID("1");
    tRandomMaterial11.category("isotropic");
    tRandomMaterial11.append("elastic modulus", "homogeneous", "1e9");
    tRandomMaterial11.append("poissons ratio", "homogeneous", "0.32");
    Plato::srom::RandomMaterialCase tRandomMatCase1;
    tRandomMatCase1.append("2", tRandomMaterial11);

    // 1.2 MATERIAL CASE 1: CREATE RANDOM MATERIAL 2
    Plato::srom::RandomMaterial tRandomMaterial12;
    tRandomMaterial12.blockID("2");
    tRandomMaterial12.category("isotropic");
    tRandomMaterial12.append("elastic modulus", "homogeneous", "8.0");
    tRandomMaterial12.append("poissons ratio", "homogeneous", "0.28");
    tRandomMatCase1.append("3", tRandomMaterial12);
    tRandomMatCase1.probability(0.4875);

    // 2.1 MATERIAL CASE 2: CREATE RANDOM MATERIAL 1
    Plato::srom::RandomMaterial tRandomMaterial21;
    tRandomMaterial21.blockID("1");
    tRandomMaterial21.category("isotropic");
    tRandomMaterial21.append("elastic modulus", "homogeneous", "1e9");
    tRandomMaterial21.append("poissons ratio", "homogeneous", "0.27");
    Plato::srom::RandomMaterialCase tRandomMatCase2;
    tRandomMatCase2.append("2", tRandomMaterial21);

    // 2.2 MATERIAL CASE 2: CREATE RANDOM MATERIAL 2
    Plato::srom::RandomMaterial tRandomMaterial22;
    tRandomMaterial22.blockID("2");
    tRandomMaterial22.category("isotropic");
    tRandomMaterial22.append("elastic modulus", "homogeneous", "8.0");
    tRandomMaterial22.append("poissons ratio", "homogeneous", "0.28");
    tRandomMatCase2.append("3", tRandomMaterial22);
    tRandomMatCase2.probability(0.1625);

    // 3.1 MATERIAL CASE 3: CREATE RANDOM MATERIAL 1
    Plato::srom::RandomMaterial tRandomMaterial31;
    tRandomMaterial31.blockID("1");
    tRandomMaterial31.category("isotropic");
    tRandomMaterial31.append("elastic modulus", "homogeneous", "1e9");
    tRandomMaterial31.append("poissons ratio", "homogeneous", "0.32");
    Plato::srom::RandomMaterialCase tRandomMatCase3;
    tRandomMatCase3.append("2", tRandomMaterial31);

    // 3.2 MATERIAL CASE 3: CREATE RANDOM MATERIAL 2
    Plato::srom::RandomMaterial tRandomMaterial32;
    tRandomMaterial32.blockID("2");
    tRandomMaterial32.category("isotropic");
    tRandomMaterial32.append("elastic modulus", "homogeneous", "17.0");
    tRandomMaterial32.append("poissons ratio", "homogeneous", "0.28");
    tRandomMatCase3.append("3", tRandomMaterial32);
    tRandomMatCase3.probability(0.2625);

    // 4.1 MATERIAL CASE 4: CREATE RANDOM MATERIAL 1
    Plato::srom::RandomMaterial tRandomMaterial41;
    tRandomMaterial41.blockID("1");
    tRandomMaterial41.category("isotropic");
    tRandomMaterial41.append("elastic modulus", "homogeneous", "1e9");
    tRandomMaterial41.append("poissons ratio", "homogeneous", "0.27");
    Plato::srom::RandomMaterialCase tRandomMatCase4;
    tRandomMatCase4.append("2", tRandomMaterial41);

    // 4.2 MATERIAL CASE 4: CREATE RANDOM MATERIAL 2
    Plato::srom::RandomMaterial tRandomMaterial42;
    tRandomMaterial42.blockID("2");
    tRandomMaterial42.category("isotropic");
    tRandomMaterial42.append("elastic modulus", "homogeneous", "17.0");
    tRandomMaterial42.append("poissons ratio", "homogeneous", "0.28");
    tRandomMatCase4.append("3", tRandomMaterial42);
    tRandomMatCase4.probability(0.0875);

    // 5. APPEND RANDOM MATERIAL CASES
    std::vector<Plato::srom::RandomMaterialCase> tRandomMaterialCases;
    tRandomMaterialCases.push_back(tRandomMatCase1);
    tRandomMaterialCases.push_back(tRandomMatCase2);
    tRandomMaterialCases.push_back(tRandomMatCase3);
    tRandomMaterialCases.push_back(tRandomMatCase4);

    // 6. APPEND DETERMINISTIC MATERIAL.
    Plato::srom::Material tDeterministicMaterial;
    tDeterministicMaterial.blockID("3");
    tDeterministicMaterial.materialID("4");
    tDeterministicMaterial.category("isotropic");
    tDeterministicMaterial.append("poissons ratio", "homogeneous", "0.4");
    tDeterministicMaterial.append("elastic modulus", "homogeneous", "1.0");
    std::vector<Plato::srom::Material> tDeterministicMaterialSet;
    tDeterministicMaterialSet.push_back(tDeterministicMaterial);
    EXPECT_NO_THROW(Plato::srom::append_deterministic_materials(tDeterministicMaterialSet, tRandomMaterialCases));

    const double tTolerance = 1e-4;
    const std::vector<double> tGoldProbs = { 0.4875, 0.1625, 0.2625, 0.0875 };
    const std::vector<std::vector<std::string>> tGoldMatIDs =
        { {"2", "3", "4"}, {"2", "3", "4"}, {"2", "3", "4"}, {"2", "3", "4"} };
    const std::vector<std::vector<std::string>> tGoldBlockIDs =
        { {"1", "2", "3"}, {"1", "2", "3"}, {"1", "2", "3"}, {"1", "2", "3"} };
    const std::vector<std::vector<std::string>> tGoldTags =
        { {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"},
          {"elastic modulus", "poissons ratio"} };
    const std::vector<std::vector<std::vector<std::string>>> tGoldSamples =
        {
          { {"1e9", "0.32"}, {"8.0", "0.28"}, {"1.0", "0.4"} },
          { {"1e9", "0.27"}, {"8.0", "0.28"}, {"1.0", "0.4"} },
          { {"1e9", "0.32"}, {"17.0", "0.28"}, {"1.0", "0.4"} },
          { {"1e9", "0.27"}, {"17.0", "0.28"}, {"1.0", "0.4"} },
        };

    for(auto& tRandomMaterialCase : tRandomMaterialCases)
    {
        ASSERT_EQ(3u, tRandomMaterialCase.numMaterials());

        auto tCaseIndex = &tRandomMaterialCase - &tRandomMaterialCases[0];
        ASSERT_NEAR(tGoldProbs[tCaseIndex], tRandomMaterialCase.probability(), tTolerance);

        auto tMaterialIDs = tRandomMaterialCase.materialIDs();
        ASSERT_EQ(3u, tMaterialIDs.size());
        for(auto& tMatID : tMaterialIDs)
        {
            auto tMatIdIndex = &tMatID - &tMaterialIDs[0];
            ASSERT_STREQ(tGoldMatIDs[tCaseIndex][tMatIdIndex].c_str(), tMatID.c_str());
            ASSERT_STREQ(tGoldBlockIDs[tCaseIndex][tMatIdIndex].c_str(), tRandomMaterialCase.blockID(tMatID).c_str());
            ASSERT_STREQ("isotropic", tRandomMaterialCase.category(tMatID).c_str());

            auto tTags = tRandomMaterialCase.tags(tMatID);
            ASSERT_EQ(2u, tTags.size());
            for(auto& tTag : tTags)
            {
                auto tTagIndex = &tTag - &tTags[0];
                ASSERT_STREQ(tGoldTags[tCaseIndex][tTagIndex].c_str(), tTag.c_str());
                ASSERT_STREQ("homogeneous", tRandomMaterialCase.attribute(tMatID, tTag).c_str());
                ASSERT_STREQ(tGoldSamples[tCaseIndex][tMatIdIndex][tTagIndex].c_str(), tRandomMaterialCase.value(tMatID, tTag).c_str());
            }
        }
    }
}

}
// namespace UncertainMaterial