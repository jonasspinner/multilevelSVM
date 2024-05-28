#include "jarnik_prim.h"

#include <queue>
#include <utility>

#include "data_structure/graph_access.h"
#include "definitions.h"
#include "tools/random_functions.h"

struct JPEdge {
    EdgeID id;
    NodeID from;
    NodeID to;
    EdgeWeight weight;

    JPEdge(EdgeID i, NodeID f, NodeID t, EdgeWeight w) : id(i), from(f), to(t), weight(w) {}
};

bool operator<(const JPEdge &lhs, const JPEdge &rhs) { return lhs.weight < rhs.weight; }

std::pair<std::unique_ptr<graph_access>, NodeID> jarnik_prim::spanning_tree(const graph_access &G) {
    NodeID size = G.number_of_nodes();

    // could be faster with priority queue of Nodes but this needs a decreaseKey operation
    // and algorithms in data_structure don't support arbitrary additional data
    std::priority_queue<JPEdge> pq;

    std::vector<NodeID> parent = std::vector<NodeID>(size, std::numeric_limits<NodeID>::max());

    NodeID start_id = random_functions::nextInt(0, size - 1);

    // put start_id edges in priority queue
    forall_out_edges(G, e, start_id) {
        assert((EdgeWeight)((NodeID)G.getEdgeWeight(e)) == G.getEdgeWeight(e));
        pq.emplace(e, start_id, G.getEdgeTarget(e), G.getEdgeWeight(e));
    }
    endfor

        parent[start_id] = start_id;

    while (!pq.empty()) {
        JPEdge cur_edge = pq.top();
        pq.pop();
        NodeID cur_node = cur_edge.to;
        // add node only if not already in the spanning tree
        if (parent[cur_node] != std::numeric_limits<NodeID>::max()) {
            continue;
        }

        forall_out_edges(G, e, cur_node) {
            NodeID target = G.getEdgeTarget(e);
            // add node only if not already in the spanning tree
            if (parent[target] != std::numeric_limits<NodeID>::max()) {
                continue;
            }
            pq.emplace(e, cur_node, target, G.getEdgeWeight(e));
        }
        endfor

            parent[cur_node] = cur_edge.from;
    }

    std::vector<std::vector<NodeID>> children = std::vector<std::vector<NodeID>>(size);

    for (size_t i = 0; i < size; ++i) {
        if (i == start_id)
            continue;
        if (parent[i] == std::numeric_limits<NodeID>::max()) {
            continue;
        }
        children[parent[i]].push_back(i);
    }

    auto tree = std::make_unique<graph_access>();
    tree->start_construction(size, size);

    for (size_t current_node = 0; current_node < size; ++current_node) {
        tree->new_node();

        for (auto c : children[current_node]) {
            tree->new_edge(current_node, c);
        }
    }

    return std::make_pair(std::move(tree), start_id);
}
