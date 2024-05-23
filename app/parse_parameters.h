/******************************************************************************
 * parse_parameters.h
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 *
 ******************************************************************************
 * Copyright (C) 2013-2015 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/


#ifndef PARSE_PARAMETERS_GPJMGSM8
#define PARSE_PARAMETERS_GPJMGSM8

#include <argtable2.h>
#include <regex.h>
#include <cstring>
#include <omp.h>
#include <sstream>

int parse_parameters(int argn, char **argv,
                     PartitionConfig & partition_config) {

        const char *progname = argv[0];

        // Setup argtable parameters.
        struct arg_lit *help                                 = arg_lit0(nullptr, "help","Print help.");
        struct arg_str *filename                             = arg_strn(nullptr, nullptr, "FILE", 1, 1, "Path to graph file to partition.");
        struct arg_str *filename_output                      = arg_str0(nullptr, "output_filename", nullptr, "Specify the name of the output file (that contains the partition).");
        struct arg_int *user_seed                            = arg_int0(nullptr, "seed", nullptr, "Seed to use for the PRNG.");
        struct arg_int *num_experiments                      = arg_int0("e", "num_experiments", nullptr, "Number of experiments i.e. full kfold runs (default 1)");
	struct arg_rex *validation_type                      = arg_rex0(nullptr, "validation", "^(kfold|kfold_import|once|train_test_split)$", "TYPE", REG_EXTENDED, "Type of validation. One of {kfold, kfold_import, once, train_test_split} (Default: kfold)"  );
        struct arg_int *kfold_iterations                     = arg_int0("k", "kfold_iterations", nullptr, "Number of kfold iterations (Default: 5)");
        struct arg_dbl *time_limit                           = arg_dbl0(nullptr, "time_limit", nullptr, "Time limit in s. Default 0s .");
        struct arg_int *timeout                              = arg_int0(nullptr, "timeout", nullptr, "Timeout in seconds after the timeout (for a single kfold) run is readched the program is aborted (Default: 0)");
        struct arg_lit *export_graph                         = arg_lit0(nullptr, "export_graph","Export the graph at every level (this exits after one multilevel cycle).");
        struct arg_str *export_model_path                    = arg_str0(nullptr, "export_model", nullptr, "Specify the path of the output model (it contains the trained SVM model for later usage) ( a number and \".model\" will be appended to the path).");
        struct arg_int *n_cores                              = arg_int0("c", "n_cores", nullptr, "How many cores are used (Default: 0 aka. every core)");

        // matching/clustering
        struct arg_rex *edge_rating                          = arg_rex0(nullptr, "edge_rating", "^(weight|realweight|expansionstar|expansionstar2|expansionstar2deg|punch|expansionstar2algdist|expansionstar2algdist2|algdist|algdist2|sepmultx|sepaddx|sepmax|seplog|r1|r2|r3|r4|r5|r6|r7|r8)$", "RATING", REG_EXTENDED, "Edge rating to use. One of {weight, expansionstar, expansionstar2, punch, sepmultx, sepaddx, sepmax, seplog, " " expansionstar2deg}. Default: weight"  );
        struct arg_rex *matching_type                        = arg_rex0(nullptr, "matching", "^(random|gpa|randomgpa|lp_clustering|simple_clustering|low_diameter)$", "TYPE", REG_EXTENDED, "Type of matchings to use during coarsening. One of {random, gpa, randomgpa, lp_clustering, simple_clustering, low_diameter}."  );
        struct arg_lit *gpa_grow_internal                    = arg_lit0(nullptr, "gpa_grow_internal", "If the graph is allready partitions the paths are grown only block internally.");
        struct arg_rex *permutation_quality                  = arg_rex0(nullptr, "permutation_quality", "^(none|fast|good|cacheefficient)$", "QUALITY", REG_EXTENDED, "The quality of permutations to use. One of {none, fast," " good, cacheefficient}."  );
        // stop rule
        struct arg_rex *stop_rule                            = arg_rex0(nullptr, "stop_rule", "^(simple-fix)$", "VARIANT", REG_EXTENDED, "Stop rule to use. One of {simple-fix}. Default: simple-fix" );
        struct arg_int *num_vert_stop_factor                 = arg_int0(nullptr, "num_vert_stop_factor", nullptr, "x*k (for multiple_k stop rule). Default 20.");
        struct arg_int *fix_num_vert_stop                    = arg_int0(nullptr, "fix_num_vert_stop", nullptr, "Number of vertices to fix stop coarsening at.");

        struct arg_lit *balance_edges                        = arg_lit0(nullptr, "balance_edges", "Turn on balancing of edges among blocks.");

        // label propagation
        struct arg_int *cluster_upperbound                   = arg_int0(nullptr, "cluster_upperbound", nullptr, "Set a size-constraint on the size of a cluster. Default: none");
        struct arg_int *label_propagation_iterations         = arg_int0(nullptr, "label_propagation_iterations", nullptr, "Set the number of label propgation iterations. Default: 10.");

        // low_diameter
        struct arg_dbl *diameter_upperbound                  = arg_dbl0(nullptr, "diameter_upperbound", nullptr, "Set a size-constraint on the size of a low diameter cluster. Default: 20");

        // KASVM import
        /* struct arg_lit *import_kfold                         = arg_lit0(nullptr, "import_kfold", "Import the kfold crossvalidation instead of computing them from the data."); */
        struct arg_int *num_nn                               = arg_int0("n", "num_nn", nullptr, "Number of nearest neighbors to consider when building the graphs. (Default: 10)");
        struct arg_lit *bidirectional                        = arg_lit0("b", "bidirectional", "Make the nearest neighbor graph bidirectional");

        struct arg_dbl *sample_percent                       = arg_dbl0("s", "sample", nullptr, "Percentage of data that is use. Usefull if very slow on large datasets (Default: 1.0 aka use all data)");

        struct arg_dbl *validation_percent                   = arg_dbl0(nullptr, "validation_percent", nullptr, "Percentage of data that is use for validation (Default: 0.1)");
        struct arg_lit *validation_seperate                  = arg_lit0(nullptr, "validation_seperate", "Should the validation data be also used for training (Default: 'no' for kasvm  'yes' for single_level - this flag invertse the choice)");


        // KASVM refinement
	struct arg_rex *refinement_type                      = arg_rex0(nullptr, "refinement", "^(ud|bayes|fix)$", "TYPE", REG_EXTENDED, "Type of refinement. One of {ud, bayes, fix} (Default: ud)"  );
	struct arg_dbl *fix_C                                = arg_dbl0("C", nullptr, nullptr, "value of the C parameter when using fix refinement. (use logarithmic scale)");
	struct arg_dbl *fix_gamma                            = arg_dbl0("g", nullptr, nullptr, "value of the gamma parameter when using fix refinement. (use logarithmic scale)");
	struct arg_dbl *beta                                 = arg_dbl0(nullptr, "beta", nullptr, "value of the beta parameter when using low diameter clustering. (Default: 0.4)");
        struct arg_int *num_skip_ms                          = arg_int0(nullptr, "num_skip_ms", nullptr, "Size of the problem on which no model selection is skipped and only the best parameters of the previous level are used (Default: 10000)");
        struct arg_lit *no_inherit_ud                        = arg_lit0(nullptr, "no_inherit_ud", "Don't inherit the first UD sweep and do only the second UD sweep in the refinement.");

        struct arg_end *end                                  = arg_end(100);

        // Define argtable.
        void* argtable[] = {
                            help,
                            filename,
                            user_seed,
#if defined MODE_KASVM
                            num_experiments,
                            kfold_iterations,
			    sample_percent,
                            validation_type,
                            validation_percent,
                            validation_seperate,
                            num_nn,
                            bidirectional,
                            stop_rule,
                            fix_num_vert_stop,
                            matching_type,
                            cluster_upperbound,
                            label_propagation_iterations,
                            diameter_upperbound,
			    beta,
			    refinement_type,
			    fix_C,
			    fix_gamma,
                            num_skip_ms,
                            no_inherit_ud,
			    export_graph,
                            filename_output,
			    export_model_path,
                            timeout,
			    n_cores,
#endif
                            end
        };

        // Parse arguments.
        int nerrors = arg_parse(argn, argv, argtable);

        // Catch case that help was requested.
        if (help->count > 0) {
                printf("Usage: %s", progname);
                arg_print_syntax(stdout, argtable, "\n");
                arg_print_glossary(stdout, argtable,"  %-40s %s\n");
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                exit(0);
        }


        if (nerrors > 0) {
                arg_print_errors(stderr, end, progname);
                printf("Try '%s --help' for more information.\n",progname);
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1;
        }

        if(filename->count > 0) {
                partition_config.filename = filename->sval[0];
        }

        if(filename_output->count > 0) {
                partition_config.filename_output = filename_output->sval[0];
        }

        if(balance_edges->count > 0) {
                partition_config.balance_edges = true;
        }

        if(fix_num_vert_stop->count > 0) {
                partition_config.fix_num_vert_stop = fix_num_vert_stop->ival[0];
        }

        if(gpa_grow_internal->count > 0) {
                partition_config.gpa_grow_paths_between_blocks = false;
        }

        if(time_limit->count > 0) {
                partition_config.time_limit = time_limit->dval[0];
        }

        if(n_cores->count > 0) {
                partition_config.n_cores = n_cores->ival[0];
        }

        if(num_vert_stop_factor->count > 0) {
                partition_config.num_vert_stop_factor = num_vert_stop_factor->ival[0];
        }

        if (user_seed->count > 0) {
                partition_config.seed = user_seed->ival[0];
        }

        if (edge_rating->count > 0) {
                if(strcmp("expansionstar", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = EXPANSIONSTAR;
                } else if (strcmp("expansionstar2", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = EXPANSIONSTAR2;
                } else if (strcmp("weight", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = WEIGHT;
                } else if (strcmp("realweight", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = REALWEIGHT;
                } else if (strcmp("expansionstar2algdist", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = EXPANSIONSTAR2ALGDIST;
                } else if (strcmp("geom", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = PSEUDOGEOM;
                } else if (strcmp("sepaddx", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_ADDX;
                } else if (strcmp("sepmultx", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_MULTX;
                } else if (strcmp("sepmax", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_MAX;
                } else if (strcmp("seplog", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_LOG;
                } else if (strcmp("r1", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_R1;
                } else if (strcmp("r2", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_R2;
                } else if (strcmp("r3", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_R3;
                } else if (strcmp("r4", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_R4;
                } else if (strcmp("r5", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_R5;
                } else if (strcmp("r6", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_R6;
                } else if (strcmp("r7", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_R7;
                } else if (strcmp("r8", edge_rating->sval[0]) == 0) {
                        partition_config.edge_rating = SEPARATOR_R8;
                } else {
                        fprintf(stderr, "Invalid edge rating variant: \"%s\"\n",
				edge_rating->sval[0]);
                        exit(0);
                }
        }

        if (stop_rule->count > 0) {
                if(strcmp("simple-fix", stop_rule->sval[0]) == 0) {
                        partition_config.stop_rule = STOP_RULE_SIMPLE_FIXED;
                } else {
                        fprintf(stderr, "Invalid stop rule: \"%s\"\n", stop_rule->sval[0]);
                        exit(0);
                }
        }

        if (permutation_quality->count > 0) {
                if(strcmp("none", permutation_quality->sval[0]) == 0) {
                        partition_config.permutation_quality = PERMUTATION_QUALITY_NONE;
                } else if (strcmp("fast", permutation_quality->sval[0]) == 0) {
                        partition_config.permutation_quality = PERMUTATION_QUALITY_FAST;
                } else if (strcmp("good", permutation_quality->sval[0]) == 0) {
                        partition_config.permutation_quality = PERMUTATION_QUALITY_GOOD;
                } else {
                        fprintf(stderr, "Invalid permutation quality variant: \"%s\"\n",
				permutation_quality->sval[0]);
                        exit(0);
                }

        }

        if (matching_type->count > 0) {
                if(strcmp("random", matching_type->sval[0]) == 0) {
                        partition_config.matching_type = MATCHING_RANDOM;
                } else if (strcmp("gpa", matching_type->sval[0]) == 0) {
                        partition_config.matching_type = MATCHING_GPA;
                } else if (strcmp("randomgpa", matching_type->sval[0]) == 0) {
                        partition_config.matching_type = MATCHING_RANDOM_GPA;
                } else if (strcmp("lp_clustering", matching_type->sval[0]) == 0) {
                        partition_config.matching_type = LP_CLUSTERING;
                } else if (strcmp("simple_clustering", matching_type->sval[0]) == 0) {
                        partition_config.matching_type = SIMPLE_CLUSTERING;
                } else if (strcmp("low_diameter", matching_type->sval[0]) == 0) {
                        partition_config.matching_type = LOW_DIAMETER;
                } else {
                        fprintf(stderr, "Invalid matching variant: \"%s\"\n",
				matching_type->sval[0]);
                        exit(0);
                }
        }

	if (validation_type->count > 0) {
		if (strcmp("kfold", validation_type->sval[0]) == 0) {
			partition_config.validation_type = KFOLD;
		} else if (strcmp("kfold_import", validation_type->sval[0]) == 0) {
			partition_config.validation_type = KFOLD_IMPORT;
		} else if (strcmp("once", validation_type->sval[0]) == 0) {
			partition_config.validation_type = ONCE;
		} else if (strcmp("train_test_split", validation_type->sval[0]) == 0) {
			partition_config.validation_type = TRAIN_TEST_SPLIT;
		} else {
                        fprintf(stderr, "Invalid validation variant: \"%s\"\n",
				validation_type->sval[0]);
                        exit(0);
		}
	}

        if (label_propagation_iterations->count > 0) {
                partition_config.label_iterations = label_propagation_iterations->ival[0];
        }

        if (cluster_upperbound->count > 0) {
                partition_config.cluster_upperbound = cluster_upperbound->ival[0];
        }

        if (diameter_upperbound->count > 0) {
                partition_config.diameter_upperbound = diameter_upperbound->dval[0];
        }

        if (beta->count > 0) {
                partition_config.beta = beta->dval[0];
        }

        if (num_experiments->count > 0) {
                partition_config.num_experiments = num_experiments->ival[0];
        }

        if (kfold_iterations->count > 0) {
                partition_config.kfold_iterations = kfold_iterations->ival[0];
        }

	if (sample_percent->count > 0) {
		partition_config.sample_percent = sample_percent->dval[0];
	}

        if (validation_percent->count > 0) {
                partition_config.validation_percent = validation_percent->dval[0];
        }

        if (validation_seperate->count > 0) {
                partition_config.validation_seperate = !partition_config.validation_seperate;
        }

        if (num_nn->count > 0) {
                partition_config.num_nn = num_nn->ival[0];
        }

	if (refinement_type->count > 0) {
		if (strcmp("ud", refinement_type->sval[0]) == 0) {
			partition_config.refinement_type = UD;
		} else if (strcmp("bayes", refinement_type->sval[0]) == 0) {
			partition_config.refinement_type = BAYES;
		} else if (strcmp("fix", refinement_type->sval[0]) == 0) {
			partition_config.refinement_type = FIX;
		} else {
                        fprintf(stderr, "Invalid refinement variant: \"%s\"\n",
				refinement_type->sval[0]);
                        exit(0);
		}
	}

	if(fix_C->count > 0) {
		partition_config.fix_C = fix_C->dval[0];
	}

	if(fix_gamma->count > 0) {
		partition_config.fix_gamma = fix_gamma->dval[0];
	}

        if(num_skip_ms->count > 0) {
                partition_config.num_skip_ms = num_skip_ms->ival[0];
        }

        if(no_inherit_ud->count > 0) {
                partition_config.inherit_ud = false;
        }

        if(timeout->count > 0) {
                partition_config.timeout = timeout->ival[0];
        }

        if(export_graph->count > 0) {
		partition_config.export_graph = true;
        }

        if(export_model_path->count > 0) {
                partition_config.export_model_path = export_model_path->sval[0];
        }

        if(bidirectional->count > 0) {
                partition_config.bidirectional = true;
        }

        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 0;
}

#endif /* end of include guard: PARSE_PARAMETERS_GPJMGSM8 */
