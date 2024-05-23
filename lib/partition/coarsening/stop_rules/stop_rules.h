/******************************************************************************
 * stop_rules.h 
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

#ifndef STOP_RULES_SZ45JQS6
#define STOP_RULES_SZ45JQS6

#include <cmath>

#include "partition/partition_config.h"
#include <iostream>

class stop_rule {
public:
    stop_rule() = default;

    virtual ~stop_rule() = default;

    virtual bool stop(NodeID number_of_finer_vertices, NodeID number_of_coarser_vertices) = 0;
};


class simple_fixed_stop_rule : public stop_rule {
public:
    simple_fixed_stop_rule(PartitionConfig &config, NodeID _number_of_nodes) {
        num_stop = config.fix_num_vert_stop;
    };

    ~simple_fixed_stop_rule() override = default;

    bool stop(NodeID no_of_finer_vertices, NodeID no_of_coarser_vertices) override;

private:
    NodeID num_stop;
};

inline bool simple_fixed_stop_rule::stop(NodeID no_of_finer_vertices, NodeID no_of_coarser_vertices) {
    double contraction_rate = no_of_finer_vertices / (double) no_of_coarser_vertices;
    return contraction_rate >= 1.05 && no_of_coarser_vertices >= num_stop;
}

#endif /* end of include guard: STOP_RULES_SZ45JQS6 */
