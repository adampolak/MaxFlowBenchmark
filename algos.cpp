//
// Created by Max Zub on 25/05/2022.
//

#include "algos.h"


const int ALGOS_COUNT = 3;

void algos::run(
    distribution& d,
    const std::string& preprocessed_flows_filename,
    const std::string& results_file,
    int n_test_samples,
    int algorithms_bitmask
) {

    lemon::SmartDigraph& graph = const_cast<lemon::SmartDigraph&>(d.graph());
    const lemon::SmartDigraph::Node s = d.s();
    const lemon::SmartDigraph::Node t = d.t();

    std::ofstream output_result(results_file);

    output_result << "boy_kol, preflow, add_edge_preflow" << std::endl;

    std::cout << results_file << std::endl;

    std::default_random_engine generator;


    std::cout << n_test_samples << std::endl;

   std::ifstream input_preprocessed_flows(preprocessed_flows_filename);
    std::vector<std::pair<std::pair<int, int>, long long> > vec;
    if (input_preprocessed_flows) {
        for (int i = 0; i < graph.arcNum(); i++) {
            int a, b;
            long long c;
            input_preprocessed_flows >> a >> b >> c;
            vec.push_back({{a-1, b-1}, c});
        }
    }

    for (int L = 0; L < n_test_samples; L++) {

        lemon::SmartDigraph::ArcMap<int64_t> capacityi(graph);
        

        d.sample_capacities(capacityi, generator);

        lemon::SmartDigraph::ArcMap<long long> capacity(graph);
        for (auto aIt = lemon::SmartDigraph::ArcIt(graph); aIt != lemon::INVALID; ++aIt)
            capacity[aIt] = capacityi[aIt];


        int64_t flows_returned[ALGOS_COUNT];
        std::set<long long> flows_returned_set;
        lemon::SmartDigraph::ArcIt aIt(graph);

        algorithm* arr[ALGOS_COUNT] = {
            new preflow(graph, &capacity, s, t),
            new learning_augmented_add_edges_lemon(graph, &capacity, s, t, vec),
            new learning_augmented_paths_removal_lemon(graph, &capacity, s, t, vec),
        };

        for (int i = 0; i < ALGOS_COUNT; i++) {
            if (!((algorithms_bitmask>>i)&1))
                continue;
            std::cout << "looking for flow with \"" << arr[i]->name << "\" algorithm" << std::endl;

            arr[i]->build();

            auto start_time = std::chrono::steady_clock::now();
            long long found_flow = arr[i]->find_flow();
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
                std::cout << arr[i]->name << ": " << flows_returned[i] << std::endl;
            }
            output_result.close();
            return;
        }
    }
}