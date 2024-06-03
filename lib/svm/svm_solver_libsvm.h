#ifndef SVM_SOLVER_LIBSVM_H
#define SVM_SOLVER_LIBSVM_H

#include <svm.h>
#include <utility>

#include "svm_solver.h"

class svm_solver_libsvm : public svm_solver<svm_model> {
  public:
    svm_solver_libsvm() : svm_solver<svm_model>(){};

    explicit svm_solver_libsvm(svm_instance instance) : svm_solver<svm_model>(std::move(instance)){};

    void train() override;

    int predict(const std::vector<svm_node> &nodes) override;

    void export_to_file(const std::string &path) override;

    std::pair<std::vector<NodeID>, std::vector<NodeID>> get_SV() override;
};

#endif /* SVM_SOLVER_LIBSVM_H */
