//
// Created by Max Zub on 02/07/2022.
//

#include "dimacs_input.h"


void dimacs_input::read(
    std::string graph_filename,
    lemon::SmartDigraph &g,
    lemon::SmartDigraph::ArcMap<long long> &capacities,
    lemon::SmartDigraph::Node &s,
    lemon::SmartDigraph::Node &t
) {
    std::ifstream graph_input(graph_filename);
    if (!graph_input) {
        std::cerr << "cannot open file: " << graph_filename << std::endl;
        exit(1);
    }
    lemon::readDimacsMax(graph_input, g, capacities, s, t);
}