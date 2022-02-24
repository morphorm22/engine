/*
 * XMLGeneratorParseCriteria.hpp
 *
 *  Created on: Jun 23, 2020
 */

#pragma once

#include "XMLGeneratorParseMetadata.hpp"
#include "XMLGeneratorParserUtilities.hpp"
#include "XMLGeneratorCriterionMetadata.hpp"

namespace XMLGen
{

class ParseCriteria : public XMLGen::ParseMetadata<std::vector<XMLGen::Criterion>>
{
private:
    XMLGen::MetaDataTags mTags; /*!< map from plato input file tags to valid tokens-value pairs, i.e. map<tag, pair<tokens,value> > */
    std::vector<XMLGen::Criterion> mData; /*!< criteria metadata */

private:
    /******************************************************************************//**
     * \fn allocate
     * \brief Allocate map from valid tags to valid tokens-value pair
    **********************************************************************************/
    void allocate();

    /******************************************************************************//**
     * \fn setCriterionType
     * \brief Set 'criterion type' keyword, throw error if it is not defined.
     * \param [in/out] aInputFile parsed input metadata
    **********************************************************************************/
    void setCriterionType(XMLGen::Criterion& aMetadata);

    /******************************************************************************//**
     * \fn setCriterionIDs
     * \brief Set 'criterion_ids' keyword for 'composite' criteria.
     * \param [in/out] aInputFile parsed input metadata
    **********************************************************************************/
    void setCriterionIDs(XMLGen::Criterion& aMetadata);

    /******************************************************************************//**
     * \fn setCriterionWeights
     * \brief Set 'criterion_weights' keyword for 'composite' criteria.
     * \param [in/out] aInputFile parsed input metadata
    **********************************************************************************/
    void setCriterionWeights(XMLGen::Criterion& aMetadata);

    /******************************************************************************//**
     * \fn setModesToExclude
     * \brief Set 'modes_to_exclude' keyword for modal inverse
     * \param [in/out] aInputFile parsed input metadata
    **********************************************************************************/
    void setModesToExclude(XMLGen::Criterion& aMetadata);

    /******************************************************************************//**
     * \fn setMatchNodesetIDs
     * \brief Set nodeset ids to be used when matching frf or modal data
     * \param [in] aScenario Scenario metadata
    **********************************************************************************/
    void setMatchNodesetIDs(XMLGen::Criterion &aMetadata);

    /******************************************************************************//**
     * \fn setMetaData
     * \brief Set XMLGen::Criterion metadata.
     * \param [in/out] aInputFile parsed input metadata
    **********************************************************************************/
    void setMetadata(XMLGen::Criterion& aMetadata);

    /******************************************************************************//**
     * \fn checkUniqueIDs
     * \brief Throw error if criterion block identification numbers are not unique.
    **********************************************************************************/
    void checkUniqueIDs();

    /******************************************************************************//**
     * \fn setTags
     * \brief Set criterion metadata.
     * \param [in] aCriterion criterion metadata
    **********************************************************************************/
    void setTags(XMLGen::Criterion& aCriterion);

    /******************************************************************************//**
     * \fn setMassProperties
     * \brief Set criterion metadata for mass properties.
     * \param [in] aCriterion criterion metadata
    **********************************************************************************/
    void setMassProperties(XMLGen::Criterion& aCriterion);

    /******************************************************************************//**
     * \fn checkVolumePenaltyExponent
     * \brief Make sure the default value for material penalty exponent is correct for volume criteria
     * \param [in] aCriterion criterion metadata
    **********************************************************************************/
    void checkVolumePenaltyExponent(XMLGen::Criterion& aCriterion);

    /******************************************************************************//**
     * \fn errorCheckDisplacementCriterion
     * \brief Make sure the correct options are set for displacement type criteria
     * \param [in] aCriterion criterion metadata
    **********************************************************************************/
    void errorCheckDisplacementCriterion(XMLGen::Criterion &aMetadata);

    /******************************************************************************//**
     * \fn setDisplacementDirection
     * \brief Set the displacement direction member variable
     * \param [in] aCriterion criterion metadata
    **********************************************************************************/
    void setDisplacementDirection(XMLGen::Criterion &aMetadata);

    /******************************************************************************//**
     * \fn setTargetSolutionVector
     * \brief Set the target solution vector member variable
     * \param [in] aCriterion criterion metadata
    **********************************************************************************/
    void setTargetSolutionVector(XMLGen::Criterion &aMetadata);

    /******************************************************************************//**
     * \fn insertTag
     * \brief Insert a tag, possibly with a default value
     * \param [in] aCriterion criterion metadata
    **********************************************************************************/
    void insertTag(std::string keyword, std::string defaultValue = "");
public:
    /******************************************************************************//**
     * \fn data
     * \brief Return criterion metadata.
     * \return metadata
    **********************************************************************************/
    std::vector<XMLGen::Criterion> data() const override;

    /******************************************************************************//**
     * \fn parse
     * \brief Parse output metadata.
     * \param [in] aInputFile input file metadata.
    **********************************************************************************/
    void parse(std::istream &aInputFile) override;
};
// class ParseCriteria

}
// namespace XMLGen
