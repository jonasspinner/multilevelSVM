#include "simple_clustering.h"
#include <memory>

#include "algorithms/jarnik_prim.h"
#include "data_structure/graph_access.h"

void simple_clustering::match(const PartitionConfig &config, graph_access &G, Matching &, CoarseMapping &coarse_mapping,
                              NodeID &no_of_coarse_vertices, NodePermutationMap &permutation) {
    permutation.resize(G.number_of_nodes());
    coarse_mapping.resize(G.number_of_nodes(), std::numeric_limits<NodeID>::max());

    auto [tree, root] = jarnik_prim::spanning_tree(G);

    NodeID cur_cluster = 0;
    NodeID max_cluster_nodes = config.cluster_upperbound;
    NodeID cur_cluster_nodes = 0;

    std::vector<NodeID> queue = {root};
    while (!queue.empty()) {
        NodeID cur_node = queue.back();
        queue.pop_back();
        if (cur_cluster_nodes >= max_cluster_nodes) {
            cur_cluster++;
            cur_cluster_nodes = 0;
        }
        coarse_mapping[cur_node] = cur_cluster;
        cur_cluster_nodes++;

        forall_out_edges((*tree), e, cur_node) { queue.push_back(tree->getEdgeTarget(e)); }
        endfor
    }

    // unvisited nodes are single coarse nodes
    for (auto &coarseID : coarse_mapping) {
        if (coarseID == std::numeric_limits<NodeID>::max()) {
            coarseID = ++cur_cluster;
        }
    }

    no_of_coarse_vertices = cur_cluster + 1;
}
