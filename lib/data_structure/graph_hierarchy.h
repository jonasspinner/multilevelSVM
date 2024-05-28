/******************************************************************************
 * graph_hierarchy.h
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 *
 ******************************************************************************
 * Copyright (C) 2013-2015 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2015 of the License, or (at your option)
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

#ifndef GRAPH_HIERARCHY_UMHG74CO
#define GRAPH_HIERARCHY_UMHG74CO

#include <stack>

#include "graph_access.h"

class graph_hierarchy {
  public:
    graph_hierarchy() = default;

    ~graph_hierarchy();

    void push_back(graph_access *G, CoarseMapping *coarse_mapping);

    graph_access *pop_finer_and_project();

    graph_access *get_coarsest();

    CoarseMapping *get_mapping_of_current_finer();

    [[nodiscard]] bool isEmpty() const;

    [[nodiscard]] unsigned int size() const;

  private:
    // private functions
    graph_access *pop_coarsest();

    std::stack<graph_access *> m_the_graph_hierarchy;
    std::stack<CoarseMapping *> m_the_mappings;
    std::vector<CoarseMapping *> m_to_delete_mappings;
    std::vector<graph_access *> m_to_delete_hierarchies;
    graph_access *m_current_coarser_graph{nullptr};
    graph_access *m_coarsest_graph{nullptr};
    graph_access *m_finest_graph{nullptr};
    CoarseMapping *m_current_coarse_mapping{nullptr};
};

#endif /* end of include guard: GRAPH_HIERARCHY_UMHG74CO */
