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

// PlatoSubproblemLibraryVersion(8): a stand-alone library for the kernel filter for plato.
#pragma once

#include "PSL_Abstract_ParallelExchanger.hpp"

#include <vector>
#include <cstddef>

namespace PlatoSubproblemLibrary
{
class AbstractAuthority;
namespace AbstractInterface
{
class ParallelVector;

class ParallelExchanger_Managed : public AbstractInterface::ParallelExchanger
{
public:
    ParallelExchanger_Managed(AbstractAuthority* authority);
    virtual ~ParallelExchanger_Managed();

    virtual void build() = 0;

    // get indexes of local points that are locally owned
    virtual std::vector<size_t> get_local_contracted_indexes();
    // convert data vector in parallel format to vector of values for locally owned points
    virtual std::vector<double> get_contraction_to_local_indexes(ParallelVector* input_data_vector);
    // communicate between processors to expand the locally owned data to parallel format with some values shared on processors
    virtual void get_expansion_to_parallel_vector(const std::vector<double>& input_data_vector, ParallelVector* output_data_vector);
    // determine maximum absolute parallel error
    virtual double get_maximum_absolute_parallel_error(ParallelVector* input_data_vector);

protected:
    // for contraction
    std::vector<size_t> m_contracted_to_local;

    // for expansion
    std::vector<std::vector<size_t> > m_local_index_to_send;
    std::vector<std::vector<size_t> > m_local_index_to_recv;

};

}

}
