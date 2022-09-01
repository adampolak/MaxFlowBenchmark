//
// Created by Max Zub on 25/05/2022.
//

#include "algos.h"
#include <cassert>
#include "learning/learning.h"

const int ALGOS_COUNT = 3;

void algos::run(
    distribution& d,
    const std::string& preprocessed_flows_filename,
    const std::string& results_file,
    int n_test_samples,
    int algorithms_bitmask
) {

    typedef typeof(&preflow_run) AlgorithmPtr;
    AlgorithmPtr ALGOS[ALGOS_COUNT] = {
        preflow_run,
        learning_augmented_add_edges_lemon_run,
        learning_augmented_paths_removal_lemon_run,
    };

    std::string ANAMES[ALGOS_COUNT] = {
        "preflow", "add_edges", "paths_removal"
    };



    const lemon::SmartDigraph& graph = d.graph();
    const lemon::SmartDigraph::Node s = d.s();
    const lemon::SmartDigraph::Node t = d.t();

    std::ofstream output_result(results_file);

    output_result << "boy_kol, preflow, add_edge_preflow" << std::endl;

    std::cout << results_file << std::endl;

    std::default_random_engine generator;

    std::cout << n_test_samples << std::endl;

    lemon::SmartDigraph::ArcMap<int64_t> predictions(graph);
    load_predictions(d.graph(), preprocessed_flows_filename, predictions);
    verify_predictions(d.graph(), d.s(), d.t(), predictions);


    for (int L = 0; L < n_test_samples; L++) {

        lemon::SmartDigraph::ArcMap<int64_t> capacityi(graph);
        

        d.sample_capacities(capacityi, generator);


        int64_t flows_returned[ALGOS_COUNT];
        std::set<long long> flows_returned_set;

        for (int i = 0; i < ALGOS_COUNT; i++) {
            if (!((algorithms_bitmask>>i)&1))
                continue;
            std::cout << "looking for flow with \"" << ANAMES[i] << "\" algorithm" << std::endl;

            auto start_time = std::chrono::steady_clock::now();
            long long found_flow = ALGOS[i](graph, capacityi, s, t, predictions);
            auto time = std::chrono::steady_clock::now() - start_time;

            flows_returned[i] = found_flow;
            flows_returned_set.insert(found_flow);

            double seconds_elapsed = (double) std::chrono::duration_cast<std::chrono::milliseconds>(time).count() / 1000.0;

            std::cout << "time elapsed: " << std::setprecision(3) << std::fixed << seconds_elapsed << std::endl;
            std::cout << std::endl;
            output_result << std::setprecision(3) << std::fixed << seconds_elapsed << ", ";
        }

        std::cout << std::endl;
        if (flows_returned_set.size() > 1) {
            std::cout << "flows_returned" << std::endl;
            for (int i = 0; i < ALGOS_COUNT; i++) {
                std::cout << ANAMES[i] << ": " << flows_returned[i] << std::endl;
            }
            output_result.close();
            return;
        }
    }
}