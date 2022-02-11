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
 * Plato_SystemCall.hpp
 *
 *  Created on: Oct 23, 2019
 */

#pragma once

#include "Plato_LocalOperation.hpp"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

#include <mpi.h>

namespace Plato
{

struct SystemCallMetadata
{
    std::unordered_map<std::string,std::vector<double>*> mInputArgumentMap;
};

class InputData;

/******************************************************************************//**
 * \brief Call a shell script
**********************************************************************************/
class SystemCall
{
public:
    /******************************************************************************//**
     * \brief Constructor
     * \param [in] aPlatoApp PLATO application
     * \param [in] aNode input metadata
    **********************************************************************************/
    SystemCall(const Plato::InputData & aNode, const Plato::SystemCallMetadata& aMetaData);

    /******************************************************************************//**
     * \brief perform local operation to call a shell script.
    **********************************************************************************/
    void operator()(const Plato::SystemCallMetadata& aMetaData);

    /******************************************************************************//**
     * \brief Return local operation's argument list
     * \param [out] aLocalArgs argument list
    **********************************************************************************/
    void getArguments(std::vector<Plato::LocalArg> & aLocalArgs);

    /******************************************************************************//**
     * \brief Return number of ranks used in mpi system call.
     * \return number of ranks
    **********************************************************************************/
    int numRanks() const { return mNumRanks; }

    /******************************************************************************//**
     * \brief Return on change flag 
     * \return boolean flag
    **********************************************************************************/
    bool onChange() const { return mOnChange; }

    /******************************************************************************//**
     * \brief Return append input flag 
     * \return boolean flag
    **********************************************************************************/
    bool appendInput() const { return mAppendInput; }

    /******************************************************************************//**
     * \brief Return user defined name for system call operation.
     * \return operation name
    **********************************************************************************/
    std::string name() const{ return mName;}

    /******************************************************************************//**
     * \brief Return main command to run shell script.
     * \return command string
    **********************************************************************************/
    std::string command() const{ return mStringCommand;}

    /******************************************************************************//**
     * \brief Return list of options appended to shell script command.
     * \return list of options
    **********************************************************************************/
    std::vector<std::string> options() const {return mOptions; }

    /******************************************************************************//**
     * \brief Return list of arguments appended to shell script command.
     * \return list of arguments
    **********************************************************************************/
    std::vector<std::string> arguments() const {return mArguments; }

    /******************************************************************************//**
     * \brief Return list of input shared data argument names.
     * \return list of names
    **********************************************************************************/
    std::vector<std::string> inputNames() const {return mInputNames; }

    /******************************************************************************//**
     * \brief Return string for the full command, including arguments and options, \n
     *        used to execute shell script.
     * \return list of names
    **********************************************************************************/
    std::string commandPlusArguments() const {return mCommandPlusArguments; }

private:
    /******************************************************************************//**
     * \brief Append input values to the end of the argument list.
     * \param [out] aArguments command argument list
    **********************************************************************************/
    void appendOptionsAndValues(const Plato::SystemCallMetadata& aMetaData, std::vector<std::string>& aArguments);

    /******************************************************************************//**
     * \brief If the parameterd changed since the last call to the shell script, save the new parameters.
     * \return boolean flag indicating if parameters changed since the last call to the shell script. 
    **********************************************************************************/
    void saveParameters(const Plato::SystemCallMetadata& aMetaData);

    /******************************************************************************//**
     * \brief perform system call to a shell script.
    **********************************************************************************/
    void performSystemCall(const Plato::SystemCallMetadata& aMetaData);

    /******************************************************************************//**
     * \brief Check if the parameters were modified locally; if modified, save the new parameters.
     * \return boolean flag indicating if parameters changed since the last call to the shell script. 
    **********************************************************************************/
    bool checkForLocalParameterChanges(const Plato::SystemCallMetadata& aMetaData);

    /******************************************************************************//**
     * \brief Set input shared data argument names.
     * \param [in] aNode input XML metadata
    **********************************************************************************/
    void setInputSharedDataNames(const Plato::InputData& aNode);

    /******************************************************************************//**
     * \brief Set list of command arguments.
     * \param [in] aNode input XML metadata
    **********************************************************************************/
    void setArguments(const Plato::InputData& aNode);

    /******************************************************************************//**
     * \brief Set list of command options.
     * \param [in] aNode input XML metadata
    **********************************************************************************/
    void setOptions(const Plato::InputData& aNode, const Plato::SystemCallMetadata& aMetaData);

    /******************************************************************************//**
     * \brief Return the total number of parameters.
     * \return total number of parameters
    **********************************************************************************/
    size_t countTotalNumParameters(const Plato::SystemCallMetadata& aMetaData) const;

    /******************************************************************************//**
     * \brief Execute call to the shell script.
     * \param [in] aArguments arguments to be appended to shell command.
    **********************************************************************************/
    virtual void executeCommand(const std::vector<std::string> &aArguments);
    
private:
    bool mOnChange;
    bool mAppendInput;
    
    std::string mName;

    std::vector<std::string> mOptions;
    std::vector<std::string> mArguments;
    std::vector<std::string> mInputNames;
    std::vector<std::vector<double>> mSavedParameters;

protected:
    int mNumRanks = 1;
    std::string mStringCommand;
    std::string mCommandPlusArguments;
};
// class SystemCall

class SystemCallMPI : public SystemCall {
public:
SystemCallMPI(const Plato::InputData & aNode, const Plato::SystemCallMetadata& aMetaData, const MPI_Comm& aComm);
private:
void executeCommand(const std::vector<std::string> &arguments) override;
const MPI_Comm& mComm;
};

}
// namespace Plato
