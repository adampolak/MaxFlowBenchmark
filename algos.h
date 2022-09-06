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
#include "algorithms/learning_augmented_lower_bounds_lemon.h"
#include "algorithms/learning_augmented_paths_removal_lemon.h"

#include <cassert>
#include "learning/learning.h"

#include <lemon/time_measure.h>


inline void algos_run(
    const distribution& d,
    const lemon::SmartDigraph::ArcMap<int64_t>& predictions,
    int n_test_samples,
    int algorithms_bitmask
    ) {

    const int N_ALGOS = 3;
    typedef typeof(&preflow_run) AlgorithmPtr;
    AlgorithmPtr ALGOS[N_ALGOS] = {
        preflow_run,
        learning_augmented_add_edges_lemon_run,
        learning_augmented_lower_bounds_lemon_run,
        // learning_augmented_paths_removal_lemon_run,
    };
    std::string ANAMES[N_ALGOS] = {
        "goldberg_tarjan",
        "add_edges",
        "lower_bounds",
        // "paths_removal",
    };

    const lemon::SmartDigraph& graph = d.graph();
    const lemon::SmartDigraph::Node s = d.s();
    const lemon::SmartDigraph::Node t = d.t();

    std::default_random_engine generator;

    for (int i = 0; i < N_ALGOS; i++) {
        if (i > 0)
            std::cout << ", ";
        std::cout << ANAMES[i];
    }
    std::cout << std::endl;

    std::cerr << "DEBUG: number of test samples: " << n_test_samples << std::endl;

    for (int L = 0; L < n_test_samples; L++) {
        lemon::SmartDigraph::ArcMap<int64_t> capacityi(graph);
        d.sample_capacities(capacityi, generator);

        int64_t flows_returned[N_ALGOS];
        std::set<int64_t> flows_returned_set;

        for (int i = 0; i < N_ALGOS; i++) {
            if (!((algorithms_bitmask>>i)&1))
                continue;
            std::cerr << "looking for flow with \"" << ANAMES[i] << "\" algorithm" << std::endl;

            // auto start_time = std::chrono::steady_clock::now();

            lemon::Timer timer;
            int64_t found_flow = ALGOS[i](graph, capacityi, s, t, predictions);
            timer.halt();
            // auto time = std::chrono::steady_clock::now() - start_time;

            flows_returned[i] = found_flow;
            flows_returned_set.insert(found_flow);

            // double seconds_elapsed = (double) std::chrono::duration_cast<std::chrono::milliseconds>(time).count() / 1000.0;

            std::cerr << "time elapsed: " << std::setprecision(3) << std::fixed << timer.userTime() << std::endl;
            std::cerr << std::endl;
            std::cout << std::setprecision(3) << std::fixed << timer.userTime() << ", ";
        }

        std::cerr << std::endl;
        if (flows_returned_set.size() > 1) {
            std::cerr << "flows_returned" << std::endl;
            for (int i = 0; i < N_ALGOS; i++) {
                std::cerr << ANAMES[i] << ": " << flows_returned[i] << std::endl;
            }
            return;
        }
    }
}

#endif //MAXFLOWBENCHMARK_ALGOS_H
