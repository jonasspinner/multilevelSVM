/******************************************************************************
 * coarsening_configurator.h
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

#ifndef COARSENING_CONFIGURATOR_8UJ78WYS
#define COARSENING_CONFIGURATOR_8UJ78WYS

#include "contraction.h"
#include "data_structure/graph_hierarchy.h"
#include "definitions.h"
#include "edge_rating/edge_ratings.h"
#include "matching/gpa/gpa_matching.h"
#include "matching/random_matching.h"
#include "clustering/simple_clustering.h"
#include "clustering/size_constraint_label_propagation.h"
#include "clustering/low_diameter_clustering.h"
#include "stop_rules/stop_rules.h"

class coarsening_configurator {
public:
    static void configure_coarsening(const PartitionConfig &partition_config,
                                     std::unique_ptr<matching> *edge_matcher,
                                     unsigned level);
};

inline void coarsening_configurator::configure_coarsening(const PartitionConfig &partition_config,
                                                          std::unique_ptr<matching> *edge_matcher,
                                                          unsigned level) {

    switch (partition_config.matching_type) {
        case MATCHING_RANDOM:
            *edge_matcher = std::make_unique<random_matching>();
            break;
        case MATCHING_GPA:
            *edge_matcher = std::make_unique<gpa_matching>();
            break;
        case MATCHING_RANDOM_GPA:
            *edge_matcher = std::make_unique<gpa_matching>();
            break;
        case LP_CLUSTERING:
            *edge_matcher = std::make_unique<size_constraint_label_propagation>();
            break;
        case SIMPLE_CLUSTERING:
            *edge_matcher = std::make_unique<simple_clustering>();
            break;
        case LOW_DIAMETER:
            *edge_matcher = std::make_unique<low_diameter_clustering>();
            break;
    }

    if (partition_config.matching_type == MATCHING_RANDOM_GPA && level < partition_config.aggressive_random_levels) {
        PRINT(std::cout << "random matching" << std::endl;)
        *edge_matcher = std::make_unique<random_matching>();
    }
}

#endif /* end of include guard: COARSENING_CONFIGURATOR_8UJ78WYS */
