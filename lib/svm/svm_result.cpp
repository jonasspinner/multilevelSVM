#include <algorithm>
#include <utility>

#include <thundersvm/model/svc.h>
#include "svm_result.h"

template<class T>
svm_result<T>::svm_result(svm_instance  instance)
	: instance(std::move(instance)) {
}

template<class T>
svm_result<T>::svm_result(const std::vector<svm_summary<T>> & summaries, svm_instance  instance)
        : summaries(summaries), instance(std::move(instance)) {

        this->sort_summaries();
}

template<class T>
svm_summary<T> svm_result<T>::best() {
        return summaries[0];
}

template<class T>
void svm_result<T>::add(const svm_result<T> & result) {
        this->add(result.summaries);
}

template<class T>
void svm_result<T>::add(const std::vector<svm_summary<T>> & to_add) {
        this->summaries.insert(this->summaries.end(), to_add.begin(), to_add.end());
        this->sort_summaries();
}

template<class T>
void svm_result<T>::sort_summaries() {
        // insertion_sort
        size_t j;

        for (size_t i = 0; i < this->summaries.size(); i++){
                j = i;

                while (j > 0 && summary_cmp_better_gmean_sv::comp(this->summaries[j], this->summaries[j-1])) {
                        svm_summary<T> temp = std::move(this->summaries[j]);
                        this->summaries[j] = std::move(this->summaries[j-1]);
                        this->summaries[j-1] = std::move(temp);
                        j--;
                }
        }
}

template<class T>
size_t svm_result<T>::get_best_index(const std::vector<std::pair<svm_summary<T>,svm_instance>> &vec) {
        size_t best_index = 0;

        for (size_t i = 1; i < vec.size(); i++) {
                if (summary_cmp_better_gmean::comp(vec[i].first, vec[best_index].first)) {
                        best_index = i;
                }
        }
        return best_index;
}

template class svm_result<svm_model>;
template class svm_result<SVC>;
