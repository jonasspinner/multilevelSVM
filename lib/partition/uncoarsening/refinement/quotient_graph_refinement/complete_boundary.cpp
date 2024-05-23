/******************************************************************************
 * complete_boundary.cpp 
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

#include "complete_boundary.h"
#include "tools/quality_metrics.h"

complete_boundary::complete_boundary(graph_access * G) {
        m_graph_ref   = G;
        m_pb_lhs_lazy = 0;
        m_pb_rhs_lazy = 0;
        m_last_pair   = 0;
        m_last_key    = -1;
        m_block_infos.resize(G->get_partition_count());
        delete Q.graphref;
        Q.graphref    = NULL;
}

complete_boundary::~complete_boundary() {
}
