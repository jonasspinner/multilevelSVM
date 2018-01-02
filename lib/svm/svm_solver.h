#ifndef SVM_SOLVER_H
#define SVM_SOLVER_H

#include <svm.h>
#include "data_structure/graph_access.h"
#include "svm_summary.h"

typedef std::vector<svm_summary> svm_result;

class svm_solver
{
public:
        svm_solver();
        svm_solver(const svm_solver & solver);
        virtual ~svm_solver();

        void read_problem(const graph_access & G_min, const graph_access & G_maj);

        void train();
        svm_result train_initial(const std::vector<std::vector<svm_node>>& min_sample,
                                 const std::vector<std::vector<svm_node>>& maj_sample);
        svm_result train_range(const std::vector<std::pair<float,float>> & params,
                               const std::vector<std::vector<svm_node>>& min_sample,
                               const std::vector<std::vector<svm_node>>& maj_sample);

        int predict(const std::vector<svm_node> & node);

        svm_summary predict_validation_data(const std::vector<std::vector<svm_node>> & min,
                                            const std::vector<std::vector<svm_node>> & maj);

private:
        void add_graph_to_problem(const graph_access & G, int label, NodeID offset);
        static svm_summary select_best_model(std::vector<svm_summary> & vec);
        static svm_result make_result(const std::vector<svm_summary> & vec);

        svm_problem prob;
        svm_parameter param;
        svm_model *model;

};


#endif /* SVM_SOLVER_H */
