#include <iostream>
#include <fstream>

#include <boost/graph/read_dimacs.hpp>
#include "learning/learning.h"
#include <algos.h>

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
    std::cerr << "X_TRAIN: " << X << std::endl;
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
        algos alg;
        alg.run(graph_filename, preprocessed_flows_filename, results_file, test_samples, X);
        //algos(graph_filename, preprocessed_flows_filename, results_file, test_samples, X);
        return 0;
    }

    std::cerr << "incorrect args!";

    return 1;
}
