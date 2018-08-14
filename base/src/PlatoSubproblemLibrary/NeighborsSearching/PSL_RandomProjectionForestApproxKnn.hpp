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

#include "PSL_KNearestNeighborsSearcher.hpp"
#include <vector>
#include <queue>
#include <functional>

namespace PlatoSubproblemLibrary
{
class AbstractAuthority;
class ParameterData;

class RandomProjectionForestApproxKnn : public KNearestNeighborsSearcher
{
public:
    RandomProjectionForestApproxKnn(AbstractAuthority* authority);
    virtual ~RandomProjectionForestApproxKnn();

    virtual bool is_exact();
    virtual void extract_parameters(ParameterData* parameters);

    // for data flow
    virtual void unpack_data();
    virtual void allocate_data();

    // for searching
    virtual void initialize();
    virtual void get_neighbors(const int& num_neighbors,
                               const std::vector<double>& input_to_search,
                               std::vector<double>& worst_to_best_distances,
                               std::vector<int>& worst_to_best_class_outputs);

protected:

    void recursively_build_project_forest(const int& tree,
                                          const int& this_level,
                                          const int& this_cell,
                                          const std::vector<std::vector<std::vector<double> > >& row_tree_level,
                                          const std::vector<int>& active_rows);
    void recursively_follow_project_forest(const int& tree,
                                           const int& this_level,
                                           const int& this_cell,
                                           const int& num_neighbors,
                                           const std::vector<double>& query_point,
                                           std::priority_queue<std::pair<double, int> >& distance_then_outputClass,
                                           std::vector<bool>& have_searched,
                                           const std::vector<double>& projected_values);

    int m_initial_num_trees;
    int m_initial_approx_leaf_size;

    int* m_num_trees;
    int* m_num_levels;
    std::vector<std::vector<std::vector<int>*> > m_tree_then_leaf_then_rows;
    std::vector<std::vector<double>*> m_tree_then_split_points;
    std::vector<AbstractInterface::DenseMatrix**> m_random_projections;

};

}