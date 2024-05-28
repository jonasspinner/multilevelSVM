#ifndef K_FOLD_ONCE_H
#define K_FOLD_ONCE_H

#include "k_fold_build.h"
#include "partition/partition_config.h"

class k_fold_once : public k_fold_build {
  public:
    k_fold_once(const PartitionConfig &config, const std::string &basename);

    ~k_fold_once() override = default;

  protected:
    void next_intern(double &io_time) override;
};

#endif /* K_FOLD_ONCE_H */
