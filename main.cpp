#include <iostream>
#include <fstream>

#include <boost/graph/read_dimacs.hpp>

#include "algorithms/algorithm.h"
#include "algorithms/boykov_kolmogorov.h"
#include "algorithms/push_relabel.h"
#include "algorithms/learning_augmented.h"


const int ALGOS_COUNT = 3;

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cout << "specify filename!";
        return 1;
    }

    std::ifstream input_graph(argv[1]);
    if (!input_graph) {
        std::cout << "cannot open file: " << argv[1] << std::endl;
        return 1;
    }

    algorithm* arr[ALGOS_COUNT] = { new learning_augmented(),new boykov_kolmogorov(), new push_relabel() };


    Graph g;
    property_map<Graph, edge_capacity_t>::type
            capacity = get(edge_capacity, g);
    property_map<Graph, edge_reverse_t>::type
            rev = get(edge_reverse, g);
    Traits::vertex_descriptor s, t;
    std::cout << "reading graph" << std::endl;
    read_dimacs_max_flow(g, capacity, rev, s, t, input_graph);
    std::cout << "finished reading graph" << std::endl;
    input_graph.close();

    long flows_returned[ALGOS_COUNT];

    for (int i = 0; i < ALGOS_COUNT; i++) {
        std::cout << "looking for flow with \"" << arr[i]->name << "\" algorithm" << std::endl;

        auto start_time = std::chrono::steady_clock::now();
        long found_flow = arr[i]->find_flow(g, s, t);
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

    return 0;
}
