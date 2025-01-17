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
#ifndef PLATO_UTILS_H
#define PLATO_UTILS_H

#include <vector>
#include <string>
#include <unistd.h>

namespace Plato
{

namespace Utils
{

template<typename T>
T* byName(const std::vector<T*> & aArgumentVector, const std::string & aName);

/******************************************************************************//**
 * \brief Utility function used to avoid warning related to unused variables.
 * \param [in] aInput input
**********************************************************************************/
template<typename Type>
void ignore_unused(Type& aInput);

/******************************************************************************//**
 * \brief Call the chdir command to change the current working directory. chdir \n
 *     changes the current working directory of the calling process to the directory \n
 *     specified in aPath if it is not an empty string.
 * \param [in] aPath directory path
**********************************************************************************/
inline void change_directory(const std::string& aPath);

inline std::string current_working_directory();

/// @brief A strongly-typed wrapper for clarifying interfaces. 
/// 
/// The purpose of this class is to facilitate using strong types for making 
/// function signatures clear. This can help catch errors at compile time that would
/// normally be caught at run time. For example, if we have a function for loading an 
/// xml file into an object, we might need the file name and a node name as string arguments:
/// @code{.cpp}
/// void loadXML(const std::string& aFileName, const std::string& aNodeName);
/// @endcode
/// However, this interface is error-prone since it is easy to swap the arguments.
/// Instead, this wrapper may be used, for example:
/// @code{.cpp}
/// using XMLFileName = NamedType<std::string, struct XMLFileNameTag>;
/// using XMLNodeName = NamedType<std::string, struct XMLNodeNameTag>;
/// void loadXML(const XMLFileName& aFileName, const XMLNodeName& aNodeName);
/// @endcode
/// This involves more code, but prevents errors because now `loadXML` can only be called
/// with the strong types, such as:
/// @code{.cpp}
/// loadXML(XMLFileName{"myXmlFile.xml"}, XMLNodeName{"TopNode"});
/// @endcode
/// 
/// @tparam T The wrapped type.
/// @tparam NamedTag A tag to associate with the type, typically an empty struct with a meaningful
///  name shown in the example. This prevents copying two NamedType with the same wrapped type T.
template <typename T, typename NameTag>
struct NamedType
{
    explicit NamedType(const T& value) : mValue(value) {}
    explicit NamedType(T&& value) : mValue(std::move(value)) {}

    T mValue;
};

}
// namespace Utils

}
// namespace Plato

/************* byName **************/
template <typename T>
T* Plato::Utils::byName(const std::vector<T*> & aArgumentVector, const std::string & aName)
{
  for( T* item : aArgumentVector ){
    if( item->myName() == aName ){
      return item;
    }
  }
  return nullptr;
}
/************* byName **************/

/************* ignore_unused **************/
template<typename Type>
void Plato::Utils::ignore_unused(Type& aInput)
{ return; }
/************** ignore_unused **************/

/************** change_directory **************/
inline void Plato::Utils::change_directory(const std::string& aPath)
{
    if(!aPath.empty())
    {
        auto tTrash = chdir(aPath.c_str());
        Plato::Utils::ignore_unused(tTrash);
    }
}
/************** change_directory **************/

/************** current_working_directory **************/
inline std::string Plato::Utils::current_working_directory()
{
    char* tCWD = get_current_dir_name();
    std::string tOutput(tCWD);
    std::free(tCWD);
    return tOutput;
}
/************** current_working_directory **************/

#endif