#ifndef SVM_SOLVER_H
#define SVM_SOLVER_H

#include <vector>
#include <utility>
#include <memory>
#include <svm.h>

#include "svm_definitions.h"
#include "data_structure/graph_access.h"
#include "svm_summary.h"
#include "svm_result.h"

template<class T>
class svm_solver {
public:
    svm_solver() = default;

    explicit svm_solver(svm_instance instance);

    virtual ~svm_solver() = default;

    virtual void train() = 0;

    svm_result<T> train_range(const std::vector<svm_param> &params,
                              const svm_data &min_sample,
                              const svm_data &maj_sample);

    svm_summary<T> train_single(svm_param,
                                const svm_data &min_sample,
                                const svm_data &maj_sample);

    virtual std::vector<int> predict_batch(const svm_data &data);

    virtual int predict(const std::vector<svm_node> &node) = 0;

    virtual void export_to_file(const std::string &path) = 0;

    virtual std::pair<std::vector<NodeID>, std::vector<NodeID>> get_SV() = 0;

    svm_summary<T> build_summary(const svm_data &min, const svm_data &maj);

    void set_C(float C);

    void set_gamma(float gamma);

    virtual void set_model(std::shared_ptr<T> new_model);

    const svm_instance &get_instance();

protected:
    svm_parameter param{};
    svm_instance instance{};
    std::shared_ptr<T> model;
};

#endif /* SVM_SOLVER_H */
