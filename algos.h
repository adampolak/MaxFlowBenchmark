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

#include "distribution.h"

#include "algorithms/preflow.h"
#include "algorithms/learning_augmented_add_edges_lemon.h"
#include "algorithms/learning_augmented_paths_removal_lemon.h"


class algos {
public:
    void run(
        distribution& d,
        const std::string& preprocessed_flows_filename,
        const std::string& results_file,
        int n_test_samples,
        int algorithms_bitmask
    );
};


#endif //MAXFLOWBENCHMARK_ALGOS_H
