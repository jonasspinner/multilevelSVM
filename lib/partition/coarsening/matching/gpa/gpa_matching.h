/******************************************************************************
 * gpa_matching.h
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 *
 ******************************************************************************
 * Copyright (C) 2013-2015 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef GPA_MATCHING_NXLQ0SIT
#define GPA_MATCHING_NXLQ0SIT

#include "partition/coarsening/matching/matching.h"
#include "path.h"
#include "path_set.h"

class gpa_matching : public matching {
  public:
    gpa_matching() = default;

    ~gpa_matching() override = default;

    void match(const PartitionConfig &config, graph_access &G, Matching &_matching, CoarseMapping &coarse_mapping,
               NodeID &no_of_coarse_vertices, NodePermutationMap &permutation) override;

  private:
    static void init(graph_access &G, const PartitionConfig &partition_config, NodePermutationMap &permutation,
                     Matching &edge_matching, std::vector<EdgeID> &edge_permutation, std::vector<NodeID> &sources);

    static void extract_paths_apply_matching(graph_access &G, std::vector<NodeID> &sources, Matching &edge_matching,
                                             path_set &pathset);

    template <typename VectorOrDeque>
    static void unpack_path(const path &p, const path_set &pathset, VectorOrDeque &unpacked_path);

    template <typename VectorOrDeque>
    static void maximum_weight_matching(graph_access &G, VectorOrDeque &unpacked_path,
                                        std::vector<EdgeID> &matched_edges, EdgeRatingType &final_rating);

    static void apply_matching(graph_access &G, std::vector<EdgeID> &matched_edges, std::vector<NodeID> &sources,
                               Matching &edge_matching);
};

#endif /* end of include guard: GPA_MATCHING_NXLQ0SIT */
