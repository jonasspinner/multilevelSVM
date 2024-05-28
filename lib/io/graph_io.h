/******************************************************************************
 * graph_io.h
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

#ifndef GRAPHIO_H_
#define GRAPHIO_H_

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>
#include <vector>

#include "data_structure/graph_access.h"
#include "definitions.h"

class graph_io {
  public:
    static int writeGraphGDF(const graph_access &G_min, const graph_access &G_maj, const std::string &filename);

    static int readFeatures(graph_access &G, const std::vector<FeatureVec> &data);

    static int readGraphFromVec(graph_access &G, const std::vector<std::vector<Edge>> &data, EdgeID num_edges);

    static EdgeID makeEdgesBidirectional(std::vector<std::vector<Edge>> &data);
};

#endif /*GRAPHIO_H_*/
