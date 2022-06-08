//
// Created by Max Zub on 25/05/2022.
//

#include "algos.h"
#include "algorithms/preflow.h"
#include "algorithms/edmonds_carp.h"
#include "algorithms/learning_augmented_add_edges_lemon.h"
#include "algorithms/learning_augmented_paths_removal_lemon.h"

#include <iostream>
#include <fstream>
#include <random>
#include <learning/random_util.h>
#include <lemon/list_graph.h>
#include <lemon/dimacs.h>
#include <boost/graph/read_dimacs.hpp>
#include <algorithms/boykov_kolmogorov.h>
#include <algorithms/push_relabel.h>
#include <algorithms/learning_augmented_add_edges.h>
#include <algorithms/learning_augmented_paths_removal.h>
#include <utility>
#include <chrono>
#include <iomanip>

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

        Graph g[ALGOS_COUNT];
        property_map<Graph, edge_capacity_t>::type
            capacity = get(edge_capacity, g[0]);
        property_map<Graph, edge_reverse_t>::type
            rev = get(edge_reverse, g[0]);

        Traits::vertex_descriptor s, t;
        read_dimacs_max_flow(g[0], capacity, rev, s, t, input_graph);
        input_graph.close();

        for (int i = 1; i < ALGOS_COUNT; i++) {
            std::ifstream input_graph(graph_filename);
            property_map<Graph, edge_capacity_t>::type
                capacity = get(edge_capacity, g[i]);
            property_map<Graph, edge_reverse_t>::type
                rev = get(edge_reverse, g[i]);
            read_dimacs_max_flow(g[i], capacity, rev, s, t, input_graph);
            input_graph.close();
        }

        std::ifstream input_preprocessed_flows(preprocessed_flows_filename);
        int num_ver = num_vertices(g[0]);
        int num_edge = num_edges(g[0])/2;
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

        auto edges = boost::edges(g[0]);
        std::vector<long long> orig_cap;
        for (auto edge = edges.first; edge != edges.second; edge++) {
            orig_cap.push_back(capacity[*edge]);
        }

        rand_gen.randomize_capacities(g[0], orig_cap, generator);
        for (int i = 1; i < ALGOS_COUNT; i++) {
            auto edges = boost::edges(g[i]);
            auto edges_zero = boost::edges(g[0]);
            auto it_zero = edges_zero.first;
            property_map<Graph, edge_capacity_t>::type
                capacity_new = get(edge_capacity, g[i]);
            for (auto it = edges.first; it != edges.second; it++, it_zero++) {
                capacity_new[*it] = capacity[*it_zero];
            }
        }

        algorithm* arr[ALGOS_COUNT] = {
            //new boykov_kolmogorov(g[0], s, t),
            new preflow(g[0], s, t),
            new learning_augmented_add_edges_lemon(g[1], s, t, vec),
            new learning_augmented_paths_removal_lemon(g[2], s, t, vec),
            //new push_relabel(g[2], s, t),
            //new edmonds_carp(g[3], s, t)
            //new learning_augmented_add_edges(g[2], s, t, vec),
            //new learning_augmented_paths_removal(g[3], s, t, vec)
        };

        for (int i = 0; i < ALGOS_COUNT; i++) {
            std::cout << "looking for flow with \"" << arr[i]->name << "\" algorithm" << std::endl;

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