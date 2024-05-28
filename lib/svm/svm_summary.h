#ifndef SVM_SUMMARY_H
#define SVM_SUMMARY_H

#include <vector>
#include <memory>
#include <svm.h>
#include "definitions.h"
#include "svm_instance.h"

template<class T>
class svm_summary {
public:
    svm_summary(NodeID tp, NodeID tn, NodeID fp, NodeID fn);

    void print() const;

    void print_short() const;

    [[nodiscard]] NodeID num_SV_min() const;

    [[nodiscard]] NodeID num_SV_maj() const;

    bool operator>(const svm_summary &other) const {
        return (this->Gmean > other.Gmean);
    }

    float eval(const svm_instance &instance);

    std::shared_ptr<T> model;

    int TP{};
    int TN{};
    int FP{};
    int FN{};
    double Acc{};
    double Sens{};
    double Spec{};
    double Gmean{};
    double F1{};
    double PPV{};
    double NPV{};

    std::vector<NodeID> SV_min;
    std::vector<NodeID> SV_maj;

    double C{};
    double gamma{};

    double C_log{};
    double gamma_log{};
};

struct summary_cmp_better_gmean {
    template<class T>
    static bool comp(const svm_summary<T> &a, const svm_summary<T> &b) {
        return a.Gmean > b.Gmean;
    }
};

struct summary_cmp_better_gmean_sv {
    template<class T>
    static bool comp(const svm_summary<T> &a, const svm_summary<T> &b) {
        float filter_range = 0.02;
        if ((a.Gmean - b.Gmean) > filter_range)         //a has completely better gmean than b
            return true;
        else {
            if ((b.Gmean - a.Gmean) > filter_range)     //b has completely better gmean than a
                return false;
            else {                                                    //similar gmean
                // a has less nSV than b which is better
                return (a.SV_min.size() + a.SV_maj.size() < b.SV_min.size() + b.SV_maj.size());
            }
        }
    }
};

#endif /* SVM_SUMMARY_H */
