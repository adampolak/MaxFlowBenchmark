#include <iostream>
#include <fstream>

#include <boost/graph/read_dimacs.hpp>
#include <chrono>
#include <iomanip>

#include "algorithms/algorithm.h"
#include "algorithms/boykov_kolmogorov.h"
#include "algorithms/push_relabel.h"
#include "algorithms/learning_augmented_add_edges.h"
#include "algorithms/learning_augmented_paths_removal.h"
#include "learning/learning.h"
#include "algorithms/dinic_algo.h"
#include "learning/random_util.h"

/**
 *    usage:
 *    1. run algorithms benchmark:
 *       algo graph_filename preprocessed_flows_filename results_file test_samples_number X
 *
 *    2. run learning
 *       learn graph_filename n_samples X
 *
 */

const int ALGOS_COUNT = 4;

void learn(std::string graph_filename, int n_samples, double X) {
    std::ifstream input_graph(graph_filename);
    if (!input_graph) {
        std::cerr << "cannot open file: " << graph_filename << std::endl;
        exit(1);
    }
    MinCostGraph g;
    property_map<MinCostGraph, edge_capacity_t>::type
            capacity = get(edge_capacity, g);
    property_map<MinCostGraph, edge_reverse_t>::type
            rev = get(edge_reverse, g);
    Traits::vertex_descriptor s, t;
    std::cerr << "reading graph..." << std::endl;
    read_dimacs_max_flow(g, capacity, rev, s, t, input_graph);
    std::cerr << "read graph..." << std::endl;
    learning learn;
    learn.start(g, s, t, n_samples, X);
}

void algos(
    const std::string& graph_filename,
    const std::string& preprocessed_flows_filename,
    const std::string& results_file,
    int test_samples,
    double X
    ) {


    std::ofstream output_result(results_file);

    output_result << "boy_kol, push_rel, edges, paths, edges_prep, paths_prep" << std::endl;


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
        std::vector<std::pair<std::pair<int, int>, long> > vec;
        if (input_preprocessed_flows) {
            for (int i = 0; i < num_edge; i++) {
                int a, b, c;
                input_preprocessed_flows >> a >> b >> c;
                vec.push_back({{a-1, b-1}, c});
            }
        }


        long flows_returned[ALGOS_COUNT];
        std::set<long> flows_returned_set;

        auto edges = boost::edges(g[0]);
        std::vector<long> orig_cap;
        for (auto edge = edges.first; edge != edges.second; edge++) {
            orig_cap.push_back(capacity[*edge]);
        }

        rand_gen.randomize_capacities(g[0], orig_cap, generator);
        for (int i = 1; i < ALGOS_COUNT; i++) {
            auto edges = boost::edges(g[i]);
            auto edges_zero = boost::edges(g[0]);
            auto it_zero = edges_zero.first;
            std::ifstream input_graph(graph_filename);
            property_map<Graph, edge_capacity_t>::type
                capacity_new = get(edge_capacity, g[i]);
            for (auto it = edges.first; it != edges.second; it++, it_zero++) {
                capacity_new[*it] = capacity[*it_zero];
            }
        }

        algorithm* arr[ALGOS_COUNT] = {
            new boykov_kolmogorov(g[0], s, t),
            new push_relabel(g[1], s, t),
            new learning_augmented_add_edges(g[2], s, t, vec),
            new learning_augmented_paths_removal(g[3], s, t, vec)
        };

        for (int i = 0; i < ALGOS_COUNT; i++) {
            std::cout << "looking for flow with \"" << arr[i]->name << "\" algorithm" << std::endl;

            auto start_time = std::chrono::steady_clock::now();
            long found_flow = arr[i]->find_flow();
            auto time = std::chrono::steady_clock::now() - start_time;

            flows_returned[i] = found_flow;
            flows_returned_set.insert(found_flow);

            double seconds_elapsed =
                (double) std::chrono::duration_cast<std::chrono::milliseconds>(time).count() / 1000.0;

            std::cout << "time elapsed: " << std::setprecision(3) << std::fixed << seconds_elapsed << std::endl;
            std::cout << std::endl;
            output_result << std::setprecision(3) << std::fixed << seconds_elapsed << ", ";
        }
        output_result << std::setprecision(3) << std::fixed << arr[2]->time_preprocess << ", ";
        output_result << std::setprecision(3) << std::fixed << arr[3]->time_preprocess << std::endl;

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

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "incorrect args!";
        return 1;
    }

    std::string type = argv[1];

    if (type == "learn") {
        if (argc < 5) {
            std::cerr << "incorrect args!";
            return 1;
        }
        std::string graph_filename = argv[2];
        int n_samples = atoi(argv[3]);
        std::stringstream strIn(argv[4]);
        double X;
        strIn >> X;
        learn(graph_filename, n_samples, X);
        return 0;
    }

    if (type == "algo") {
        if (argc < 7) {
            std::cerr << "incorrect args!";
            return 1;
        }
        std::string graph_filename = argv[2];
        std::string preprocessed_flows_filename = argv[3];
        std::string results_file = argv[4];
        int test_samples = atoi(argv[5]);
        std::stringstream strIn(argv[6]);
        double X;
        strIn >> X;
        algos(graph_filename, preprocessed_flows_filename, results_file, test_samples, X);
        return 0;
    }

    std::cerr << "incorrect args!";

    return 1;
}
