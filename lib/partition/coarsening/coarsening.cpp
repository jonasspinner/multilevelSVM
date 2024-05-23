/******************************************************************************
 * coarsening.cpp
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

#include <limits>
#include <sstream>
#include <memory>

#include "coarsening.h"
#include "coarsening_configurator.h"
#include "contraction.h"
#include "data_structure/graph_hierarchy.h"
#include "definitions.h"
#include "edge_rating/edge_ratings.h"
#include "stop_rules/stop_rules.h"

void coarsening::perform_coarsening(const PartitionConfig & partition_config, graph_access & G, graph_hierarchy & hierarchy) {

        NodeID no_of_coarser_vertices = G.number_of_nodes();
        NodeID no_of_finer_vertices   = std::numeric_limits<NodeID>::max();

        edge_ratings rating(partition_config);
        CoarseMapping* coarse_mapping = nullptr;

        graph_access* finer                      = &G;
        matching* edge_matcher                   = nullptr;
        std::unique_ptr<contraction> contracter                  = std::make_unique<contraction>();
        PartitionConfig copy_of_partition_config = partition_config;

        std::unique_ptr<stop_rule> coarsening_stop_rule;

        switch (partition_config.stop_rule) {
        case STOP_RULE_SIMPLE_FIXED:
                coarsening_stop_rule = std::make_unique<simple_fixed_stop_rule>(copy_of_partition_config, G.number_of_nodes());
                break;
        default:
                coarsening_stop_rule = std::make_unique<simple_fixed_stop_rule>(copy_of_partition_config, G.number_of_nodes());
        }

        coarsening_configurator coarsening_config;

        unsigned int level    = 0;

        while (coarsening_stop_rule->stop(no_of_finer_vertices, no_of_coarser_vertices)) {
                auto* coarser = new graph_access();
                coarse_mapping        = new CoarseMapping();
                Matching edge_matching;
                NodePermutationMap permutation;
                no_of_finer_vertices = no_of_coarser_vertices;

                coarsening_config.configure_coarsening(copy_of_partition_config,
						       &edge_matcher, level);
                rating.rate(*finer, level);

                edge_matcher->match(copy_of_partition_config, *finer,
				    edge_matching, *coarse_mapping,
				    no_of_coarser_vertices, permutation);

                delete edge_matcher;

                contracter->contract(copy_of_partition_config, *finer, *coarser,
				     edge_matching, *coarse_mapping,
				     no_of_coarser_vertices, permutation);

                hierarchy.push_back(finer, coarse_mapping);

                std::cout <<  "no of coarser vertices " << no_of_coarser_vertices
                          <<  " and no of edges " <<  coarser->number_of_edges() << std::endl;

                finer = coarser;

                level++;
        }

        hierarchy.push_back(finer, nullptr); // append the last created level
}
