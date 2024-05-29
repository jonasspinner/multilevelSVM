#include "k_fold_once.h"

k_fold_once::k_fold_once(const PartitionConfig &config, const std::string &basename) : k_fold_build(config, basename) {}

void k_fold_once::next_intern(double &io_time) {
    k_fold_build::next_intern(io_time);
    this->iterations = 1;
}
