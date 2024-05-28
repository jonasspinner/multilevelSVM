cmake_minimum_required(VERSION 3.22)
project(multilevelSVM)

set(CMAKE_CXX_STANDARD 17)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/collect-sources.cmake)
include(cmake/clang-format.cmake)
include(cmake/clang-tidy.cmake)
include(cmake/cppcheck.cmake)


### thundersvm
set(USE_CUDA OFF CACHE BOOL "" FORCE)
set(USE_EIGEN ON CACHE BOOL "" FORCE)
add_subdirectory(extern/thundersvm EXCLUDE_FROM_ALL SYSTEM)
include_directories(SYSTEM extern/thundersvm/include)
include_directories(SYSTEM ${PROJECT_BINARY_DIR}/extern/thundersvm) # to use config.h
link_directories(${PROJECT_BINARY_DIR}/extern/thundersvm)

### bayesopt
find_library(BAYESOPT bayesopt HINTS extern/bayesopt/build/lib REQUIRED NO_DEFAULT_PATH)
find_library(NLOPT nlopt HINTS extern/bayesopt//build/lib REQUIRED NO_DEFAULT_PATH)
include_directories(SYSTEM extern/bayesopt/include extern/bayesopt/utils)

### argtable2
find_library(ARGTABLE2 argtable2 HINTS extern/argtable-2.10/lib REQUIRED NO_DEFAULT_PATH)
include_directories(SYSTEM extern/argtable-2.10/include)

### libsvm
set(LIBSVM_SRC extern/libsvm-3.22/src/svm.cpp)
set_property(SOURCE ${LIBSVM_SRC} PROPERTY SYSTEM)
include_directories(SYSTEM extern/libsvm-3.22/include)

### flann
include_directories(SYSTEM extern/flann-1.8.4/include)

### multilevelsvm
include_directories(lib)


#add_compile_options(-fsanitize=address,undefined)
#add_link_options(-fsanitize=address,undefined)

add_compile_options(-Wall -Wextra)
add_compile_options(-Wunused -Wunused-function)
add_compile_options(-Wnon-virtual-dtor -Woverloaded-virtual)
add_compile_options(-Wnull-dereference)
add_compile_options(-Wshadow -Wpedantic)
add_compile_options(-Wduplicated-cond -Wduplicated-branches -Wlogical-op)
#add_compile_options( -Wsign-conversion -Wold-style-cast -Wuseless-cast  -Wcast-align)
add_compile_options(-Wmisleading-indentation -Wdouble-promotion -Wformat=2 -Wno-comment)

add_compile_definitions(KAFFPAOUTPUT)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -DNDEBUG -Wno-unused-parameter -funroll-loops")


set(KAFFPA_SRC
        lib/algorithms/jarnik_prim.cpp
        lib/data_structure/graph_hierarchy.cpp
        lib/io/graph_io.cpp
        lib/partition/partition_config.cpp
        lib/partition/coarsening/coarsening.cpp
        lib/partition/coarsening/contraction.cpp
        lib/partition/coarsening/edge_rating/edge_ratings.cpp
        lib/partition/coarsening/matching/random_matching.cpp
        lib/partition/coarsening/matching/gpa/gpa_matching.cpp
        lib/partition/coarsening/matching/gpa/path_set.cpp
        lib/partition/coarsening/clustering/size_constraint_label_propagation.cpp
        lib/partition/coarsening/clustering/simple_clustering.cpp
        lib/partition/coarsening/clustering/low_diameter_clustering.cpp
        lib/partition/uncoarsening/refinement/quotient_graph_refinement/complete_boundary.cpp
        lib/tools/random_functions.cpp
)


set(KASVM_SRC
        lib/svm/k_fold.cpp
        lib/svm/k_fold_build.cpp
        lib/svm/k_fold_import.cpp
        lib/svm/k_fold_once.cpp
        lib/svm/svm_flann.cpp
        lib/io/svm_io.cpp
        lib/svm/svm_convert.cpp
        lib/svm/results.cpp
        lib/svm/svm_solver.cpp
        lib/svm/svm_solver_libsvm.cpp
        lib/svm/svm_solver_thunder.cpp
        lib/svm/svm_instance.cpp
        lib/svm/svm_summary.cpp
        lib/svm/svm_result.cpp
        lib/svm/param_search.cpp
        lib/svm/svm_refinement.cpp
        lib/svm/ud_refinement.cpp
        lib/svm/bayes_refinement.cpp
        lib/svm/fix_refinement.cpp
        ${LIBSVM_SRC}
)
add_library(kasvm_library ${KAFFPA_SRC} ${KASVM_SRC})


add_compile_definitions(MODE_KASVM)
#add_compile_definitions(SVM_SOLVER=svm_solver_thunder SVM_MODEL=SVC)
add_compile_definitions(SVM_SOLVER=svm_solver_libsvm SVM_MODEL=svm_model)


add_executable(kasvm app/kasvm-train.cpp)
target_link_libraries(kasvm PRIVATE kasvm_library ${BAYESOPT} ${NLOPT} ${ARGTABLE2} thundersvm gomp)


add_executable(kasvm_predict app/kasvm-predict.cpp)
target_link_libraries(kasvm_predict PRIVATE kasvm_library ${BAYESOPT} ${NLOPT} ${ARGTABLE2} thundersvm gomp)


add_executable(single_level_svm app/single_level_svm.cpp)
target_link_libraries(single_level_svm PRIVATE kasvm_library ${BAYESOPT} ${NLOPT} ${ARGTABLE2} thundersvm gomp)


add_executable(prepare app/prepare.cpp lib/svm/svm_flann.cpp)
target_link_libraries(prepare PRIVATE ${ARGTABLE2} gomp)
