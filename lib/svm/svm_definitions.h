#ifndef SVM_DEFINITIONS_H
#define SVM_DEFINITIONS_H

#include <svm.h>
#include <utility>
#include <vector>

typedef std::vector<svm_node> svm_feature;
typedef std::vector<svm_feature> svm_data;
typedef std::pair<float, float> svm_param;

#endif /* SVM_DEFINITIONS_H */
