/******************************************************************************
 * size_constraint_label_propagation.h
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

#include <unordered_map>

#include "../edge_rating/edge_ratings.h"
#include "../matching/gpa/gpa_matching.h"
#include "node_ordering.h"
#include "tools/random_functions.h"

#include "size_constraint_label_propagation.h"

void size_constraint_label_propagation::match(const PartitionConfig &partition_config, graph_access &G, Matching &,
                                              CoarseMapping &coarse_mapping, NodeID &no_of_coarse_vertices,
                                              NodePermutationMap &permutation) {
    permutation.resize(G.number_of_nodes());
    coarse_mapping.resize(G.number_of_nodes());
    no_of_coarse_vertices = 0;

    if (partition_config.ensemble_clusterings) {
        ensemble_clusterings(partition_config, G, coarse_mapping, no_of_coarse_vertices);
    } else {
        match_internal(partition_config, G, coarse_mapping, no_of_coarse_vertices);
    }
}

void size_constraint_label_propagation::match_internal(const PartitionConfig &partition_config, graph_access &G,
                                                       CoarseMapping &coarse_mapping, NodeID &no_of_coarse_vertices) {

    std::vector<NodeWeight> cluster_id(G.number_of_nodes());
    NodeWeight block_upperbound =
        ceil(partition_config.upper_bound_partition / (double)partition_config.cluster_coarsening_factor);

    label_propagation(partition_config, G, block_upperbound, cluster_id, no_of_coarse_vertices);
    create_coarsemapping(G, cluster_id, coarse_mapping);
}

void size_constraint_label_propagation::ensemble_two_clusterings(const graph_access &G, const std::vector<NodeID> &lhs,
                                                                 const std::vector<NodeID> &rhs,
                                                                 std::vector<NodeID> &output,
                                                                 NodeID &no_of_coarse_vertices) {

    hash_ensemble new_mapping;
    no_of_coarse_vertices = 0;
    for (NodeID node = 0; node < lhs.size(); node++) {
        ensemble_pair cur_pair{};
        cur_pair.lhs = lhs[node];
        cur_pair.rhs = rhs[node];
        cur_pair.n = G.number_of_nodes();

        if (new_mapping.find(cur_pair) == new_mapping.end()) {
            new_mapping[cur_pair].mapping = no_of_coarse_vertices;
            no_of_coarse_vertices++;
        }

        output[node] = new_mapping[cur_pair].mapping;
    }

    no_of_coarse_vertices = new_mapping.size();
}

void size_constraint_label_propagation::ensemble_clusterings(const PartitionConfig &partition_config, graph_access &G,
                                                             CoarseMapping &coarse_mapping,
                                                             NodeID &no_of_coarse_vertices) {
    int runs = partition_config.number_of_clusterings;
    std::vector<NodeID> cur_cluster(G.number_of_nodes(), 0);
    std::vector<NodeID> ensemble_cluster(G.number_of_nodes(), 0);

    int new_cf = partition_config.cluster_coarsening_factor;
    for (int i = 0; i < runs; i++) {
        PartitionConfig config = partition_config;
        config.cluster_coarsening_factor = new_cf;

        NodeID cur_no_blocks = 0;
        label_propagation(config, G, cur_cluster, cur_no_blocks);

        if (i != 0) {
            ensemble_two_clusterings(G, cur_cluster, ensemble_cluster, ensemble_cluster, no_of_coarse_vertices);
        } else {
            for (auto node : G.nodes()) {
                ensemble_cluster[node] = cur_cluster[node];
            }

            no_of_coarse_vertices = cur_no_blocks;
        }
        new_cf = static_cast<int>(random_functions::nextInt(10, 30));
    }

    create_coarsemapping(G, ensemble_cluster, coarse_mapping);
}

void size_constraint_label_propagation::label_propagation(const PartitionConfig &partition_config, graph_access &G,
                                                          std::vector<NodeWeight> &cluster_id,
                                                          NodeID &number_of_blocks) {
    NodeWeight block_upperbound =
        ceil(partition_config.upper_bound_partition / (double)partition_config.cluster_coarsening_factor);

    label_propagation(partition_config, G, block_upperbound, cluster_id, number_of_blocks);
}

void size_constraint_label_propagation::label_propagation(const PartitionConfig &partition_config, graph_access &G,
                                                          const NodeWeight &block_upperbound,
                                                          std::vector<NodeWeight> &cluster_id,
                                                          NodeID &number_of_blocks) {
    // coarse_mapping stores cluster id and the mapping (it is identical)
    std::vector<PartitionID> hash_map(G.number_of_nodes(), 0);
    std::vector<NodeID> permutation(G.number_of_nodes());
    std::vector<NodeWeight> cluster_sizes(G.number_of_nodes());
    std::vector<NodeWeight> cluster_local_sizes(G.number_of_nodes());
    cluster_id.resize(G.number_of_nodes());

    for (auto node : G.nodes()) {
        cluster_sizes[node] = G.getNodeWeight(node);
        cluster_id[node] = node;
        cluster_local_sizes[node] = 1;
    }

    node_ordering::order_nodes(partition_config, G, permutation);

    for (int j = 0; j < partition_config.label_iterations; j++) {
        for (auto i : G.nodes()) {
            NodeID node = permutation[i];
            // now move the node to the cluster that is most common in the neighborhood

            forall_out_edges(G, e, node) {
                NodeID target = G.getEdgeTarget(e);
                hash_map[cluster_id[target]] += static_cast<PartitionID>(G.getEdgeWeight(e));
            }
            endfor

                // second sweep for finding max and resetting array
                PartitionID max_block = cluster_id[node];
            PartitionID my_block = cluster_id[node];

            PartitionID max_value = 0;
            forall_out_edges(G, e, node) {
                NodeID target = G.getEdgeTarget(e);
                PartitionID cur_block = cluster_id[target];
                PartitionID cur_value = hash_map[cur_block];
                if ((cur_value > max_value || (cur_value == max_value && random_functions::nextBool())) &&
                    (cluster_local_sizes[cur_block] + 1 < partition_config.cluster_upperbound ||
                     cur_block == my_block) &&
                    (cluster_sizes[cur_block] + G.getNodeWeight(node) < block_upperbound || cur_block == my_block) &&
                    (!partition_config.combine ||
                     G.getSecondPartitionIndex(node) == G.getSecondPartitionIndex(target))) {
                    max_value = cur_value;
                    max_block = cur_block;
                }

                hash_map[cur_block] = 0;
            }
            endfor

                cluster_sizes[cluster_id[node]] -= G.getNodeWeight(node);
            cluster_local_sizes[cluster_id[node]]--;
            cluster_sizes[max_block] += G.getNodeWeight(node);
            cluster_local_sizes[max_block]++;
            cluster_id[node] = max_block;
        }
    }

    remap_cluster_ids(G, cluster_id, number_of_blocks);
}

void size_constraint_label_propagation::create_coarsemapping(const graph_access &G, std::vector<NodeWeight> &cluster_id,
                                                             CoarseMapping &coarse_mapping) {
    for (auto node : G.nodes()) {
        coarse_mapping[node] = cluster_id[node];
    }
}

void size_constraint_label_propagation::remap_cluster_ids(graph_access &G, std::vector<NodeWeight> &cluster_id,
                                                          NodeID &no_of_coarse_vertices, bool apply_to_graph) {

    PartitionID cur_no_clusters = 0;
    std::unordered_map<PartitionID, PartitionID> remap;
    for (auto node : G.nodes()) {
        PartitionID cur_cluster = cluster_id[node];
        // check wether we already had that
        if (remap.find(cur_cluster) == remap.end()) {
            remap[cur_cluster] = cur_no_clusters++;
        }

        cluster_id[node] = remap[cur_cluster];
    }

    if (apply_to_graph) {
        for (auto node : G.nodes()) {
            G.setPartitionIndex(node, cluster_id[node]);
        }
        G.set_partition_count(cur_no_clusters);
    }

    no_of_coarse_vertices = cur_no_clusters;
}
