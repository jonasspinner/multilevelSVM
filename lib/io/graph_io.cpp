/******************************************************************************
 * graph_io.cpp 
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

#include <sstream>
#include <unordered_set>
#include "graph_io.h"

graph_io::graph_io() {
                
}

graph_io::~graph_io() {
                
}

int graph_io::writeGraphGDF(const graph_access & G_min, const graph_access & G_maj, std::string filename) {
        std::ofstream f(filename.c_str());

        size_t min_nodes = G_min.number_of_nodes();

	f << "nodedef>name VARCHAR,class VARCHAR,partition VARCHAR, weight DOUBLE";
	for (size_t i = 0; i < G_min.getFeatureVec(0).size(); ++i) {
		f << ",feature" << i << " DOUBLE";
	}
	f << std::endl;

	// NODES
	forall_nodes(G_min, node) {
		f <<  node << ",-1," << G_min.getPartitionIndex(node) << "," << G_min.getNodeWeight(node);
		for (auto &feature : G_min.getFeatureVec(node)) {
			f << "," << feature;
		}
		f << std::endl;
	} endfor

	forall_nodes(G_maj, node) {
		f <<  node + min_nodes << ",1," << G_maj.getPartitionIndex(node) << "," << G_maj.getNodeWeight(node);
		for (auto &feature : G_maj.getFeatureVec(node)) {
			f << "," << feature;
		}
		f << std::endl;
	} endfor


	f << "edgedef>from VARCHAR,to VARCHAR" << std::endl;

	// EDGES
	forall_nodes(G_min, node) {
                forall_out_edges(G_min, e, node) {
                        f << node << "," << G_min.getEdgeTarget(e) << std::endl;
                } endfor 
        } endfor
	forall_nodes(G_maj, node) {
                forall_out_edges(G_maj, e, node) {
                        f << node + min_nodes << "," << G_maj.getEdgeTarget(e) + min_nodes << std::endl;
                } endfor 
        } endfor
        f.close();
        return 0;
}

int graph_io::readFeatures(graph_access & G, const std::vector<FeatureVec> & data) {
        forall_nodes(G, node) {
                G.setFeatureVec(node, data[node]);
        } endfor
        return 0;
}

int graph_io::readGraphFromVec(graph_access & G, const std::vector<std::vector<Edge>> & data, EdgeID num_edges) {
        G.start_construction(data.size(), num_edges);

        for (auto& nodeData : data) {
                NodeID node = G.new_node();
                G.setPartitionIndex(node, 0);
                G.setNodeWeight(node, 1);

                for (auto & edge : nodeData) {
                        EdgeID e = G.new_edge(node, edge.target);
                        G.setEdgeWeight(e, edge.weight);
                }
        }

        G.finish_construction();
        return 0;
}

EdgeID graph_io::makeEdgesBidirectional(std::vector<std::vector<Edge>> & data) {
        size_t pre = 0;
        size_t post = 0;

        std::vector<std::unordered_set<NodeID>> neighbors(data.size());

        for (NodeID from = 0; from < data.size(); ++from) {
                for (EdgeID edge = 0; edge < data[from].size(); ++edge) {
                        NodeID target = data[from][edge].target;
                        neighbors[from].insert(target);
                        pre++;
                }
        }


        for (NodeID from = 0; from < data.size(); ++from) {
                for (EdgeID edge = 0; edge < data[from].size(); ++edge) {
                        Edge e = data[from][edge];
                        if (neighbors[e.target].find(from) == neighbors[from].end()) {
                                Edge back;
                                back.target = from;
                                back.weight = e.weight;
                                data[e.target].push_back(back);
                                neighbors[e.target].insert(from);
                        }
                }
        }

        for (auto& nodeData : data) {
                for (auto & edge : nodeData) {
                        post++;
                }
        }

        std::cout << "edges pre: " << pre << " post: " << post << std::endl;
        return post;
}
