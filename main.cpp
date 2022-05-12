#include <iostream>
#include <fstream>

#include <boost/graph/read_dimacs.hpp>

#include "algorithms/algorithm.h"
#include "algorithms/boykov_kolmogorov.h"
#include "algorithms/push_relabel.h"
#include "algorithms/learning_augmented_add_edges.h"
#include "algorithms/learning_augmented_paths_removal.h"
#include "learning/learning.h"


/**
 *    usage:
 *    1. run algorithms benchmark:
 *       algo graph_filename preprocessed_flows_filename
 *
 *    2. run learning
 *       learn graph_filename n_samples max_capacity X
 *
 */

const int ALGOS_COUNT = 4;

void learn(std::string graph_filename, int n_samples, int max_capacity, int X) {
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
    std::cout << "FIRSTLINE" << std::endl;
    learning learn;
    learn.start(g, s, t, n_samples, max_capacity, X);
}

void algos(std::string graph_filename, std::string preprocessed_flows_filename) {
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
    std::cout << "reading graph" << std::endl;
    read_dimacs_max_flow(g[0], capacity, rev, s, t, input_graph);
    std::cout << "finished reading graph" << std::endl;
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
    if (!input_preprocessed_flows) {
        std::cerr << "cannot open file " << preprocessed_flows_filename << std::endl;
        exit(1);
    }
    int num_ver = num_vertices(g[0]);
    int num_edge = num_edges(g[0])/2;
    std::vector<std::pair<std::pair<int, int>, long> > vec;
    for (int i = 0; i < num_edge; i++) {
        int a, b, c;
        input_preprocessed_flows >> a >> b >> c;
        vec.push_back({{a-1, b-1}, c});
    }

    algorithm* arr[ALGOS_COUNT] = {
            new boykov_kolmogorov(g[0], s, t),
            new push_relabel(g[1], s, t),
            new learning_augmented_add_edges(g[2], s, t, vec),
            new learning_augmented_paths_removal(g[3], s, t, vec)
    };

    long flows_returned[ALGOS_COUNT];

    for (int i = 0; i < ALGOS_COUNT; i++) {
        std::cout << "looking for flow with \"" << arr[i]->name << "\" algorithm" << std::endl;

        auto start_time = std::chrono::steady_clock::now();
        long found_flow = arr[i]->find_flow();
        auto time = std::chrono::steady_clock::now() - start_time;

        flows_returned[i] = found_flow;

        double seconds_elapsed = (double)std::chrono::duration_cast<std::chrono::milliseconds>(time).count() / 1000.0;

        std::cout << "time elapsed: " << std::setprecision(3) << std::fixed << seconds_elapsed << std::endl;
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "flows_returned" << std::endl;
    for (int i = 0; i < ALGOS_COUNT; i++) {
        std::cout << arr[i]->name << " " << flows_returned[i] << std::endl;
    }
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "incorrect args!";
        return 1;
    }

    std::string type = argv[1];

    if (type == "learn") {
        if (argc < 6) {
            std::cerr << "incorrect args!";
        }
        std::string graph_filename = argv[2];
        int n_samples = atoi(argv[3]);
        int max_capacity = atoi(argv[4]);
        int X = atoi(argv[5]);
        learn(graph_filename, n_samples, max_capacity, X);
        return 0;
    }

    if (type == "algo") {
        if (argc < 4) {
            std::cerr << "incorrect args!";
        }
        std::string graph_filename = argv[2];
        std::string preprocessed_flows_filename = argv[3];
        algos(graph_filename, preprocessed_flows_filename);
        return 0;
    }

    std::cerr << "incorrect args!";

    return 1;
}
