/******************************************************************************
 * quality_metrics.cpp 
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

#include <algorithm>
#include <cmath>

#include "quality_metrics.h"
#include "data_structure/union_find.h"

#include <unordered_map>
#include <numeric>

quality_metrics::quality_metrics() {
}

quality_metrics::~quality_metrics () {
}

EdgeWeight quality_metrics::edge_cut(graph_access & G) {
        EdgeWeight edgeCut = 0;
        forall_nodes(G, n) { 
                PartitionID partitionIDSource = G.getPartitionIndex(n);
                forall_out_edges(G, e, n) {
                        NodeID targetNode = G.getEdgeTarget(e);
                        PartitionID partitionIDTarget = G.getPartitionIndex(targetNode);

                        if (partitionIDSource != partitionIDTarget) {
                                edgeCut += G.getEdgeWeight(e);
                        }
                } endfor 
        } endfor
        return edgeCut/2;
}

EdgeWeight quality_metrics::edge_cut(graph_access & G, int * partition_map) {
        EdgeWeight edgeCut = 0;
        forall_nodes(G, n) { 
                PartitionID partitionIDSource = partition_map[n];
                forall_out_edges(G, e, n) {
                        NodeID targetNode = G.getEdgeTarget(e);
                        PartitionID partitionIDTarget = partition_map[targetNode];

                        if (partitionIDSource != partitionIDTarget) {
                                edgeCut += G.getEdgeWeight(e);
                        }
                } endfor 
        } endfor
        return edgeCut/2;
}

EdgeWeight quality_metrics::edge_cut(graph_access & G, PartitionID lhs, PartitionID rhs) {
        EdgeWeight edgeCut = 0;
        forall_nodes(G, n) { 
                PartitionID partitionIDSource = G.getPartitionIndex(n);
                if(partitionIDSource != lhs) continue;
                forall_out_edges(G, e, n) {
                        NodeID targetNode = G.getEdgeTarget(e);
                        PartitionID partitionIDTarget = G.getPartitionIndex(targetNode);

                        if(partitionIDTarget == rhs) {
                                edgeCut += G.getEdgeWeight(e);
                        }
                } endfor 
        } endfor
        return edgeCut;
}

