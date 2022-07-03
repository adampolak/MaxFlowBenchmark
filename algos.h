//
// Created by Max Zub on 25/05/2022.
//

#ifndef MAXFLOWBENCHMARK_ALGOS_H
#define MAXFLOWBENCHMARK_ALGOS_H


#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <utility>
#include <chrono>
#include <iomanip>
#include <set>


#include "algorithms/preflow.h"
#include "algorithms/learning_augmented_add_edges_lemon.h"
#include "algorithms/learning_augmented_paths_removal_lemon.h"
#include "dimacs_input.h"

#include "random_util.h"

class algos {
public:
    void run(
        const std::string& graph_filename,
        const std::string& preprocessed_flows_filename,
        const std::string& results_file,
        int test_samples,
        int algorithms_bitmask,
        double X
    );
};


#endif //MAXFLOWBENCHMARK_ALGOS_H
