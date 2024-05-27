/******************************************************************************
 * graph_access.h
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

#ifndef GRAPH_ACCESS_EFRXO4X2
#define GRAPH_ACCESS_EFRXO4X2

#include <bitset>
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include "definitions.h"

struct refinementNode {
    PartitionID partitionIndex{};
    FeatureVec featureVector;
};

struct coarseningEdge {
    EdgeRatingType rating;
};

template<class T>
class Range {
    T m_end;

    struct Iterator {
        T m_value;

        friend class Range;

    public:
        T operator*() const { return m_value; }

        const Iterator &operator++() {
            ++m_value;
            return *this;
        }

        Iterator operator++(int) {
            Iterator copy(*this);
            ++m_value;
            return copy;
        }

        bool operator==(const Iterator &other) const { return m_value == other.m_value; }

        bool operator!=(const Iterator &other) const { return m_value != other.m_value; }

    protected:
        explicit Iterator(T value) : m_value(value) {}
    };

public:
    explicit Range(T end) : m_end(end) {}

    [[nodiscard]] Iterator begin() const { return Iterator{0}; }

    [[nodiscard]] Iterator end() const { return Iterator{m_end}; }
};

class graph_access;

//construction etc. is encapsulated in basicGraph / access to properties etc. is encapsulated in graph_access
class basicGraph {
    friend class graph_access;

private:
    //methods only to be used by friend class
    [[nodiscard]] EdgeID number_of_edges() const { return m_edges.size(); }

    [[nodiscard]] NodeID number_of_nodes() const { return m_nodes.size() - 1; }

    [[nodiscard]] Range<EdgeID> edges() const { return Range<EdgeID>{number_of_edges()}; }

    [[nodiscard]] Range<NodeID> nodes() const { return Range<NodeID>{number_of_nodes()}; }

    inline EdgeID get_first_edge(const NodeID &node) {
        return m_nodes[node].firstEdge;
    }

    inline EdgeID get_first_invalid_edge(const NodeID &node) {
        return m_nodes[node + 1].firstEdge;
    }

    // construction of the graph
    void start_construction(NodeID n, EdgeID m) {
        m_building_graph = true;
        current_node = 0;
        current_edge = 0;
        m_last_source = -1;

        //resizes property arrays
        m_nodes.resize(n + 1);
        m_refinement_node_props.resize(n + 1);
        m_edges.resize(m);
        m_coarsening_edge_props.resize(m);

        m_nodes[current_node].firstEdge = current_edge;
    }

    EdgeID new_edge(NodeID source, NodeID target) {
        ASSERT_TRUE(m_building_graph);
        ASSERT_TRUE(current_edge < m_edges.size());

        m_edges[current_edge].target = target;
        EdgeID e_bar = current_edge;
        ++current_edge;

        ASSERT_TRUE(source + 1 < m_nodes.size());
        m_nodes[source + 1].firstEdge = current_edge;

        //fill isolated sources at the end
        if ((NodeID) (m_last_source + 1) < source) {
            for (NodeID i = source; i > (NodeID) (m_last_source + 1); i--) {
                m_nodes[i].firstEdge = m_nodes[m_last_source + 1].firstEdge;
            }
        }
        m_last_source = source;
        return e_bar;
    }

    NodeID new_node() {
        ASSERT_TRUE(m_building_graph);
        return current_node++;
    }

    void finish_construction() {
        // inert dummy node
        m_nodes.resize(current_node + 1);
        m_refinement_node_props.resize(current_node + 1);

        m_edges.resize(current_edge);
        m_coarsening_edge_props.resize(current_edge);

        m_building_graph = false;

        //fill isolated sources at the end
        if ((unsigned int) (m_last_source) != current_node - 1) {
            //in that case at least the last node was an isolated node
            for (NodeID i = current_node; i > (unsigned int) (m_last_source + 1); i--) {
                m_nodes[i].firstEdge = m_nodes[m_last_source + 1].firstEdge;
            }
        }
    }

    // %%%%%%%%%%%%%%%%%%% DATA %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // split properties for coarsening and uncoarsening
    std::vector<Node> m_nodes;
    std::vector<Edge> m_edges;

    std::vector<refinementNode> m_refinement_node_props;
    std::vector<coarseningEdge> m_coarsening_edge_props;

    // construction properties
    bool m_building_graph{false};
    NodeID m_last_source{};
    NodeID current_node{}; //current node that is constructed
    EdgeID current_edge{}; //current edge that is constructed
};

//makros - graph access
#define forall_edges(G, e) { for(EdgeID e : G.edges()) {
#define forall_nodes(G, n) { for(NodeID n : G.nodes()) {
#define forall_out_edges(G, e, n) { for(EdgeID e = G.get_first_edge(n), end = G.get_first_invalid_edge(n); e < end; ++e) {
#define endfor }}


class complete_boundary;

class graph_access {
    friend class complete_boundary;

public:
    graph_access() : graphref(std::make_unique<basicGraph>()) {}

    /* ============================================================= */
    /* build methods */
    /* ============================================================= */
    void start_construction(NodeID nodes, EdgeID edges);

    NodeID new_node();

    EdgeID new_edge(NodeID source, NodeID target);

    void finish_construction();

    /* ============================================================= */
    /* graph access methods */
    /* ============================================================= */
    [[nodiscard]] NodeID number_of_nodes() const;

    [[nodiscard]] EdgeID number_of_edges() const;

    [[nodiscard]] Range<NodeID> nodes() const { return graphref->nodes(); }

    [[nodiscard]] Range<EdgeID> edges() const { return graphref->edges(); }

    [[nodiscard]] EdgeID get_first_edge(NodeID node) const;

    [[nodiscard]] EdgeID get_first_invalid_edge(NodeID node) const;

    [[nodiscard]] PartitionID get_partition_count() const;

    void set_partition_count(PartitionID count);

    [[nodiscard]] PartitionID getPartitionIndex(NodeID node) const;

    void setPartitionIndex(NodeID node, PartitionID id);

    [[nodiscard]] PartitionID getSecondPartitionIndex(NodeID node) const;

    void setSecondPartitionIndex(NodeID node, PartitionID id);

    [[nodiscard]] const FeatureVec &getFeatureVec(NodeID node) const;

    void setFeatureVec(NodeID node, const FeatureVec &vec);

    //to be called if combine in meta heuristic is used
    void resizeSecondPartitionIndex(unsigned no_nodes);

    [[nodiscard]] NodeWeight getNodeWeight(NodeID node) const;

    void setNodeWeight(NodeID node, NodeWeight weight);

    [[nodiscard]] EdgeWeight getNodeDegree(NodeID node) const;

    [[nodiscard]] EdgeWeight getWeightedNodeDegree(NodeID node) const;

    [[nodiscard]] EdgeWeight getEdgeWeight(EdgeID edge) const;

    void setEdgeWeight(EdgeID edge, EdgeWeight weight);

    [[nodiscard]] NodeID getEdgeTarget(EdgeID edge) const;

    [[nodiscard]] EdgeRatingType getEdgeRating(EdgeID edge) const;

    void setEdgeRating(EdgeID edge, EdgeRatingType rating);

    void copy(graph_access &G_bar);

