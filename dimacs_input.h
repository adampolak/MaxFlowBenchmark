//
// Created by Max Zub on 02/07/2022.
//

#ifndef MAXFLOWBENCHMARK_DIMACS_INPUT_H
#define MAXFLOWBENCHMARK_DIMACS_INPUT_H

#include <lemon/dimacs.h>
#include <lemon/smart_graph.h>
#include <fstream>

class dimacs_input {
public:
    void read(
        std::string graph_filename,
        lemon::SmartDigraph &g,
        lemon::SmartDigraph::ArcMap<long long> &capacities,
        lemon::SmartDigraph::Node &s,
        lemon::SmartDigraph::Node &t
        );
};


#endif //MAXFLOWBENCHMARK_DIMACS_INPUT_H
