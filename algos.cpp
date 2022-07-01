//
// Created by Max Zub on 25/05/2022.
//

#include "algos.h"
#include "algorithms/preflow.h"
#include "algorithms/learning_augmented_add_edges_lemon.h"
#include "algorithms/learning_augmented_paths_removal_lemon.h"

#include <iostream>
#include <fstream>
#include <random>
#include <learning/random_util.h>
#include <utility>
#include <chrono>
#include <iomanip>
#include <lemon/dimacs.h>

const int ALGOS_COUNT = 3;

void algos::run(
    const std::string& graph_filename,
    const std::string& preprocessed_flows_filename,
    const std::string& results_file,
    int test_samples,
    double X
) {

    std::ofstream output_result(results_file);

    output_result << "boy_kol, preflow, add_edge_preflow" << std::endl;

    std::cerr << results_file << std::endl;

    random_util rand_gen(X);

    std::default_random_engine generator;


    for (int L = 0; L < test_samples; L++) {
        std::ifstream input_graph(graph_filename);
        if (!input_graph) {
            std::cerr << "cannot open file: " << graph_filename << std::endl;
            exit(1);
        }

        lemon::SmartDigraph g[ALGOS_COUNT];


        lemon::SmartDigraph::ArcMap<long long>* capacity[ALGOS_COUNT];
        for (int i = 0; i < ALGOS_COUNT; i++)
            capacity[i] = new lemon::SmartDigraph::ArcMap<long long>(g[0]);

        lemon::SmartDigraph::Node s, t;


        lemon::readDimacsMax(input_graph, g[0], *(capacity[0]), s, t);

        input_graph.close();

        for (int i = 1; i < ALGOS_COUNT; i++) {
            std::ifstream input_graph(graph_filename);
            lemon::readDimacsMax(input_graph, g[i], *(capacity[i]), s, t);
            input_graph.close();
        }

        std::ifstream input_preprocessed_flows(preprocessed_flows_filename);
        int num_edge = g[0].arcNum();
        std::vector<std::pair<std::pair<int, int>, long long> > vec;
        if (input_preprocessed_flows) {
            for (int i = 0; i < num_edge; i++) {
                int a, b;
                long long c;
                input_preprocessed_flows >> a >> b >> c;
                vec.push_back({{a-1, b-1}, c});
            }
        }


        long long flows_returned[ALGOS_COUNT];
        std::set<long long> flows_returned_set;
        lemon::SmartDigraph::ArcIt aIt(g[0]);

        std::vector<long long> orig_cap;
        for (; aIt != lemon::INVALID; ++aIt) {
            orig_cap.push_back((*capacity[0])[aIt]);
        }

        rand_gen.randomize_capacities(g[0], *capacity[0], orig_cap, generator);
        for (int i = 1; i < ALGOS_COUNT; i++) {
            lemon::SmartDigraph::ArcIt aIt0(g[0]);
            lemon::SmartDigraph::ArcIt aItI(g[i]);
            for (; aIt0 != lemon::INVALID; ++aIt0, ++aItI) {
                (*capacity[i])[aItI] = (*capacity[0])[aIt0];
            }
        }

        algorithm* arr[ALGOS_COUNT] = {
            new preflow(g[0], capacity[0], s, t),
            new learning_augmented_add_edges_lemon(g[1], capacity[1], s, t, vec),
            new learning_augmented_paths_removal_lemon(g[2], capacity[2], s, t, vec),
        };

        for (int i = 0; i < ALGOS_COUNT; i++) {
            std::cout << "looking for flow with \"" << arr[i]->name << "\" algorithm" << std::endl;

            arr[i]->build();

            auto start_time = std::chrono::steady_clock::now();
            long long found_flow = arr[i]->find_flow();
            auto time = std::chrono::steady_clock::now() - start_time;

            flows_returned[i] = found_flow;
            flows_returned_set.insert(found_flow);

            double seconds_elapsed =
                (double) std::chrono::duration_cast<std::chrono::milliseconds>(time).count() / 1000.0;

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