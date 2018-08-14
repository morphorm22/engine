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

#pragma once

#include "PSL_ParameterDataEnums.hpp"
#include "PSL_Preprocessor.hpp"
#include <vector>

namespace PlatoSubproblemLibrary
{
class AbstractAuthority;
class ParameterData;
class ClassificationArchive;

class MultistagePreprocessor : public Preprocessor
{
public:
    MultistagePreprocessor(AbstractAuthority* authority,
                           const std::vector<preprocessor_t::preprocessor_t>& preprocess_stages,
                           DataSequence* data_sequence);
    virtual ~MultistagePreprocessor();

    // for data flow
    virtual void unpack_data();
    virtual void allocate_data();

    // for preprocessor
    virtual void preprocess(std::vector<double>& input_all);
    virtual void internal_initialize(ParameterData* parameters,
                                     ClassificationArchive* archive,
                                     AbstractInterface::DenseMatrix* input,
                                     const int& considered_columns,
                                     int& processed_length);

protected:
    std::vector<Preprocessor*> m_staged_preprocessors;

};

}