#ifndef REFINEMENT_H
#define REFINEMENT_H

#include "data_structure/graph_hierarchy.h"
#include "definitions.h"
#include "partition/partition_config.h"
#include "svm_result.h"

template <class T> class svm_refinement {
  public:
    svm_refinement(graph_hierarchy &min_hierarchy, graph_hierarchy &maj_hierarchy, const svm_result<T> &initial_result,
                   const PartitionConfig &conf);

    virtual ~svm_refinement() = default;

    [[nodiscard]] bool is_done() const;

    [[nodiscard]] int get_level() const;

    void uncoarse(const std::vector<NodeID> &sv_min, const std::vector<NodeID> &sv_maj);

    static svm_data uncoarse_SV(const graph_access &G, const CoarseMapping &coarse_mapping,
                                const std::vector<NodeID> &sv, std::vector<NodeID> &data_mapping);

    virtual svm_result<T> step(const svm_data &min_sample, const svm_data &maj_sample) = 0;

    graph_access *G_min{};
    graph_access *G_maj{};
    std::vector<NodeID> data_mapping_min{};
    std::vector<NodeID> data_mapping_maj{};

  protected:
    graph_hierarchy *min_hierarchy{};
    graph_hierarchy *maj_hierarchy{};
    svm_data uncoarsed_data_min{};
    svm_data uncoarsed_data_maj{};
    svm_result<T> result{};

    bool training_inherit{};
    int num_skip_ms{};
};

#endif /* REFINEMENT_H */
