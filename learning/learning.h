//
// Created by Max Zub on 04/05/2022.
//

#ifndef MAXFLOWBENCHMARK_LEARNING_H
#define MAXFLOWBENCHMARK_LEARNING_H

#include <fstream>
#include <iostream>
#include "lemon/smart_graph.h"
#include "lemon/cost_scaling.h"
#include "lemon/preflow.h"
#include "dimacs_input.h"

#include "random_util.h"

class learning {
public:
    void start(
        std::string graph_filename,
        int samples_count,
        double X
    );
private:
    void add_edge(
            lemon::SmartDigraph &g,
            lemon::SmartDigraph::Node u,
            lemon::SmartDigraph::Node v,
            std::vector<long long> &computed_flows,
            lemon::SmartDigraph::ArcMap<long long> &cap,
            lemon::SmartDigraph::ArcMap<long long> &wght
            );
    static void print_learned_edges(
            lemon::SmartDigraph &g,
            lemon::CostScaling<lemon::SmartDigraph> &cs,
            lemon::SmartDigraph::ArcMap<long long> &cap,
            lemon::SmartDigraph::Node s,
            lemon::SmartDigraph::Node t
        );
};


#endif //MAXFLOWBENCHMARK_LEARNING_H
