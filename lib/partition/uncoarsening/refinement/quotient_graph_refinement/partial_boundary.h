/******************************************************************************
 * partial_boundary.h 
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

#ifndef PARTIAL_BOUNDARY_963CRO9F_
#define PARTIAL_BOUNDARY_963CRO9F_

#include <unordered_map>
#include "definitions.h"

struct compare_nodes_contains {
    bool operator()(const NodeID lhs, const NodeID rhs) const {
        return (lhs == rhs);
    }
};


struct is_boundary {
    bool contains;

    is_boundary() {
        contains = false;
    }
};


struct hash_boundary_nodes {
    size_t operator()(const NodeID idx) const {
        return idx;
    }
};

typedef std::unordered_map<const NodeID, is_boundary, hash_boundary_nodes, compare_nodes_contains> is_boundary_node_hashtable;

class PartialBoundary {
public:
    bool contains(NodeID node) {
        return internal_boundary.find(node) != internal_boundary.end();
    }

    inline void insert(NodeID node) { internal_boundary[node].contains = true; }

    inline NodeID size() const { return internal_boundary.size(); }

    inline void clear() { return internal_boundary.clear(); }

    is_boundary_node_hashtable internal_boundary;
};

#endif /* end of include guard: PARTIAL_BOUNDARY_963CRO9F_ */
