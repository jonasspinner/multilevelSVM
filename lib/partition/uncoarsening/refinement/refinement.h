/******************************************************************************
 * refinement.h 
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

#ifndef REFINEMENT_UJN9IBHM
#define REFINEMENT_UJN9IBHM

#include "data_structure/graph_access.h"
#include "partition/partition_config.h"
#include "partition/uncoarsening/refinement/quotient_graph_refinement/complete_boundary.h"

class refinement {
public:
        refinement( );
        virtual ~refinement();
        
        virtual EdgeWeight perform_refinement(PartitionConfig & config, 
                                              graph_access & G, 
                                              complete_boundary & boundary) = 0;
};


#endif /* end of include guard: REFINEMENT_UJN9IBHM */
