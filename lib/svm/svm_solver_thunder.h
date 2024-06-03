#ifndef SVM_SOLVER_THUNDER_H
#define SVM_SOLVER_THUNDER_H

#include <thundersvm/model/svc.h>
#include <utility>

#include "svm_solver.h"

class svm_solver_thunder : public svm_solver<SVC> {
  public:
    svm_solver_thunder() : svm_solver<SVC>(){};

    explicit svm_solver_thunder(svm_instance instance) : svm_solver<SVC>(std::move(instance)){};

    ~svm_solver_thunder() override = default;

    void train() override;

    int predict(const std::vector<svm_node> &nodes) override;

    std::vector<int> predict_batch(const svm_data &data) override;

    void export_to_file(const string &path) override;

    std::pair<std::vector<NodeID>, std::vector<NodeID>> get_SV() override;
};

#endif /* SVM_SOLVER_THUNDER_H */
