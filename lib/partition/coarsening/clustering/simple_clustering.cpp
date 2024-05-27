#include <memory>
#include "simple_clustering.h"

#include "algorithms/jarnik_prim.h"
#include "data_structure/graph_access.h"


void simple_clustering::match(const PartitionConfig &config,
                              graph_access &G,
                              Matching &,
                              CoarseMapping &coarse_mapping,
                              NodeID &no_of_coarse_vertices,
                              NodePermutationMap &permutation) {
    permutation.resize(G.number_of_nodes());
    coarse_mapping.resize(G.number_of_nodes(), std::numeric_limits<NodeID>::max());

    NodeID root{};
    std::tie(this->tree, root) = jarnik_prim::spanning_tree(G);

    this->cur_cluster = 0;
    this->coarse_mapping = &coarse_mapping;
    this->max_cluster_nodes = config.cluster_upperbound;

    visit_children(root);

    // unvisited nodes are single coarse nodes
    for (auto &coarseID: coarse_mapping) {
        if (coarseID == std::numeric_limits<NodeID>::max()) {
            coarseID = ++cur_cluster;
        }
    }

    no_of_coarse_vertices = cur_cluster + 1;

    this->tree.reset();
}

// we have a tree and it is assured that we are not visiting a node twice
void simple_clustering::visit_children(NodeID cur_node) {
    if (cur_cluster_nodes >= max_cluster_nodes) {
        cur_cluster++;
        cur_cluster_nodes = 0;
    }
    (*coarse_mapping)[cur_node] = cur_cluster;
    cur_cluster_nodes++;

    forall_out_edges ((*tree), e, cur_node)
            {
                visit_children(tree->getEdgeTarget(e));
            }
    endfor
}
