/******************************************************************************
 * constraint_label_propagation.cpp
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

#ifndef SIZE_CONSTRAINT_LABEL_PROPAGATION_7SVLBKKT
#define SIZE_CONSTRAINT_LABEL_PROPAGATION_7SVLBKKT

#include "../matching/matching.h"
#include <unordered_map>

struct ensemble_pair {
    PartitionID n; // number of nodes in the graph
    PartitionID lhs;
    PartitionID rhs;
};

struct compare_ensemble_pair {
    bool operator()(const ensemble_pair pair_a, const ensemble_pair pair_b) const {
        bool eq = (pair_a.lhs == pair_b.lhs && pair_a.rhs == pair_b.rhs);
        return eq;
    }
};

struct hash_ensemble_pair {
    size_t operator()(const ensemble_pair pair) const { return pair.lhs * pair.n + pair.rhs; }
};

struct data_ensemble_pair {
    NodeID mapping{0};
};

using hash_ensemble =
    std::unordered_map<const ensemble_pair, data_ensemble_pair, hash_ensemble_pair, compare_ensemble_pair>;

class size_constraint_label_propagation : public matching {
  public:
    size_constraint_label_propagation() = default;

    ~size_constraint_label_propagation() override = default;

    void match(const PartitionConfig &config, graph_access &G, Matching &_matching, CoarseMapping &coarse_mapping,
               NodeID &no_of_coarse_vertices, NodePermutationMap &permutation) override;

    static void ensemble_clusterings(const PartitionConfig &partition_config, graph_access &G,
                                     CoarseMapping &coarse_mapping, NodeID &no_of_coarse_vertices);

    static void ensemble_two_clusterings(const graph_access &G, const std::vector<NodeID> &lhs,
                                         const std::vector<NodeID> &rhs, std::vector<NodeID> &output,
                                         NodeID &no_of_coarse_vertices);

    static void match_internal(const PartitionConfig &config, graph_access &G, CoarseMapping &coarse_mapping,
                               NodeID &no_of_coarse_vertices);

    static void remap_cluster_ids(graph_access &G, std::vector<NodeID> &cluster_id, NodeID &no_of_coarse_vertices,
                                  bool apply_to_graph = false);

    static void create_coarsemapping(const graph_access &G, std::vector<NodeID> &cluster_id,
                                     CoarseMapping &coarse_mapping);

    static void label_propagation(const PartitionConfig &partition_config, graph_access &G,
                                  const NodeWeight &block_upperbound,
                                  std::vector<NodeID> &cluster_id, // output paramter
                                  NodeID &number_of_blocks);       // output parameter

    static void label_propagation(const PartitionConfig &partition_config, graph_access &G,
                                  std::vector<NodeWeight> &cluster_id, NodeID &number_of_blocks);
};

#endif /* end of include guard: SIZE_CONSTRAINT_LABEL_PROPAGATION_7SVLBKKT */
