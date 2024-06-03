#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <thundersvm/model/svc.h>
#include <utility>

#include "svm/param_search.h"
#include "svm/svm_convert.h"
#include "svm/svm_solver.h"
#include "tools/timer.h"

template <class T> svm_solver<T>::svm_solver(svm_instance instance) : instance(std::move(instance)) {
    this->param.svm_type = C_SVC;
    this->param.kernel_type = RBF;
    this->param.degree = 3;
    this->param.gamma = 2; // doc suggests 1/num_features
    this->param.coef0 = 0;
    this->param.nu = 0.5;
    this->param.cache_size = static_cast<size_t>(8192);
    this->param.C = 32;
    this->param.eps = 1e-3;
    this->param.p = 0.1;
    this->param.shrinking = 1;
    this->param.probability = 0;
    this->param.nr_weight = 0;
    this->param.weight_label = nullptr;
    this->param.weight = nullptr;
}

template <class T>
svm_result<T> svm_solver<T>::train_range(const std::vector<svm_param> &params, const svm_data &min_sample,
                                         const svm_data &maj_sample) {
    std::vector<svm_summary<T>> summaries;

    // #pragma omp parallel for
    for (auto &&p : params) {
        auto summary = train_single(p, min_sample, maj_sample);

        // #pragma omp critical
        { summaries.push_back(summary); }
    }

    return svm_result<T>(summaries, this->instance);
}

template <class T>
svm_summary<T> svm_solver<T>::train_single(svm_param p, const svm_data &min_sample, const svm_data &maj_sample) {
    timer t;

    this->param.C = pow(2, p.first);
    this->param.gamma = pow(2, p.second);

    std::cout << std::setprecision(2) << std::fixed << "log C=" << std::setw(6) << p.first
              << "\tlog gamma=" << std::setw(6) << p.second << std::flush;

    this->train();

    std::cout << "\ttime=" << t.elapsed() << std::flush;

    svm_summary<T> summary = this->build_summary(min_sample, maj_sample);
    summary.print_short();
    return summary;
}

template <class T> svm_summary<T> svm_solver<T>::build_summary(const svm_data &min, const svm_data &maj) {
    size_t tp = 0, tn = 0, fp = 0, fn = 0;

    for (int res : this->predict_batch(min)) {
        if (res == 1) {
            tp++;
        } else {
            fn++;
        }
    }

    for (int res : this->predict_batch(maj)) {
        if (res == -1) {
            tn++;
        } else {
            fp++;
        }
    }

    auto SV_pair = this->get_SV();

    svm_summary<T> summary(tp, tn, fp, fn);

    summary.model = model;
    summary.SV_min = SV_pair.first;
    summary.SV_maj = SV_pair.second;
    summary.C = this->param.C;
    summary.gamma = this->param.gamma;
    summary.C_log = std::log(this->param.C) / std::log(2);
    summary.gamma_log = std::log(this->param.gamma) / std::log(2);

    return summary;
}

template <class T> std::vector<int> svm_solver<T>::predict_batch(const svm_data &data) {
    std::vector<int> result;
    result.reserve(data.size());
    for (const auto &instance : data) {
        result.push_back(this->predict(instance));
    }
    return result;
}

template <class T> void svm_solver<T>::set_C(float C) { this->param.C = C; }

template <class T> void svm_solver<T>::set_gamma(float gamma) { this->param.gamma = gamma; }

template <class T> void svm_solver<T>::set_model(std::shared_ptr<T> new_model) { this->model = new_model; }

template <class T> const svm_instance &svm_solver<T>::get_instance() const { return this->instance; }

template class svm_solver<svm_model>;

template class svm_solver<SVC>;
