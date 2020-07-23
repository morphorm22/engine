/*
 * XMLGeneratorParseScenario.hpp
 *
 *  Created on: Jun 18, 2020
 */

#pragma once

#include "XMLGeneratorParseMetadata.hpp"
#include "XMLGeneratorParserUtilities.hpp"
#include "XMLGeneratorScenarioMetadata.hpp"

namespace XMLGen
{

/******************************************************************************//**
 * \class ParseScenario
 * \brief Parse inputs in scenario block and store values in XMLGen::Scenario.
**********************************************************************************/
class ParseScenario : public XMLGen::ParseMetadata<XMLGen::Scenario>
{
private:
    XMLGen::Scenario mData; /*!< scenario metadata */
    /*!< map from main scenario tags to pair< valid tokens, pair<value,default> >, \n
     * i.e. map< tag, pair<valid tokens, pair<value,default>> > */
    XMLGen::MetaDataTags mMainTags;

private:
    /******************************************************************************//**
     * \fn allocate
     * \brief Allocate map from valid tags to valid tokens-value pair
    **********************************************************************************/
    void allocate();

    /******************************************************************************//**
     * \fn setMainTags
     * \brief Set main tags metadata.
    **********************************************************************************/
    void setMainTags();

    /******************************************************************************//**
     * \fn check
     * \brief Check XMLGen::Scenario metadata.
    **********************************************************************************/
    void check();

    /******************************************************************************//**
     * \fn checkCode
     * \brief If 'code' keyword value is not supported, throw error.
    **********************************************************************************/
    void checkCode();

    /******************************************************************************//**
     * \fn checkPhysics
     * \brief Set 'physics' keyword, throw error if input keyword is empty.
    **********************************************************************************/
    void checkPhysics();

    /******************************************************************************//**
     * \fn checkPerformer
     * \brief If 'performer' keyword is empty, set 'performer' keyword to default \n
     * value: 'code_keyword' + '0', where 'code_keyword' denotes the value set for \n
     * 'code' keyword.
    **********************************************************************************/
    void checkPerformer();

    /******************************************************************************//**
     * \fn checkSpatialDimensions
     * \brief If 'dimensions' keyword value is not supported, throw error.
    **********************************************************************************/
    void checkSpatialDimensions();

    /******************************************************************************//**
     * \fn checkScenarioID
     * \brief If scenario 'id' keyword is empty, set 'id' to default = 'code_keyword' + \n
     * 'physics_keyword' + '0', where \n 'code_keyword' denotes the value set for \n
     * 'code' keyword and 'physics_keyword' denotes the value set for 'physics' keyword.
    **********************************************************************************/
    void checkScenarioID();

    /******************************************************************************//**
     * \fn setMainTagsMetaData
     * \brief Set main scenario tags.
     * \param [in] aInputFile input file metadata.
    **********************************************************************************/
    void setMainTagsMetaData(std::istream &aInputFile);

public:
    /******************************************************************************//**
     * \fn data
     * \brief Return scenario metadata.
     * \return metadata
    **********************************************************************************/
    XMLGen::Scenario data() const override;

    /******************************************************************************//**
     * \fn parse
     * \brief Parse output metadata.
     * \param [in] aInputFile input file metadata.
    **********************************************************************************/
    void parse(std::istream &aInputFile) override;
};

}
// namespace XMLGen
