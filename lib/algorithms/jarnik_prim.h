#ifndef JARNIK_PRIM_H
#define JARNIK_PRIM_H

#include <utility>
#include <memory>

#include "data_structure/graph_access.h"

class jarnik_prim {
public:
    static std::pair<std::unique_ptr<graph_access>, NodeID> spanning_tree(const graph_access &graph);
};

#endif /* JARNIK_PRIM_H */
