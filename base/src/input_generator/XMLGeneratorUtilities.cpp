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
 * XMLGeneratorUtilities.cpp
 *
 *  Created on: Nov 19, 2019
 *
 */

#include "XMLGeneratorUtilities.hpp"

namespace XMLGen
{

/******************************************************************************/
bool addChild(pugi::xml_node parent_node,
              const std::string &name,
              const std::string &value)
/******************************************************************************/
{
    pugi::xml_node tmp_node = parent_node.append_child(name.c_str());
    tmp_node = tmp_node.append_child(pugi::node_pcdata);
    tmp_node.set_value(value.c_str());
    return true;
}

/******************************************************************************/
bool addNTVParameter(pugi::xml_node parent_node,
                     const std::string &name,
                     const std::string &type,
                     const std::string &value)
/******************************************************************************/
{
    pugi::xml_node node = parent_node.append_child("Parameter");
    node.append_attribute("name") = name.c_str();
    node.append_attribute("type") = type.c_str();
    node.append_attribute("value") = value.c_str();
    return true;
}

/******************************************************************************/
void getUncertaintyFlags(const InputData &aInputData,
                         bool &aHasUncertainties,
                         bool &aRequestedVonMisesOutput)
/******************************************************************************/
{
    for(size_t i=0; i<aInputData.objectives.size(); ++i)
    {
        const XMLGen::Objective cur_obj = aInputData.objectives[i];
        for(size_t k=0; k<cur_obj.load_case_ids.size(); k++)
        {
            std::string cur_load_string = cur_obj.load_case_ids[k];
            for(size_t j=0; aRequestedVonMisesOutput == false && j<cur_obj.output_for_plotting.size(); j++)
            {
                if(cur_obj.output_for_plotting[j] == "vonmises")
                {
                    aRequestedVonMisesOutput = true;
                }
            }
            for(size_t j=0; aHasUncertainties == false && j<aInputData.uncertainties.size(); ++j)
            {
                if(cur_load_string == aInputData.uncertainties[j].id)
                {
                    aHasUncertainties = true;
                }
            }
        }
    }
}

/******************************************************************************/
pugi::xml_node createSingleUserNodalSharedData(pugi::xml_document &aDoc,
                                                             const std::string &aName,
                                                             const std::string &aType,
                                                             const std::string &aOwner,
                                                             const std::string &aUser)
/******************************************************************************/
{   
    pugi::xml_node sd_node = aDoc.append_child("SharedData");
    addChild(sd_node, "Name", aName);
    addChild(sd_node, "Type", aType);
    addChild(sd_node, "Layout", "Nodal Field");
    addChild(sd_node, "OwnerName", aOwner);
    addChild(sd_node, "UserName", aUser);
    return sd_node;
}

/******************************************************************************/
pugi::xml_node createSingleUserNodalSharedData(pugi::xml_node &aNode,
                                                             const std::string &aName,
                                                             const std::string &aType,
                                                             const std::string &aOwner,
                                                             const std::string &aUser)
/******************************************************************************/
{   
    pugi::xml_node sd_node = aNode.append_child("SharedData");
    addChild(sd_node, "Name", aName);
    addChild(sd_node, "Type", aType);
    addChild(sd_node, "Layout", "Nodal Field");
    addChild(sd_node, "OwnerName", aOwner);
    addChild(sd_node, "UserName", aUser);
    return sd_node;
}

/******************************************************************************/
pugi::xml_node createSingleUserElementSharedData(pugi::xml_document &aDoc,
                                                               const std::string &aName,
                                                               const std::string &aType,
                                                               const std::string &aOwner,
                                                               const std::string &aUser)
/******************************************************************************/
{
    pugi::xml_node sd_node = aDoc.append_child("SharedData");
    addChild(sd_node, "Name", aName);
    addChild(sd_node, "Type", aType);
    addChild(sd_node, "Layout", "Element Field");
    addChild(sd_node, "OwnerName", aOwner);
    addChild(sd_node, "UserName", aUser);
    return sd_node;
}

/******************************************************************************/
pugi::xml_node createSingleUserElementSharedData(pugi::xml_node &aNode,
                                                               const std::string &aName,
                                                               const std::string &aType,
                                                               const std::string &aOwner,
                                                               const std::string &aUser)
/******************************************************************************/
{
    pugi::xml_node sd_node = aNode.append_child("SharedData");
    addChild(sd_node, "Name", aName);
    addChild(sd_node, "Type", aType);
    addChild(sd_node, "Layout", "Element Field");
    addChild(sd_node, "OwnerName", aOwner);
    addChild(sd_node, "UserName", aUser);
    return sd_node;
}

/******************************************************************************/
pugi::xml_node createSingleUserGlobalSharedData(pugi::xml_document &aDoc,
                                                              const std::string &aName,
                                                              const std::string &aType,
                                                              const std::string &aSize,
                                                              const std::string &aOwner,
                                                              const std::string &aUser)
/******************************************************************************/
{
    pugi::xml_node sd_node = aDoc.append_child("SharedData");
    addChild(sd_node, "Name", aName);
    addChild(sd_node, "Type", aType);
    addChild(sd_node, "Layout", "Global");
    addChild(sd_node, "Size", aSize);
    addChild(sd_node, "OwnerName", aOwner);
    addChild(sd_node, "UserName", aUser);
    return sd_node;
}

/******************************************************************************/
pugi::xml_node createSingleUserGlobalSharedData(pugi::xml_node &aNode,
                                                              const std::string &aName,
                                                              const std::string &aType,
                                                              const std::string &aSize,
                                                              const std::string &aOwner,
                                                              const std::string &aUser)
/******************************************************************************/
{
    pugi::xml_node sd_node = aNode.append_child("SharedData");
    addChild(sd_node, "Name", aName);
    addChild(sd_node, "Type", aType);
    addChild(sd_node, "Layout", "Global");
    addChild(sd_node, "Size", aSize);
    addChild(sd_node, "OwnerName", aOwner);
    addChild(sd_node, "UserName", aUser);
    return sd_node;
}

/******************************************************************************/
pugi::xml_node createMultiUserGlobalSharedData(pugi::xml_document &aDoc,
                                                              const std::string &aName,
                                                              const std::string &aType,
                                                              const std::string &aSize,
                                                              const std::string &aOwner,
                                                              const std::vector<std::string> &aUsers)
/******************************************************************************/
{
    pugi::xml_node sd_node = aDoc.append_child("SharedData");
    addChild(sd_node, "Name", aName);
    addChild(sd_node, "Type", aType);
    addChild(sd_node, "Layout", "Global");
    addChild(sd_node, "Size", aSize);
    addChild(sd_node, "OwnerName", aOwner);
    for(size_t i=0; i<aUsers.size(); ++i)
    {
        addChild(sd_node, "UserName", aUsers[i]);
    }
    return sd_node;
}




}