private:
    std::unique_ptr<basicGraph> graphref;
    unsigned int m_partition_count{};
    std::vector<PartitionID> m_second_partition_index;
};

/* graph build methods */
inline void graph_access::start_construction(NodeID nodes, EdgeID edges) {
    graphref->start_construction(nodes, edges);
}

inline NodeID graph_access::new_node() {
    return graphref->new_node();
}

inline EdgeID graph_access::new_edge(NodeID source, NodeID target) {
    return graphref->new_edge(source, target);
}

inline void graph_access::finish_construction() {
    graphref->finish_construction();
}

/* graph access methods */
inline NodeID graph_access::number_of_nodes() const {
    return graphref->number_of_nodes();
}

inline EdgeID graph_access::number_of_edges() const {
    return graphref->number_of_edges();
}

inline void graph_access::resizeSecondPartitionIndex(unsigned no_nodes) {
    m_second_partition_index.resize(no_nodes);
}

inline EdgeID graph_access::get_first_edge(NodeID node) const {
#ifdef NDEBUG
    return graphref->m_nodes[node].firstEdge;
#else
    return graphref->m_nodes.at(node).firstEdge;
#endif
}

inline EdgeID graph_access::get_first_invalid_edge(NodeID node) const {
    return graphref->m_nodes[node + 1].firstEdge;
}

inline PartitionID graph_access::get_partition_count() const {
    return m_partition_count;
}

inline PartitionID graph_access::getSecondPartitionIndex(NodeID node) const {
#ifdef NDEBUG
    return m_second_partition_index[node];
#else
    return m_second_partition_index.at(node);
#endif
}

