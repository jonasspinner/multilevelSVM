#ifndef SIMPLE_CLUSTERING_H
#define SIMPLE_CLUSTERING_H

#include "definitions.h"
#include "partition/coarsening/matching/matching.h"

class simple_clustering : public matching {
  public:
    simple_clustering() = default;

    ~simple_clustering() override = default;

    void match(const PartitionConfig &config, graph_access &G, Matching &_matching, CoarseMapping &coarse_mapping,
               NodeID &no_of_coarse_vertices, NodePermutationMap &permutation) override;

  private:
};

#endif /* SIMPLE_CLUSTERING_H */
