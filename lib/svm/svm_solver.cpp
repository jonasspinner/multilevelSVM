#include <iostream>
#include <algorithm>
#include <cmath>

#include "svm_solver.h"
#include "svm_convert.h"
#include "grid_search.h"
#include "timer.h"


void print_null(const char *s) {}


svm_solver::svm_solver() {
        svm_set_print_string_function(&print_null);

        // default values
        this->param.svm_type = C_SVC;
        this->param.kernel_type = RBF;
        this->param.degree = 3;
        this->param.gamma = 2;	// 1/num_features
        this->param.coef0 = 0;
        this->param.nu = 0.5;
        this->param.cache_size = 100;
        this->param.C = 32;
        this->param.eps = 1e-3;
        this->param.p = 0.1;
        this->param.shrinking = 1;
        this->param.probability = 0;
        this->param.nr_weight = 0;
        this->param.weight_label = NULL;
        this->param.weight = NULL;
}

svm_solver::svm_solver(const svm_solver & solver) {
        this->param = solver.param;
        this->prob = solver.prob;
        this->model = nullptr;
}

svm_solver::~svm_solver() {
        svm_free_and_destroy_model(&(this->model));
        // svm_destroy_param(&(this->param));
        // if (prob.y) {
        //         delete[] prob.y;
        // }
        // if (prob.x) {
        //         for (int i = 0; i < this->prob.l; ++i) {
        //                 delete [] this->prob.x[i];
        //         }
        // }
}

void svm_solver::read_problem(const graph_access & G_min, const graph_access & G_maj) {
        size_t features = G_min.getFeatureVec(0).size();

        this->param.gamma = 1/(float) features;

        this->prob.l = G_min.number_of_nodes() + G_maj.number_of_nodes();
        this->prob.y = new double [this->prob.l];
        this->prob.x = new svm_node* [this->prob.l];
        for (int i = 0; i < this->prob.l; ++i) {
                //this is probably bigger than needed because we omit zero valued entries
                this->prob.x[i] = new svm_node[features+1];
        }

        // vector<vector<svm_node> > nodes(prob.l, vector<svm_node>());

        add_graph_to_problem(G_min, 1, 0);
        add_graph_to_problem(G_maj, -1, G_min.number_of_nodes());
}

void svm_solver::add_graph_to_problem(const graph_access & G, int label, NodeID offset) {
        const FeatureData EPS = 0.000001;
        size_t features = G.getFeatureVec(0).size();

        forall_nodes(G, node) {
                NodeID prob_node = node + offset;
                this->prob.y[prob_node] = label;

                const FeatureVec vec = G.getFeatureVec(node);
                int att_num = 0;
                for (size_t i = 0; i < features; ++i) {
                        if (std::abs(vec[i]) < EPS) {
                                continue;
                        }// skip zero valued features

                        svm_node n;
                        n.index = i+1;
                        n.value = vec[i];
                        this->prob.x[prob_node][att_num] = n;
                        ++att_num;
                }
                svm_node n; // end node
                n.index = -1;
                n.value = 0;
                this->prob.x[prob_node][att_num] = n;
        } endfor
}

void svm_solver::train() {
        const char * error_msg = svm_check_parameter(&(this->prob), &(this->param));
        if (error_msg != NULL) {
                std::cout << error_msg << std::endl;
        }

        this->model = svm_train(&(this->prob), &(this->param));
}

svm_result svm_solver::train_initial(const std::vector<std::vector<svm_node>>& min_sample,
                                     const std::vector<std::vector<svm_node>>& maj_sample) {
        const char * error_msg = svm_check_parameter(&(this->prob), &(this->param));
        if (error_msg != NULL) {
                std::cout << error_msg << std::endl;
        }


        // first grid search
        // grid_search gs(-5,15,2,3,-15,-2);
        // auto params = gs.get_sequence();
        auto params = grid_search::mlsvm_method(-10, 10, -10, 10, true);

        svm_result result = train_range(params, min_sample, maj_sample);
        svm_summary good = result[0];
        int trained_combis = params.size();

        std::cout << "continue with log best C=" << good.C_log << " log gamma=" << good.gamma_log << std::endl;
        good.print();

        // second (finer) grid search
        // grid_search gs2 = grid_search::around(good.C_log, 2, 0.25, good.gamma_log, 2, 0.25);
        // params = gs2.get_sequence();
        params = grid_search::mlsvm_method(-10, 10, -10, 10, false, true, good.C_log, good.gamma_log);
        // params.pop_back(); // the last parameters are equal to the input params

        svm_result second_res = train_range(params, min_sample, maj_sample);
        result.insert(result.end(), second_res.begin(), second_res.end());
        svm_summary best = select_best_model(result);
        trained_combis += params.size();

        std::cout << "trained and validated " << trained_combis << " parameter combinations." << std::endl;
        std::cout << "BEST log C=" << best.C_log << " log gamma=" << best.gamma_log << std::endl;
        best.print();

        // train this solver to the best found parameters
        this->param.C = best.C;
        this->param.gamma = best.gamma;
        this->train();

        return svm_solver::make_result(result);
}

svm_result svm_solver::train_range(const std::vector<std::pair<float,float>> & params,
                                   const std::vector<std::vector<svm_node>>& min_sample,
                                   const std::vector<std::vector<svm_node>>& maj_sample) {
        std::vector<svm_summary> summaries;

        for (auto&& p : params) {
                svm_solver cur_solver(*this); // (copy ctor) use this instances prob and param values
                cur_solver.param.C = pow(2, p.first);
                cur_solver.param.gamma = pow(2, p.second);

                cur_solver.train();

                svm_summary cur_summary = cur_solver.predict_validation_data(min_sample, maj_sample);
                cur_summary.C_log = p.first;
                cur_summary.gamma_log = p.second;

                cur_summary.print_short();

                summaries.push_back(cur_summary);
        }
        svm_solver::select_best_model(summaries);
        return svm_solver::make_result(summaries);
}

svm_summary svm_solver::select_best_model(std::vector<svm_summary> & vec) {
        std::sort(vec.begin(), vec.end(),
                  [](const svm_summary & a, const svm_summary & b){
                          return summary_cmp_better_gmean_sn::comp(a, b);
                  });

        for (size_t i = 0; i < vec.size(); ++i) {
                if(vec[i].Gmean > 0.05) {
                        return vec[i];
                }
        }
        // in case there is no model with gmean larger than zero, return the 1st one
        return vec[0];
}

svm_result svm_solver::make_result(const std::vector<svm_summary> & vec) {
        // assert the vector is sorted (this happened in select_best_model)
        std::vector<svm_summary> res;
        res.push_back(vec[0]);

        for (size_t i = 1; i < vec.size(); ++i) {
                if(vec[i].Gmean > res[0].Gmean - 0.1f) {
                        res.push_back(vec[i]);
                }
        }

        return res;
}

int svm_solver::predict(const std::vector<svm_node> & nodes) {
        return svm_predict(this->model, nodes.data());
}

svm_summary svm_solver::predict_validation_data(const std::vector<std::vector<svm_node>> & min,
                                                const std::vector<std::vector<svm_node>> & maj) {
        size_t tp = 0, tn = 0, fp = 0, fn = 0;

        for (const auto& instance : min) {
                int res = this->predict(instance);
                if (res == 1) {
                        tp++;
                } else {
                        fn++;
                }
        }

        for (const auto& instance : maj) {
                int res = this->predict(instance);
                if (res == -1) {
                        tn++;
                } else {
                        fp++;
                }
        }

        svm_summary summary(*(this->model), tp, tn, fp, fn);

        return summary;
}
