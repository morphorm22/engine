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
 * XMLGeneratorUtilities.hpp
 *
 *  Created on: Nov 19, 2019
 *
 */

#ifndef SRC_XMLGENERATORUTILITIES_HPP_
#define SRC_XMLGENERATORUTILITIES_HPP_

#include "Plato_Parser.hpp"
#include "XMLGeneratorDataStruct.hpp"

namespace XMLGen
{

    bool addNTVParameter(pugi::xml_node parent_node,
                     const std::string &name,
                     const std::string &type,
                     const std::string &value);
    void getUncertaintyFlags(const InputData &aInputData,
                         bool &aHasUncertainties,
                         bool &aRequestedVonMisesOutput);
    bool addChild(pugi::xml_node parent_node, 
              const std::string &name, 
              const std::string &value);

    pugi::xml_node createSingleUserNodalSharedData(pugi::xml_document &aDoc,
                                                   const std::string &aName,
                                                   const std::string &aType,
                                                   const std::string &aOwner,
                                                   const std::string &aUser);
    pugi::xml_node createSingleUserNodalSharedData(pugi::xml_node &aNode,
                                                   const std::string &aName,
                                                   const std::string &aType,
                                                   const std::string &aOwner,
                                                   const std::string &aUser);
    pugi::xml_node createSingleUserElementSharedData(pugi::xml_document &aDoc,
                                                     const std::string &aName,
                                                     const std::string &aType,
                                                     const std::string &aOwner,
                                                     const std::string &aUser);
    pugi::xml_node createSingleUserElementSharedData(pugi::xml_node &aNode,
                                                     const std::string &aName,
                                                     const std::string &aType,
                                                     const std::string &aOwner,
                                                     const std::string &aUser);
    pugi::xml_node createSingleUserGlobalSharedData(pugi::xml_document &aDoc,
                                                    const std::string &aName,
                                                    const std::string &aType,
                                                    const std::string &aSize,
                                                    const std::string &aOwner,
                                                    const std::string &aUser);
    pugi::xml_node createSingleUserGlobalSharedData(pugi::xml_node &aNode,
                                                    const std::string &aName,
                                                    const std::string &aType,
                                                    const std::string &aSize,
                                                    const std::string &aOwner,
                                                    const std::string &aUser);
    pugi::xml_node createMultiUserGlobalSharedData(pugi::xml_document &aDoc,
                                                   const std::string &aName,
                                                   const std::string &aType,
                                                   const std::string &aSize,
                                                   const std::string &aOwner,
                                                   const std::vector<std::string> &aUsers);

}

#endif /* SRC_XMLGENERATORUTILITIES_HPP_ */
