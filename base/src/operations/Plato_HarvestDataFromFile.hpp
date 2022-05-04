/*
 * Plato_HarvestDataFromFile.hpp
 *
 *  Created on: Feb 09, 2022
 */

#pragma once

#include <unordered_map>

#include "Plato_LocalOperation.hpp"

class PlatoApp;

namespace Plato
{

class InputData;

/******************************************************************************//**
 * \struct Table
 * \brief Metadata describing the table data and algebraic operation to be performed on the data.
**********************************************************************************/
struct Table
{
    size_t mRow = 0; /*!< number of rows > */
    size_t mCol = 0; /*!< number of columns > */
    std::string mOperation = "max"; /*!< algebraic operation > */
    std::vector<std::vector<double>> mData; /*!< numeric values > */
};
// struct Table

/******************************************************************************//**
 * \fn compute
 * \brief Perform operation (max, min, or sum) and return outcome. 
 * \param [in]  aTable structure table data harvested from a text file. 
 * \return output value
**********************************************************************************/
double compute(Plato::Table& aTable);

/******************************************************************************//**
 * \class HarvestDataFromFile
 * \brief This class is used to harvest numeric data from a file on disk. The \n
 *        numeric data is assumed to follow a table format, each column denotes \n 
 *        a unique quantity of interest. 
**********************************************************************************/
class HarvestDataFromFile : public Plato::LocalOp
{
public:
    /******************************************************************************//**
     * \brief class constructor
     * \param [in] aPlatoApp Plato service application
     * \param [in] aNode     input metadata for local operation
    **********************************************************************************/
    HarvestDataFromFile(PlatoApp* aPlatoApp, Plato::InputData& aNode);

    /******************************************************************************//**
     * \brief class destructor
    **********************************************************************************/
    virtual ~HarvestDataFromFile(){}

    /******************************************************************************//**
     * \brief perform local operation
    **********************************************************************************/
    void operator()();

    /******************************************************************************//**
     * \fn getArguments 
     * \brief Return local operation's argument list
     * \param [out] aLocalArgs argument list
    **********************************************************************************/
    void getArguments(std::vector<Plato::LocalArg>& aLocalArgs);

    /******************************************************************************//**
     * \fn name
     * \brief Return local operation name assigned by the user.
     * \return local operation name (string)
    **********************************************************************************/
    std::string name() const;

    /******************************************************************************//**
     * \fn file
     * \brief Returns name of the text file read from disk.
     * \return local operation name (string)
    **********************************************************************************/
    std::string file() const;

    /******************************************************************************//**
     * \fn column
     * \brief Returns the table's column index of interest.
     * \return local operation name (string)
    **********************************************************************************/
    std::string column() const;

    /******************************************************************************//**
     * \fn function
     * \brief Returns the name of the local operation.
     * \return local operation name (string)
    **********************************************************************************/
    std::string function() const;

    /******************************************************************************//**
     * \fn operation
     * \brief Returns the name of the algebraic operation (max, min, or sum) applied to the data.
     * \return local operation name (string)
    **********************************************************************************/
    std::string operation() const;

    /******************************************************************************//**
     * \fn result
     * \brief Returns result from algebraic operation.
     * \return result (double)
    **********************************************************************************/
    double result() const;

    /******************************************************************************//**
     * \fn unittest
     * \brief Flag for unit testing purposes. It facilitates unit testing of the operator()() function.
     * \param [in] aUnitTest boolean flag (default = false)
    **********************************************************************************/
    void unittest(const bool aUnitTest);

private:
    /******************************************************************************//**
     * \fn checkInputs
     * \brief This function checks if all the required input data was set.
    **********************************************************************************/
    void checkInputs();

    /******************************************************************************//**
     * \fn checkInput
     * \brief This function checks if input 'keyword' was set.
     * \return throw error if data was not set
    **********************************************************************************/
    void checkInput(const std::string& aKeyword);

    /******************************************************************************//**
     * \fn initialize
     * \brief Initialize class metadata.
    **********************************************************************************/
    void initialize();

    /******************************************************************************//**
     * \fn getInputArguments
       \brief Get input arguments for local operation.
     * \param [in] aLocalArgs list of local arguments.
    **********************************************************************************/
    void getInputArguments(std::vector<Plato::LocalArg>& aLocalArgs);

    /******************************************************************************//**
     * \fn getOutputArguments
       \brief Get output arguments for local operation.
     * \param [in] aLocalArgs list of local arguments.
    **********************************************************************************/
    void getOutputArguments(std::vector<Plato::LocalArg>& aLocalArgs);

private:
    bool mUnitTest = false; /*!< flag used to indicate that the operator()() function is being unit tested > */
    double mLocalOutputValue = 0.0; /*!< local copy of the output from operator()() > */

    Plato::InputData mInputData; /*!< local operation metadata > */
    std::unordered_map<std::string, std::vector<std::string>> mInputStrKeyValuePairs; /*!< map from input keyword to value > */
};
// class HarvestDataFromFile

}
// namespace Plato