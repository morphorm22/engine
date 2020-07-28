/*
 * XMLGeneratorOutputMetadata.hpp
 *
 *  Created on: Jul 4, 2020
 */

#pragma once

#include <map>
#include <vector>
#include <string>
#include <unordered_map>

namespace XMLGen
{

/******************************************************************************//**
 * \struct Output
 * \brief The Output metadata structure owns the random and deterministic output \n
 * Quantities of Interests (QoIs) defined by the user in the plato input file.
**********************************************************************************/
struct Output
{
private:
    /******************************************************************************//**
     * \var QoI maps
     * \brief Maps from QoIs identifier (ID) to map from attribute name to attribute \n
     * value, i.e. map<ID, map<attribute, value>>, where attributes are quantities \n
     * used to describe the data.  For instance, valid attributes are 'ArgumentName', \n
     * 'SharedDataName', and \n 'DataLayout'.
    **********************************************************************************/
    std::map<std::string, std::unordered_map<std::string, std::string>> mRandomQoIs;
    std::map<std::string, std::unordered_map<std::string, std::string>> mDeterministicQoIs;

    std::unordered_map<std::string, std::string> mMetaData; /*!< scenario metadata, map< tag, value > */

private:
    /******************************************************************************//**
     * \fn getValue
     * \brief Return string value for property with input tag; else, throw an error if \n
     * property is not defined in the metadata.
     * \param [in] aTag property tag
     * \return property string value (default = "")
     **********************************************************************************/
    std::string getValue(const std::string &aTag) const;

    /******************************************************************************//**
     * \fn getBool
     * \brief Return bool value for property with input tag; else, throw an error if \n
     * property is not defined in the metadata.
     * \param [in] aTag property tag
     * \return property bool value (default = false)
     **********************************************************************************/
    bool getBool(const std::string &aTag) const;

public:
    /******************************************************************************//**
     * \fn value
     * \brief Return value for property with input tag; else, throw an error if \n
     * property is not defined in the metadata.
     * \param [in] aTag property tag
     * \return property string value
    **********************************************************************************/
    std::string value(const std::string& aTag) const;

    /******************************************************************************//**
     * \fn tags
     * \brief Return list of parameter tags.
     * \return parameter tags
    **********************************************************************************/
    std::vector<std::string> tags() const;

    /******************************************************************************//**
     * \fn append
     * \brief Append parameter to metadata.
     * \param [in] aTag   parameter tag
     * \param [in] aValue parameter value
    **********************************************************************************/
    void append(const std::string& aTag, const std::string& aValue);

    /******************************************************************************//**
     * \fn outputData
     * \brief Return 'output_data_to_file' keyword value.
     * \return value
    **********************************************************************************/
    bool outputData() const;

    /******************************************************************************//**
     * \fn outputData
     * \brief Set 'output_data_to_file' keyword
     * \param [in] aInput keyword value
    **********************************************************************************/
    void outputData(const std::string& aInput);

    /******************************************************************************//**
     * \fn outputSamples
     * \brief Return 'output_samples' keyword value.
     * \return value
    **********************************************************************************/
    bool outputSamples() const;

    /******************************************************************************//**
     * \fn outputSamples
     * \brief Set 'output_samples' keyword.
     * \param [in] aInput keyword value
     * \return value
    **********************************************************************************/
    void outputSamples(const std::string& aInput);

    /******************************************************************************//**
     * \fn scenarioID
     * \brief Set scenario identifier associated with output quantities of interest.
     * \param [in] aInput scenario identifier (id)
    **********************************************************************************/
    void scenarioID(const std::string& aInput);

    /******************************************************************************//**
     * \fn scenarioID
     * \brief Return scenario identifier associated with output quantities of interest.
     * \return scenario identifier (id)
    **********************************************************************************/
    std::string scenarioID() const;

    /******************************************************************************//**
     * \fn appendRandomQoI
     * \brief Append random Quantities of Interest (QoI) to list of outputs. Quantities \n
     * are saved in output file, e.g. platomain.exo.
     * \param [in] aID         quantity of interest identifier
     * \param [in] aDataLayout shared data layout
    **********************************************************************************/
    void appendRandomQoI(const std::string& aID, const std::string& aDataLayout);

    /******************************************************************************//**
     * \fn appendDeterminsiticQoI
     * \brief Append deterministic Quantities of Interest (QoI) to list of outputs. \n
     * Quantities are saved in output file, e.g. platomain.exo.
     * \param [in] aID         quantity of interest identifier
     * \param [in] aDataLayout shared data layout
    **********************************************************************************/
    void appendDeterminsiticQoI(const std::string& aID, const std::string& aDataLayout);

    /******************************************************************************//**
     * \fn randomLayout
     * \brief Return shared data layout of requested random quantities of interest.
     * \param [in] aID quantity of interest identifier
     * \return shared data layout
    **********************************************************************************/
    std::string randomLayout(const std::string& aID) const;

    /******************************************************************************//**
     * \fn deterministicLayout
     * \brief Return shared data layout of requested deterministic quantities of interest.
     * \param [in] aID quantity of interest identifier
     * \return shared data layout
    **********************************************************************************/
    std::string deterministicLayout(const std::string& aID) const;

    /******************************************************************************//**
     * \fn randomArgumentName
     * \brief Return argument name of requested random quantities of interest.
     * \param [in] aID quantity of interest identifier
     * \return argument name
    **********************************************************************************/
    std::string randomArgumentName(const std::string& aID) const;

    /******************************************************************************//**
     * \fn deterministicArgumentName
     * \brief Return argument name of requested deterministic quantities of interest.
     * \param [in] aID quantity of interest identifier
     * \return argument name
    **********************************************************************************/
    std::string deterministicArgumentName(const std::string& aID) const;

    /******************************************************************************//**
     * \fn randomSharedDataName
     * \brief Return shared data name of requested random quantities of interest.
     * \param [in] aID quantity of interest identifier
     * \return shared data name
    **********************************************************************************/
    std::string randomSharedDataName(const std::string& aID) const;

    /******************************************************************************//**
     * \fn randomSharedDataName
     * \brief Return shared data name of requested deterministic quantities of interest.
     * \param [in] aID quantity of interest identifier
     * \return shared data name
    **********************************************************************************/
    std::string deterministicSharedDataName(const std::string& aID) const;

    /******************************************************************************//**
     * \fn randomIDs
     * \brief Return random quantities of interest identifiers.
     * \return random quantities of interest identifiers
    **********************************************************************************/
    std::vector<std::string> randomIDs() const;

    /******************************************************************************//**
     * \fn deterministicIDs
     * \brief Return deterministic quantities of interest identifiers.
     * \return deterministic quantities of interest identifiers
    **********************************************************************************/
    std::vector<std::string> deterministicIDs() const;

    /******************************************************************************//**
     * \fn isRandomMapEmpty
     * \brief Returns true if container of random quantities of interest is empty.
     * \return flag
    **********************************************************************************/
    bool isRandomMapEmpty() const;

    /******************************************************************************//**
     * \fn isDeterministicMapEmpty
     * \brief Returns true if container of deterministic quantities of interest is empty.
     * \return flag
    **********************************************************************************/
    bool isDeterministicMapEmpty() const;
};
// struct Output

}
// namespace XMLGen