inline void graph_access::setSecondPartitionIndex(NodeID node, PartitionID id) {
#ifdef NDEBUG
    m_second_partition_index[node] = id;
#else
    m_second_partition_index.at(node) = id;
#endif
}

inline PartitionID graph_access::getPartitionIndex(NodeID node) const {
#ifdef NDEBUG
    return graphref->m_refinement_node_props[node].partitionIndex;
#else
    return graphref->m_refinement_node_props.at(node).partitionIndex;
#endif
}

inline void graph_access::setPartitionIndex(NodeID node, PartitionID id) {
#ifdef NDEBUG
    graphref->m_refinement_node_props[node].partitionIndex = id;
#else
    graphref->m_refinement_node_props.at(node).partitionIndex = id;
#endif
}

inline const FeatureVec &graph_access::getFeatureVec(NodeID node) const {
#ifdef NDEBUG
    return graphref->m_refinement_node_props[node].featureVector;
#else
    return graphref->m_refinement_node_props.at(node).featureVector;
#endif
}

inline void graph_access::setFeatureVec(NodeID node, const FeatureVec &vec) {
#ifdef NDEBUG
    graphref->m_refinement_node_props[node].featureVector = vec;
#else
    graphref->m_refinement_node_props.at(node).featureVector = vec;
#endif
}


inline NodeWeight graph_access::getNodeWeight(NodeID node) const {
#ifdef NDEBUG
    return graphref->m_nodes[node].weight;
#else
    return graphref->m_nodes.at(node).weight;
#endif
}

inline void graph_access::setNodeWeight(NodeID node, NodeWeight weight) {
#ifdef NDEBUG
    graphref->m_nodes[node].weight = weight;
#else
    graphref->m_nodes.at(node).weight = weight;
#endif
}

inline EdgeWeight graph_access::getEdgeWeight(EdgeID edge) const {
#ifdef NDEBUG
    return graphref->m_edges[edge].weight;
#else
    return graphref->m_edges.at(edge).weight;
#endif
}

inline void graph_access::setEdgeWeight(EdgeID edge, EdgeWeight weight) {
#ifdef NDEBUG
    graphref->m_edges[edge].weight = weight;
#else
    graphref->m_edges.at(edge).weight = weight;
#endif
}

inline NodeID graph_access::getEdgeTarget(EdgeID edge) const {
#ifdef NDEBUG
    return graphref->m_edges[edge].target;
#else
    return graphref->m_edges.at(edge).target;
#endif
}

inline EdgeRatingType graph_access::getEdgeRating(EdgeID edge) const {
#ifdef NDEBUG
    return graphref->m_coarsening_edge_props[edge].rating;
#else
    return graphref->m_coarsening_edge_props.at(edge).rating;
#endif
}

inline void graph_access::setEdgeRating(EdgeID edge, EdgeRatingType rating) {
#ifdef NDEBUG
    graphref->m_coarsening_edge_props[edge].rating = rating;
#else
    graphref->m_coarsening_edge_props.at(edge).rating = rating;
#endif
}

inline EdgeWeight graph_access::getNodeDegree(NodeID node) const {
    return graphref->m_nodes[node + 1].firstEdge - graphref->m_nodes[node].firstEdge;
}

inline EdgeWeight graph_access::getWeightedNodeDegree(NodeID node) const {
    EdgeWeight degree = 0;
    for (unsigned e = graphref->m_nodes[node].firstEdge; e < graphref->m_nodes[node + 1].firstEdge; ++e) {
        degree += getEdgeWeight(e);
    }
    return degree;
}

inline void graph_access::set_partition_count(PartitionID count) {
    m_partition_count = count;
}

inline void graph_access::copy(graph_access &G_bar) {
    G_bar.start_construction(number_of_nodes(), number_of_edges());

    basicGraph &ref = *graphref;
    forall_nodes(ref, node)
            {
                NodeID shadow_node = G_bar.new_node();
                G_bar.setNodeWeight(shadow_node, getNodeWeight(node));
                forall_out_edges(ref, e, node)
                        {
                            NodeID target = getEdgeTarget(e);
                            EdgeID shadow_edge = G_bar.new_edge(shadow_node, target);
                            G_bar.setEdgeWeight(shadow_edge, getEdgeWeight(e));
                        }
                endfor
            }
    endfor

    G_bar.finish_construction();
}

#endif /* end of include guard: GRAPH_ACCESS_EFRXO4X2 */
