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

#include "Plato_InitializeMLSPoints.hpp"
#include "Plato_MLS.hpp"
#include "Plato_Parser.hpp"
#include "Plato_InputData.hpp"
#include "Plato_Exceptions.hpp"
#include "Plato_MetaDataMLS.hpp"
#include "Plato_LocalOperation.hpp"
#include "PlatoApp.hpp"

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
BOOST_CLASS_EXPORT_IMPLEMENT(Plato::InitializeMLSPoints<1>)
BOOST_CLASS_EXPORT_IMPLEMENT(Plato::InitializeMLSPoints<2>)
BOOST_CLASS_EXPORT_IMPLEMENT(Plato::InitializeMLSPoints<3>)

namespace Plato
{

template<int SpaceDim, typename ScalarType>
InitializeMLSPoints<SpaceDim, ScalarType>::InitializeMLSPoints(PlatoApp* aPlatoApp, Plato::InputData& aNode) :
        Plato::LocalOp(aPlatoApp),
        mOutputName("MLS Point Values")
{
    auto tName = Plato::Get::String(aNode, "MLSName");
    auto& tMLS = mPlatoApp->getMovingLeastSquaredData();
    if(tMLS.count(tName) == 0)
    {
        throw Plato::ParsingException("Requested PointArray that doesn't exist.");
    }
    mMLS = mPlatoApp->getMovingLeastSquaredData()[tName];

    mFieldName = Plato::Get::String(aNode, "Field");
}

template<int SpaceDim, typename ScalarType>
void InitializeMLSPoints<SpaceDim, ScalarType>::operator()()
{
    auto tFields = Plato::any_cast<MLS_Type>(mMLS->mls).getPointFields();
    auto tField = tFields[mFieldName];

    std::vector<double>* tLocalData = mPlatoApp->getValue(mOutputName);
    Kokkos::View<ScalarType*, Kokkos::HostSpace, Kokkos::MemoryUnmanaged>
        tLocalDataHost(tLocalData->data(), tLocalData->size());
    Kokkos::deep_copy(tLocalDataHost, tField);
}

template<int SpaceDim, typename ScalarType>
void InitializeMLSPoints<SpaceDim, ScalarType>::getArguments(std::vector<Plato::LocalArg>& aLocalArgs)
{
    int tNumPoints = Plato::any_cast<MLS_Type>(mMLS->mls).getNumPoints();
    aLocalArgs.push_back(Plato::LocalArg
        { Plato::data::layout_t::SCALAR, mOutputName, tNumPoints });
}

template class InitializeMLSPoints<3>;
template class InitializeMLSPoints<2>;
template class InitializeMLSPoints<1>;

}
// namespace Plato
