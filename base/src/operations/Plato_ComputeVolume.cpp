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
 * Plato_ComputeVolume.cpp
 *
 *  Created on: Jun 28, 2019
 */

#include "PlatoApp.hpp"
#include "Plato_InputData.hpp"
#include "Plato_PenaltyModel.hpp"
#include "Plato_ComputeVolume.hpp"

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(Plato::ComputeVolume)

namespace Plato
{

ComputeVolume::ComputeVolume(const std::string& aVolumeName,
                             const std::string& aGradientName,
                             Plato::PenaltyModel* aPenaltyModel,
                             const std::string& aTopologyName):
                             mVolumeName(aVolumeName),
                             mGradientName(aGradientName),
                             mPenaltyModel(aPenaltyModel),
                             mTopologyName(aTopologyName)
{
}


ComputeVolume::ComputeVolume(PlatoApp* aPlatoApp, Plato::InputData& aNode) :
        Plato::LocalOp(aPlatoApp),
        mTopologyName("Topology"),
        mVolumeName("Volume"),
        mGradientName("Volume Gradient"),
        mPenaltyModel(nullptr)
{
    Plato::PenaltyModelFactory tPenaltyModelFactory;
    mPenaltyModel = tPenaltyModelFactory.create(aNode);
}

ComputeVolume::~ComputeVolume()
{
    if(mPenaltyModel)
    {
        delete mPenaltyModel;
        mPenaltyModel = nullptr;
    } 
}

void ComputeVolume::getArguments(std::vector<Plato::LocalArg>& aLocalArgs)
{
    aLocalArgs.push_back(Plato::LocalArg {Plato::data::layout_t::SCALAR_FIELD, mTopologyName});
    aLocalArgs.push_back(Plato::LocalArg {Plato::data::layout_t::SCALAR, mVolumeName,/*length=*/1});
    aLocalArgs.push_back(Plato::LocalArg {Plato::data::layout_t::SCALAR_FIELD, mGradientName});
}

void ComputeVolume::operator()()
{
    // Input:  Topology
    // Output: Volume, Volume Gradient

    // get local topology
    auto& tTopology = *(mPlatoApp->getNodeField(mTopologyName));

    // get local volume
    std::vector<double>* tData = mPlatoApp->getValue(mVolumeName);
    double& tVolumeValue = (*tData)[0];

    // get local gradient
    auto& tVolumeGradient = *(mPlatoApp->getNodeField(mGradientName));

    mPlatoApp->getMeshServices()->getCurrentVolume(tTopology, tVolumeValue, tVolumeGradient);

    return;
}

}
// namespace Plato
