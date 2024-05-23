/******************************************************************************
 * graph_hierarchy.cpp
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

#include "graph_hierarchy.h"


graph_hierarchy::~graph_hierarchy() {
        for(auto & m_to_delete_mapping : m_to_delete_mappings) {
                delete m_to_delete_mapping;
        }

        for( unsigned i = 1; i < m_to_delete_hierachies.size(); i++) {
                delete m_to_delete_hierachies[i];
        }
}

void graph_hierarchy::push_back(graph_access * G, CoarseMapping * coarse_mapping) {
        m_the_graph_hierarchy.push(G);
        m_the_mappings.push(coarse_mapping);
        m_to_delete_mappings.push_back(coarse_mapping);
        m_to_delete_hierachies.push_back(G);
        m_coarsest_graph = G;
        if (m_finest_graph == nullptr) {
                m_finest_graph = G;
        }
}

graph_access* graph_hierarchy::pop_finer_and_project() {
        graph_access* finer = pop_coarsest();

        CoarseMapping* coarse_mapping = m_the_mappings.top(); // mapps finer to coarser nodes
        m_the_mappings.pop();

        if(finer == m_coarsest_graph) {
                m_current_coarser_graph = finer;
                finer = pop_coarsest();
                finer->set_partition_count(m_current_coarser_graph->get_partition_count());

                coarse_mapping = m_the_mappings.top();
                m_the_mappings.pop();
        }

        ASSERT_EQ(m_the_graph_hierarchy.size(), m_the_mappings.size());

        //perform projection
        graph_access& fRef = *finer;
        graph_access& cRef = *m_current_coarser_graph;
        forall_nodes(fRef, n) {
                NodeID coarser_node              = (*coarse_mapping)[n];
                PartitionID coarser_partition_id = cRef.getPartitionIndex(coarser_node);
                fRef.setPartitionIndex(n, coarser_partition_id);
        } endfor

        m_current_coarse_mapping = coarse_mapping;
        finer->set_partition_count(m_current_coarser_graph->get_partition_count());
        m_current_coarser_graph = finer;

        return finer;
}

CoarseMapping * graph_hierarchy::get_mapping_of_current_finer() {
        return m_current_coarse_mapping;
}

graph_access* graph_hierarchy::get_coarsest( ) {
        return m_coarsest_graph;
}

graph_access* graph_hierarchy::pop_coarsest( ) {
        graph_access* current_coarsest = m_the_graph_hierarchy.top();
        m_the_graph_hierarchy.pop();
        return current_coarsest;
}

bool graph_hierarchy::isEmpty( ) {
        ASSERT_EQ(m_the_graph_hierarchy.size(), m_the_mappings.size());
        return size() <= 0;
}

unsigned int graph_hierarchy::size() {
        if(!m_the_graph_hierarchy.empty() &&
           m_the_graph_hierarchy.top() == m_coarsest_graph) {
                return m_the_graph_hierarchy.size() - 1;
        }
        return m_the_graph_hierarchy.size();
}
