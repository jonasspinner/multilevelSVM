/******************************************************************************
 * contraction.cpp
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

#include "contraction.h"
#include "partition/uncoarsening/refinement/quotient_graph_refinement/complete_boundary.h"
#include "tools/macros_assertions.h"
#include "tools/timer.h"
#include <algorithm>

// for documentation see technical reports of christian schulz
void contraction::contract(const PartitionConfig &partition_config, graph_access &finer, graph_access &coarser,
                           const Matching &edge_matching, const CoarseMapping &coarse_mapping,
                           const NodeID &no_of_coarse_vertices, const NodePermutationMap &permutation) {

    if (partition_config.matching_type == LP_CLUSTERING || partition_config.matching_type == SIMPLE_CLUSTERING ||
        partition_config.matching_type == LOW_DIAMETER) {
        return contract_clustering(partition_config, finer, coarser, edge_matching, coarse_mapping, no_of_coarse_vertices,
                                   permutation);
    }

    if (partition_config.combine) {
        coarser.resizeSecondPartitionIndex(no_of_coarse_vertices);
    }

    std::vector<NodeID> new_edge_targets(finer.number_of_edges());
    for (auto e : finer.edges()) {
        new_edge_targets[e] = coarse_mapping[finer.getEdgeTarget(e)];
    }

    std::vector<EdgeID> edge_positions(no_of_coarse_vertices, UNDEFINED_EDGE);

    // we don't know the number of edges jet, so we use the old number for
    // construction of the coarser graph and then resize the field according
    // to the number of edges we really got
    coarser.start_construction(no_of_coarse_vertices, finer.number_of_edges());

    NodeID cur_no_vertices = 0;

    for (auto n : finer.nodes()) {
        NodeID node = permutation[n];
        // we look only at the coarser nodes
        if (coarse_mapping[node] != cur_no_vertices)
            continue;

        NodeID coarseNode = coarser.new_node();
        coarser.setNodeWeight(coarseNode, finer.getNodeWeight(node));
        coarser.setFeatureVec(coarseNode, finer.getFeatureVec(node));

        if (partition_config.combine) {
            coarser.setSecondPartitionIndex(coarseNode, finer.getSecondPartitionIndex(node));
        }

        // do something with all outgoing edges (in auxillary graph)
        forall_out_edges(finer, e, node) { visit_edge(finer, coarser, edge_positions, coarseNode, e, new_edge_targets); }
        endfor

            // this node was really matched
            NodeID matched_neighbor = edge_matching[node];
        if (node != matched_neighbor) {
            NodeWeight node_weight = finer.getNodeWeight(node);
            NodeWeight neighbor_weight = finer.getNodeWeight(matched_neighbor);

            // update weight of coarser node
            NodeWeight new_coarse_weight = node_weight + neighbor_weight;
            coarser.setNodeWeight(coarseNode, new_coarse_weight);

            // update feature vector weighted
            FeatureVec v1 = finer.getFeatureVec(node);
            FeatureVec v2 = finer.getFeatureVec(matched_neighbor);

            FeatureVec new_feature_vec = combineFeatureVec(v1, node_weight, v2, neighbor_weight);

            coarser.setFeatureVec(coarseNode, new_feature_vec);

            forall_out_edges(finer, e, matched_neighbor) {
                visit_edge(finer, coarser, edge_positions, coarseNode, e, new_edge_targets);
            }
            endfor
        }
        forall_out_edges(coarser, e, coarseNode) { edge_positions[coarser.getEdgeTarget(e)] = UNDEFINED_EDGE; }
        endfor

            cur_no_vertices++;
    }

    ASSERT_RANGE_EQ(edge_positions, 0, edge_positions.size(), UNDEFINED_EDGE);
    ASSERT_EQ(no_of_coarse_vertices, cur_no_vertices);

    // this also resizes the edge fields ...
    coarser.finish_construction();
}

void contraction::contract_clustering(const PartitionConfig &partition_config, graph_access &finer, graph_access &coarser,
                                      const Matching &, const CoarseMapping &coarse_mapping,
                                      const NodeID &no_of_coarse_vertices, const NodePermutationMap &) {

    if (partition_config.combine) {
        coarser.resizeSecondPartitionIndex(no_of_coarse_vertices);
    }

    auto k = finer.get_partition_count();
    for (auto node : finer.nodes()) {
        finer.setPartitionIndex(node, coarse_mapping[node]);
    }

    finer.set_partition_count(no_of_coarse_vertices);

    complete_boundary bnd(&finer);
    bnd.build();
    bnd.getUnderlyingQuotientGraph(coarser);

    finer.set_partition_count(k);

    // variables for calculating the feature vec of the coarse nodes
    std::vector<NodeWeight> block_size(no_of_coarse_vertices);
    auto num_features = finer.getFeatureVec(0).size();
    std::vector<FeatureVec> combined_feature_vecs(no_of_coarse_vertices, FeatureVec(num_features, 0));

    for (auto node : finer.nodes()) {
        NodeID coarsed_node = coarse_mapping[node];
        coarser.setPartitionIndex(coarsed_node, finer.getPartitionIndex(node));

        addWeightedToVec(combined_feature_vecs[coarsed_node], finer.getFeatureVec(node), finer.getNodeWeight(node));
        block_size[coarsed_node] += finer.getNodeWeight(node);

        if (partition_config.combine) {
            coarser.setSecondPartitionIndex(coarse_mapping[node], finer.getSecondPartitionIndex(node));
        }
    }

    for (auto node : coarser.nodes()) {
        divideVec(combined_feature_vecs[node], block_size[node]);
        coarser.setFeatureVec(node, combined_feature_vecs[node]);
    }

    timer t;

    // calculate edge weights based on the distance of the feature vec
    for (auto node : coarser.nodes()) {
        forall_out_edges(coarser, e, node) {
            NodeID target = coarser.getEdgeTarget(e);
            EdgeWeight newWeight = 1 / calcFeatureDist(coarser.getFeatureVec(node), coarser.getFeatureVec(target));
            coarser.setEdgeWeight(e, newWeight);
            endfor
        }
    }

    std::cout << "calc new weights took " << t.elapsed() << std::endl;
}

FeatureVec contraction::combineFeatureVec(const FeatureVec &vec1, NodeWeight weight1, const FeatureVec &vec2,
                                          NodeWeight weight2) {
    size_t features = vec1.size();
    FeatureVec combined_features(features);

    for (size_t i = 0; i < features; ++i) {
        combined_features[i] = (weight1 * vec1[i] + weight2 * vec2[i]) / ((float)(weight1 + weight2));
    }

    return combined_features;
}

void contraction::divideVec(FeatureVec &vec, NodeWeight weights) {
    for (double &f : vec) {
        f /= weights;
    }
}

void contraction::addWeightedToVec(FeatureVec &vec, const FeatureVec &vecToAdd, NodeWeight weight) {
    for (size_t i = 0; i < vec.size(); ++i) {
        vec[i] += vecToAdd[i] * weight;
    }
}

EdgeWeight contraction::calcFeatureDist(const FeatureVec &vec1, const FeatureVec &vec2) {
    size_t features = vec1.size();
    EdgeWeight dist = 0;

    for (size_t i = 0; i < features; ++i) {
        EdgeWeight tmp = vec1[i] - vec2[i];
        dist += tmp * tmp;
    }

    return std::sqrt(dist);
}
