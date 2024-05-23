/******************************************************************************
 * complete_boundary.h
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

#ifndef COMPLETE_BOUNDARY_URZZFDEI
#define COMPLETE_BOUNDARY_URZZFDEI

#include <execinfo.h>
#include <unordered_map>
#include <utility>

#include "boundary_lookup.h"
#include "data_structure/graph_access.h"
#include "partition/uncoarsening/refinement/quotient_graph_refinement/partial_boundary.h"
#include "partition/partition_config.h"

struct block_informations {
        NodeWeight block_weight;
        NodeID block_no_nodes;
};

typedef std::vector<boundary_pair> QuotientGraphEdges;

class complete_boundary {
        public:
                explicit complete_boundary(graph_access * G );
                virtual ~complete_boundary() = default;

                void build();

                inline void insert(NodeID node, PartitionID insert_node_into, boundary_pair * pair);
                inline bool contains(NodeID node, PartitionID partition, boundary_pair * pair);

                inline NodeID size(PartitionID partition, boundary_pair * pair);

                inline void getQuotientGraphEdges(QuotientGraphEdges & qgraph_edges);

                inline void getUnderlyingQuotientGraph( graph_access & Q_bar );

        private:
                //updates lazy values that the access functions need
                inline void update_lazy_values(boundary_pair * pair);

                //lazy members to avoid hashtable loop ups
                PartialBoundary*   m_pb_lhs_lazy{nullptr};
                PartialBoundary*   m_pb_rhs_lazy{nullptr};
                PartitionID        m_lazy_lhs{};
                PartitionID        m_lazy_rhs{};
                size_t             m_last_key;
                hash_boundary_pair m_hbp;

                graph_access * m_graph_ref;
                //implicit quotient graph structure
                //
                block_pairs m_pairs;
                std::vector<block_informations> m_block_infos;

                //explicit quotient graph structure / may be outdated!
                graph_access Q;
                std::vector< NodeID > m_singletons;

                //////////////////////////////////////////////////////////////
                ///////// Data Structure Invariants
                //////////////////////////////////////////////////////////////
#ifndef NDEBUG
        public:
    [[maybe_unused]] bool assert_bnodes_in_boundaries();

    [[maybe_unused]] bool assert_boundaries_are_bnodes();
#endif
};



inline void complete_boundary::build() {
        graph_access & G = *m_graph_ref;

        for(PartitionID block = 0; block < G.get_partition_count(); block++) {
                m_block_infos[block].block_weight   = 0;
                m_block_infos[block].block_no_nodes = 0;
        }

        forall_nodes(G, n) {
                PartitionID source_partition = G.getPartitionIndex(n);
                m_block_infos[source_partition].block_weight   += G.getNodeWeight(n);
                m_block_infos[source_partition].block_no_nodes += 1;

                if(G.getNodeDegree(n) == 0) {
                        m_singletons.push_back(n);
                }

                forall_out_edges(G, e, n) {
                        NodeID targetID              = G.getEdgeTarget(e);
                        PartitionID target_partition = G.getPartitionIndex(targetID);
                        bool is_cut_edge             = (source_partition != target_partition);

                        if(is_cut_edge) {
                                boundary_pair bp{};
                                bp.k   = m_graph_ref->get_partition_count();
                                bp.lhs = source_partition;
                                bp.rhs = target_partition;
                                update_lazy_values(&bp);
                                m_pairs[bp].edge_cut += G.getEdgeWeight(e);
                                insert(n, source_partition, &bp);
                        }
                } endfor
        } endfor

        block_pairs::iterator iter;
        for(iter = m_pairs.begin(); iter != m_pairs.end(); iter++ ) {
                data_boundary_pair& value = iter->second;
                value.edge_cut /= 2;
        }

}

inline void complete_boundary::insert(NodeID node, PartitionID insert_node_into, boundary_pair * pair) {
        update_lazy_values(pair);
        ASSERT_TRUE((m_lazy_lhs == pair->lhs && m_lazy_rhs == pair->rhs)
                 || (m_lazy_lhs == pair->rhs && m_lazy_rhs == pair->lhs));

        if(insert_node_into == m_lazy_lhs) {
                ASSERT_EQ(m_graph_ref->getPartitionIndex(node),m_lazy_lhs);
                m_pb_lhs_lazy->insert(node);
        } else {
                ASSERT_EQ(m_graph_ref->getPartitionIndex(node),m_lazy_rhs);
                m_pb_rhs_lazy->insert(node);
        }
}

inline bool complete_boundary::contains(NodeID node, PartitionID partition, boundary_pair * pair){
        update_lazy_values(pair);
        if(partition == m_lazy_lhs) {
                ASSERT_EQ(m_graph_ref->getPartitionIndex(node),m_lazy_lhs);
                return m_pb_lhs_lazy->contains(node);
        } else {
                ASSERT_EQ(m_graph_ref->getPartitionIndex(node),m_lazy_rhs);
                return m_pb_rhs_lazy->contains(node);
        }
}

inline NodeID complete_boundary::size(PartitionID partition, boundary_pair * pair){
        update_lazy_values(pair);
        if(partition == m_lazy_lhs) {
                return m_pb_lhs_lazy->size();
        } else {
                return m_pb_rhs_lazy->size();
        }
}

inline void complete_boundary::getQuotientGraphEdges(QuotientGraphEdges & qgraph_edges) {
        //the quotient graph is stored implicitly in the pairs hashtable
        block_pairs::iterator iter;
        for(iter = m_pairs.begin(); iter != m_pairs.end(); iter++ ) {
                boundary_pair key = iter->first;
                qgraph_edges.push_back(key);
        }
}

inline void complete_boundary::update_lazy_values(boundary_pair * pair) {
        ASSERT_NEQ(pair->lhs, pair->rhs);

        boundary_pair & bp = *pair;
        size_t key = m_hbp(bp);
        if(key != m_last_key) {
                data_boundary_pair & dbp = m_pairs[*pair];
                if(!dbp.initialized) {
                        m_pairs[*pair].lhs = pair->lhs;
                        m_pairs[*pair].rhs = pair->rhs;
                        dbp.initialized = true;
                }

                m_pb_lhs_lazy = &dbp.pb_lhs;
                m_pb_rhs_lazy = &dbp.pb_rhs;
                m_lazy_lhs    = dbp.lhs;
                m_lazy_rhs    = dbp.rhs;
                m_last_key    = key;
        }
}

inline void complete_boundary::getUnderlyingQuotientGraph( graph_access & Q_bar ) {
         auto * graphref = new basicGraph;


                delete Q_bar.graphref;

         Q_bar.graphref = graphref;

         std::vector< std::vector< std::pair<PartitionID, EdgeWeight> > >  building_tool;
         building_tool.resize(m_block_infos.size());

         block_pairs::iterator iter;
         for(iter = m_pairs.begin(); iter != m_pairs.end(); iter++ ) {
                 boundary_pair cur_pair = iter->first;

                 std::pair<PartitionID, EdgeWeight> qedge_lhs;
                 qedge_lhs.first  = cur_pair.rhs;
                 qedge_lhs.second = m_pairs[cur_pair].edge_cut;
                 building_tool[cur_pair.lhs].push_back(qedge_lhs);

                 std::pair<PartitionID, EdgeWeight> qedge_rhs;
                 qedge_rhs.first  = cur_pair.lhs;
                 qedge_rhs.second = m_pairs[cur_pair].edge_cut;
                 building_tool[cur_pair.rhs].push_back(qedge_rhs);
         }

         Q_bar.start_construction(building_tool.size(), 2*m_pairs.size());

         for( unsigned p = 0; p < building_tool.size(); p++) {
                 NodeID node = Q_bar.new_node();
                 Q_bar.setNodeWeight(node,  m_block_infos[p].block_weight);

                 for(auto & j : building_tool[p]) {
                         EdgeID e = Q_bar.new_edge(node, j.first);
                         Q_bar.setEdgeWeight(e, j.second);
                 }
         }

         Q_bar.finish_construction();
}


#ifndef NDEBUG

[[maybe_unused]] inline bool complete_boundary::assert_bnodes_in_boundaries() {
        PartitionID k = m_graph_ref->get_partition_count();

        for(PartitionID lhs = 0; lhs < k; lhs++) {
                for(PartitionID rhs = 0; rhs < k; rhs++) {
                        if(rhs == lhs || lhs > rhs) continue;

                        boundary_pair bp{};
                        bp.k = m_graph_ref->get_partition_count();
                        bp.lhs = lhs;
                        bp.rhs = rhs;
                        graph_access & G = *m_graph_ref;

                        NodeWeight lhs_part_weight = 0;
                        NodeWeight rhs_part_weight = 0;

                        NodeID lhs_no_nodes = 0;
                        NodeID rhs_no_nodes = 0;

                        EdgeWeight edge_cut = 0;
                        forall_nodes(G, n) {
                                PartitionID source_partition = G.getPartitionIndex(n);
                                if(source_partition == lhs){
                                        lhs_part_weight += G.getNodeWeight(n);
                                        lhs_no_nodes++;
                                } else if(source_partition == rhs){
                                        rhs_part_weight += G.getNodeWeight(n);
                                        rhs_no_nodes++;
                                }

                                forall_out_edges(G, e, n) {
                                        NodeID targetID = G.getEdgeTarget(e);
                                        PartitionID target_partition = G.getPartitionIndex(targetID);
                                        bool is_cut_edge =  (source_partition == lhs &&  target_partition == rhs)
                                                         || (source_partition == rhs &&  target_partition == lhs);

                                        if(is_cut_edge) {
                                                edge_cut += G.getEdgeWeight(e);
                                                ASSERT_TRUE(contains(n, source_partition,&bp));
                                        }

                                } endfor
                        } endfor

                        ASSERT_EQ(m_block_infos[lhs].block_weight, lhs_part_weight);
                        ASSERT_EQ(m_block_infos[rhs].block_weight, rhs_part_weight);
                        ASSERT_EQ(m_block_infos[lhs].block_no_nodes, lhs_no_nodes);
                        ASSERT_EQ(m_block_infos[rhs].block_no_nodes, rhs_no_nodes);
                        ASSERT_EQ(m_pairs[bp].edge_cut,edge_cut/2);
                }
        }

        return true;
}

[[maybe_unused]] inline bool complete_boundary::assert_boundaries_are_bnodes() {
        graph_access & G = *m_graph_ref;
        forall_nodes(G, n) {
                 PartitionID partition = G.getPartitionIndex(n);
                 forall_out_edges(G, e, n) {
                         NodeID target = G.getEdgeTarget(e);
                         PartitionID targets_partition = G.getPartitionIndex(target);

                         if(partition != targets_partition) {
                                boundary_pair bp{};
                                bp.k   = G.get_partition_count();
                                bp.lhs = partition;
                                bp.rhs = targets_partition;

                                ASSERT_TRUE(contains(n, partition, &bp));
                                ASSERT_TRUE(contains(target, targets_partition, &bp));

                         }
                 } endfor

         } endfor
         QuotientGraphEdges qgraph_edges;
         getQuotientGraphEdges(qgraph_edges);
         for(auto & pair : qgraph_edges) {
                 ASSERT_NEQ(pair.lhs, pair.rhs);
         }

        return true;
}
#endif // #ifndef NDEBUG

#endif /* end of include guard: COMPLETE_BOUNDARY_URZZFDEI */
